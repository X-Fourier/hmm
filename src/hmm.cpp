#include "hmm.h"
#include <stdint.h>
#include "util/common.h"

double Hmm::getTransProb(Transition *trans) {
    return _transition.get(trans->from()->state(), trans->to()->state());
}

double Hmm::getEmitProb(Transition *trans) {
    return _emission.get(trans->to()->state(), trans->obs());
}

// 计算前向概率值
void Hmm::forward() {
    TimeSlot* t0 = _timeSlots[0];
    ModelNode* init = (*t0)[0];
    init->logAlpha(0);

    for (uint32_t t = 1; t < _timeSlots.size(); t++) {
        TimeSlot* ts = _timeSlots[t];
        for (TimeSlot::iterator it = ts->begin(); it != ts->end(); ++it) {
            std::vector<Transition*>& ins = (*it)->ins();
            std::vector<double> logProbs(ins.size());

            for (uint32_t i = 0; i < ins.size(); i++) {
                Transition* trans = ins[i];
                double logProb = trans->from()->logAlpha() + getTransProb(trans) + getEmitProb(trans);
                logProbs[i] = logProb;
            }
            (*it)->logAlpha(sumLogProb(logProbs));
        }
    }
}
