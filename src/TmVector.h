//
// Created by kingaj on 10/9/18.
//

#ifndef TENSORMODEL_TMVECTOR_H
#define TENSORMODEL_TMVECTOR_H

#include "TmIndexed.h"
#include "TmNamed.h"
#include "TmSet.h"

namespace tensormodel {

    class TmVector : public TmIndexed, public TmNamed {
    private:
        double *d_;
        bool zeroDim;
    public:

        TmVector() : TmIndexed(1) {
            try {
                d_ = new double[1];
                d_[0] = 0.0;
            }
            catch (exception &e) {
                throw e;
            }
            zeroDim = true;
        }

        explicit TmVector(const TmSet &s1, const TmSet &s2 = TmSet(), const TmSet &s3 = TmSet(), const TmSet &s4 = TmSet(), const TmSet &s5 = TmSet())
                :
                TmIndexed(s1.getNum(), s2.getNum(), s3.getNum(), s4.getNum(), s5.getNum()) {
            try {
                unsigned int sz = this->getSize();
                d_ = new double[sz];
                memset(d_, 0, sz * sizeof(double));
            }
            catch (exception &e) {
                throw e;
            }

            this->setDim(1);

            if (!s2.isEmptySet())
                this->setDim(2);
            if (!s3.isEmptySet())
                this->setDim(3);
            if (!s4.isEmptySet())
                this->setDim(4);
            if (!s5.isEmptySet())
                this->setDim(5);

            zeroDim = false;
        }

        double getValue(unsigned int i1 = 0, unsigned int i2 = 0, unsigned int i3 = 0, unsigned int i4 = 0, unsigned int i5 = 0) {
            return  d_[index(i1, i2, i3, i4, i5)];
        }

        double operator()(unsigned int i1 = 0, unsigned int i2 = 0, unsigned int i3 = 0, unsigned int i4 = 0, unsigned int i5 = 0) {
            return d_[index(i1, i2, i3, i4, i5)];
        }

/*
	  double operator () () {
		  return *d_;
	  }
*/
        void display() {
            cout << "****" << this->getName() << "****\n";
            if (zeroDim) {
                cout << this->getName();
                cout << ":\t" << d_[0] << "\n";
            } else {
                for (unsigned int n = 0; n < this->getSize(); n++) {
                    cout << this->getName();
                    TmIndexed::display(n);
                    cout << ":\t" << d_[n] << "\n";
                }
            }
        }

//	  void setValues( string s);



        void setValue(double d, unsigned int i1 = 0, unsigned int i2 = 0, unsigned int i3 = 0, unsigned int i4 = 0, unsigned int i5 = 0) {
            //cout << "Setting value "<<d<< " for index "<< i1 << " " << i2 << " " << i3 << " " << i4 << " " << i5 << endl;
            d_[index(i1, i2, i3, i4, i5)] = d;
        }
    };
}
#endif //TENSORMODEL_TMVECTOR_H
