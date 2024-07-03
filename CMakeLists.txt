cmake_minimum_required(VERSION 3.5)
project(Star-Trek-Red-Alert VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()



file(GLOB SRC_FILES src/*.cpp)
file(GLOB INC_FILES src/*.hpp)
add_executable(Star-Trek-Red-Alert ${SRC_FILES} ${INC_FILES})

file(GLOB INC_FILES tests/*.cpp)

cmake_policy(SET CMP0074 NEW)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")

include_directories(${CMAKE_SOURCE_DIR}/libs)
include_directories(c:/SFML/include)
include_directories(c:/Catch2-devel)


#set(SFML_ROOT c:/SFML)
#set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")
#Commented out so CMake searches for SFML instead of manually specifying directory.
#Re-implement if there are issues.
find_package(SFML REQUIRED audio network graphics window system)
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(Star-Trek-Red-Alert ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
endif()



if(WIN32)
    file(GLOB BINARY_DEP_DLLS "${SFML_INCLUDE_DIR}/../bin/*.dll")
    file(COPY ${BINARY_DEP_DLLS} DESTINATION ${CMAKE_BINARY_DIR})
    file(GLOB MINGW_DEP_DLLS "C:/mingw64/bin/*.dll")
    file(COPY ${MINGW_DEP_DLLS} DESTINATION ${CMAKE_BINARY_DIR})
endif()
