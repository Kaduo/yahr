#include "utils.h"

// TODO ;  pas très utile, delete me...

unsigned powi(unsigned base, unsigned exp) {
    unsigned res = 1;
    while (exp) {
        if (exp%2) {
            res *= base;
        }
        exp /= 2;
        base *= base;
    }
    return res;
}

unsigned rawBytesToUnsignedInt(char *bytes, unsigned nbBytes) {
    unsigned res = 0;
    unsigned base = 1;
    for (int i =0; i < nbBytes; i++) {
        res += base*bytes[i];
        base *= 2;
    }
    return res;
}