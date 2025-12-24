#include "shims.h"
#include <string.h>

int xor_decrypt(const uint8_t *input, size_t input_len, uint64_t key, uint8_t *output) {
    uint8_t k = (uint8_t)key;
    for (size_t i = 0; i < input_len; i++) {
        output[i] = input[i] ^ k;
    }
    return 0;
}
