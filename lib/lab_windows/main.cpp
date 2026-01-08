#include <iostream>
#include "Number.h"
#include "Vector.h"

int main() {
   
    Number a = makeNumber(5);
    Number b = makeNumber(3);
    Number c = a * b + ONE;
    std::cout << "a = 5 " << std::endl;
    std::cout << "b = 3 " << std::endl;

    std::cout << "a * b + 1 = " << c.get() << std::endl;

  
    Vector v1 = VEC_ONE;
    Vector v2(makeNumber(2), makeNumber(3));
    Vector v3 = v1 + v2;

    std::cout << "v1 = "; v1.print(); std::cout << std::endl;
    std::cout << "v2 = "; v2.print(); std::cout << std::endl;

    std::cout << "v3 = v1 + v2 = "; v3.print(); std::cout << std::endl;

    std::cout << "Radius of v3 = " << v3.radius().get() << std::endl;
    std::cout << "Angle of v3 = " << v3.angle().get() << std::endl;

    return 0;
}