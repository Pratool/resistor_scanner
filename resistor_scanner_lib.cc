#include "resistor_scanner_lib.hh"

#include <cassert>

namespace color
{

[[nodiscard]] const char* color_name(const Color c)
{
  switch (c)
  {
    case Color::black:
      return "black";
    case Color::red:
      return "red";
    case Color::blue:
      return "blue";
    case Color::brown:
      return "brown";
    case Color::white:
      return "white";
  }
  return "unknown";
}

} // end namespace color

namespace rscan
{

[[nodiscard]] std::vector<color::Color>
remove_background(const std::vector<color::Color>& resistor_colors)
{
  std::vector<color::Color> resistor_bands = {};
  for (auto resistor_color_itr = std::cbegin(resistor_colors) + 1;
       resistor_color_itr != std::cend(resistor_colors) &&
       resistor_color_itr + 1 != std::cend(resistor_colors);
       resistor_color_itr += 2)
  {
    resistor_bands.emplace_back(*resistor_color_itr);
  }
  return resistor_bands;
}

[[nodiscard]] ResistorValue calculate_resistor(const std::vector<color::Color>& resistor_bands)
{
  using color::Color;

  assert(resistor_bands.size() <= 6);
  assert(resistor_bands.size() >= 3);

  ResistorValue value;

  auto resistor_band_reverse_itr = std::crbegin(resistor_bands);
  if (resistor_bands.size() == 6)
  {
    resistor_band_reverse_itr++;
  }

  if (resistor_bands.size() > 3)
  {
    switch (*resistor_band_reverse_itr)
    {
      case Color::black:
        value.tolerance_percent = 0;
        break;
      case Color::brown:
        value.tolerance_percent = 1;
        break;
      case Color::red:
        value.tolerance_percent = 2;
        break;
      case Color::blue:
        value.tolerance_percent = 0.25;
        break;
      case Color::white:
        value.tolerance_percent = 0;
        break;
    }
    resistor_band_reverse_itr++;
  }

  uint32_t multiplier = 1U;
  for (uint32_t i = 0; i < static_cast<uint32_t>(*resistor_band_reverse_itr); ++i)
  {
    multiplier *= 10;
  }

  resistor_band_reverse_itr++;

  uint32_t digit_multiplier = 1U;
  for (; resistor_band_reverse_itr != std::crend(resistor_bands); ++resistor_band_reverse_itr)
  {
    value.resistance_ohms += digit_multiplier * static_cast<uint32_t>(*resistor_band_reverse_itr);
    digit_multiplier *= 10;
  }

  value.resistance_ohms *= multiplier;

  return value;
}

} // end namespace rscan
