cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
project(chroniqueue)

add_compile_options(
  -Wall
  -Werror
)

add_subdirectory(include)

find_package( Boost 1.82 COMPONENTS program_options REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )

add_executable(main main.cpp)

target_link_libraries( main LINK_PUBLIC ${Boost_LIBRARIES} )

target_include_directories(main
    PUBLIC
    ./include
)