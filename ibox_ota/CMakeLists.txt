#######################################################################################################################
#  COPYRIGHT
#  -------------------------------------------------------------------------------------------------------------------
#  \verbatim
#  Copyright (c) 2020 by Zhito Co., Ltd. All rights reserved.
#
#                This software is copyright protected and proprietary to Zhito Co., Ltd.
#                Vector Informatik GmbH grants to you only those rights as set out in the license conditions.
#                All other rights remain with Zhito Co., Ltd.
#  \endverbatim
#  -------------------------------------------------------------------------------------------------------------------
#  FILE DESCRIPTION
#  -----------------------------------------------------------------------------------------------------------------*/
#
#         \file  CMakeLists.txt
#        \brief  This file is part of the CMake build-configuration.
#
#      \details  This file is part of the CMake build-configuration.
#
#######################################################################################################################
cmake_minimum_required(VERSION 3.10)

set(TARGET_NAME OTAManager)

# Automatically add the current source- and build directories to the include path.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Collect Static Sources
file(GLOB_RECURSE APPLICATION_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
message(STATUS "APPLICATION_SRCS: ${APPLICATION_SRCS}")

add_executable(${TARGET_NAME} ${APPLICATION_SRCS} ${IDL_GEN_SRCS})

target_include_directories(OTAManager
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

target_link_libraries(OTAManager
PUBLIC
pthread
curl
crypto
ssl
)

install(FILES rsa_public_2048.pem
    DESTINATION etc)

install(TARGETS ${TARGET_NAME}
    DESTINATION bin)



  #install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/etc/  
# DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/install/bin/etc)

message(STATUS "-------------------------------------------------------------")
