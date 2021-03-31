cmake_minimum_required(VERSION 3.7...3.18)
project(computeShaderTest)

SET(CPP_COMPILE_FLAGS "-std=c++17")
set(CMAKE_INSTALL_PREFIX /usr/local/bin)
set(SRC_FILES
    main.cpp
)

add_executable(computeShaderTest ${SRC_FILES})

target_link_libraries(computeShaderTest glfw pthread GLEW GLU GL rt Xrandr Xxf86vm Xi Xinerama X11)

target_compile_options(computeShaderTest PRIVATE "${CPP_COMPILE_FLAGS}")

install(TARGETS computeShaderTest
    DESTINATION ${CMAKE_INSTALL_PREFIX}
)