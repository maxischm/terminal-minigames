cmake_minimum_required(VERSION 3.0.0)
project(TerminalMinigames VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

include_directories(
    G:/Repositories/vcpkg/installed/x64-windows/include/
)
link_directories(
    G:/Repositories/vcpkg/installed/x64-windows/lib/
)

add_executable(TerminalMinigames src/main.cpp "src/main_menu.h" "src/snake_game.h")

add_library(terminalMinigamesLib STATIC src/main_menu.cpp "src/snake_game.cpp" "src/main_menu.h" "src/snake_game.h")
target_include_directories(terminalMinigamesLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

find_package(ftxui CONFIG REQUIRED)
target_link_libraries(TerminalMinigames PRIVATE ftxui::dom ftxui::screen ftxui::component)
