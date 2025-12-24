#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "shims.h"
#include "devault.h"

#define MAX_BUFFER 1024 * 1024 // 1MB input buffer limit for this prototype

void print_usage(const char *prog_name) {
    printf("Usage: %s [options]\n", prog_name);
    printf("Options:\n");
    printf("  --input <file>      Path to encrypted input file\n");
    printf("  --algo <name>       Algorithm shim to use (e.g., xor, caesar)\n");
    printf("  --start <int>       Start key (default: 0)\n");
    printf("  --end <int>         End key (default: 255)\n");
    printf("  --buffer-limit <n>  Max number of .dec files to keep (default: 100)\n");
    printf("  --out-dir <dir>     Output directory (default: .devault)\n");
    printf("  --help              Show this help message\n");
}

int main(int argc, char *argv[]) {
    char *input_file = NULL;
    char *algo_name = NULL;
    char *out_dir = ".devault";
    uint64_t start_key = 0;
    uint64_t end_key = 255;
    int buffer_limit = 100;

    static struct option long_options[] = {
        {"input", required_argument, 0, 'i'},
        {"algo", required_argument, 0, 'a'},
        {"start", required_argument, 0, 's'},
        {"end", required_argument, 0, 'e'},
        {"buffer-limit", required_argument, 0, 'b'},
        {"out-dir", required_argument, 0, 'd'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "i:a:s:e:b:d:h", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'i': input_file = optarg; break;
            case 'a': algo_name = optarg; break;
            case 's': start_key = strtoull(optarg, NULL, 0); break;
            case 'e': end_key = strtoull(optarg, NULL, 0); break;
            case 'b': buffer_limit = atoi(optarg); break;
            case 'd': out_dir = optarg; break;
            case 'h': print_usage(argv[0]); return 0;
            default: print_usage(argv[0]); return 1;
        }
    }

    if (!input_file || !algo_name) {
        fprintf(stderr, "Error: --input and --algo are required.\n");
        print_usage(argv[0]);
        return 1;
    }

    // Initialize core
    if (devault_init(out_dir) != 0) {
        return 1;
    }

    // Get shim
    const Shim *shim = get_shim(algo_name);
    if (!shim) {
        fprintf(stderr, "Error: Unknown algorithm '%s'\n", algo_name);
        return 1;
    }

    // Read input file
    FILE *f = fopen(input_file, "rb");
    if (!f) {
        perror("Failed to open input file");
        return 1;
    }
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (fsize > MAX_BUFFER) {
        fprintf(stderr, "Error: Input file too large (>1MB)\n");
        fclose(f);
        return 1;
    }

    uint8_t *input_buf = malloc(fsize);
    uint8_t *output_buf = malloc(fsize);
    if (!input_buf || !output_buf) {
        perror("Memory allocation failed");
        fclose(f);
        return 1;
    }

    if (fread(input_buf, 1, fsize, f) != (size_t)fsize) {
        perror("Failed to read input file");
        fclose(f);
        free(input_buf);
        free(output_buf);
        return 1;
    }
    fclose(f);

    printf("Starting attack using '%s' from key %lu to %lu...\n", algo_name, start_key, end_key);

    for (uint64_t key = start_key; key <= end_key; key++) {
        char unique_id[256];
        snprintf(unique_id, sizeof(unique_id), "%s_key_%lu", algo_name, key);

        if (devault_is_attempted(unique_id)) {
            // printf("Skipping %s (already attempted)\n", unique_id);
            continue;
        }

        if (shim->func(input_buf, fsize, key, output_buf) == 0) {
            devault_save_dec(unique_id, output_buf, fsize);
            devault_log_attempt(unique_id);

            // Enforce buffer limit (maybe not every single iteration for performance, but good enough for now)
            devault_enforce_buffer(buffer_limit);
        } else {
            fprintf(stderr, "Failed to decrypt with key %lu\n", key);
        }
    }

    printf("Attack range complete.\n");

    free(input_buf);
    free(output_buf);
    return 0;
}
