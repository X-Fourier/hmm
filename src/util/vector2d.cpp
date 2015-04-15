#include "vector2d.h"
#include "common.h"

const double SMOOTHEDZEROCOUNT = -40;

double vector2d::smoothedZeroCount() const {
    return _smoothedZeroCount;
}

// 从向量中读取数据
double vector2d::get(unsigned long event) const {
    vector2d::const_iterator it = find(event);
    if (it == end()) {
        return smoothedZeroCount();
    } else {
        return it->second;
    }
}

// 往向量中添加数据
void vector2d::add(unsigned long event, double count) {
    vector2d::iterator it = find(event);
    if (it == end()) {
        (*this)[event] += count;
    } else {
        it->second = sumLogProb(it->second, count);
    }
}

vector2d::vector2d() {
    _smoothedZeroCount = SMOOTHEDZEROCOUNT;
}

vector2d::~vector2d() {

}
