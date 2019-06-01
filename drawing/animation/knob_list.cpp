//
// Created by Ivan Galakhov on 2019-06-01.
//

#include "knob_list.h"

KnobList::KnobList() {
    vals.clear();
}

void KnobList::print(){
    for(auto &i : vals){
        printf("[%s %f] ", i.first, i.second);
    }
    printf("\n");
}