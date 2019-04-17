#pragma once

#include <stdio.h>
#include <ostream>
#include <arpa/inet.h> // htons, htonl

#include "types.h"

namespace BitUtil
{
    /// @brief Prints a hex dump of a buffer.
    /// @note The debug are rows of 16 octets with their hexadecimal value on
    /// the left and if printable their character representation on the right
    /// or a dot (.) if not printable.
    /// @param buff The buffer to print.
    /// @param len Number of octets in buff.
    /// @param file The C file stream to write to (default standard output).
    void ChexDump(const char* buff, size_t len, FILE* file);

    /// @brief Prints a hex dump of a buffer.
    /// @note The debug are rows of 16 octets with their hexadecimal value on
    /// the left and if printable their character representation on the right
    /// or a dot (.) if not printable.
    /// @param buff The buffer to print.
    /// @param len Number of octets in buff.
    /// @param file The C++ file stream to write to (default standard output).
    void HexDump(const char* buff, size_t len, std::ostream& os);
}

// generate mask for lowest/lsb 'n' bits. E.g. BITMASK(6) => 0x3f
#ifndef BITMASK
#define BITMASK(n) ((1<<(n))-1)
#endif

// test if the 'n'th lowest/lsb is set
#ifndef BITTEST
#define BITTEST(v,n) ((bool)((v) & (1 << (n))))
#endif

// set the 'n'th lowest/lsb bit
#ifndef BITSET
#define BITSET(v,n) (v |= (1 << (n)))
#endif

// get u8/u16/u32 at octed data (pointed by p) without alignment&endian constraints
inline uint8_t  getU8(const char* p) { return *reinterpret_cast<const uint8_t*>(p); }
inline uint16_t getU16(const char* p) { return htons(*(uint16_t*)p); }
inline uint32_t getU32(const char* p) { return htonl(*(uint32_t*)p); }

