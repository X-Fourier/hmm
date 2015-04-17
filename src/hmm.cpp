#include "hmm.h"
#include <stdint.h>
#include "util/common.h"
#include <stdio.h>
#include <math.h>
#include <algorithm>

Hmm::Hmm() {

}

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

// 将某个观察值加入到序列中
void Hmm::addObs(unsigned long obs) {
    std::vector<unsigned long> stateIds;
    ULSet* cntx = _emission.getCntx(obs);

    if (NULL == cntx) {
        for (matrix2d::iterator it = _emission.begin(); it != _emission.end(); ++it) {
            stateIds.push_back(it->first);
        }
    } else {
        for (ULSet::iterator it = cntx->begin(); it != cntx->end(); ++it) {
            stateIds.push_back(*it);
        }
    }

    if (_timeSlots.empty()) {
        TimeSlot* t0 = new TimeSlot;
        t0->push_back(new ModelNode(0, _initState, this));
        _timeSlots.push_back(t0);
    }

    TimeSlot* ts = new TimeSlot;
    int time = _timeSlots.size();
    for (uint32_t i = 0; i < stateIds.size(); i++) {
        ModelNode* node = new ModelNode(time, stateIds[i], this);
        ts->push_back(node);
        TimeSlot* pre = _timeSlots[time-1];
        for (TimeSlot::iterator it = pre->begin(); it != pre->end(); ++it) {
            ULSet* possCntx = _transition.getCntx(node->state());
            if (possCntx && possCntx->find((*it)->state()) != possCntx->end()) {
                new Transition(*it, node, obs);
            }
        }
    }
    _timeSlots.push_back(ts);
}

// 计算观察序列的概率值
double Hmm::obsProb() {
    if (_timeSlots.size() < 1) {
        return 1;
    }
    forward();
    TimeSlot* last = _timeSlots[_timeSlots.size()];
    std::vector<double> alphaT;
    for (TimeSlot::iterator it = last->begin(); it != last->end(); ++it) {
        alphaT.push_back((*it)->logAlpha());
    }
    return sumLogProb(alphaT);
}

// 根据观察序列对模型参数进行设置
double Hmm::setModelParams(ModelParam& counts) {
    double pOfObs = obsProb();
    backward();

    for (uint32_t t = 0; t < _timeSlots.size(); t++) {
        TimeSlot* ts = _timeSlots[t];
        TimeSlot::iterator it = ts->begin();
        std::vector<double> logprobs;
        for (; it != ts->end(); ++it) {
            logprobs.push_back((*it)->logAlpha() + (*it)->logBeta());
        }
        double sum = sumLogProb(logprobs);

        for (it = ts->begin(); it != ts->end(); ++it) {
            ModelNode* node = *it;
            double stateCount = node->logAlpha() + node->logBeta() - sum;
            counts.stateCount().add(node->state(), stateCount);

            std::vector<Transition*>& ins = node->ins();
            uint32_t k;
            for (k = 0; k < ins.size(); k++) {
                Transition* trans = ins[k];
                ModelNode* from = trans->from();
                double transCount = from->logAlpha() + getTransProb(trans) + getEmitProb(trans) + node->logBeta() - pOfObs;
                counts.emitCount().add(node->state(), trans->obs(), transCount);
            }

            std::vector<Transition*>& outs = node->outs();
            for (k = 0; k < outs.size(); k++) {
                Transition* trans = outs[k];
                ModelNode* to = trans->to();
                double transCount = node->logAlpha() + getTransProb(trans) + getEmitProb(trans) + to->logBeta() - pOfObs;
                counts.transCount().add(node->state(), to->state(), transCount);
            }
        }
    }

    return pOfObs;
}

// 迭代过程中更新模型的参数
void Hmm::updateParams(ModelParam& counts) {
    _transition.clear();
    _emission.clear();
    
    matrix2d::iterator it = counts.transCount().begin();
    for (; it != counts.transCount().end(); ++it) {
        unsigned long from = it->first;
        double fromCount = counts.stateCount().get(from);
        vector2d& cnts = *it->second;
        for (vector2d::iterator itr = cnts.begin(); itr != cnts.end(); ++itr) {
            _transition.add(from, itr->first, itr->second-fromCount);
        }
    }

    for (it = counts.emitCount().begin(); it != counts.emitCount().end(); ++it) {
        unsigned long state = it->first;
        double stateCount = counts.stateCount().get(state);
        vector2d& cnts = *it->second;
        for (vector2d::iterator itr = cnts.begin(); itr != cnts.end(); ++itr) {
            _emission.add(state, itr->first, itr->second-stateCount);
        }
    }
}

// 根据观察序列训练模型的EM算法
void Hmm::baumWelch(const std::vector<std::vector<unsigned long>*>& seqs, int maxIters) {
    double preTotalLogProb = 0;
    for (int k = 0; k < maxIters; k++) {
        ModelParam counts;
        double totalLogProb = 0;
        for (uint32_t i = 0; i < seqs.size(); i++) {
            std::vector<unsigned long>& seq = *seqs[i];
            for (uint32_t j = 0; j < seq.size(); j++) {
                addObs(seq[i]);
            }
            totalLogProb += setModelParams(counts);
            reset();
        }
        if (preTotalLogProb != 0 && (totalLogProb - preTotalLogProb < 1)) {
            break;
        } else {
            preTotalLogProb = totalLogProb;
        }
        updateParams(counts);
    }
}

void Hmm::reset() {
    for (uint32_t t = 0; t < _timeSlots.size(); t++) {
        delete _timeSlots[t];
    }
    _timeSlots.clear();
}

Hmm::~Hmm() {
    reset();
}
