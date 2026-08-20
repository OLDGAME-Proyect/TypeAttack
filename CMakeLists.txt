cmake_minimum_required(VERSION 3.10)
project(TypeAttack C)

if(WIN32)
      set(CMAKE_GENERATOR_PLATFORM Win32)
 endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)









add_definitions(-D_CRT_SECURE_NO_WARNINGS)

file(GLOB SOURCES "source/*.c")

if(WIN32)
    set(RESOURCES "resource.rc")
else()
    set(RESOURCES "")
endif()

add_executable(TypeAttack ${SOURCES} ${RESOURCES})

if(WIN32)
    include_directories("${CMAKE_SOURCE_DIR}/includes")
    target_link_directories(TypeAttack PRIVATE "${CMAKE_SOURCE_DIR}/libs")
    target_link_libraries(TypeAttack pdcurses advapi32)

    add_custom_command(TARGET TypeAttack POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/words"

            "$<TARGET_FILE_DIR:TypeAttack>/words"


        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/congrats"

            "$<TARGET_FILE_DIR:TypeAttack>/congrats"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different

            "${CMAKE_SOURCE_DIR}/loser"

            "$<TARGET_FILE_DIR:TypeAttack>/loser"

)
   else()
         find_package(Curses REQUIRED)
    include_directories(${CURSES_INCLUDE_DIR})
    
    target_link_libraries(TypeAttack ${CURSES_LIBRARIES})
endif()
