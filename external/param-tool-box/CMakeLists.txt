cmake_minimum_required(VERSION 3.15.0)

include(FetchContent)

FetchContent_Declare(
    param-tool-box
    GIT_REPOSITORY http://gitea.fritz.box/hansen-audio/param-tool-box.git
    GIT_TAG origin/master
)

FetchContent_MakeAvailable(param-tool-box)