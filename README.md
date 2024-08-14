# LLVMBrewer - Helper for LLVM

## What is LLVMBrewer

LLVMBrewer is a simple C++ library I wrote in one day. It's invaluable in cases where you don't want to write an entire
language frontend, plus a LLVM backend which both can be very time-consuming. LLVMBrewer takes a lot of that work from
you, enabling you to focus on building the language you ever wanted to build. So if you want, for example, write the toy
language "Kaleidoscope" from the LLVM tutorial. You would have to create four — yes, only __four__ — parts: the parsing
functions and structs for the *def*, *extern* and *if* keyword. I did this myself, and it ended up taking about 260
lines of code. And it works. You can check the example out; you can find it inside the *test* directory, together with
the fibonacci example. But the list of things goes on: for example, you are also able to write a fully typed language,
in contrast to the "Kaleidoscope" language.

## Required Knowledge

You need to have some experience in the LLVM C API, because this library does not wrap around LLVM, it provides a
starting point for a programming language. I used the LLVM C API because it is more easy and straight forward to use
than LLVM in C++. This is mostly for the point of me struggling to link the damn LLVM lib files against my cmake
target... yeah, no, Windows... I bet you if I just did it on Linux, there would be no issues at all. So back to reality:
what knowledge do you need to use this? You need to know C/C++, LLVM, and a bit of how to parse statements and
expressions. But even if you don't have any of this knowledge, don't worry. You can learn it. E.g., check out the
*test* project, watch YouTube videos about people making programming languages, and read the LLVM documentation.

## Usage

The first step is to create a parser instance. You pass an input stream and a filename (for debugging purposes). On your
instance, you can register parsing functions for keywords; those get called whenever the parser stops on the given
keyword. Statement parsing functions are useful for function declaration parsing, whilst expression parsing is called
for operands.

Step two is to create a builder instance. The builder is the thing that interacts with LLVM, so it is part of the
backend of your language. The only thing you have to pass it is a name for the module, which is usually the input
filename.

After you have an instance to both of them, you "iterate" over the parser, i.e., you loop over it
until ```Parser::AtEOF()``` returns true. Inside the loop you want to call ```Statement::GenIR()``` on the pointer
returned by ```Parser::Parse()```, passing the builder. After the loop, you have to close the builder by
calling ```Builder::Close()```, and emitting it into a file by calling ```Builder::EmitToFile```.

## Motivation for this project

I started this project because I work on programming languages very often, just for fun. But something I realized was me
rewriting the same stuff every time, over and over again. Now I have a starting point, so I don't have to do this
anymore.

## License

I don't know why I would ever need to license this. Just take it and use it, I don't care. The only thing I want is that
IF someone uses this project, just tell me, so I can be proud that someone used something I made. Thanks <3
