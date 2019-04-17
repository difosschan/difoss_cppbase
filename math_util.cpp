#include "math_util.h"
#include <cstdlib>
#include <ctime>

void MathUtil::RandomInit() {
    srandom(std::time(0));
}

unsigned int MathUtil::Random(unsigned int max_value) {
    return (unsigned int) (max_value * ((float(rand())) / RAND_MAX));
}
