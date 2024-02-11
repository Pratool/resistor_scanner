#include "resistor_scanner_lib.hh"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <spdlog/cfg/env.h>
#include <spdlog/spdlog.h>

#include <cmath>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

int main(int argc, char** argv)
{
  spdlog::cfg::load_env_levels();

  if (argc < 2)
  {
    spdlog::critical("Input resistor image path not provided");
    return 0;
  }

  std::string input_image(argv[1]);
  spdlog::info("Reading input file {}", input_image);

  auto img = cv::imread(input_image);

  // create a horizontal line at 33%, 50%, 67%
  const auto img_size = img.size();
  spdlog::info("width: {}, height: {}", img_size.width, img_size.height);

  const auto height = static_cast<float>(img_size.height);

  const std::array<uint32_t, 3> height_samples = {static_cast<uint32_t>(0.33 * height),
                                                  static_cast<uint32_t>(0.5 * height),
                                                  static_cast<uint32_t>(0.67 * height)};
  std::array<std::vector<color::Color>, 3> band_categories = {};

  for (uint32_t band_idx = 0; band_idx < height_samples.size(); ++band_idx)
  {
    const auto height_sample = height_samples.at(band_idx);
    auto& band_category = band_categories[band_idx];
    spdlog::debug("band {}", band_idx + 1);

    for (int32_t idx = 0; idx < img_size.width; ++idx)
    {
      const auto v = img.at<cv::Vec3b>(height_sample, idx);

      if (color::magnitude(v.val) < color::magnitude(color::black_v))
      {
        img.at<cv::Vec3b>(height_sample, idx) = cv::Vec3b(0, 0, 0);
        spdlog::debug("black");
        band_category.push_back(color::Color::black);
        continue;
      }

      if (color::magnitude(v.val) > color::magnitude(color::white_v))
      {
        img.at<cv::Vec3b>(height_sample, idx) = cv::Vec3b(255, 255, 255);
        band_category.push_back(color::Color::white);
        continue;
      }

      using color::Color;
      using ColorTuple = std::tuple<Color, float, std::array<float, 3U>>;

      const std::array<ColorTuple, 5U> color_similarities = {
          ColorTuple(Color::red, color::similar(color::red_v, v.val), color::red_v),
          ColorTuple(Color::blue, color::similar(color::blue_v, v.val), color::blue_v),
          ColorTuple(Color::brown, color::similar(color::brown_v, v.val), color::brown_v),
          ColorTuple(Color::orange, color::similar(color::orange_v, v.val), color::orange_v),
          ColorTuple(Color::yellow, color::similar(color::yellow_v, v.val), color::yellow_v)};

      const auto similar_itr = std::max_element(
          std::cbegin(color_similarities), std::cend(color_similarities),
          [](const auto& lhs, const auto& rhs) { return std::get<1>(lhs) < std::get<1>(rhs); });
      spdlog::debug("{} = {} correlation", color::color_name(std::get<0>(*similar_itr)),
                    std::get<1>(*similar_itr));

      // Highlight the selected color in the source image.
      auto s = std::get<2>(*similar_itr);
      if (std::get<0>(*similar_itr) == Color::blue)
      {
        s = std::array<float, 3>({255, 0, 0});
      }
      else if (std::get<0>(*similar_itr) == Color::red)
      {
        s = std::array<float, 3>({0, 0, 255});
      }
      else if (std::get<0>(*similar_itr) == Color::yellow)
      {
        s = std::array<float, 3>({100, 96, 36});
      }
      else if (std::get<0>(*similar_itr) == Color::orange)
      {
        s = std::array<float, 3>({8, 45, 87});
      }

      img.at<cv::Vec3b>(height_sample, idx) = cv::Vec3b(s[0], s[1], s[2]);
      band_category.push_back(std::get<0>(*similar_itr));
    }
  }

  cv::imwrite("test.jpg", img);

  constexpr float agreement_width_ratio = 0.035f;
  const uint32_t agreement_width =
      static_cast<uint32_t>(std::round(img_size.width * agreement_width_ratio));

  std::vector<color::Color> colors;
  std::optional<color::Color> prev;
  uint32_t agreement_count = 0;
  for (uint32_t idx = 0; idx < band_categories.front().size(); ++idx)
  {
    const auto top = band_categories.at(0).at(idx);
    const auto middle = band_categories.at(1).at(idx);
    const auto bottom = band_categories.at(2).at(idx);
    auto band = color::Color::white;
    // Do 2 of the band colors agree? Then use the agreed-upon color
    if (top == middle || top == bottom)
    {
      band = top;
    }
    else if (middle == bottom)
    {
      band = middle;
    }

    // If they did not agree or they agreed that they were white,
    // then skip this vertical section.
    if (band == color::Color::white)
    {
      continue;
    }

    // If the previous color does not match the agreed-upon color,
    // reset the agreement count.
    if (prev != band)
    {
      agreement_count = 0;
      prev = band;
    }

    // The colors agreed in this horizontal slice, so increment
    // the agreement count.
    ++agreement_count;

    // Once the colors have agreed for more than the minimum
    // width (this adds hysteresis), then use the color as a
    // band color.
    if (agreement_count >= agreement_width && (colors.size() == 0 || colors.back() != band))
    {
      colors.push_back(band);
    }
  }
  std::stringstream ss;
  for (const auto& c : colors)
  {
    ss << color::color_name(c) << ' ';
  }
  spdlog::info("colors with background: {}", ss.str());

  colors = rscan::remove_background(colors);
  ss.clear();
  for (const auto& c : colors)
  {
    ss << color::color_name(c) << ' ';
  }
  spdlog::info("colors: {}", ss.str());

  const auto resistor_value = rscan::calculate_resistor(colors);
  spdlog::info("resistance: {} ohms", resistor_value.resistance_ohms);
  spdlog::info("tolerance: {}%", resistor_value.tolerance_percent);

  return 0;
}
