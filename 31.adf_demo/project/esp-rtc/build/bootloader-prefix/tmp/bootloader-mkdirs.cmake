# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "F:/myesp/.espressif/frameworks/esp-idf-v4.4.2/components/bootloader/subproject"
  "F:/Github/ksdiy_korvo_demo/31.adf_demo/project/esp-rtc/build/bootloader"
  "F:/Github/ksdiy_korvo_demo/31.adf_demo/project/esp-rtc/build/bootloader-prefix"
  "F:/Github/ksdiy_korvo_demo/31.adf_demo/project/esp-rtc/build/bootloader-prefix/tmp"
  "F:/Github/ksdiy_korvo_demo/31.adf_demo/project/esp-rtc/build/bootloader-prefix/src/bootloader-stamp"
  "F:/Github/ksdiy_korvo_demo/31.adf_demo/project/esp-rtc/build/bootloader-prefix/src"
  "F:/Github/ksdiy_korvo_demo/31.adf_demo/project/esp-rtc/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "F:/Github/ksdiy_korvo_demo/31.adf_demo/project/esp-rtc/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
