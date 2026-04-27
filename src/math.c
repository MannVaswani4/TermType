#include "math.h"

int my_multiply(int a, int b) {
    int neg = 0;
    if (a < 0) { a = -a; neg = !neg; }
    if (b < 0) { b = -b; neg = !neg; }
    int result = 0;
    while (b > 0) {
        if (b & 1) {
            result += a;
        }
        a <<= 1;
        b >>= 1;
    }
    return neg ? -result : result;
}
int my_divide(int a, int b) {
    if (b == 0) return 0;

    int neg = 0;

    if (a < 0) { a = -a; neg = !neg; }
    if (b < 0) { b = -b; neg = !neg; }

    int q = 0;
    for (int i = 31; i >= 0; i--) {
        if ((a >> i) >= b) {
            q += (1 << i);
            a -= (b << i);
        }
    }
    return neg ? -q : q;
}