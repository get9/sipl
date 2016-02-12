#include <array>

namespace sipl
{

// Helpful defines for vector types
template <typename T>
using Vector3<T> = std::array<T, 3>;

using Vector3b = typename Vector3<uint8_t>;

using RGBPixel = typename Vector3b;
}
