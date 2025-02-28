# sets the minimum cmake version
cmake_minimum_required(VERSION 3.10)

#see above
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#designate project
project(CompuTron)

#inlcude your .h file
include_directories(${CMAKE_SOURCE_DIR}/include)

# add your executable components
add_executable(CompuTron src/main.cpp src/computron.cpp)

#################################################

#create test executable using test.cpp
add_executable(my_test test/test.cpp  src/computron.cpp)

#include header in this also
target_include_directories(my_test PRIVATE ${PROJECT_SOURCE_DIR}/include)

#self-explanatory
enable_testing()

#create test
add_test(NAME my_test COMMAND my_test)