#include "common.h"
#include <math.h>

double sumLogProb(std::vector<double>& logprobs) {
    double max = 0;
    unsigned int i;
    for (i = 0; i<logprobs.size(); i++) {
        if (i == 0 || logprobs[i] > max) {
            max = logprobs[i];
        }
    }
    if (isinf(max)) {
        return max;
    }

    double p = 0;
    for (i = 0; i<logprobs.size(); i++) {
        p += exp(logprobs[i]-max);
    }
    return max + log(p);
}

double sumLogProb(double logprob1, double logprob2) {
    if (isinf(logprob1) && isinf(logprob2)) {
        return logprob1;
    }
    if (logprob1 > logprob2) {
        return logprob1 + log(1+exp(logprob2-logprob1));
    } else {
        return logprob2 + log(1+exp(logprob1-logprob2));
    }
}
