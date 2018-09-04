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
target_link_libraries(ionic_joysticks AperLambda::lambdacommon ionicengine GLFW::GLFW ${LD_LIBRARY} ${X11_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} OpenGL::GL GLEW::GLEW)

add_executable(ionic_image image.cpp)
target_link_libraries(ionic_image AperLambda::lambdacommon ionicengine GLFW::GLFW ${LD_LIBRARY} ${X11_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} OpenGL::GL GLEW::GLEW)

add_executable(ionic_fire_animation fire.cpp)
target_link_libraries(ionic_fire_animation AperLambda::lambdacommon ionicengine GLFW::GLFW ${LD_LIBRARY} ${X11_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} OpenGL::GL GLEW::GLEW)

add_executable(ionic_text_area text_input.cpp)
target_link_libraries(ionic_text_area AperLambda::lambdacommon ionicengine GLFW::GLFW ${LD_LIBRARY} ${X11_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} OpenGL::GL GLEW::GLEW)

add_executable(ionic_cubes_lightning cubes_lightning.cpp learnopengl/camera.h)
target_link_libraries(ionic_cubes_lightning AperLambda::lambdacommon ionicengine ${CMAKE_THREAD_LIBS_INIT} GLFW::GLFW ${LD_LIBRARY} ${X11_LIBRARIES} OpenGL::GL GLEW::GLEW)

add_executable(ionic_monitors monitors.cpp)
target_link_libraries(ionic_monitors ionicengine AperLambda::lambdacommon GLFW::GLFW ${LD_LIBRARY} ${X11_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} OpenGL::GL GLEW::GLEW)

add_executable(ionic_resources_monitor resources_monitor.cpp)
target_link_libraries(ionic_resources_monitor AperLambda::lambdacommon ionicengine GLFW::GLFW OpenGL::GL GLEW::GLEW ${CMAKE_THREAD_LIBS_INIT} ${LD_LIBRARY} ${X11_LIBRARIES} Freetype::Freetype)