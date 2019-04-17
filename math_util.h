#pragma once

namespace MathUtil {

    template <class T_> T_ Max(const T_& a, const T_& b) {
        return (a > b) ? a : b;
    }
    
    template <class T_> T_& MaxRef(T_& a, T_& b) {
        return (a > b) ? a : b;
    }
    
    template <class T_> T_ Min(const T_& a, const T_& b) {
        return (a < b) ? a : b;
    }
    
    template <class T_> T_& MinRef(T_& a, T_& b) {
        return (a < b) ? a : b;
    }
    
    void RandomInit();
    
    // generate a random unsigned integer betweem 0 and max_value
    unsigned int Random(unsigned int max_value);
}
