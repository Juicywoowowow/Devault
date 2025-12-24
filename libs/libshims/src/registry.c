#include "shims.h"
#include <string.h>

// Forward declarations of shim functions
int xor_decrypt(const uint8_t *input, size_t input_len, uint64_t key, uint8_t *output);
int caesar_decrypt(const uint8_t *input, size_t input_len, uint64_t key, uint8_t *output);

static const Shim registry[] = {
    {"xor", xor_decrypt, 255},
    {"caesar", caesar_decrypt, 255},
    {NULL, NULL, 0}
};

const Shim* get_shim(const char *name) {
    for (int i = 0; registry[i].name != NULL; i++) {
        if (strcmp(registry[i].name, name) == 0) {
            return &registry[i];
        }
    }
    return NULL;
}

const Shim** get_all_shims(size_t *count) {
    // For now, not strictly needed by the prompt, but good to have for "list" command
    // Keeping it simple.
    (void)count;
    return NULL;
}
