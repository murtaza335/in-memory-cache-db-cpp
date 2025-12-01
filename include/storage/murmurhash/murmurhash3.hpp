#ifndef MURMURHASH3_HPP
#define MURMURHASH3_HPP

#include <cstdint>
#include <string>

uint32_t MurmurHash3_x86_32(const void* key, int len, uint32_t seed = 0);

// Convenience overload for std::string
uint32_t MurmurHash3_x86_32(const std::string& str, uint32_t seed = 0);

#endif // MURMURHASH3_HPP
