cmake_minimum_required(VERSION 3.10)
project(TypeAttack C)

# Forzar arquitectura Win32 (x86)
if(WIN32)
    set(CMAKE_GENERATOR_PLATFORM Win32)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Código fuente
file(GLOB SOURCES "source/*.c")

add_executable(TypeAttack ${SOURCES})

if(WIN32)
    # Incluye headers desde includes/
    include_directories("${CMAKE_SOURCE_DIR}/includes")

    # Enlaza libs desde libs/
    target_link_directories(TypeAttack PRIVATE "${CMAKE_SOURCE_DIR}/libs")

    # Nombre de la lib (pdcurses.lib o libpdcurses.a)
    target_link_libraries(TypeAttack pdcurses)

else()
    find_package(Curses REQUIRED)
    include_directories(${CURSES_INCLUDE_DIR})
    target_link_libraries(TypeAttack ${CURSES_LIBRARIES})
endif()
