cmake_minimum_required(VERSION 3.7...3.18)
project(slimeSim)

SET(CPP_COMPILE_FLAGS "-std=c++17")
set(CMAKE_INSTALL_PREFIX /usr/local/bin)
set(SRC_FILES
    main.cpp
)

add_executable(slimeSim ${SRC_FILES})

target_link_libraries(slimeSim glfw pthread GLEW GLU GL rt Xrandr Xxf86vm Xi Xinerama X11)

target_compile_options(slimeSim PRIVATE "${CPP_COMPILE_FLAGS}")

install(TARGETS slimeSim
    DESTINATION ${CMAKE_INSTALL_PREFIX}
)