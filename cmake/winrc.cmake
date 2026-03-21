macro(add_winrc target)
    cmake_parse_arguments(ARG "" "NAME;DESCRIPTION;COPYRIGHT" "" ${ARGN})
    
    set(_rc_content "#include <windows.h>

#ifndef VS_VERSION_INFO
#define VS_VERSION_INFO 1
#endif

#define _STRINGIFY(x) #x
#define STRINGIFY(x) _STRINGIFY(x)

VS_VERSION_INFO VERSIONINFO
    FILEVERSION    ${PROJECT_VERSION_MAJOR},${PROJECT_VERSION_MINOR},${PROJECT_VERSION_PATCH},${PROJECT_VERSION_TWEAK}
    PRODUCTVERSION ${PROJECT_VERSION_MAJOR},${PROJECT_VERSION_MINOR},${PROJECT_VERSION_PATCH},${PROJECT_VERSION_TWEAK}
{
    BLOCK \"StringFileInfo\"
    {
       // U.S. English - Windows, Multilingual
       BLOCK \"040904E4\"
       {
          VALUE \"FileDescription\", STRINGIFY(${ARG_DESCRIPTION})
          VALUE \"FileVersion\", STRINGIFY(${PROJECT_VERSION})
          VALUE \"ProductName\", STRINGIFY(${ARG_NAME})
          VALUE \"ProductVersion\", STRINGIFY(${PROJECT_VERSION})
          VALUE \"LegalCopyright\", STRINGIFY(${ARG_COPYRIGHT})
        }
    }
    BLOCK \"VarFileInfo\"
    {
        VALUE \"Translation\", 0x409, 1252 // 1252 = 0x04E4
    }
}")

    set(_rc_file ${CMAKE_CURRENT_BINARY_DIR}/${target}_res.rc)
    file(WRITE ${_rc_file} ${_rc_content})
    target_sources(${target} PRIVATE ${_rc_file})
endmacro()