//
// Created by kingaj on 10/9/18.
//

#include "TensorModel.h"

using namespace tensormodel;

TensorModel::p_TmVariable
TensorModel::createColumn(string t_variableName,
                          int t,
                          const TmSet &s1,
                          const TmSet &s2,
                          const TmSet &s3,
                          const TmSet &s4,
                          const TmSet &s5) {
    p_TmVariable tmVariable = std::make_shared<TmVariable>(t_variableName, t, s1, s2, s3, s4, s5);
    tmVariable->setOffset(ncols);
    colObjects.insert(make_pair(t_variableName, tmVariable));
    ncols += tmVariable->getSize();
    return tmVariable;
}

TensorModel::p_TmVariable
TensorModel::createRow(string t_variableName,
                       int t,
                       const TmSet &s1,
                       const TmSet &s2,
                       const TmSet &s3,
                       const TmSet &s4,
                       const TmSet &s5) {
    p_TmVariable tmVariable = std::make_shared<TmVariable>(t_variableName, t, s1, s2, s3, s4, s5);
    tmVariable->setOffset(nrows);
    rowObjects.insert(make_pair(t_variableName, tmVariable));
    nrows += tmVariable->getSize();
    return tmVariable;
}

void TensorModel::display(ostream &out, string matrixFile) {

    out << "ROWS:\n";
    for (auto row : rowObjects) {
        row.second->display(out);
    }

    out << "COLUMNS:\n";
    for (auto col : colObjects) {
        col.second->display(out);
    }

    out << "MATRIX:\n";

    mat.dumpMatrix(matrixFile.c_str());
}

void TensorModel::initializeDataStructures() {


    try {
        dclo = new double[ncols];
        dcup = new double[ncols];
        dobj = new double[ncols];
        cstg = new int[ncols];
        memset(dclo, 0, ncols * sizeof(double));
        memset(dcup, 0, ncols * sizeof(double));
        memset(dobj, 0, ncols * sizeof(double));
        memset(cstg, 0, ncols * sizeof(int));
        drlo = new double[nrows];
        drup = new double[nrows];
        rstg = new int[nrows];
        memset(drlo, 0, nrows * sizeof(double));
        memset(drup, 0, nrows * sizeof(double));
        memset(rstg, 0, nrows * sizeof(int));
    }
    catch (exception &e) {
        throw e;
    }

    for (auto row: rowObjects) {
        auto p_row = row.second;
        int offset = p_row->getOffset();
        p_row->initializeDataStructures(drlo + offset, drup + offset);

        int stage = p_row->getStage();
        for (unsigned int ii = 0; ii < p_row->getSize(); ii++) {
            rstg[ii + offset] = stage;
        }

    }
    for (auto col: colObjects) {
        auto p_col = col.second;
        int offset = p_col->getOffset();
        p_col->initializeDataStructures(dclo + offset, dcup + offset, dobj + offset);

        int stage = p_col->getStage();
        for (unsigned int jj = 0; jj < p_col->getSize(); jj++) {
            cstg[jj + offset] = stage;
        }
    }

}

void TensorModel::loadMat(int m, int n, double d) {
    if (isMatrixLoaded) {
        mat.modifyCoefficient(m, n, d);
    } else {
        mrow.push_back(m);
        mcol.push_back(n);
        dels.push_back(d);
    }
}

