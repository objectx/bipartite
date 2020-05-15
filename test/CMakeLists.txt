#
# Copyright (c) 2020 Masashi Fujita. All rights reserved.
#
cmake_minimum_required (VERSION 3.17)

set (t_ test_bipartite)
    add_executable (${t_} main.cpp bipartite.cpp doctest-rapidcheck.hpp)
    target_link_libraries (${t_} PRIVATE bipartite CONAN_PKG::doctest CONAN_PKG::rapidcheck)
    target_compile_options (${t_} PRIVATE $<$<CONFIG:Debug>:-fsanitize=address,undefined>)
    target_link_options (${t_} PRIVATE $<$<CONFIG:Debug>:-fsanitize=address,undefined>)
