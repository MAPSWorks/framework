#ifndef PROTOZERO_BYTESWAP_HPP
#define PROTOZERO_BYTESWAP_HPP

/*****************************************************************************

protozero - Minimalistic protocol buffer decoder and encoder in C++.

This file is from https://github.com/mapbox/protozero where you can find more
documentation.

*****************************************************************************/

/**
 * @file byteswap.hpp
 *
 * @brief Contains functions to swap bytes in values (for different endianness).
 */

#include <cstdint>
#include <cassert>

namespace protozero {

/**
 * Swap N byte value between endianness formats. This template function must
 * be specialized to actually work.
 */
template <int N>
inline void byteswap(const char* /*data*/, char* /*result*/) {
    static_assert(N == 1, "Can only swap 1, 4, or 8 byte values");
}

/**
 * Swap 1 byte value between endianness formats. (Basically just a copy).
 */
template <>
inline void byteswap<1>(const char* data, char* result) {
    result[0] = data[0];
}

/**
 * Swap 4 byte value (int32_t, uint32_t, float) between endianness formats.
 */
template <>
inline void byteswap<4>(const char* data, char* result) {
# if defined(__GNUC__) || defined(__clang__)
    *reinterpret_cast<uint32_t*>(result) = __builtin_bswap32(*reinterpret_cast<const uint32_t*>(data));
# else
    result[3] = data[0];
    result[2] = data[1];
    result[1] = data[2];
    result[0] = data[3];
#endif
}

/**
 * Swap 8 byte value (int64_t, uint64_t, double) between endianness formats.
 */
template <>
inline void byteswap<8>(const char* data, char* result) {
# if defined(__GNUC__) || defined(__clang__)
    *reinterpret_cast<uint64_t*>(result) = __builtin_bswap64(*reinterpret_cast<const uint64_t*>(data));
# else
    result[7] = data[0];
    result[6] = data[1];
    result[5] = data[2];
    result[4] = data[3];
    result[3] = data[4];
    result[2] = data[5];
    result[1] = data[6];
    result[0] = data[7];
#endif
}

} // end namespace protozero

#endif // PROTOZERO_BYTESWAP_HPP
