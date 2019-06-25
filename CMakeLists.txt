# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -pthread")

#project(SEALExamples VERSION 3.3.0 LANGUAGES CXX)
project(gene VERSION 1.0 LANGUAGES CXX)
# Executable will be in ../bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin)
  
set(
    SOURCES
    gene.cpp
    networking.cpp
    gene_random.cpp
)

add_executable(gene_server     server.cpp  ${SOURCES})
add_executable(gene_client client.cpp  ${SOURCES})

#target_sources(sealexamples
#    PRIVATE
#        1_bfv_basics.cpp
#        2_encoders.cpp
#        3_levels.cpp
#        4_ckks_basics.cpp
#        5_rotation.cpp
#        6_performance.cpp
#)
find_package(Boost 1.67 REQUIRED COMPONENTS system thread filesystem regex)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIR})

#target_link_libraries(gene ${Boost_LIBRARIES})
# Import Microsoft SEAL
find_package(SEAL 3.3.0 EXACT REQUIRED)

# Link Microsoft SEAL
target_link_libraries(gene_client SEAL::seal ${Boost_LIBRARIES})
target_link_libraries(gene_server SEAL::seal ${Boost_LIBRARIES})