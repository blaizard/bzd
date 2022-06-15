cmake_minimum_required(VERSION 3.5)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(EXTRA_COMPONENT_DIRS "./main_stub")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(bzd)
