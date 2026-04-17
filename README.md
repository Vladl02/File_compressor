# File Compressor (UTF-8 Huffman)

This project is a C++ file compressor/decompressor for UTF-8 text.
It builds a frequency table per UTF-8 character, creates a Huffman coding tree, writes a compact binary format, then reconstructs the original text.

## What It Does

- `compress.cpp`
  - Reads `file.txt` as binary.
  - Counts UTF-8 character frequencies.
  - Builds a Huffman tree.
  - Writes compressed output to `fileBin.bin`.
- `decompress.cpp`
  - Reads `fileBin.bin`.
  - Rebuilds the encoding table from the header.
  - Decodes bitstream data.
  - Writes decompressed output to `decompresedFile.txt`.

## Project Files

- `compress.cpp`: main compressor implementation.
- `decompress.cpp`: main decompressor implementation.
- `pureBinarytext.cpp`: helper for visualizing bit-level output.
- `verify.cpp`: helper for output verification.
- `file.txt`: input file used for compression.

## Binary File Format

The compressed file (`fileBin.bin`) has this structure:

1. Header entries repeated:
   - `1 byte`: UTF-8 character byte length
   - `N bytes`: UTF-8 character bytes
   - `1 byte`: code length
   - `M bytes`: code bits as ASCII (`'0'` / `'1'`)
2. Header delimiter: `###\n`
3. Encoded binary payload bytes
4. `1 byte`: number of valid bits in the last payload byte


## Key Implementation Aspects

- UTF-8 aware character grouping (`bytesPerCharacter`) to treat multibyte characters as single symbols.
- Huffman tree generation using frequency-sorted node merges.
- Compact header + payload layout that allows decoding without external metadata.
- Windows newline handling fix in decompression output by writing in binary mode.


## User Guide

The compiler should have the following flags set:

- Visual Studio compiler: `/utf-8 /DUNICODE`
- MinGW compiler: `-finput-charset=utf-8 -DUNICODE`

To compress: insert the content that you want to compress in `file.txt` and run the `compress` executable.

To decompress: make sure the compressed binary file `fileBin.bin` exists, run the `decompress` executable, and check `decompresedFile.txt`.
