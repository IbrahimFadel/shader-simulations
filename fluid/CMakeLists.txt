cmake_minimum_required(VERSION 3.7...3.18)
project(fluidSim)

SET(CPP_COMPILE_FLAGS "-std=c++17")
set(CMAKE_INSTALL_PREFIX /usr/local/bin)
set(SRC_FILES
    main.cpp
    Fluid.cpp
    mouse.cpp
)

add_executable(fluidSim ${SRC_FILES})

target_link_libraries(fluidSim glfw pthread GLEW GLU GL rt Xrandr Xxf86vm Xi Xinerama X11)

target_compile_options(fluidSim PRIVATE "${CPP_COMPILE_FLAGS}")

install(TARGETS fluidSim
    DESTINATION ${CMAKE_INSTALL_PREFIX}
)