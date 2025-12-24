# devault

devault is a decryption framework designed to perform iterative attacks on lightweight encryption. It is implemented in pure C and follows a modular monorepo structure.

## Features

- **Monorepo Structure**: Logic is separated into core libraries (`libs/libdevault`, `libs/libshims`) and the application (`apps/cli`).
- **Brigade of Shims**: Modular decryption algorithms. Currently supports XOR and Caesar cipher.
- **Smart History**: Tracks all attempted keys in a `history.log` to prevent redundant computations, even if the output files are deleted.
- **Rolling Buffer**: Manages storage by limiting the number of generated `.dec` files, automatically deleting the oldest ones to respect the limit.

## Build Instructions

To build the project, run:

```bash
make
```

This will compile the libraries and the `devault` executable in the root directory.

To clean the build artifacts:

```bash
make clean
```

## Usage

The `devault` tool requires several flags to operate. It does not assume any default algorithms.

```bash
./devault --input <file> --algo <name> [options]
```

### Options

- `--input <file>`: Path to the encrypted input file. (Required)
- `--algo <name>`: The decryption algorithm to use. Available: `xor`, `caesar`. (Required)
- `--start <int>`: The starting key for the iteration range. Default: 0.
- `--end <int>`: The ending key for the iteration range. Default: 255.
- `--buffer-limit <n>`: The maximum number of `.dec` files to keep in the output directory. Default: 100.
- `--out-dir <dir>`: The directory to store output files and logs. Default: `.devault`.
- `--help`: Display the help message.

### Example

To attack a file named `secret.enc` using XOR with keys from 0 to 50, keeping only the last 10 result files:

```bash
./devault --input secret.enc --algo xor --start 0 --end 50 --buffer-limit 10
```

## Architecture

### Directory Structure

- `apps/cli`: Contains the main command-line interface application.
- `libs/libdevault`: Core logic for file management, logging, and buffer enforcement.
- `libs/libshims`: Implementation of decryption algorithms.
- `include`: Shared header files.

### Output

The tool creates a `.devault` directory (configurable) containing:
- `history.log`: A text file recording every unique attempt (algorithm + key).
- `*.dec`: The decrypted output files. These are rotated based on the `--buffer-limit`.
