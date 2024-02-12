cmake_minimum_required(VERSION 3.22.1)
project(bank_terminal_cli C)

set(CMAKE_C_STANDARD 11)
enable_testing()

find_package(PostgreSQL REQUIRED)
include_directories(${PostgreSQL_INCLUDE_DIRS})

add_subdirectory(libs/libbcrypt)
add_executable(bank_terminal_cli main.c database.c utils.c terminal.c)
target_link_libraries(bank_terminal_cli ${PostgreSQL_LIBRARY} bcrypt)

add_executable(db_setup database_setup.c database.c)
target_link_libraries(db_setup ${PostgreSQL_LIBRARY})

add_executable(utils_test tests/utils.test.c utils.c)
add_test(NAME utils_test COMMAND utils_test)