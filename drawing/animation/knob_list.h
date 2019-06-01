//
// Created by Ivan Galakhov on 2019-06-01.
//

#ifndef WORK_01_LINE_KNOB_LIST_H
#define WORK_01_LINE_KNOB_LIST_H

#include <unordered_map>

class KnobList {
public:
    KnobList();
    void print();
    std::unordered_map<char *, double> vals;
};


#endif //WORK_01_LINE_KNOB_LIST_H
