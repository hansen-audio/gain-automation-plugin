cmake_minimum_required(VERSION 3.14.0)

include(FetchContent)

set(CMAKE_CXX_STANDARD 17)

project(gain-automator)

add_subdirectory(external)

smtg_enable_vst3_sdk()

smtg_add_vst3plugin(Gain-Automator     
    source/version.h
    source/gain_automator_cids.h
    source/gain_automator_processor.h
    source/gain_automator_processor.cpp
    source/gain_automator_controller.h
    source/gain_automator_controller.cpp
    source/gain_automator_entry.cpp
)

target_link_libraries(Gain-Automator
    PRIVATE
        sdk
        param-tool-box
)

#- VSTGUI Wanted ----
if(SMTG_ADD_VSTGUI)
	target_sources(Gain-Automator
		PRIVATE
			resource/gain_automator_editor.uidesc
	)
    target_link_libraries(Gain-Automator
        PRIVATE
            vstgui_support
    )
    smtg_add_vst3_resource(Gain-Automator "resource/gain_automator_editor.uidesc")
endif()
# -------------------

smtg_add_vst3_resource(Gain-Automator resource/big_knob.png)
smtg_add_vst3_resource(Gain-Automator resource/big_knob_2.0x.png)
smtg_add_vst3_snapshot(Gain-Automator resource/CCA38A9855E653C88080AD5A9F58C064_snapshot.png)
smtg_add_vst3_snapshot(Gain-Automator resource/CCA38A9855E653C88080AD5A9F58C064_snapshot_2.0x.png)

if(SMTG_MAC)
    smtg_set_bundle(Gain-Automator INFOPLIST 
        "${CMAKE_CURRENT_LIST_DIR}/resource/Info.plist" 
        PREPROCESS
    )
elseif(SMTG_WIN)
    target_sources(Gain-Automator PRIVATE 
        resource/win32resource.rc
    )
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Gain-Automator)
	
	set_target_properties(Gain-Automator PROPERTIES
		VS_DEBUGGER_COMMAND
			"$(ProgramW6432)/Steinberg/VST3PluginTestHost/VST3PluginTestHost.exe"
		VS_DEBUGGER_COMMAND_ARGUMENTS
			" --pluginfolder \"$(OutDir)\" "
	)
endif()


