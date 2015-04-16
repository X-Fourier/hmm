#ifndef _HMM_H_
#define _HMM_H_

#include "transition.h"
#include "time_slot.h"
#include "model_param.h"
#include "util/matrix.h"

class Hmm {
public:
    // 计算前向概率值
    void forward();

    // 计算后向概率值
    void backward();

    double getTransProb(Transition* trans);

    double getEmitProb(Transition* trans);

    // 计算隐含状态序列的viterbi算法
    double viterbi(std::vector<Transition*>& path);

    // 根据观察序列学习模型的EM算法
    void baumWelch(const std::vector<std::vector<unsigned long>*>&, int);

    // 更新模型参数
    void updateParams(ModelParam& counts);

    double getModelParams(ModelParam& counts);

private:
    unsigned long _initState; // 初始状态值

    matrix2d _transition; // 转移概率矩阵

    matrix2d _emission; // 发射概率矩阵

    std::vector<TimeSlot*> _timeSlots; // 时间槽

    double _minLogProb;
};

#endif
