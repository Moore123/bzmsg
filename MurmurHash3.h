//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#ifndef MURMURHASH3_H
#define MURMURHASH3_H

//-----------------------------------------------------------------------------
// Platform-specific functions and macros
#include <stdint.h>

//-----------------------------------------------------------------------------

void MurmurHash3_x86_32(const void *key, int len, uint32_t seed, void *out);

uint64_t Murmurhash64A(const void *, int, unsigned int);
uint64_t Murmurhash64B(const void *, int, unsigned int);

void MurmurHash3_x86_128(const void *key, int len, uint32_t seed, void *out);

void MurmurHash3_x64_128(const void *key, int len, uint32_t seed, void *out);

//-----------------------------------------------------------------------------

#endif								  // MURMURHASH3_H
