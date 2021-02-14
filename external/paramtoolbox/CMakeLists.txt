cmake_minimum_required(VERSION 3.14.0)

include(FetchContent)

set(CMAKE_CXX_STANDARD 14)

### External
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG 609281088cfefc76f9d0ce82e1ff6c30cc3591e5
)

FetchContent_MakeAvailable(googletest)

project(ParamToolBox)

add_library(paramtoolbox STATIC  
    include/ha/ptb/paramvaluequeueprocessor.h
    include/ha/ptb/paramramp.h
    source/paramramp.cpp
    source/paramvaluequeueprocessor.cpp
)

target_include_directories(paramtoolbox
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/source
)

enable_testing()

add_executable(paramtoolbox_test
    test/paramvaluequeueprocessor_test.cpp
)

target_link_libraries(paramtoolbox_test
    PRIVATE
        paramtoolbox
        gtest
        gtest_main
)