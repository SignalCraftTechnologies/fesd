#
# Copyright 2023 SignalCraft Technologies Inc.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

### Project Setup #############################################################
cmake_minimum_required(VERSION 3.22.0)
project(fesd C CXX)

### Configure Compiler ########################################################
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


if(NOT WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}")
endif()

add_compile_definitions(FESD_EXPORTS)

find_package(Boost 1.74 REQUIRED)

if (ENABLE_PY_BUILD AND ENABLE_STATIC_BUILD)
    message(FATAL_ERROR "You cannot set both ENABLE_PY_BUILD and ENABLE_STATIC_BUILD to TRUE")
endif()

if(ENABLE_PY_BUILD)
    message("Python Enabled")
    if (WIN32)
        set(PY_VENV_EX "python" )
        set(PY_VENV_PY "${CMAKE_BINARY_DIR}/.cmake-venv/Scripts/python.exe")
    else()
        set(PY_VENV_EX "/usr/bin/python3.12" )
        set(PY_VENV_PY "${CMAKE_BINARY_DIR}/.cmake-venv/bin/python")
    endif()
    
    if (NOT EXISTS "${PY_VENV_PY}")
        execute_process(COMMAND ${PY_VENV_EX} -m venv "${CMAKE_BINARY_DIR}/.cmake-venv" RESULT_VARIABLE rv1)
    
        if(NOT rv1 EQUAL 0)
            message(FATAL_ERROR "Failed to create Python venv")
        endif()
        execute_process(COMMAND "${PY_VENV_PY}" -m pip install --upgrade pip setuptools wheel RESULT_VARIABLE rv2)
        if(NOT rv2 EQUAL 0)
            message(FATAL_ERROR "Failed to bootstrap pip/setuptools/wheel in venv , error message: ${rv2}")
        endif()
    endif()
    set(Python3_EXECUTABLE "${PY_VENV_PY}" CACHE FILEPATH "Python for wheel builds" FORCE)

    find_package(Python3 3.12 COMPONENTS Interpreter Development)
    message("Subsequent builds will use: ${Python3_EXECUTABLE}")
    set(PYBIND11_FINDPYTHON ON) # See: https://github.com/pybind/pybind11/issues/5467
    message("Python fetching content...")
    include(FetchContent)
    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        # Ver 2.12
        GIT_TAG 3e9dfa2866941655c56877882565e7577de6fc7b
    )
    FetchContent_MakeAvailable(pybind11)
    FetchContent_Declare(
        python_cmake_wheel
        GIT_REPOSITORY https://github.com/Klebert-Engineering/python-cmake-wheel.git
        # Ver 1.2.0
        GIT_TAG de39c08a927462820f502bcdf6f5207e11719dbd
    )
    FetchContent_MakeAvailable(python_cmake_wheel)
    
    FetchContent_GetProperties(python_cmake_wheel)
    if(NOT python_cmake_wheel_POPULATED)
        FetchContent_Populate(python_cmake_wheel)
    endif()
    list(PREPEND CMAKE_MODULE_PATH "${python_cmake_wheel_SOURCE_DIR}")
    message("Python Content fetched")
endif()

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

link_directories(${Boost_LIBRARY_DIRS})

if (ENABLE_STATIC_BUILD)
message("Static Build Enabled")
add_compile_definitions(FESD_STATIC_BUILD)
add_library(${PROJECT_NAME} STATIC)
else()
add_compile_definitions(FESD_SHARED_BUILD)
add_library(${PROJECT_NAME} SHARED)
endif()

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/lib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_sources(${PROJECT_NAME}
    PRIVATE
        lib/version.cpp
        lib/FESerialDriver.cpp
        lib/BaseCommander.cpp
        lib/DeviceConnection.cpp
        lib/GeneralProcessor.cpp
        lib/MessageBuilder.cpp
        lib/sc2470/SC2470Commander.cpp
        lib/sc2470/SC2470Processor.cpp
        lib/SerialConsole.cpp
        lib/bindings/FESerialDriver_C.cpp
        lib/Utility.cpp
)


if(DEFINED CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX
        "${CMAKE_SOURCE_DIR}/install"
        CACHE PATH "Where the library will be installed to" FORCE
    )
endif()
set(public_headers
    include/fesd/fesd.hpp
    include/fesd/fesd.h
    include/fesd/config.h
    include/fesd/version.hpp
    include/fesd/FESerialDriver.hpp
    include/fesd/BaseCommander.hpp
    include/fesd/SC2470Commander.hpp
    include/fesd/types/Common.hpp
    include/fesd/types/SC2470.hpp
    include/fesd/types/Exception.hpp
    include/fesd/types/DeviceDetails.hpp
)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${public_headers}")
target_link_libraries(${PROJECT_NAME} PUBLIC ${Boost_LIBRARIES})
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    EXPORT "${PROJECT_NAME}Targets"
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if (NOT ENABLE_PY_BUILD)
add_executable(
    exampleCpp
    example/example.cpp
    lib/version.cpp
    lib/FESerialDriver.cpp
    lib/BaseCommander.cpp
    lib/DeviceConnection.cpp
    lib/GeneralProcessor.cpp
    lib/MessageBuilder.cpp
    lib/sc2470/SC2470Commander.cpp
    lib/sc2470/SC2470Processor.cpp
    lib/SerialConsole.cpp
    lib/bindings/FESerialDriver_C.cpp
    lib/Utility.cpp
)

