#include "model_node.h"
#include "transition.h"

ModelNode::ModelNode(int time, unsigned long state) {
    _time = time;
    _state = state;
    _logAlpha = 0;
    _logBeta = 0;
}

ModelNode::~ModelNode() {
    for (unsigned int i = 0; i < _outs.size(); i++) {
        delete _outs[i];
    }
}
