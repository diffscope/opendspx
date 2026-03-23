cmake_minimum_required(VERSION 3.17)

project(opendspx VERSION 0.0.1.5 LANGUAGES C CXX)

# ----------------------------------
# Build Options
# ----------------------------------
option(OPENDSPX_BUILD_STATIC "Build static library" OFF)
option(OPENDSPX_BUILD_TESTS "Build test cases" OFF)
option(OPENDSPX_BUILD_SERIALIZER "Build serializer" ON)
option(OPENDSPX_BUILD_CONVERTER "Build converter" ON)
option(OPENDSPX_INSTALL "Install library" ON)

# ----------------------------------
# CMake Settings
# ----------------------------------
if (NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif ()

if (NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif ()

if (NOT DEFINED CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif ()

if (NOT DEFINED CMAKE_BUILD_SHARE_DIR)
    set(CMAKE_BUILD_SHARE_DIR ${CMAKE_BINARY_DIR}/share)
endif ()

if (MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /manifest:no")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /manifest:no")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /manifest:no")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")

    if (NOT DEFINED CMAKE_DEBUG_POSTFIX)
        set(CMAKE_DEBUG_POSTFIX "d")
    endif ()
endif ()

if (OPENDSPX_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)
endif ()

# ----------------------------------
# Project Variables
# ----------------------------------
set(OPENDSPX_VERSION ${PROJECT_VERSION})
set(OPENDSPX_INSTALL_NAME ${PROJECT_NAME})

# ----------------------------------
# Main Project
# ----------------------------------
set(CMAKE_POSITION_INDEPENDENT_CODE on)

set(OPENDSPX_MODEL_TARGET ${PROJECT_NAME}model)
set(OPENDSPX_SERIALIZER_TARGET ${PROJECT_NAME}serializer)
set(OPENDSPX_CONVERTER_TARGET ${PROJECT_NAME}converter)
set(CURRENT_YEAR)
string(TIMESTAMP CURRENT_YEAR "%Y")

find_package(QT NAMES Qt6 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(stdcorelib CONFIG REQUIRED)
if (WIN32)
    include(cmake/winrc.cmake)
endif()

if (TRUE)
    # Add library
    add_library(${OPENDSPX_MODEL_TARGET} INTERFACE)
    add_library(${PROJECT_NAME}::model ALIAS ${OPENDSPX_MODEL_TARGET})

    # Add features
    target_compile_features(${OPENDSPX_MODEL_TARGET} INTERFACE cxx_std_20 cxx_std_17)

    # Include directories
    target_include_directories(${OPENDSPX_MODEL_TARGET} INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    )

    # Link libraries
    target_link_libraries(${OPENDSPX_MODEL_TARGET} INTERFACE Qt${QT_VERSION_MAJOR}::Core)
    target_link_libraries(${OPENDSPX_MODEL_TARGET} INTERFACE nlohmann_json::nlohmann_json)

    if (OPENDSPX_INSTALL)
        target_include_directories(${OPENDSPX_MODEL_TARGET} INTERFACE
            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
        )

        install(TARGETS ${OPENDSPX_MODEL_TARGET}
                EXPORT ${OPENDSPX_INSTALL_NAME}Targets
                RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" OPTIONAL
                LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" OPTIONAL
                ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" OPTIONAL
        )
    endif ()
endif ()

macro(_opendspx_add_library identifier)
    string(TOUPPER "${identifier}" IDENTIFIER)
    if (OPENDSPX_BUILD_STATIC)
        add_library(${OPENDSPX_${IDENTIFIER}_TARGET} STATIC)
        target_compile_definitions(${OPENDSPX_${IDENTIFIER}_TARGET} PUBLIC OPENDSPX_${IDENTIFIER}_STATIC)
    else ()
        add_library(${OPENDSPX_${IDENTIFIER}_TARGET} SHARED)
    endif ()

    add_library(${PROJECT_NAME}::${identifier} ALIAS ${OPENDSPX_${IDENTIFIER}_TARGET})

    # Add sources
    file(GLOB_RECURSE _src include/opendspx${identifier}/*.h src/${identifier}/*.h src/${identifier}/*.cpp)
    target_sources(${OPENDSPX_${IDENTIFIER}_TARGET} PRIVATE ${_src})

    # Add features
    target_compile_features(${OPENDSPX_${IDENTIFIER}_TARGET} PRIVATE cxx_std_20 cxx_std_17)

    # Add options
    target_compile_definitions(${OPENDSPX_${IDENTIFIER}_TARGET} PRIVATE OPENDSPX_${IDENTIFIER}_LIBRARY)

    # Include directories
    target_include_directories(${OPENDSPX_${IDENTIFIER}_TARGET} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    )
    target_include_directories(${OPENDSPX_${IDENTIFIER}_TARGET} PRIVATE include/opendspx${identifier})

    if(IDENTIFIER EQUAL CONVERTER)
        target_link_libraries(${OPENDSPX_${IDENTIFIER}_TARGET} PUBLIC Qt${QT_VERSION_MAJOR}::Core)
    endif()
    target_link_libraries(${OPENDSPX_${IDENTIFIER}_TARGET} PUBLIC ${OPENDSPX_MODEL_TARGET})
    target_link_libraries(${OPENDSPX_${IDENTIFIER}_TARGET} PRIVATE stdcorelib::stdcorelib)

    # Add platform specific
    if (WIN32)
        add_winrc(${OPENDSPX_${IDENTIFIER}_TARGET}
            NAME "${PROJECT_NAME}${identifier}"
            DESCRIPTION "${PROJECT_NAME} ${identifier}"
            COPYRIGHT "Copyright 2023-2024 Team OpenVPI"
        )
    endif ()

    if (OPENDSPX_INSTALL)
        target_include_directories(${OPENDSPX_${IDENTIFIER}_TARGET} PUBLIC
            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
        )

        install(TARGETS ${OPENDSPX_${IDENTIFIER}_TARGET}
            EXPORT ${OPENDSPX_INSTALL_NAME}Targets
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" OPTIONAL
            LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" OPTIONAL
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" OPTIONAL
        )
    endif ()
endmacro()

if (OPENDSPX_BUILD_SERIALIZER)
    _opendspx_add_library(serializer)
endif()

if (OPENDSPX_BUILD_CONVERTER)
    find_package(wolf-midi CONFIG REQUIRED)
    _opendspx_add_library(converter)
    target_link_libraries(${OPENDSPX_CONVERTER_TARGET} PRIVATE wolf-midi::wolf-midi)
endif()

if (OPENDSPX_BUILD_TESTS)
    add_subdirectory(tests)
endif ()

if (OPENDSPX_INSTALL)
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
endif ()