#include "model_node.h"

ModelNode::ModelNode(int time, unsigned long state) {
    _time = time;
    _state = state;
    _logAlpha = 0;
    _logBeta = 0;
}

ModelNode::~ModelNode() {
    
}
