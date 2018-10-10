//
// Created by kingaj on 10/9/18.
//

#ifndef TENSORMODEL_TMBOUNDS_H
#define TENSORMODEL_TMBOUNDS_H

#include <cstring>
#include <exception>
#include <iostream>
#include <fstream>

namespace tensormodel {

    using namespace std;

    class TmBounds {
    private:
        double *lower;
        double *upper;
        bool ownTmData;
    public:
        explicit TmBounds(int s) {
            try {
                lower = new double[s];
                memset(lower, 0, s * sizeof(double));
                upper = new double[s];
                memset(upper, 0, s * sizeof(double));
                ownTmData = true;
            }
            catch (exception &e) {
                throw e;
            }
        }

        class BoundsError : public exception {
        public:
            virtual const char *what() {
                return "Incoming bounds are null";
            }
        } boundsError;

        TmBounds(double *l, double *u) {
            if (l == nullptr || u == nullptr)
                throw boundsError;
            lower = l;
            upper = u;
            ownTmData = false;
        }

        ~TmBounds() {
            if (ownTmData) {
                delete[] lower;
                delete[] upper;
            }
        }

        void load(int n, double dl, double du) {
            lower[n] = dl;
            upper[n] = du;
        }

        void display(int n, ostream &out) {
            out << lower[n] << ", " << upper[n];
        }

        const double *getLower() { return lower; }

        const double *getUpper() { return upper; }
    };
}

#endif //TENSORMODEL_TMBOUNDS_H
