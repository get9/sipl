#include <array>

namespace sipl
{
template <typename T>
using Vector3<T> = std::array<T, 3>;

using Vector3b = typename Vector3<uint8_t>;
}
