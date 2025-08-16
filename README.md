# Two-Pass Assembler (C)

A C-based two-pass assembler for a custom assembly language. It preprocesses macros, validates and parses source, resolves symbols/addresses, and emits standard output files.

## What it does
- Reads assembly source files, handles macros, validates syntax, resolves labels/addresses, and writes output files.
- Language basics: 16 opcodes, registers `r0`–`r7`, addressing modes (immediate `#`, direct label, matrix, register). Memory starts at 100.

## Build
```sh
make
```
Clean:
```sh
make clean
```

## Run
Pass file base names without the `.as` extension (the program appends it):
```sh
./assembler file1 file2
# Reads file1.as, file2.as
```

## Outputs
- `file.am` — macro-expanded source
- `file.ob` — object code/data
- `file.ent` — only if `.entry` exists
- `file.ext` — only if `.extern` exists

## Example
```sh
./assembler valid_example_1_macro ps
```

See `valid_example_*.as` and `non_valid_example_*.as` for reference inputs.
