# BDD binary format

This document has been moved to [this page](https://github.com/junkawahara/dd_documents/blob/main/formats/bdd_binary_format.md).

## Byte order

SBDD_helper reads and writes each multi-byte value of this format in
little endian with a fixed width (16/32/64 bits), independently of the byte
order and the type sizes of the machine. Therefore a file written by
SBDD_helper can be read by SBDD_helper on any machine.
