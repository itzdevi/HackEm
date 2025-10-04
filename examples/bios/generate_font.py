#!/usr/bin/env python3
# generate_font.py
# Parse font8x8_basic.h and emit Hack assembly with left/right 16-bit words
# Usage:
#   python3 generate_font.py font8x8_basic.h --mode data   > font_data.asm
#   python3 generate_font.py font8x8_basic.h --mode write  > font_write.asm

import re
import sys
import argparse
from pathlib import Path

FONT_START_ADDR = 128
ASCII_START = 0x20
ASCII_END = 0x7E

def parse_font_file(path):
    txt = Path(path).read_text(encoding="utf-8", errors="ignore")
    # find all {...} blocks (C initializer arrays)
    raw_blocks = re.findall(r"\{([^}]*)\}", txt, flags=re.MULTILINE)
    # convert each block to list of hex bytes (0xNN)
    glyphs = []
    for block in raw_blocks:
        hex_vals = re.findall(r"0x[0-9A-Fa-f]{1,2}", block)
        if not hex_vals:
            # fallback: also accept decimal numbers just in case
            hex_vals = re.findall(r"\b\d+\b", block)
        row_vals = [int(h, 16) for h in hex_vals]
        glyphs.append(row_vals)
    return glyphs

def emit_header_data_mode():
    # Jump over font data (so CPU doesn't execute data as code)
    print("// Generated Hack font data (data mode) — font words stored as simple A-instructions")
    print("@SKIP_FONT")
    print("0;JMP\n")
    print("// FONT_DATA start")
    print("(FONT_DATA)")

def emit_header_write_mode():
    print("// Generated Hack font data (write mode) — emits write pattern to RAM starting at R0")
    print("// This file will write into RAM[R0] sequentially. R0 is initialized to {}.".format(FONT_START_ADDR))
    print("")

def emit_footer_data_mode():
    print("\n(SKIP_FONT)")
    print("    // execution continues here")
    print("")

def emit_value_data(val, comment=None):
    # emit a single A-instruction containing the 16-bit value
    # decimal immediate is safer for many Hack assemblers
    if comment:
        print(f"// {comment}")
    print(f"@{val}")

def emit_value_write(val, comment=None):
    # emit the 6-line write pattern that writes 'val' into RAM[R0++].
    if comment:
        print(f"// {comment}")
    print(f"@{val}")
    print("D=A")
    print("@R0")
    print("A=M")
    print("M=D")
    print("@R0")
    print("M=M+1")
    print("")

def main():
    ap = argparse.ArgumentParser(description="Generate Hack ASM from font8x8_basic.h (left/right words)")
    ap.add_argument("fontfile", help="path to font8x8_basic.h")
    ap.add_argument("--mode", choices=("data","write"), default="data",
                    help="data = compact ROM-only A-instructions (you must write a loader to copy into RAM). "
                         "write = emit write-pattern instructions that populate RAM[R0] (bigger ROM).")
    ap.add_argument("--start-addr", type=int, default=FONT_START_ADDR, help="R0 start address in RAM when using write mode")
    args = ap.parse_args()

    glyphs = parse_font_file(args.fontfile)
    if len(glyphs) < ASCII_END+1:
        print(f"Error: parsed only {len(glyphs)} glyph blocks, but expected at least {ASCII_END+1}.", file=sys.stderr)
        sys.exit(2)

    # Build list of words: for each ASCII char (0x20..0x7E), for row in 8: append left, right
    font_words = []
    for code in range(ASCII_START, ASCII_END+1):
        rows = glyphs[code]
        if len(rows) < 8:
            # If glyph has fewer than 8 rows, pad with zeros
            rows = (rows + [0]*8)[:8]
        for r in rows[:8]:
            left = (r << 8) & 0xFFFF
            right = r & 0xFFFF
            font_words.append((left, right))

    # Emit
    if args.mode == "data":
        emit_header_data_mode()
        # Emit flat sequence: left then right for each row
        idx = 0
        for code in range(ASCII_START, ASCII_END+1):
            print(f"// glyph 0x{code:02X} '{chr(code)}'")
            for row_i in range(8):
                left, right = font_words[idx]
                emit_value_data(left, comment=f"glyph 0x{code:02X} row {row_i} LEFT (0x{left:04X})")
                emit_value_data(right, comment=f"glyph 0x{code:02X} row {row_i} RIGHT (0x{right:04X})")
                idx += 1
            print("")
        emit_footer_data_mode()
        # small helpful comment about usage
        print("// NOTES:")
        print("// - The data above is a list of A-instructions, one @<value> per font word.")
        print("// - This is ROM-compact: 1 machine word per font word.")
        print("// - To use it at runtime you must copy these ROM values into RAM or execute a loader that")
        print("//   consumes these @<value> entries. 'write' mode emits a loader pattern instead (bigger ROM).")
    else:
        # write mode: emit the write pattern that writes the font into RAM starting at R0.
        emit_header_write_mode()
        # initialize R0 to start address
        print(f"@{args.start_addr}")
        print("D=A")
        print("@R0")
        print("M=D\n")
        idx = 0
        for code in range(ASCII_START, ASCII_END+1):
            print(f"// glyph 0x{code:02X} '{chr(code)}'")
            for row_i in range(8):
                left, right = font_words[idx]
                emit_value_write(left, comment=f"glyph 0x{code:02X} row {row_i} LEFT (0x{left:04X})")
                emit_value_write(right, comment=f"glyph 0x{code:02X} row {row_i} RIGHT (0x{right:04X})")
                idx += 1
            print("")
        print("// Done writing font into RAM. You can now (optionally) jump to your main program.")
        print("(FONT_DONE)")
        print("    0;JMP")

if __name__ == "__main__":
    main()
