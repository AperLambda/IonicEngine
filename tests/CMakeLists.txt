cmake_minimum_required(VERSION 3.8)
project(ionicengine_tests)

set(CMAKE_CXX_STANDARD 17)

include_directories(../include)

add_executable(ionic_joysticks joysticks.cpp)
target_link_libraries(ionic_joysticks ${LAMBDACOMMON_LIBRARIES} ionicengine ${GLFW_LIBRARIES} ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARIES})

add_executable(ionic_monitors monitors.cpp)
target_link_libraries(ionic_monitors ${LAMBDACOMMON_LIBRARIES} ionicengine ${GLFW_LIBRARIES} ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARIES})