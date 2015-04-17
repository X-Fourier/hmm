#include "model_node.h"
#include "transition.h"

ModelNode::ModelNode(int time, unsigned long state, Hmm* hmm) {
    _time = time;
    _state = state;
    _logAlpha = 0;
    _logBeta = 0;
    _hmm = hmm;
    _psi = NULL;
}

ModelNode::~ModelNode() {
    for (unsigned int i = 0; i < _outs.size(); i++) {
        delete _outs[i];
    }
}
