//
// Created by kingaj on 10/9/18.
//

#ifndef TENSORMODEL_TMOBJECTIVE_H
#define TENSORMODEL_TMOBJECTIVE_H

#include <cstring>
#include <iostream>
#include <fstream>
#include <exception>

namespace tensormodel {

    using namespace std;

    class TmObjective {
    private:
        double *obj;
        bool ownTmData;
    public:
        explicit TmObjective(int s) {
            try {
                obj = new double[s];
                memset(obj, 0, s * sizeof(double));
                ownTmData = true;
            }
            catch (exception &e) {
                throw e;
            }
        }

        class ObjectiveError : public exception {
        public:
            virtual const char *what() {
                return "Incoming objective pointer is NULL";
            }
        } objectiveError;

        explicit TmObjective(double *d) {
            if (d == nullptr)
                throw objectiveError;
            obj = d;
            ownTmData = false;
        }

        ~TmObjective() {
            if (ownTmData)
                delete[] obj;
        }

        void load(int n, double d) {
            obj[n] = d;
        }

        void display(int n, ostream &out) {
            out << obj[n];
        }

        const double *getObjective() { return obj; }
    };
}

#endif //TENSORMODEL_TMOBJECTIVE_H
