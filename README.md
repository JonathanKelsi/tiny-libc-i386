# mini-libc-ctf

A tiny libc-style implementation for **Linux i386**, designed for CTF challenges.

This project avoids glibc entirely and provides a minimal set of
syscall wrappers, string utilities, and a custom `_start` entrypoint.

## Features

- Linux **i386** syscalls via `int 0x80`
- Custom `_start` / entrypoint (no CRT)
- Minimal syscall wrappers:
  - `read`, `write`, `open`, `close`
  - `fork`, `execve`, `waitpid`
  - `pipe`, `dup2`, `mmap2`
- Small string / utility functions:
  - `strlen`, `strcmp`, `strcat`
  - `atoi`, `itoa`
  - `puts`
- Simple `popen` helper for shell execution
- Designed to be compiled with `-nostdlib`

## Non-goals

No support for:
- `errno`
- threads
- signals
- locale
- wide characters
- POSIX compliance

If you need those — use glibc or musl.

## Usage

### Example

```c
#include "libc.h"

int main(int argc, char **argv)
{
    puts("hello world");
    return 0;
}
```

### Build

```sh
gcc -m32 -nostdlib libc.c example.c -o example
```
