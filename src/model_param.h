#ifndef _MODEL_PARAM_H_
#define _MODEL_PARAM_H_

#include "util/vector2d.h"
#include "util/matrix.h"

class ModelParam {
public:
    ModelParam() {
    
    }

    vector2d& stateCount() { return _stateCount; }

    matrix2d& transCount() { return _transCount; }

    matrix2d& emitCount() { return _emitCount; }

    ~ModelParam() {
    }

private:
    vector2d _stateCount;
    matrix2d _transCount;
    matrix2d _emitCount;
};

#endif
