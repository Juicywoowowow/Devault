#ifndef SHIMS_H
#define SHIMS_H

#include <stddef.h>
#include <stdint.h>

// Definition for a decryption function pointer
// input: pointer to input buffer
// input_len: length of input buffer
// key: iteration key (simple integer for now to support 0..N iteration)
// output: pointer to output buffer (must be allocated by caller, usually same size as input)
// Returns: 0 on success, non-zero on error
typedef int (*decrypt_func)(const uint8_t *input, size_t input_len, uint64_t key, uint8_t *output);

typedef struct {
    const char *name;
    decrypt_func func;
    uint64_t default_max_key; // Hint for the max key range (e.g., 255 for byte XOR)
} Shim;

// Registry functions
const Shim* get_shim(const char *name);
const Shim** get_all_shims(size_t *count);

#endif
