#ifndef _MODEL_NODE_H_
#define _MODEL_NODE_H_

class ModelNode {
public:
    ModelNode(int time, unsigned long state);

    int time() const {
        return _time;
    }

    unsigned long state() const;

    void logAlpha(double logAlpha) {
        _logAlpha = logAlpha;
    }

    double logAlpha() const {
        return _logAlpha;
    }

    void logBeta(double logBeta) {
        _logBeta = logBeta;
    }

    double logBeta() const {
        return _logBeta;
    }

    ~ModelNode();

private:
    int _time; // 当前节点所属时间槽

    unsigned long _state; // 隐含状态

    double _logAlpha;

    double _logBeta;
};

#endif
