cmake_minimum_required(VERSION 3.8)
project(ionicengine)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(GenerateExportHeader)

set(CMAKE_CXX_STANDARD 17)

#option(IONICENGINE_BUILD_STATIC "Build static libraries" OFF)
option(IONICENGINE_INSTALL "Generate installation target" ON)
option(IONICENGINE_BUILD_TESTS "Build the ionicengine test programs" ON)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -march=native -mtune=native -ffast-math -D__extern_always_inline=\"extern __always_inline\"")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -march=native -mtune=native -ffast-math")
elseif (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif ()

find_package(LAMBDACOMMON REQUIRED)
find_package(Threads)
find_package(GLFW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(Freetype REQUIRED)
find_package(libsndfile REQUIRED)
#find_package(HarfBuzz REQUIRED)

include_directories(${LAMBDACOMMON_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS} ${HARFBUZZ_INCLUDE_DIRS} ${OPENAL_INCLUDE_DIR} ${libsndfile_INCLUDE_DIRS} dependencies/stb)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(HEADERS_GRAPHICS include/ionicengine/graphics/graphics.h include/ionicengine/graphics/screen.h include/ionicengine/graphics/textures.h include/ionicengine/graphics/shader.h include/ionicengine/graphics/font.h include/ionicengine/graphics/animation.h include/ionicengine/graphics/gui.h include/ionicengine/graphics/utils.h)
set(HEADERS_INPUT include/ionicengine/input/inputmanager.h include/ionicengine/input/controller.h)
set(HEADERS_SOUND include/ionicengine/sound/sound.h include/ionicengine/sound/wav.h)
set(HEADERS_WINDOW include/ionicengine/window/monitor.h include/ionicengine/window/window.h)
set(HEADERS_FILES ${HEADERS_GRAPHICS} ${HEADERS_INPUT} ${HEADERS_SOUND} ${HEADERS_WINDOW} include/ionicengine/ionicengine.h include/ionicengine/includes.h)
set(SOURCES_GRAPHICS src/graphics/graphics.cpp src/graphics/screen.cpp src/graphics/textures.cpp src/graphics/shader.cpp src/graphics/font.cpp src/graphics/animation.cpp src/graphics/gui.cpp src/graphics/utils.cpp)
set(SOURCES_INPUT src/input/inputmanager.cpp src/input/controller.cpp)
set(SOURCES_SOUND src/sound/sound.cpp src/sound/wav.cpp)
set(SOURCES_WINDOW src/window/monitor.cpp src/window/window.cpp)
set(SOURCES_FILES ${SOURCES_GRAPHICS} ${SOURCES_INPUT} ${SOURCES_SOUND} ${SOURCES_WINDOW} src/ionicengine.cpp)

# Now build the library
# Build static if the option is on.
#if (IONICENGINE_BUILD_STATIC)
#    add_library(ionicengine_static STATIC ${HEADERS_FILES} ${SOURCES_FILES})
#    target_link_libraries(ionicengine_static lambdacommon_static glfw ${GLEW_LIBRARIES})
#endif ()

add_library(ionicengine ${HEADERS_FILES} ${SOURCES_FILES})
target_link_libraries(ionicengine AperLambda::lambdacommon ${CMAKE_THREAD_LIBS_INIT} GLFW::GLFW OpenGL::GL GLEW::GLEW Freetype::Freetype ${OPENAL_LIBRARY} MegaNerd::libsndfile ${HARFBUZZ_LIBRARIES})

GENERATE_EXPORT_HEADER(ionicengine
        BASE_NAME ionicengine
        EXPORT_MACRO_NAME IONICENGINE_API
        EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/ionicengine_exports.h
        STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC)
target_compile_definitions(ionicengine PUBLIC -DSHARED_EXPORTS_BUILT_AS_STATIC)
target_include_directories(ionicengine PUBLIC ${CMAKE_BINARY_DIR}/exports)
#if (IONICENGINE_BUILD_STATIC)
#    target_compile_definitions(ionicengine_static PUBLIC -DSHARED_EXPORTS_BUILT_AS_STATIC)
#    target_include_directories(ionicengine_static PUBLIC ${CMAKE_BINARY_DIR}/exports)
#endif ()

# Install if the option is on.
if (IONICENGINE_INSTALL)
    foreach (LOOP_HEADER ${HEADERS_FILES})
        get_filename_component(HEADER_PARENT_DIR ${LOOP_HEADER} DIRECTORY)
        install(FILES ${LOOP_HEADER} DESTINATION ${HEADER_PARENT_DIR})
    endforeach ()
    install(FILES ${CMAKE_BINARY_DIR}/exports/ionicengine_exports.h DESTINATION include/ionicengine)
	install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources/ionicengine DESTINATION resources)
    install(TARGETS ionicengine
            RUNTIME DESTINATION bin COMPONENT libraries
            LIBRARY DESTINATION lib COMPONENT libraries
            ARCHIVE DESTINATION lib COMPONENT libraries)

	# Uninstall target
	if(NOT TARGET uninstall)
		configure_file(
				"${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
				"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
				IMMEDIATE @ONLY)

		add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
	endif()
#    if (IONICENGINE_BUILD_STATIC)
#        install(TARGETS ionicengine_static
#                RUNTIME DESTINATION bin COMPONENT libraries
#                LIBRARY DESTINATION lib COMPONENT libraries
#                ARCHIVE DESTINATION lib COMPONENT libraries)
#    endif ()
endif ()

# Build the tests if the option is on.
if (IONICENGINE_BUILD_TESTS)
    add_subdirectory(tests)
endif ()