//
// Created by kingaj on 10/9/18.
//

#ifndef TENSORMODEL_TMINDEXED_H
#define TENSORMODEL_TMINDEXED_H

// Indexing for rows and columns
// Limit is 5 indexes
static const int TM_MAX_DIM = 5;

#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <exception>
#include <array>


namespace tensormodel {
    using namespace std;

    class TmIndexed {
    private:
        unsigned int s1;
        unsigned int s2;
        unsigned int s3;
        unsigned int s4;
        unsigned int s5;
        unsigned int size_{};
        unsigned int dim{};
        array<unsigned int, TM_MAX_DIM> v{};
        array<unsigned int, TM_MAX_DIM> s{};

    protected:
        void setDim(unsigned int i) {
            dim = i;
        }

    public:
        TmIndexed(TmIndexed &smI) :
                s1(smI.s1), s2(smI.s2), s3(smI.s3), s4(smI.s4), s5(smI.s5) {

            init();
        }

        explicit TmIndexed(array<unsigned int, TM_MAX_DIM> arr) :
                s1(arr[0]), s2(arr[1]), s3(arr[2]), s4(arr[3]), s5(arr[4]) {
            init();

        }

        friend void swap(TmIndexed &first, TmIndexed &second) {
            using std::swap;
            swap(first.s1, second.s1);
            swap(first.s2, second.s2);
            swap(first.s3, second.s3);
            swap(first.s4, second.s4);
            swap(first.s5, second.s5);

            swap(first.size_, second.size_);
            swap(first.dim, second.dim);
            swap(first.s, second.s);
        }

        TmIndexed &operator=(TmIndexed smI) {
            swap(*this, smI);
            return *this;
        }

        class IndexingError : public exception {
        public:
            virtual const char *what() {
                return "Error in Indexing class";
            }
        } indexingError;

        unsigned int index(unsigned int i1 = 0, unsigned int i2 = 0, unsigned int i3 = 0, unsigned int i4 = 0, unsigned int i5 = 0) const {
            if (i1 >= s1 || i2 >= s2 || i3 >= s3 || i4 >= s4 || i5 >= s5) {
                cerr << "index: ";
                int i = 0;
                cerr << "[" << i1;
                i++;
                if (i < dim) {
                    cerr << "," << i2;
                    i++;
                }
                if (i < dim) {
                    cerr << "," << i3;
                    i++;
                }
                if (i < dim) {
                    cerr << "," << i4;
                    i++;
                }
                if (i < dim) {
                    cerr << "," << i5;
                }
                cerr << "]";
                cerr << " out of range.\n";
                throw indexingError;
            }

            unsigned int i = i1;
            i *= s2;
            i += i2;
            i *= s3;
            i += i3;
            i *= s4;
            i += i4;
            i *= s5;
            i += i5;
            return i;
        }

        array<unsigned int, 5> invIndex(unsigned int n) {
            if (n >= size_) {
                cerr << "invIndex: size " << n << " out of range.\n";
                throw indexingError;
            }
            v[4] = n % s5;
            n = (n - v[4]) / s5;
            v[3] = n % s4;
            n = (n - v[3]) / s4;
            v[2] = n % s3;
            n = (n - v[2]) / s3;
            v[1] = n % s2;
            n = (n - v[1]) / s2;
            v[0] = n;
            return v;
        }

        explicit TmIndexed(unsigned int i1 = 1, unsigned int i2 = 1, unsigned int i3 = 1, unsigned int i4 = 1, unsigned int i5 = 1) :
                s1(i1), s2(i2), s3(i3), s4(i4), s5(i5), size_(i1 * i2 * i3 * i4 * i5) {
            init();
        }

        void init() {

            dim = 0;
            if (s1 > 1) dim++;
            if (s2 > 1) dim++;
            if (s3 > 1) dim++;
            if (s4 > 1) dim++;
            if (s5 > 1) dim++;


            s[0] = s1;
            s[1] = s2;
            s[2] = s3;
            s[3] = s4;
            s[4] = s5;

            v.fill(0);

            size_ = s1 * s2 * s3 * s4 * s5;

        }

        ~TmIndexed() = default;

        unsigned int getSize() const { return size_; }

        unsigned int getDim() const { return dim; }

        const array<unsigned int, TM_MAX_DIM> &getDimSizes() {
            return s;
        };

        unsigned int getDimSize(unsigned int i) const {
            //assert(i>0 && i<dim+1 && "Dimension must be between 1 and 5." );
            return s[i - 1];
        }

        void testIndex(unsigned int i1 = 0, unsigned int i2 = 0, unsigned int i3 = 0, unsigned int i4 = 0, unsigned int i5 = 0) {
            unsigned int n = this->index(i1, i2, i3, i4, i4);

            unsigned int i = 0;
            cout << "[" << i1;
            i++;
            if (i < dim) {
                cout << "," << i2;
                i++;
            }
            if (i < dim) {
                cout << "," << i3;
                i++;
            }
            if (i < dim) {
                cout << "," << i4;
                i++;
            }
            if (i < dim) {
                cout << "," << i5;
            }
            cout << "]";
            cout << " equals index " << n << "\n";
            cout << "index " << n << " equals ";
            this->display(n);
            cout << "\n";

        }

        void testInvIndex(unsigned int n) {
            this->invIndex(n);
            cout << "index " << n << " equals ";
            cout << "[" << v[0];
            for (unsigned int i = 1; i < dim; i++) {
                cout << "," << v[i];
            }
            cout << "]\n";
            n = this->index(v[0], v[1], v[2], v[3], v[4]);
            this->display(n);
            cout << " equals index " << n << "\n";
        }

        void display(unsigned int n) {
            this->invIndex(n);
            cout << "[" << v[0];
            for (unsigned int i = 1; i < dim; i++) {
                cout << "," << v[i];
            }
            cout << "]";
        }

        void display(unsigned int n, ostream &out) {
            this->invIndex(n);
            out << "[" << v[0];
            for (unsigned int i = 1; i < dim; i++) {
                out << "," << v[i];
            }
            out << "]";
        }
    };

}
#endif //TENSORMODEL_TMINDEXED_H
