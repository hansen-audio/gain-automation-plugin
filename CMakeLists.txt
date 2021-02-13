cmake_minimum_required(VERSION 3.14.0)

include(FetchContent)

FetchContent_Declare(
    vst3sdk
    GIT_REPOSITORY https://github.com/steinbergmedia/vst3sdk.git
    GIT_TAG v3.7.1_build_50
)

project(GainAutomator)
  
set(SMTG_VSTGUI_ROOT "${vst3sdk_SOURCE_DIR}")

FetchContent_MakeAvailable(vst3sdk)
smtg_enable_vst3_sdk()

smtg_add_vst3plugin(GainAutomator     
    source/version.h
    source/gainautomatorcids.h
    source/gainautomatorprocessor.h
    source/gainautomatorprocessor.cpp
    source/gainautomatorcontroller.h
    source/gainautomatorcontroller.cpp
    source/gainautomatorentry.cpp
)

#- VSTGUI Wanted ----
if(SMTG_ADD_VSTGUI)
	target_sources(GainAutomator
		PRIVATE
			resource/gainautomatoreditor.uidesc
	)
    target_link_libraries(GainAutomator
        PRIVATE
            vstgui_support
    )
    smtg_add_vst3_resource(GainAutomator "resource/gainautomatoreditor.uidesc")
endif()
# -------------------

smtg_add_vst3_snapshot(GainAutomator resource/CCA38A9855E653C88080AD5A9F58C064_snapshot.png)
smtg_add_vst3_snapshot(GainAutomator resource/CCA38A9855E653C88080AD5A9F58C064_snapshot_2.0x.png)

target_link_libraries(GainAutomator
    PRIVATE
        sdk
)

if(SMTG_MAC)
    smtg_set_bundle(GainAutomator INFOPLIST 
        "${CMAKE_CURRENT_LIST_DIR}/resource/Info.plist" 
        PREPROCESS
    )
elseif(SMTG_WIN)
    target_sources(GainAutomator PRIVATE 
        resource/win32resource.rc
    )
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT GainAutomator)
	
	set_target_properties(GainAutomator PROPERTIES
		VS_DEBUGGER_COMMAND
			"$(ProgramW6432)/Steinberg/VST3PluginTestHost/VST3PluginTestHost.exe"
		VS_DEBUGGER_COMMAND_ARGUMENTS
			" --pluginfolder \"$(OutDir)\" "
	)
endif()


