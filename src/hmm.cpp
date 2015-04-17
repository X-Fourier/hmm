#include "hmm.h"
#include <stdint.h>
#include "util/common.h"
#include <stdio.h>
#include <math.h>
#include <algorithm>

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

// 计算后向概率值
void Hmm::backward() {
    int T = _timeSlots.size() - 1;
    if (T<1) {
        return;
    }

    for (int t = T; t>=0; t--) {
        TimeSlot* ts = _timeSlots[t];
        for (TimeSlot::iterator it = ts->begin(); it != ts->end(); ++it) {
            ModelNode* node = *it;
            if (t==T) {
                node->logBeta(0);
            } else {
                std::vector<Transition*>& outs = node->outs();
                std::vector<double> logProbs(outs.size());
                for (uint32_t i = 0; i<outs.size(); i++) {
                    Transition* trans = outs[i];
                    double logProb = trans->to()->logBeta() + getTransProb(trans) + getEmitProb(trans);
                    logProbs[i] = logProb;
                }
                node->logBeta(sumLogProb(logProbs));
            }
        }
    }
}

// 计算隐含状态序列的viterbi算法
double Hmm::viterbi(std::vector<Transition*>& path) {
    TimeSlot* ts = _timeSlots[0];
    ModelNode* init = (*ts)[0];
    init->logAlpha(0);

    for (uint32_t t = 1; t < _timeSlots.size(); t++) {
        ts = _timeSlots[t];
        for (TimeSlot::iterator it = ts->begin(); it != ts->end(); ++it) {
            ModelNode* node = *it;
            std::vector<Transition*>& ins = node->ins();
            Transition* bestTrans = NULL;
            double maxProb = log(0.0);
            for (uint32_t i = 0; i < ins.size(); i++) {
                Transition* trans = ins[i];
                double logProb = trans->from()->logAlpha() + getTransProb(trans) + getEmitProb(trans);
                if (NULL == bestTrans || maxProb < logProb) {
                    bestTrans = trans;
                    maxProb = logProb;
                }
                node->logAlpha(maxProb);
                node->psi(bestTrans);
            }
        }
    }

    ts = _timeSlots[_timeSlots.size()-1];
    ModelNode* best = NULL;
    for (TimeSlot::iterator it = ts->begin(); it != ts->end(); ++it) {
        ModelNode* node = *it;
        if (NULL == best || best->logAlpha() < node->logAlpha()) {
            best = node;
        }
    }

    for (ModelNode* nd = best; nd; ) {
        if (nd->psi()) {
            path.push_back(nd->psi());
            nd = nd->psi()->from();
        } else {
            nd = NULL;
        }
    }

    std::reverse(path.begin(), path.end());
    return best->logAlpha();
}
