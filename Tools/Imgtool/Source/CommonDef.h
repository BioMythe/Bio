#ifndef BIO_TOOLS_IMGTOOL_COMMON_DEF_H
#define BIO_TOOLS_IMGTOOL_COMMON_DEF_H

// Ceiling divide
#define IT_CDIV(dividend, divisor) (((dividend) + (divisor) - 1) / (divisor))

// Resolve Block
#define IT_RESBLK(block, bytesPerBlock) ((block) * (bytesPerBlock))

// Bytes To MiB
#define IT_BTOM(bytes) ((bytes) / 1024 / 1024)

// MiB To Bytes
#define IT_MTOB(mibs) ((mibs) * 1024 * 1024)

#endif
