#ifndef _TIME_SLOT_H_
#define _TIME_SLOT_H_

#include "model_node.h"
#include <vector>

class TimeSlot : public std::vector<ModelNode*> {
public:
    TimeSlot() {
    
    }

    virtual ~TimeSlot() {
        for (TimeSlot::iterator it = begin(); it != end(); ++it) {
            delete (*it);
        }
    }
};

#endif
