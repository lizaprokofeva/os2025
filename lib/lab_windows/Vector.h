#pragma once
#include "Number.h"
#include <cmath>
#include <iostream>
#ifdef VECTOR_EXPORTS
#define VECTOR_API __declspec(dllexport)
#else
#define VECTOR_API __declspec(dllimport)
#endif

class VECTOR_API Vector {
    Number x, y;
public:
    Vector(Number x = ZERO, Number y = ZERO) : x(x), y(y) {}

    Number radius() const { return makeNumber(std::sqrt(x.get() * x.get() + y.get() * y.get())); }
    Number angle() const { return makeNumber(std::atan2(y.get(), x.get())); }

    Vector operator+(const Vector& other) const {
        return Vector(x + other.x, y + other.y);
    }

    void print() const {
        std::cout << "(" << x.get() << ", " << y.get() << ")";
    }
};

extern VECTOR_API const Vector VEC_ZERO;
extern VECTOR_API const Vector VEC_ONE;