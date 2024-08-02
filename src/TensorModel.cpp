//
// Created by kingaj on 10/9/18.
//

#include "TensorModel.h"

using namespace tensormodel;

p_TmVariable
TensorModel::createColumn(const string& t_variableName, const TmSet &s1, const TmSet &s2, const TmSet &s3, const TmSet &s4,
                          const TmSet &s5) {
    p_TmVariable tmVariable = std::make_shared<TmVariable>(t_variableName, s1, s2, s3, s4, s5);
    tmVariable->setOffset(ncols);
    colObjects.insert(make_pair(t_variableName, tmVariable));
    ncols += tmVariable->getSize();
    return tmVariable;
}

p_TmVariable
TensorModel::createRow(const string& t_variableName, const TmSet &s1, const TmSet &s2, const TmSet &s3, const TmSet &s4,
                       const TmSet &s5) {
    p_TmVariable tmVariable = std::make_shared<TmVariable>(t_variableName, s1, s2, s3, s4, s5);
    tmVariable->setOffset(nrows);
    rowObjects.insert(make_pair(t_variableName, tmVariable));
    nrows += tmVariable->getSize();
    return tmVariable;
}

void TensorModel::display(ostream &out, const string& matrixFile) {

    out << "ROWS:\n";
    for (auto row : rowObjects) {
        row.second->display(out);
    }

    out << "COLUMNS:\n";
    for (auto col : colObjects) {
        col.second->display(out);
    }

    out << "MATRIX: \n" << "\tWritten to " << matrixFile.c_str() << "\n";

    mat->dumpMatrix(matrixFile.c_str());
}

void TensorModel::initializeDataStructures() {


    try {
        dclo = new double[ncols];
        dcup = new double[ncols];
        dobj = new double[ncols];
        cstg = new unsigned int[ncols];
        memset(dclo, 0, ncols * sizeof(double));
        memset(dcup, 0, ncols * sizeof(double));
        memset(dobj, 0, ncols * sizeof(double));
        memset(cstg, 0, ncols * sizeof(int));
        drlo = new double[nrows];
        drup = new double[nrows];
        rstg = new unsigned int[nrows];
        memset(drlo, 0, nrows * sizeof(double));
        memset(drup, 0, nrows * sizeof(double));
        memset(rstg, 0, nrows * sizeof(int));
    }
    catch (exception &e) {
        throw e;
    }

    for (auto row: rowObjects) {
        auto p_row = row.second;
        unsigned int offset = p_row->getOffset();
        p_row->initializeDataStructures(drlo + offset, drup + offset);

        unsigned int stage = p_row->getStage();
        for (unsigned int ii = 0; ii < p_row->getSize(); ii++) {
            rstg[ii + offset] = stage;
        }

    }
    for (auto col: colObjects) {
        auto p_col = col.second;
        p_col->setHasObjective();
        unsigned int offset = p_col->getOffset();
        p_col->initializeDataStructures(dclo + offset, dcup + offset, dobj + offset);

        unsigned int stage = p_col->getStage();
        for (unsigned int jj = 0; jj < p_col->getSize(); jj++) {
            cstg[jj + offset] = stage;
        }
    }

}

void TensorModel::loadMat(int m, int n, double d) {
    if (isMatrixLoaded) {
        mat->modifyCoefficient(m, n, d);
    } else {
        mrow.push_back(m);
        mcol.push_back(n);
        dels.push_back(d);
    }
}

void TensorModel::loadSM_Data() {
    //matrix
    if (this->isMatrixLoaded) return;

    mat = new CoinPackedMatrix(true, mrow.data(), mcol.data(), dels.data(), dels.size());
    isMatrixLoaded = true;

}

void TensorModel::loadOsiProblem(OsiSolverInterface *osi) {
    this->setSolver(osi);
    this->getSolver()->loadProblem(reinterpret_cast<const CoinPackedMatrix &>(mat), dclo, dcup, dobj, drlo, drup);
}

void TensorModel::solveProblem() {
    this->getSolver()->loadProblem(reinterpret_cast<const CoinPackedMatrix &>(mat), dclo, dcup, dobj, drlo, drup);
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

