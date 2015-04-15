#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "vector2d.h"
#include <map>
#include <set>

typedef std::set<unsigned long> ULSet;

class matrix2d : public std::map<unsigned long, vector2d*> {
public:
    matrix2d();

    // 从矩阵中读取数据
    double get(unsigned long context, unsigned long event) const;

    // 往矩阵中添加数据
    void add(unsigned long context, unsigned long event, double count);

    // 返回context集合
    ULSet* getCntx(unsigned long event) const;

    // 清除矩阵中的所有数据
    void clear();

    ~matrix2d();

private:
    std::map<unsigned long, ULSet*> _possCntxs;
    vector2d _backoff;

};

#endif
