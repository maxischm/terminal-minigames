cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_EXTENSIONS OFF)

# Setup dependencies:
include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(_project_options
  URL https://github.com/aminya/project_options/archive/refs/tags/v0.20.0.zip
  URL_HASH SHA1=97b2d19b4b713e3fd2e6a6e93dd3622452a3195a
  GIT_PROGRESS ON
)

FetchContent_Declare(ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
    GIT_TAG 9f9571190a9ce66f31189f65b404d5da849d3b03
    GIT_PROGRESS ON
)

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
  set(FTXUI_BUILD_DOCS OFF CACHE INTERNAL "")
  set(FTXUI_BUILD_EXAMPLES OFF CACHE INTERNAL "")
  set(FTXUI_BUILD_TESTS OFF CACHE INTERNAL "")
  set(FTXUI_BUILD_TESTS_FUZZER OFF CACHE INTERNAL "")
  set(FTXUI_ENABLE_INSTALL OFF CACHE INTERNAL "")
  set(FTXUI_CLANG_TIDY OFF CACHE INTERNAL "")
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

### Boost ###
set(BOOST_INCLUDE_LIBRARIES math filesystem system program_options chrono)
set(BOOST_ENABLE_CMAKE ON)
FetchContent_Declare(
  Boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_TAG boost-1.80.0
)
FetchContent_MakeAvailable(Boost)
###       ###

FetchContent_MakeAvailable(_project_options)
include(${_project_options_SOURCE_DIR}/Index.cmake)

project(TerminalMinigames
    VERSION 0.1.0
    LANGUAGES CXX)

find_package(Threads)

add_library(terminalMinigamesLib STATIC 
    "src/main_menu.cpp" 
    "src/main_menu.h" 
    "src/snake_game.cpp" 
    "src/snake_game.h"
    "src/block_breaker.cpp"
    "src/block_breaker.h"
    "src/util/util.cpp"
    "src/util/util.h"
    "src/util/vector2d.h"
    "src/util/vector2d.cpp")
target_include_directories(terminalMinigamesLib 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_system_libraries(terminalMinigamesLib 
    PRIVATE Threads::Threads 
    PUBLIC ftxui::dom 
    PUBLIC ftxui::screen 
    PUBLIC ftxui::component 
    PUBLIC Boost::container_hash
    PUBLIC Boost::chrono
)

### Boost ###
#target_link_libraries(
#    terminalMinigamesLib
#    
#)
###       ###

include(${_project_options_SOURCE_DIR}/src/DynamicProjectOptions.cmake)

add_executable(TerminalMinigames src/main.cpp "src/util/vector2d.cpp")
target_link_system_libraries(TerminalMinigames PRIVATE terminalMinigamesLib)



