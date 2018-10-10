//
// Created by kingaj on 10/9/18.
//

#ifndef TENSORMODEL_TMVARIABLE_H
#define TENSORMODEL_TMVARIABLE_H

#include "TmIndexed.h"
#include "TmSet.h"
#include "TmBounds.h"
#include "TmObjective.h"

namespace tensormodel {

// TmVariable is the base class for rows and columns
// limit is TM_MAX_DIM indexes (imposed by TmIndexed class)
    class TmVariable : public TmIndexed, public TmNamed {

    private:
        string name_;
        int offset_{};
        TmBounds *b_{};
        TmObjective *obj_{};
        int stage_{};
        bool hasObjective{};  // only columns have objectives

    public:
        TmVariable() = default;

        TmVariable(const string &name, unsigned int t, const TmSet &s1 = TmSet(), const TmSet &s2 = TmSet(),
                   const TmSet &s3 = TmSet(),
                   const TmSet &s4 = TmSet(), const TmSet &s5 = TmSet()) :
                TmNamed(name),
                TmIndexed(s1.getNum(), s2.getNum(), s3.getNum(), s4.getNum(), s5.getNum()), b_(), stage_(t),
                hasObjective(false) {
            this->setDim(0);
            if (!s1.isEmptySet())
                this->setDim(1);
            if (!s2.isEmptySet())
                this->setDim(2);
            if (!s3.isEmptySet())
                this->setDim(3);
            if (!s4.isEmptySet())
                this->setDim(4);
            if (!s5.isEmptySet())
                this->setDim(5);
        }

        virtual ~TmVariable() {
            delete b_;
            if (hasObjective)
                delete obj_;
        }

        void setHasObjective() {
            hasObjective = true;
        }

        int operator()(int i1 = 0, int i2 = 0, int i3 = 0, int i4 = 0, int i5 = 0) {
            return index(i1, i2, i3, i4, i5) + this->getOffset();
        }

        void loadBnd(int n, double dl, double du) {
            n -= this->getOffset();
            b_->load(n, dl, du);
        }

        void loadObj(int n, double o) {
            n -= this->getOffset();
            obj_->load(n, o);
        }

        TmBounds *getBounds() { return b_; }

        TmObjective *getObjective() { return obj_; }

        int getStage() { return stage_; }

        void setStage(unsigned int s) { stage_ = s; }

        void setOffset(int i) { offset_ = i; }

        int getOffset() { return offset_; }

        void display(ostream &out) {
            for (unsigned int n = 0; n < this->getSize(); n++) {
                out << "Stage " << this->stage_ << "\t";
                out << "Index " << n + offset_ << "\t";
                out << this->getName();
                TmIndexed::display(n, out);
                out << ": ";
                b_->display(n, out);
                if (hasObjective) {
                    out << "  obj: ";
                    obj_->display(n, out);
                }
                out << "\n";
            }
        }

        void displayValues(ostream &out, const string &s, const double *val) {
            for (unsigned int n = 0; n < this->getSize(); n++) {
                out << "Stage " << this->stage_ << "\t";
                out << "Index " << n + offset_ << "  \t";
                out << this->getName();
                TmIndexed::display(n, out);
                out << "   \t" << s << ": " << val[n + offset_] << "\n";
            }
        }


        /** called by SimpleModel::initializeDataStructures */
        void initializeDataStructures(double *lowerBnd, double *upperBnd, double *objective = nullptr) {
            b_ = new TmBounds(lowerBnd, upperBnd);
            if (hasObjective) {
                obj_ = new TmObjective(objective);
            }
        }
    };

}

#endif //TENSORMODEL_TMVARIABLE_H
