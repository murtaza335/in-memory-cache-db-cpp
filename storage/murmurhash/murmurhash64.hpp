#pragma once
#include <cstdint>
#include <string>

uint64_t murmur64(const void* key, int len, uint32_t seed = 0);
uint64_t murmur64(const std::string& key, uint32_t seed = 0);
