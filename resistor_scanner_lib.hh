#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

namespace color
{

/// BGR representation
constexpr std::array<float, 3> black_v = {25, 25, 25};
constexpr std::array<float, 3> red_v = {4, 8, 67};
constexpr std::array<float, 3> blue_v = {0, 255, 0};
constexpr std::array<float, 3> brown_v = {13, 29, 52};
constexpr std::array<float, 3> white_v = {198, 198, 198};

enum class Color
{
  black = 0,
  brown = 1,
  red = 2,
  blue = 6,
  white = 9
};

[[nodiscard]] const char* color_name(const Color c);

template <typename T> [[nodiscard]] double magnitude(const T& color)
{
  return std::cbrt(static_cast<double>(color[0]) * color[0] +
                   static_cast<double>(color[1]) * color[1] +
                   static_cast<double>(color[2]) * color[2]);
};

template <typename T1, typename T2> [[nodiscard]] double similar(const T1& color1, const T2& color2)
{
  const auto dot_product = color1[0] * static_cast<double>(color2[0]) +
                           color1[1] * static_cast<double>(color2[1]) +
                           color1[2] * static_cast<double>(color2[2]);
  return dot_product / (magnitude(color1) * magnitude(color2));
};

} // end namespace color

namespace rscan
{

struct ResistorValue
{
  uint32_t resistance_ohms = 0;
  double tolerance_percent = 0.0;
};

[[nodiscard]] std::vector<color::Color>
remove_background(const std::vector<color::Color>& resistor_colors);

[[nodiscard]] ResistorValue calculate_resistor(const std::vector<color::Color>& resistor_bands);

} // end namespace rscan
