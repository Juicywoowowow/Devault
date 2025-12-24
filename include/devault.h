#ifndef DEVAULT_H
#define DEVAULT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Initialize the devault workspace (create .devault directory)
int devault_init(const char *output_dir);

// Check if a specific attempt has already been made
// unique_id: a string representing the attempt, e.g., "xor:42"
bool devault_is_attempted(const char *unique_id);

// Record an attempt in the history log
int devault_log_attempt(const char *unique_id);

// Save a .dec file
// content: decrypted data
// len: length of data
// unique_id: used to generate filename
int devault_save_dec(const char *unique_id, const uint8_t *content, size_t len);

// Enforce rolling buffer limit
// limit: max number of .dec files allowed
int devault_enforce_buffer(int limit);

#endif
