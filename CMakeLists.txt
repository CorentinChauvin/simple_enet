cmake_minimum_required(VERSION 3.13)
project(test_enet)

# Cmake configuration
add_compile_options(-std=c++2a)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")  # threads support

# Including external packages
add_subdirectory(src/enet)  # Enet
target_compile_options(enet PRIVATE -w)

# Building
file(GLOB TARGET_HPP "src/*.hpp")
file(GLOB TARGET_CPP "src/*.cpp")
set(TARGET_SRC ${TARGET_HPP} ${TARGET_CPP})

add_executable(test_enet ${TARGET_SRC})

target_include_directories(test_enet PRIVATE
  ${PROJECT_SOURCE_DIR}
  src/enet/include
)

target_link_libraries(test_enet
  enet
)

# Compile options
target_compile_options(test_enet PRIVATE
  -Wall -Wextra -pedantic
  "$<$<CONFIG:DEBUG>:-pg>"
  "$<$<CONFIG:RELEASE>:-O3>"
)
