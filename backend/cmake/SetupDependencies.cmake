include(CPM)

CPMAddPackage(
    NAME Boost
    VERSION 1.86.0
    URL https://github.com/boostorg/boost/releases/download/boost-1.86.0/boost-1.86.0-cmake.tar.xz
    URL_HASH SHA256=2c5ec5edcdff47ff55e27ed9560b0a0b94b07bd07ed9928b476150e16b0efc57
    OPTIONS
    "BOOST_ENABLE_CMAKE ON"
    "BOOST_INCLUDE_LIBRARIES beast"
)

CPMAddPackage(
    NAME googletest
    VERSION 1.15.2
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    URL_HASH SHA256=F179EC217F9B3B3F3C6E8B02D3E7EDA997B49E4CE26D6B235C9053BEC9C0BF9F
    OPTIONS
    "BUILD_GMOCK ON"
    "INSTALL_GTEST OFF"
    "gtest_forced_shared_crt ON CACHE BOOL \"\" FORCE"
)

CPMAddPackage(
    NAME reflect-cpp
    VERSION 0.16.0
    URL https://github.com/getml/reflect-cpp/archive/refs/tags/v0.16.0.zip
    URL_HASH SHA256=16494784CF7AF86A9903213EB4F654579258D44502B6FC8B1B50467C66B5A4A3
)
target_compile_definitions(reflectcpp PUBLIC _CRT_SECURE_NO_WARNINGS)
