#ifndef _HMM_H_
#define _HMM_H_

#include "transition.h"
#include "time_slot.h"
#include "model_param.h"
#include "util/matrix.h"

class Hmm {
public:
    Hmm();

    // 计算前向概率值
    void forward();

    // 计算后向概率值
    void backward();

    double getTransProb(Transition* trans);

    double getEmitProb(Transition* trans);

    // 计算隐含状态序列的viterbi算法
    double viterbi(std::vector<Transition*>& path);

    // 将某个观察值加入到序列中
    void addObs(unsigned long obs);

    // 根据观察序列训练模型的EM算法
    void baumWelch(const std::vector<std::vector<unsigned long>*>&, int);

    // 计算观察序列的概率值
    double obsProb();

    // 迭代过程中更新模型的参数
    void updateParams(ModelParam& counts);

    // 根据观察序列对模型参数进行设置
    double setModelParams(ModelParam& counts);

    void reset();
    
    ~Hmm();

private:
    unsigned long _initState; // 初始状态值

    matrix2d _transition; // 转移概率矩阵

    matrix2d _emission; // 发射概率矩阵

    std::vector<TimeSlot*> _timeSlots; // 时间槽
};

#endif
