cmake_minimum_required(VERSION 3.14.0)

include(FetchContent)

FetchContent_Declare(
    vst3sdk
    GIT_REPOSITORY ${HA_GIT_REPOSITORY_URL}/steinbergmedia/vst3sdk.git
    GIT_TAG v3.7.1_build_50
)

# Disable annoying validator once and for all!!!
option(SMTG_RUN_VST_VALIDATOR OFF)

if(WIN32)
    set(SMTG_CREATE_PLUGIN_LINK OFF)
endif()

FetchContent_MakeAvailable(vst3sdk)

# Solve problem with public_sdk_SOURCE_DIR not being defined when using smtg_enable_vst3_sdk().
set(vst3sdk_SOURCE_DIR ${vst3sdk_SOURCE_DIR} CACHE INTERNAL "VST 3 SDK source directory.") 
