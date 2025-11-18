cmake_minimum_required(VERSION 3.13)
project(external_asset_manager LANGUAGES CXX)

set(GODOT_GDEXTENSION_DIR "${CMAKE_SOURCE_DIR}")

# if we are building on linux or mac use the compiler_launcher.sh script
if (UNIX)
	set(CMAKE_C_COMPILER_LAUNCHER "${CMAKE_SOURCE_DIR}/compiler_launcher.sh")
	set(CMAKE_CXX_COMPILER_LAUNCHER "${CMAKE_SOURCE_DIR}/compiler_launcher.sh")
else ()
	set(CMAKE_C_COMPILER_LAUNCHER "${CMAKE_SOURCE_DIR}/compiler_launcher.bat")
	set(CMAKE_CXX_COMPILER_LAUNCHER "${CMAKE_SOURCE_DIR}/compiler_launcher.bat")
endif ()


if (GODOT_DISABLE_EXCEPTIONS)
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} -D_HAS_EXCEPTIONS=0")
	else()
		set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} -fno-exceptions")
	endif()
else()
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		set(GODOT_COMPILE_FLAGS "${GODOT_COMPILE_FLAGS} /EHsc")
	endif()
endif()

# set the sources and headers
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

set(SOURCES
	extension/include/pugixml.cpp
	extension/src/register_types.cpp
	extension/src/identifier.cpp
	extension/src/dynamic_asset_indexer.cpp
	extension/src/dynmaic_prefix_handler.cpp
	extension/src/data_cache_manager.cpp
	extension/src/entity_template_manager.cpp
	extension/src/xml_loader.cpp
)
include_directories(extension/include)

add_library(external_asset_manager SHARED ${SOURCES})


if(WIN32)
	if(MINGW)
		target_link_libraries(external_asset_manager -static gcc stdc++ winpthread -dynamic)
	endif()
endif()

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# rename the output file and update the install location
string(TOLOWER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME_LOWER)
# For multi-config generators like Visual Studio, use a generator expression
if(MSVC)
	set(BUILD_TYPE_LOWER "$<LOWER_CASE:$<CONFIG>>")
else()
	string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE_LOWER)
endif()
string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} SYSTEM_PROCESSOR_LOWER)

set_target_properties(external_asset_manager PROPERTIES OUTPUT_NAME external_asset_manager.${SYSTEM_NAME_LOWER}.template_${BUILD_TYPE_LOWER}.${SYSTEM_PROCESSOR_LOWER})
install(TARGETS external_asset_manager DESTINATION ${CMAKE_SOURCE_DIR}/addons/eam/bin)

# Copy DLL to addon directory after build
add_custom_command(TARGET external_asset_manager POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:external_asset_manager> ${CMAKE_SOURCE_DIR}/addons/eam/bin/
	COMMENT "Copying extension to addon directory"
)

# Add the godot-cpp library
set(GODOT_CPP_BUILD_LIBRARY OFF CACHE BOOL "Build godot-cpp library" FORCE)
# Prevent godot-cpp from regenerating binding files on every configure
set(GODOT_CPP_SKIP_BINDING_GENERATION OFF CACHE BOOL "Skip godot-cpp binding generation" FORCE)

add_subdirectory(extension/godot-cpp EXCLUDE_FROM_ALL)
target_link_libraries(external_asset_manager godot-cpp)

# Use the godot-cpp compile arguments for this as well
set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY COMPILE_FLAGS ${GODOT_COMPILE_FLAGS})
