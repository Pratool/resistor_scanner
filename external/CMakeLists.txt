cmake_minimum_required(VERSION 3.6)

project("External Projects")

include(ExternalProject)

externalproject_add(spdlog
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/spdlog
  URL "https://github.com/gabime/spdlog/archive/refs/tags/v1.13.0.tar.gz"
  URL_HASH "SHA256=534f2ee1a4dcbeb22249856edfb2be76a1cf4f708a20b0ac2ed090ee24cfdbc9"
  DOWNLOAD_NO_PROGRESS ON
  CMAKE_ARGS
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DCMAKE_PREFIX_PATH:PATH=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_INSTALL_RPATH:PATH=$ORIGIN:$ORIGIN/../lib
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
)
