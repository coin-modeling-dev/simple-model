//
// Created by kingaj on 10/9/18.
//

#ifndef TENSORMODEL_TMSET_H
#define TENSORMODEL_TMSET_H

#define TM_MAX_STRING_LENGTH 128

#include <boost/bimap.hpp>
#include <iostream>

#include "TmNamed.h"

namespace tensormodel {

    using namespace std;

    class TmSet : public TmNamed {
    private:
        unsigned int num;

        typedef boost::bimap<string, unsigned int> names_bimap;
        typedef names_bimap::value_type names_position;
        names_bimap names;

        vector<unsigned int> IDs;
        bool isEmpty;

    public:
        class SetError : public exception {
        public:
            virtual const char *what() {
                return "TmSet: index out of bounds";
            }
        } setError;

        explicit TmSet() {
            num = 1;
            isEmpty = true;
        }

        TmSet(unsigned int n, const string &name_set, unsigned int max_num = 0) {
            this->setName(name_set);
            num = max(n, max_num);
            for (unsigned int i = 0; i < n; i++) {
                IDs.push_back(i);
                this->insertName(i);
            }
            isEmpty = false;
        }

        const bool isEmptySet() const { return isEmpty; }

        unsigned int getNum() const { return num; }

        string getNameByIndx(unsigned int index) const;

        unsigned int getIDByIndx(unsigned int index) const;

        unsigned int getIndxByName(string) const;

        bool existsName(const string &name) {
            auto itr = names.left.find(name);
            return (itr != names.left.end());
        }

        bool existsIndex(unsigned int indx) {
            auto itr = names.right.find(indx);
            return (itr != names.right.end());
        }

        unsigned int addName(const string &name) {
            unsigned int indx = names.size();
            if (!existsName(name)) {
                if (indx + 1 > num)
                    throw runtime_error("Adding name to set violates maximum names set at initialization.");
                names.insert(names_position(name, indx));
            }
            return indx;
        }

        unsigned int addName() {
            ostringstream tmp;
            tmp << this->getName() << this->getNum();
            return addName(tmp.str());
        }

        void insertName(unsigned int indx, const string &name = "") {
            if (indx > num)
                throw runtime_error("Adding name to set violates maximum names set at initialization.");
            if (name.length() == 0) {
                ostringstream tmp;
                tmp << this->getName() << indx;
                names.insert(names_position(tmp.str(), indx));
            } else {
                names.insert(names_position(name, indx));
            }
        }

        void initialize(string fileLoc);

        virtual ~TmSet() = default;

        void display() {
            cout << "****" << this->getName() << "****\n";
            auto names_end = names.end();
            for (auto itr = names.begin(); itr != names_end; ++itr) {
                cout << "Index: " << itr->right << "\tName: " << itr->left << endl;
            }
        }
    };
}
#endif //TENSORMODEL_TMSET_H
