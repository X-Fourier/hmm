#ifndef _MODEL_NODE_H_
#define _MODEL_NODE_H_

#include <vector>
class Transition;
class Hmm;

class ModelNode {
public:
    ModelNode(int time, unsigned long state);

    int time() const { return _time; }

    unsigned long state() const { return _state; }

    void logAlpha(double logAlpha) { _logAlpha = logAlpha; }

    double logAlpha() const { return _logAlpha; }

    void logBeta(double logBeta) { _logBeta = logBeta; }

    double logBeta() const { return _logBeta; }

    std::vector<Transition*>& ins() { return _ins; }

    std::vector<Transition*>& outs() { return _outs; }

    void psi(Transition* psi) { _psi = psi; }

    Transition* psi() { return _psi; }

    ~ModelNode();

private:
    int _time; // 当前节点所属时间槽

    unsigned long _state; // 隐含状态

    double _logAlpha;

    double _logBeta;
    
    std::vector<Transition*> _ins;

    std::vector<Transition*> _outs;

    Hmm* _hmm; // 属于哪个模型

    Transition* _psi; // 到达该节点最可能路径最后的状态转换
};

#endif
