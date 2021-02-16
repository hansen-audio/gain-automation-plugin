cmake_minimum_required(VERSION 3.14.0)

include(FetchContent)

FetchContent_Declare(
    vst3sdk
    GIT_REPOSITORY ${HA_GIT_REPOSITORY_URL}/steinbergmedia/vst3sdk.git
    GIT_TAG v3.7.1_build_50
)
