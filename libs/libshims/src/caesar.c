#include "shims.h"
#include <string.h>

int caesar_decrypt(const uint8_t *input, size_t input_len, uint64_t key, uint8_t *output) {
    uint8_t shift = (uint8_t)(key % 256);
    for (size_t i = 0; i < input_len; i++) {
        output[i] = input[i] - shift; // Decrypting rotation is subtracting
    }
    return 0;
}
