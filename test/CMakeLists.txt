
cmake_minimum_required (VERSION 3.17)

set (t_ test_argskimmer)
    add_executable (${t_} main.cpp argskimmer.cpp doctest-rapidcheck.hpp)
    target_link_libraries (${t_} PRIVATE argskimmer CONAN_PKG::doctest CONAN_PKG::rapidcheck)
