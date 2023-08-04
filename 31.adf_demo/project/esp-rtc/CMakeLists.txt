# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(EXTRA_COMPONENT_DIRS ./components)

set(EXTRA_COMPONENT_DIRS
    $ENV{IDF_PATH}/examples/common_components/protocol_examples_common
    $ENV{IDF_PATH}/examples/common_components/led_strip
    ../../components
	../../adf_components
	)
add_compile_options(-fdiagnostics-color=always)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# list(APPEND EXTRA_COMPONENT_DIRS "$ENV{ADF_PATH}/examples/protocols/components")
project(esp-rtc)
