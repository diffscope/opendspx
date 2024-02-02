cmake_minimum_required(VERSION 3.17)

project(opendspx VERSION 0.0.1.4 LANGUAGES C CXX)

# ----------------------------------
# Build Options
# ----------------------------------
option(OPENDSPX_BUILD_STATIC "Build static library" OFF)
option(OPENDSPX_BUILD_TESTS "Build test cases" OFF)
option(OPENDSPX_INSTALL "Install library" ON)

# ----------------------------------
# CMake Settings
# ----------------------------------
if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

if(NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif()

if(NOT DEFINED CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif()

if(NOT DEFINED CMAKE_BUILD_SHARE_DIR)
    set(CMAKE_BUILD_SHARE_DIR ${CMAKE_BINARY_DIR}/share)
endif()

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /manifest:no")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /manifest:no")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /manifest:no")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")

    if(NOT DEFINED CMAKE_DEBUG_POSTFIX)
        set(CMAKE_DEBUG_POSTFIX "d")
    endif()
endif()

if(OPENDSPX_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)
endif()

# ----------------------------------
# Project Variables
# ----------------------------------
set(OPENDSPX_VERSION ${PROJECT_VERSION})
set(OPENDSPX_INSTALL_NAME ${PROJECT_NAME})

# ----------------------------------
# Main Project
# ----------------------------------
if(TRUE)
    add_subdirectory(3rdparty)

    # Add library
    if(OPENDSPX_BUILD_STATIC)
        add_library(${PROJECT_NAME} STATIC)
        target_compile_definitions(${PROJECT_NAME} PUBLIC OPENDSPX_STATIC)
    else()
        add_library(${PROJECT_NAME} SHARED)
    endif()

    add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

    # Add sources
    file(GLOB_RECURSE _src include/opendspx/*.h src/*.h src/*.cpp)
    target_sources(${PROJECT_NAME} PRIVATE ${_src})

    # Add features
    target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20 cxx_std_17)

    # Add options
    target_compile_definitions(${PROJECT_NAME} PRIVATE OPENDSPX_LIBRARY)

    # Include directories
    target_include_directories(${PROJECT_NAME} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    )
    target_include_directories(${PROJECT_NAME} PRIVATE include/opendspx)

    # Link libraries
    target_link_libraries(${PROJECT_NAME} PRIVATE QMidi)

    find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Core)

    find_package(QNrbf REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE QNrbfFormat)

    find_package(stdutau REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE stdutau::stdutau)

    if (QT_VERSION_MAJOR GREATER_EQUAL 6)
        find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core5Compat REQUIRED)
        target_link_libraries(${PROJECT_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Core5Compat)
    endif()

    # Add pre-build actions
    find_package(QasTool REQUIRED)
    file(GLOB_RECURSE _json_headers include/opendspx/qdspx*.h)
    qas_wrap_cpp(_qasc_src ${_json_headers} TARGET ${PROJECT_NAME})
    target_sources(${PROJECT_NAME} PRIVATE ${_qasc_src})
    target_link_libraries(${PROJECT_NAME} PUBLIC qastool::core)

    # Add platform specific
    if(WIN32)
        set(RC_DESCRIPTION "DiffScope Data Exchange Format")
        set(RC_COPYRIGHT "Copyright 2023-2024 OpenVPI")
        include("cmake/winrc.cmake")
    endif()

    if(OPENDSPX_INSTALL)
        target_include_directories(${PROJECT_NAME} PUBLIC
            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
        )

        install(TARGETS ${PROJECT_NAME}
            EXPORT ${OPENDSPX_INSTALL_NAME}Targets
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" OPTIONAL
            LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" OPTIONAL
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" OPTIONAL
        )
    endif()
endif()

if(OPENDSPX_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(OPENDSPX_INSTALL)
    # Add install target
    set(_install_dir ${CMAKE_INSTALL_LIBDIR}/cmake/${OPENDSPX_INSTALL_NAME})

    # Add version file
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${OPENDSPX_INSTALL_NAME}ConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )

    # Add configuration file
    configure_package_config_file(
        ${CMAKE_CURRENT_LIST_DIR}/${OPENDSPX_INSTALL_NAME}Config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/${OPENDSPX_INSTALL_NAME}Config.cmake"
        INSTALL_DESTINATION ${_install_dir}
        NO_CHECK_REQUIRED_COMPONENTS_MACRO
    )

    # Install cmake files
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${OPENDSPX_INSTALL_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${OPENDSPX_INSTALL_NAME}ConfigVersion.cmake"
        DESTINATION ${_install_dir}
    )

    # Install cmake targets files
    install(EXPORT ${OPENDSPX_INSTALL_NAME}Targets
        FILE "${OPENDSPX_INSTALL_NAME}Targets.cmake"
        NAMESPACE opendspx::
        DESTINATION ${_install_dir}
    )

    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.h"
    )
endif()