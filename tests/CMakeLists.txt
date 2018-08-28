cmake_minimum_required(VERSION 3.8)
project(ionicengine_tests)

set(CMAKE_CXX_STANDARD 17)

include_directories(../include)

find_package(X11)

set(LD_LIBRARY "")

if (NOT WIN32)
	set(LD_LIBRARY "-ldl")
endif()

add_executable(ionic_joysticks joysticks.cpp)
target_link_libraries(ionic_joysticks ${LAMBDACOMMON_LIBRARIES} ionicengine ${GLFW_LIBRARIES} ${LD_LIBRARY} ${X11_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARIES})

add_executable(ionic_image image.cpp)
target_link_libraries(ionic_image ${LAMBDACOMMON_LIBRARIES} ionicengine ${GLFW_LIBRARIES} ${LD_LIBRARY} ${X11_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARIES})

add_executable(ionic_cubes_lightning cubes_lightning.cpp learnopengl/camera.h)
target_link_libraries(ionic_cubes_lightning ${LAMBDACOMMON_LIBRARIES} ionicengine ${CMAKE_THREAD_LIBS_INIT} ${GLFW_LIBRARIES} ${LD_LIBRARY} ${X11_LIBRARIES} ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARIES} ${FREETYPE_LIBRARIES} ${HARFBUZZ_LIBRARIES})

add_executable(ionic_monitors monitors.cpp)
target_link_libraries(ionic_monitors ionicengine ${LAMBDACOMMON_LIBRARIES} ${GLFW_LIBRARIES} ${LD_LIBRARY} ${X11_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARIES})