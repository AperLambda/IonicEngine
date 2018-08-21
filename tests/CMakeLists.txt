cmake_minimum_required(VERSION 3.8)
project(ionicengine_tests)

set(CMAKE_CXX_STANDARD 17)

include_directories(../include)

find_package(X11)
find_package(Threads)

set(LD_LIBRARY "")

if (NOT WIN32)
	set(LD_LIBRARY "-ldl")
endif()

add_executable(ionic_joysticks joysticks.cpp)
target_link_libraries(ionic_joysticks ionicengine ${LAMBDACOMMON_LIBRARIES} ${GLFW_LIBRARIES} ${LD_LIBRARY} ${X11_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARIES})

add_executable(ionic_monitors monitors.cpp)
target_link_libraries(ionic_monitors ionicengine ${LAMBDACOMMON_LIBRARIES} ${GLFW_LIBRARIES} ${LD_LIBRARY} ${X11_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARIES})