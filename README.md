# pack

A minimal DSL for creating binary files.

I created this small assembler-like language because I wanted an easy way to
create binary-encoded files without having to write custom scripts every time
or hunt for an existing tool that did what I wanted.

You're welcome to use it if you like.

## Usage

Pack is used like a compiler. You give it source code as input, and it emits
the corresponding binary as an output. The one difference from a typical
compiler is that it accepts its input from `stdin` and writes its output to
`stdout`, rather than accepting command-line arguments.

For example, if you have a pack source file called `data.txt`, and want to
encode it into a file called `data.bin`, you can run

```shell
pack < data.txt > data.bin
```

## Syntax

This language is designed to be very simple. I'll mostly document it here by
example.

### Example 1

```
u32 56
u32 -12
u8 1 2 3 4 5 6 7 8
```

This pack source will output a 16-byte binary file containing, in order:

1. The number 56 as a little-endian 32-bit unsigned integer (4 bytes)
2. The number -12 as a little-endian 32-bit unsigned integer (4 bytes, two's
   complement)
3. The numbers 1 through 8, each as a single byte (8 bytes)

### Example 2

```
bi16 -6
u16 0
f32
4.5 9.6 3.14
2.71 1.81 -3
```

This pack source will output a 28-byte binary file containing, in order:

1. The number 6 as a big-endian 16-bit signed integer (2 bytes, two's complement)
2. The number 0 as a little-endian 16-bit unsigned integer (2 null bytes).
3. Six little-endian IEEE 754 32-bit floating point numbers (24 bytes).

### Comments and whitespace

You can put comments in your source using `#`. Anything from a `#` character to
the end of the line will be ignored, like in Python, Bash, or YAML.

Tokens must be separated by at least one whitespace character. Besides that,
whitespace is ignored.

## Installation

You're on your own here. I built this project usng `clang` and `make`, so if
you have those installed, it-will-probably-just-work. Shouldn't be too hard to
build it yourself using other tools, though.

## License

This software is provided under the [MIT License](LICENSE). Go wild.
