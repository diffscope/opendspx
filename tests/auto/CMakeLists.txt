project(test_auto LANGUAGES CXX)

find_package(Boost CONFIG COMPONENTS unit_test_framework)

# Fatal rather than a warning and a return. Somebody who set OPENDSPX_BUILD_TESTS=ON and got a
# green build with no tests in it has been told nothing.
if(NOT Boost_FOUND)
    message(FATAL_ERROR
        "OPENDSPX_BUILD_TESTS is on but Boost.Test was not found. Install it, or point CMake at "
        "it with -DCMAKE_PREFIX_PATH, or turn the tests off with -DOPENDSPX_BUILD_TESTS=OFF.")
endif()

# One binary over all the modules, so a case is free to reach across them -- the serializer
# suites read models the model layer built, and there is no reason to start a second process for
# that. Each module's sources come in only when that module was built.
file(GLOB _src *.cpp)
set(_libs opendspx::model)

if(OPENDSPX_BUILD_SERIALIZER)
    file(GLOB _serializer_src serializer/*.cpp)
    list(APPEND _src ${_serializer_src})
    list(APPEND _libs opendspx::serializer)
endif()

if(OPENDSPX_BUILD_INTERPOLATOR)
    file(GLOB _interpolator_src interpolator/*.cpp)
    list(APPEND _src ${_interpolator_src})
    list(APPEND _libs opendspx::interpolator)
endif()

if(OPENDSPX_BUILD_CONVERTER)
    file(GLOB _converter_src converter/*.cpp)
    list(APPEND _src ${_converter_src})
    list(APPEND _libs opendspx::converter)
endif()

add_executable(${PROJECT_NAME} ${_src})

# The project's own libraries first. Boost.Test usually comes out of a vcpkg tree, and that tree
# is one include directory for everything in it -- including, quite possibly, an older copy of a
# dependency this project also builds against. Listing it second keeps ours in front.
target_link_libraries(${PROJECT_NAME} PRIVATE ${_libs} Boost::unit_test_framework)

# The serializer suites reach into the private declarative headers to test the mapping machinery
# on its own, rather than only through whole documents.
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_EXTENSIONS OFF
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# One entry rather than one per suite. The whole binary is a second, and a process start per
# suite costs more than the suites it would be splitting.
#
# Note that ctest exits 0 when it finds no tests at all, so anything running this in anger wants
# --no-tests=error, or it is back to a green run of nothing.
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

# The default is 1500 seconds, long enough for a hang to pass for slowness. Well under a second
# here, so anything still running at a minute is stuck.
set_tests_properties(${PROJECT_NAME} PROPERTIES TIMEOUT 60)
