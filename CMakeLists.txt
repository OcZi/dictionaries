message("Compiling practices of ALgorithm and Data structures ...")

cmake_minimum_required(VERSION 3.30.5)

project(AED_practice DESCRIPTION "A repository of Algorithms and Data structures course in UTEC" LANGUAGES CXX)

SET(SRC ${PROJECT_SOURCE_DIR}/src)

add_executable(
        ${PROJECT_NAME}
        AVL.h
        AVL_Iterator.h
        AVL_Node.h
        HashTable.h
        tester.h
        main.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
${PROJECT_SOURCE_DIR}/inc
)