add_executable(
    helloWorldCpp
    example/hello_world.cpp
    lib/version.cpp
    lib/FESerialDriver.cpp
    lib/BaseCommander.cpp
    lib/DeviceConnection.cpp
    lib/GeneralProcessor.cpp
    lib/MessageBuilder.cpp
    lib/sc2470/SC2470Commander.cpp
    lib/sc2470/SC2470Processor.cpp
    lib/SerialConsole.cpp
    lib/bindings/FESerialDriver_C.cpp
    lib/Utility.cpp
)

add_executable(
    exampleC
    example/example.c
    lib/version.cpp
    lib/FESerialDriver.cpp
    lib/BaseCommander.cpp
    lib/DeviceConnection.cpp
    lib/GeneralProcessor.cpp
    lib/MessageBuilder.cpp
    lib/sc2470/SC2470Commander.cpp
    lib/sc2470/SC2470Processor.cpp
    lib/SerialConsole.cpp
    lib/bindings/FESerialDriver_C.cpp
    lib/Utility.cpp
)

add_executable(
    helloWorldC
    example/hello_world.c
    lib/version.cpp
    lib/FESerialDriver.cpp
    lib/BaseCommander.cpp
    lib/DeviceConnection.cpp
    lib/GeneralProcessor.cpp
    lib/MessageBuilder.cpp
    lib/sc2470/SC2470Commander.cpp
    lib/sc2470/SC2470Processor.cpp
    lib/SerialConsole.cpp
    lib/bindings/FESerialDriver_C.cpp
    lib/Utility.cpp
)


target_link_libraries(exampleCpp PUBLIC ${Boost_LIBRARIES})
target_link_libraries(exampleC PUBLIC)

endif()

if(ENABLE_PY_BUILD)
    message("Python linking...")
    pybind11_add_module(pyfesd lib/bindings/FESerialDriver_Py.cpp)
    target_link_libraries(pyfesd PRIVATE ${PROJECT_NAME} ${Boost_LIBRARIES})
    
    if(WIN32)
        set(WHEEL_DEPLOY_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}")
    else()
        set(WHEEL_DEPLOY_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
    endif()

    message("Python setting up wheel directories...")
    include(python-wheel)
    set(PYTHON_PACKAGE_DIRS "${WHEEL_DEPLOY_DIRECTORY}/wheel.pkg.staging")
    set(PY_PKG_DIR "${PYTHON_PACKAGE_DIRS}/pyfesd")
    
    file(MAKE_DIRECTORY "${PYTHON_PACKAGE_DIRS}")
    file(MAKE_DIRECTORY "${PY_PKG_DIR}")
    file(WRITE "${PY_PKG_DIR}/__init__.py" "from .pyfesd import *\n")

    if (WIN32)
        set(FESD_SRC_NAME "fesd.dll")
    else()
        set(FESD_SRC_NAME "libfesd.so")
    endif()

    message("Python staging wheel pkg files...")
    add_custom_target(stage_pyfesd ALL
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${PY_PKG_DIR}"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "$<TARGET_FILE:pyfesd>" "${PY_PKG_DIR}/"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${WHEEL_DEPLOY_DIRECTORY}/${FESD_SRC_NAME}" "${PY_PKG_DIR}/"
    )

    message("Python generating stubs placeholders...")
    file(WRITE "${PY_PKG_DIR}/__init__.pyi" "")
    file(WRITE "${PY_PKG_DIR}/py.typed" "")
    file(WRITE "${PY_PKG_DIR}/fesd.pyi" "")

    message("Python generating stubs...")
    add_custom_target(gen_stubs ALL
        DEPENDS stage_pyfesd
        # Ensure tool is present in the current Python (venv or system)
        COMMAND "${Python3_EXECUTABLE}" -m pip -q install --upgrade pybind11-stubgen
        # Make the build-tree package importable for pybind11-stubgen:
        COMMAND "${CMAKE_COMMAND}" -E env
                PYTHONPATH="${PYTHON_PACKAGE_DIRS}"
                "${Python3_EXECUTABLE}" -m pybind11_stubgen pyfesd
                    -o "${PYTHON_PACKAGE_DIRS}"                 # emit under build/python/pyfesd/*.pyi
                    --stub-extension pyi               # explicit extension (default is .pyi)
                    --ignore-all-errors                 # optional: avoid failing on doc edge cases
        # PEP 561 marker so type checkers see bundled types
        COMMAND "${CMAKE_COMMAND}" -E touch "${PY_PKG_DIR}/py.typed"
        COMMENT "Generating .pyi stubs for pyfesd with pybind11-stubgen"
    )

    add_dependencies(wheel gen_stubs)

    message("Python adding wheel...")
    message("Wheel will use venv python: ${Python3_EXECUTABLE}")
    add_wheel(pyfesd
    VERSION           1.0.0
    AUTHOR            "Tom Wilson"
    EMAIL             "twilson@signalcraft.com"
    PYTHON_REQUIRES   ">=3.12"
    DESCRIPTION       "Python bindings for FESerialDriver"
    DEPLOY_FILES "${PY_PKG_DIR}/py.typed"
    "${PY_PKG_DIR}/__init__.pyi"
    "${PY_PKG_DIR}/pyfesd.pyi"
    PYTHON_PACKAGE_DIRS "${PY_PKG_DIR}"
    )
endif()
