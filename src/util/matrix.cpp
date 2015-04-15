#include "matrix.h"
#include <stdio.h>

// 从矩阵中读取数据
double matrix2d::get(unsigned long context, unsigned long event) const {
    matrix2d::const_iterator it = find(context);
    if (it == end()) {
        return _backoff.get(event);
    } else {
        return it->second->get(event);
    }
}

// 往矩阵中添加数据
void matrix2d::add(unsigned long context, unsigned long event, double count) {
    matrix2d::iterator it = find(context);
    vector2d* entry = NULL;
    if (it == end()) {
        entry = new vector2d;
        (*this)[context] = entry;
    } else {
        entry = it->second;
    }
    entry->add(event, count);

    std::map<unsigned long, ULSet*>::iterator itr = _possCntxs.find(event);
    ULSet* possCntx = NULL;
    if (itr == _possCntxs.end()) {
        possCntx = new ULSet;
        _possCntxs[event] = possCntx;
    } else {
        possCntx = itr->second;
    }
    possCntx->insert(context);
}

// 返回context集合
ULSet* matrix2d::getCntx(unsigned long event) const {
    std::map<unsigned long, ULSet*>::const_iterator it = _possCntxs.find(event);
    if (it == _possCntxs.end()) {
        return NULL;
    } else {
        return it->second;
    }
}

// 清除矩阵中的所有数据
void matrix2d::clear() {
    for (matrix2d::iterator it = begin(); it != end(); ++it) {
        delete it->second;
    }
    std::map<unsigned long, vector2d*>::clear();
    std::map<unsigned long, ULSet*>::iterator itr = _possCntxs.begin();
    for (; itr != _possCntxs.end(); ++itr) {
        delete itr->second;
    }
    _possCntxs.clear();
    _backoff.clear();
}

matrix2d::matrix2d() {

}

matrix2d::~matrix2d() {
    this->clear();
}