void TensorModel::loadSM_Data() {
    //matrix
    if (this->isMatrixLoaded) return;

    int icol = 0;
    const unsigned int nels = mcol.size();
    indices = new int[nels];
    colStart = new int[ncols + 1];
    elements = new double[nels];
    for (unsigned int j = 0; j < ncols + 1; j++)
        colStart[j] = 0;
    for (unsigned int n = 0; n < nels; n++) {
        int j = mcol[n];
        colStart[j]++;
        //cout<<"index "<<j<<" colstart "<<colStart[j]<<"\n";
    }
    colStart[ncols] = nels;
    icol = ncols - 1;
    while (icol > -1) {
        colStart[icol] = colStart[icol + 1] - colStart[icol];
        icol--;
    }
    for (unsigned int n = 0; n < nels; n++) {
        icol = mcol[n];
        int nn = colStart[icol];
        indices[nn] = mrow[n];
        elements[nn] = dels[n];
        colStart[icol]++;
    }
    int next1 = colStart[0];
    int next2 = 0;
    colStart[0] = 0;
    for (unsigned int j = 1; j < ncols + 1; j++) {
        next2 = colStart[j];
        colStart[j] = next1;
        next1 = next2;
        //cout<<"index "<<j<<" colstart "<<colStart[j]<<"\n";
    }
    int *len = nullptr;
    mat.assignMatrix(true, nrows, ncols, nels, elements, indices, colStart, len);

    isMatrixLoaded = true;

}

void TensorModel::loadOsiProblem(OsiClpSolverInterface *osi) {
    this->setSolver(osi);
    this->getSolver()->loadProblem(mat, dclo, dcup, dobj, drlo, drup);
}

void TensorModel::solveProblem() {
    this->getSolver()->loadProblem(mat, dclo, dcup, dobj, drlo, drup);
    this->getSolver()->setObjSense(-1);
    this->getSolver()->initialSolve();
}

void TensorModel::displaySolution(ostream &out) {
    double objval = this->getSolver()->getObjValue();
    const double *solution = this->getSolver()->getColSolution();
    const double *duals = this->getSolver()->getRowPrice();

    cout << "Optimal Value: " << objval << endl;
    cout << "SOLUTIONS\n";
    this->displayColumns(out, "solution", solution);
    cout << "DUALS\n";
    this->displayRows(out, "dual", duals);
}

void TensorModel::displayColumns(ostream &out, const string &s, const double *d) {

    for (auto col : colObjects) {
        col.second->displayValues(out, s, d);
    }
}

void TensorModel::displayRows(ostream &out, const string &s, const double *d) {

    for (auto row : rowObjects) {
        row.second->displayValues(out, s, d);
    }
}


#if 0
void SM_Data::setValues(string fileLoc)
    {
        string line;
        ifstream in(fileLoc);
        int tmp1,tmp2,tmp3,tmp4,tmp5;
        float dVal;

        // skip the comment line
        getline(in,line);

        // skip the empty line
        getline(in,line);

        // different line reads for different dimensions
        switch(this->getDim())
        {
        case 0:						//only one value
            getline(in,line);
            sscanf(line.c_str(), "%f",&dVal);
            this->setValue(dVal);
            break;
        case 1:						//1-dimensional
            while (! in.eof() )
            {
                getline(in,line);
                sscanf(line.c_str(), "%d: %f",&tmp1,&dVal);
                this->setValue(dVal,tmp1);
            }
            break;
        case 2:						//2-dimensional
            while (! in.eof() )
            {
                getline(in,line);
                sscanf(line.c_str(), "%d %d: %f",&tmp1,&tmp2,&dVal);
                this->setValue(dVal,tmp1,tmp2);
            }
            break;
        case 3:						//3-dimensional
            while (! in.eof() )
            {
                getline(in,line);
                sscanf(line.c_str(), "%d %d %d: %f",&tmp1,&tmp2,&tmp3,&dVal);
                this->setValue(dVal,tmp1,tmp2,tmp3);
            }
            break;
        case 4:						//4-dimensional
            while (! in.eof() )
            {
                getline(in,line);
                sscanf(line.c_str(), "%d %d %d %d: %f",&tmp1,&tmp2,&tmp3,&tmp4,&dVal);
                this->setValue(dVal,tmp1,tmp2,tmp3,tmp4);
            }
            break;
        case 5:						//5-dimensional
            while (! in.eof() )
            {
                getline(in,line);
                sscanf(line.c_str(), "%d %d %d %d %d: %f",&tmp1,&tmp2,&tmp3,&tmp4,&tmp5,&dVal);
                this->setValue(dVal,tmp1,tmp2,tmp3,tmp4,tmp5);
            }
            break;
        }

        in.close();
    }
#endif

