cmake_minimum_required(VERSION 3.7...3.18)
project(computeShaderTest)

option(BUILD_SLIME "Enable building subdir1" ON)
option(BUILD_FLUID "Enable building subdir2" ON)

include_directories(include)

if(BUILD_SLIME)
    add_subdirectory(slime)
endif()
if(BUILD_FLUID)
    add_subdirectory(fluid)
endif()

# SET(CPP_COMPILE_FLAGS "-std=c++17")
# set(CMAKE_INSTALL_PREFIX /usr/local/bin)
# set(SRC_FILES
#     main.cpp
# )

# add_executable(computeShaderTest ${SRC_FILES})

# target_link_libraries(computeShaderTest glfw pthread GLEW GLU GL rt Xrandr Xxf86vm Xi Xinerama X11)

# target_compile_options(computeShaderTest PRIVATE "${CPP_COMPILE_FLAGS}")

# install(TARGETS computeShaderTest
#     DESTINATION ${CMAKE_INSTALL_PREFIX}
# )