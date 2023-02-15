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

# Building the server
add_executable(server
  src/net/server.cpp
  src/net/base.cpp
  src/net/packet.cpp
)
target_include_directories(server PRIVATE
  ${PROJECT_SOURCE_DIR}
  src/enet/include
  src/cereal/include
)
target_link_libraries(server
  enet
)
target_compile_options(server PRIVATE
  -Wall -Wextra -pedantic
  "$<$<CONFIG:DEBUG>:-pg>"
  "$<$<CONFIG:RELEASE>:-O3>"
)

# Building the client
add_executable(client
  src/net/client.cpp
  src/net/base.cpp
  src/net/packet.cpp
)
target_include_directories(client PRIVATE
  ${PROJECT_SOURCE_DIR}
  src/enet/include
  src/cereal/include
)
target_link_libraries(client
  enet
)
target_compile_options(client PRIVATE
  -Wall -Wextra -pedantic
  "$<$<CONFIG:DEBUG>:-pg>"
  "$<$<CONFIG:RELEASE>:-O3>"
)
