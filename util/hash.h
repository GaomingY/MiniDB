#include <cstddef>
#include <cstdint>

namespace minidb{
    uint32_t Hash(const char* data, size_t n, uint32_t seed);
}

#endif