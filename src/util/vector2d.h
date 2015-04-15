#ifndef _VECTOR2D_H_
#define _VECTOR2D_H_

#include <map>

class vector2d : public std::map<unsigned long, double> {
public:
    vector2d();
    
    double smoothedZeroCount() const;

    // 从向量中读取数据
    double get(unsigned long event) const;

    // 往向量中添加数据
    void add(unsigned long event, double count);

    ~vector2d();

private:
    double _smoothedZeroCount;
};

#endif
