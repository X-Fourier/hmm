#ifndef _TRANSITION_H_
#define _TRANSITION_H_

#include "model_node.h"

class Transition {
public:
    Transition(ModelNode* from, ModelNode* to, unsigned long obs) {
        _from = from;
        _to = to;
        _obs = obs;

        if (_from && _to) {
            _from->outs().push_back(this);
            _to->ins().push_back(this);
        }
    }
    
    ~Transition() {
    
    }

    ModelNode* from() { return _from; }

    ModelNode* to() { return _to; }

    unsigned long obs() { return _obs; }

private:
    unsigned long _obs;
    ModelNode* _from;
    ModelNode* _to;
};

#endif
