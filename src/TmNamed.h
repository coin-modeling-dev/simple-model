//
// Created by kingaj on 10/9/18.
//

#ifndef TENSORMODEL_TMNAMED_H
#define TENSORMODEL_TMNAMED_H

#include <string>

namespace tensormodel {

    using namespace std;

    class TmNamed {
    private:
        string name_;
    public:
        explicit TmNamed(const string &t_name) {
            setName(t_name);
        }

        TmNamed() = default;

        ~TmNamed() = default;

        void setName(const string &s) {
            name_ = s;
        }

        string getName() { return name_; }
    };
}
#endif //TENSORMODEL_TMNAMED_H
