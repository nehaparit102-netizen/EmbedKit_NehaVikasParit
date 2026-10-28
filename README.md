# EmbedKit_NehaVikasParit

**Author:** Neha Vikas Parit

## Module: Ring Buffer (`ringbuf.c`)

A fixed-capacity, 8-byte circular FIFO buffer for `uint8_t` data. It supports initialization, byte writes, byte reads, byte count, and full/empty checks. Writes to a full buffer and reads from an empty buffer fail safely.

The implementation uses bitwise AND (`& (BUFFER_SIZE - 1U)`) for index wrap-around. This works because the buffer size is a power of two (8).

## Requirements

- GCC compiler
- Standard C (C99)
- No external libraries or embedded hardware

## Build

```bash
gcc -Wall -std=c99 ringbuf.c -o ringbuf
```

## Run

On Linux/macOS/WSL:

```bash
./ringbuf
```

On Windows (MinGW):

```bash
ringbuf.exe
```

## Demonstration

The program fills the buffer, attempts an extra write, reads three bytes, writes three more bytes to the freed slots, drains the buffer, and attempts a read from the empty buffer.
