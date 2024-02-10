#include <opencv2/imgcodecs.hpp>

#include <iostream>
#include <spdlog/spdlog.h>
#include <string>

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    spdlog::critical("Input resistor image path not provided");
    return 0;
  }

  std::string input_image(argv[1]);
  spdlog::info("Reading input file {}", input_image);
  (void)cv::imread(input_image);

  return 0;
}
