//
// Created by kingaj on 10/8/18.
//

#ifndef TENSORMODEL_TENSORMODEL_H
#define TENSORMODEL_TENSORMODEL_H

#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <exception>
#include <array>
#include <map>

#include <coin/OsiSolverInterface.hpp>
#include <coin/CoinPackedVector.hpp>
#include <coin/CoinPackedMatrix.hpp>

// TODO find out how to install OsiClpSolverInterface
// #include <coin/OsiClpSolverInterface.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/filesystem.hpp>
#include <boost/bimap.hpp>

#include "HdfWriter.h"
#include "TmIndexed.h"
#include "TmNamed.h"
#include "TmSet.h"
#include "TmBounds.h"
#include "TmObjective.h"
#include "TmVariable.h"

namespace tensormodel {
    using namespace std;


// Tensor Model class
    class TensorModel {

    private:
        OsiSolverInterface *osi_;
        double *dclo{}, *dcup{}, *dobj{}, *drlo{}, *drup{};
        int *cstg{}, *rstg{};
        int *indices{}, *colStart{};
        double *elements{};
        vector<int> mcol, mrow;
        vector<double> dels;
        CoinPackedMatrix mat;
        unsigned int nrows;
        unsigned int ncols;
        double infinity;

        bool isMatrixLoaded;

        map<string, p_TmVariable> rowObjects;
        map<string, p_TmVariable> colObjects;


    public:

        explicit TensorModel(OsiSolverInterface *osi) : mat() {
            osi_ = osi;
            nrows = 0;
            ncols = 0;
            infinity = osi_->getInfinity();
            isMatrixLoaded = false;
        }

        TensorModel() : mat(), osi_() {
            nrows = 0;
            ncols = 0;
            infinity = 1e31;
            isMatrixLoaded = false;
        }

        ~TensorModel() {

            delete[] dclo;
            delete[] dcup;
            delete[] dobj;
            delete[] drlo;
            delete[] drup;
            delete[] cstg;
            delete[] rstg;
        }

        void display(ostream &out, string matrixFile);

        void setSolver(OsiSolverInterface *osi) { osi_ = osi; }

        OsiSolverInterface *getSolver() { return osi_; }

        void initializeDataStructures();

        void loadSM_Data();

        void loadOsiProblem(OsiSolverInterface *osi);

        void solveProblem();

        void displaySolution(ostream &out);

        p_TmVariable getColumn(const string &t_name) { return colObjects[t_name]; }

        void displayColumns(ostream &out, const string &s, const double *d);

        p_TmVariable getRow(const string &t_name) { return rowObjects[t_name]; }

        void displayRows(ostream &out, const string &s, const double *d);

        double getInfinity() { return infinity; }

        p_TmVariable createColumn(string t_variableName,
                                  int t,
                                  const TmSet &s1 = TmSet(),
                                  const TmSet &s2 = TmSet(),
                                  const TmSet &s3 = TmSet(),
                                  const TmSet &s4 = TmSet(),
                                  const TmSet &s5 = TmSet());

        p_TmVariable createRow(string t_variableName,
                               int t,
                               const TmSet &s1 = TmSet(),
                               const TmSet &s2 = TmSet(),
                               const TmSet &s3 = TmSet(),
                               const TmSet &s4 = TmSet(),
                               const TmSet &s5 = TmSet());


        void loadMat(int m, int n, double d);

        int *getRowStages() { return rstg; }

        int *getColStages() { return cstg; }

        int getNrows() { return nrows; }

        int getNcols() { return ncols; }

        double *getClo() { return dclo; }

        double *getCup() { return dcup; }

        double *getObj() { return dobj; }

        double *getRlo() { return drlo; }

        double *getRup() { return drup; }

        CoinPackedMatrix &getMat() { return mat; }


    };


}
#endif //TENSORMODEL_TENSORMODEL_H
