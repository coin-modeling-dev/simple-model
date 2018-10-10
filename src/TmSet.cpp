//
// Created by kingaj on 10/9/18.
//

#include "TmSet.h"

using namespace tensormodel;
using namespace std;

string TmSet::getNameByIndx(const unsigned int index) const {
    if (index >= num)
        throw setError;

    return names.right.at(index);
}

unsigned int TmSet::getIndxByName(const string name) const {
    return names.left.at(name);
}

unsigned int TmSet::getIDByIndx(const unsigned int index) const {
    if (index >= num)
        throw setError;
    return IDs[index];
}

// initialize from file -- legacy code
#if 0
void TmSet::initialize(string fileLoc) {
    char tmp[TM_MAX_STRING_LENGTH];
    string line;
    ifstream in(fileLoc);

    // skip the comment line
    getline(in, line);

    getline(in, line);
    sscanf(line.c_str(), "%s\t%d", tmp, &num);

    // skip the empty line
    getline(in, line);
    getline(in, line);

    IDs.reserve(num);

    for (unsigned int cnt = 0; cnt < num; cnt++) {
        getline(in, line);
        sscanf(line.c_str(), "%d %s", &IDs[cnt], tmp);
        names.insert(names_position(tmp, cnt));
    }// end for count store

    in.close();
}
#endif

