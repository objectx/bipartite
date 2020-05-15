#
# Copyright (c) 2020 Masashi Fujita. All rights reserved.
#

cmake_minimum_required (VERSION 3.17)

project (bipartite
         VERSION 1.0.0
         LANGUAGES CXX)

include (${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

conan_basic_setup (TARGETS)

set (t_ bipartite)
    add_library (${t_} INTERFACE)
    target_include_directories (${t_} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/inc)
    target_compile_features (${t_} INTERFACE cxx_std_17)

add_subdirectory (test)
