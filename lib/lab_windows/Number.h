#pragma once
#include <iostream>
#define NUMBER_API


class NUMBER_API Number {
    double value;
public:
    Number(double v = 0) : value(v) {}

    Number operator+(const Number& other) const { return Number(value + other.value); }
    Number operator-(const Number& other) const { return Number(value - other.value); }
    Number operator*(const Number& other) const { return Number(value * other.value); }
    Number operator/(const Number& other) const { return Number(value / other.value); }

    double get() const { return value; }
};

extern const Number ZERO;
extern const Number ONE;

Number makeNumber(double v);