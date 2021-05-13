cmake_minimum_required(VERSION 3.14.0)

include(FetchContent)

FetchContent_Declare(
    vst3sdk
    # GIT_REPOSITORY ${HA_GIT_REPOSITORY_URL}/steinbergmedia/vst3sdk.git
    GIT_REPOSITORY https://www.github.com/steinbergmedia/vst3sdk.git
    GIT_TAG v3.7.1_build_50
)

# Disable annoying validator once and for all!!!
option(SMTG_RUN_VST_VALIDATOR OFF)

# Disable annoying link creation of Windows once and for all!!!!
if(WIN32)
    option(SMTG_CREATE_PLUGIN_LINK OFF)
    set(DEF_OPT_LINK OFF)
    set(SMTG_PLUGIN_TARGET_PATH "" CACHE PATH "Here you can redefine the VST3 Plug-ins folder")
endif()

FetchContent_MakeAvailable(vst3sdk)

# Solve problem with public_sdk_SOURCE_DIR not being defined when using smtg_enable_vst3_sdk().
set(vst3sdk_SOURCE_DIR ${vst3sdk_SOURCE_DIR} CACHE INTERNAL "VST 3 SDK source directory.") 
