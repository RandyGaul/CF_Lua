if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    if (MSVC)
        # Normally we should rely on CMAKE_SYSTEM_PROCESSOR
        # but as of CMake 3.19.1, it sets to AMD64 even when targetting
        # Windows ARM64. This needs to be reevaluated in the future.
        set(_TEMP_CHECKVAR CMAKE_SYSTEM_PROCESSOR)
        if ("${CMAKE_GENERATOR}" MATCHES "^Visual Studio")
            set(_TEMP_CHECKVAR CMAKE_GENERATOR_PLATFORM)
            # Note that MSVC_CXX_ARCHITECTURE_ID is an undocumented
            # variable, but unfortunately we need to depends on it
            # as reason stated above.
            # https://gitlab.kitware.com/cmake/cmake/-/issues/15170
        elseif (DEFINED MSVC_CXX_ARCHITECTURE_ID)
            set(_TEMP_CHECKVAR MSVC_CXX_ARCHITECTURE_ID)
        endif ()
        # FIXME: Code below also duplicates IS_X64 architecture assignment.
        if (${_TEMP_CHECKVAR} STREQUAL "ARM64")
            set(IS_ARM64 TRUE)
            set(IS_ARCH ARM64)
        else ()
            set(IS_X64 TRUE)
            set(IS_ARCH x64)
        endif ()
        unset(_TEMP_CHECKVAR)
    else ()
        set(IS_X64 TRUE)
        set(IS_ARCH x64)
    endif ()
else ()
    set(IS_X86 TRUE)
    set(IS_ARCH x86)
endif ()

# Find vcvarsall.bat and look for dynamic runtime DLLs.
if (MSVC)
    if (NOT (MSVC_TOOLSET_VERSION STREQUAL ""))
        set(VSVERSION "${MSVC_TOOLSET_VERSION}")
    elseif ((MSVC_VERSION GREATER 1909) AND (MSVC_VERSION LESS 1920))
        set(VSVERSION "141")
    elseif (MSVC_VERSION EQUAL 1900)
        set(VSVERSION "140")
    elseif (MSVC_VERSION EQUAL 1800)
        set(VSVERSION "120")
    elseif (MSVC_VERSION LESS 1800)
        message(WARNING "Visual Studio 2013 (VS12) or newer is required!")
    else ()
        message(WARNING "Cannot determine Visual Studio version!")
    endif ()

    set(PROGRAMFILES_X86_STR "ProgramFiles(x86)")
    set(PROGRAMFILES_X86 "$ENV{${PROGRAMFILES_X86_STR}}")

    set(VSCOMNTOOLSSTRING "VS${VSVERSION}COMNTOOLS")
    set(VSCOMNTOOLS "$ENV{${VSCOMNTOOLSSTRING}}")

    if (VSCOMNTOOLS STREQUAL "")
        set(VSWHERE_PATH "${PROGRAMFILES_X86}\\Microsoft Visual Studio\\Installer\\vswhere.exe")

        execute_process(COMMAND ${VSWHERE_PATH} "-property" "installationPath" OUTPUT_VARIABLE VS_INSTALL_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)

        if (NOT (VS_INSTALL_DIR STREQUAL ""))
            set(VSCOMNTOOLS "${VS_INSTALL_DIR}\\Common7\\Tools")
        endif ()
    endif ()

    if (VSCOMNTOOLS STREQUAL "")
        message(WARNING "Environment variable VSCOMNTOOLS not defined and vswhere.exe could not be found. Is Visual Studio properly installed?")
    else ()
        # Find VsDevCmd.bat
        get_filename_component(VS_VCVARSALL_BAT ${VSCOMNTOOLS}/VsDevCmd.bat ABSOLUTE)
        if (NOT (EXISTS ${VS_VCVARSALL_BAT}))
            # Find vcvarsall.bat.
            get_filename_component(VS_VCVARSALL_BAT ${VSCOMNTOOLS}/../../VC/Auxiliary/Build/vcvarsall.bat ABSOLUTE)

            if (NOT (EXISTS ${VS_VCVARSALL_BAT}))
                # older VS versions have it here
                get_filename_component(VS_VCVARSALL_BAT ${VSCOMNTOOLS}/../../VC/vcvarsall.bat ABSOLUTE)
            endif ()
        endif ()

        if (EXISTS ${VS_VCVARSALL_BAT})
            set(MSVC_VCVARSALL_BAT ${VS_VCVARSALL_BAT})

            if (VS_VCVARSALL_BAT MATCHES "VsDevCmd")
                message(STATUS "Found VsDevCmd.bat")
                if (IS_X64)
                    set(MSVC_VCVARSALL_BAT_ARG "-arch=amd64")
                elseif (IS_ARM64)
                    set(MSVC_VCVARSALL_BAT_ARG "-arch=arm64 -host_arch=amd64")
                else ()
                    set(MSVC_VCVARSALL_BAT_ARG "-arch=x86")
                endif ()
                math(EXPR VS_TOOLCHAIN_VERSION_MAJOR "${VSVERSION} / 10")
                math(EXPR VS_TOOLCHAIN_VERSION_MINOR "${VSVERSION} % 10")
                set(MSVC_VCVARSALL_BAT_ARG "${MSVC_VCVARSALL_BAT_ARG} -vcvars_ver=${VS_TOOLCHAIN_VERSION_MAJOR}.${VS_TOOLCHAIN_VERSION_MINOR}")
            else ()
                message(STATUS "Found vcvarsall.bat")
                if (IS_X86)
                    set(MSVC_VCVARSALL_BAT_ARG "x86")
                elseif (IS_ARM64)
                    set(MSVC_VCVARSALL_BAT_ARG "x64_arm64")
                else ()
                    set(MSVC_VCVARSALL_BAT_ARG "x86_amd64")
                endif ()
            endif ()
        else ()
            message(WARNING "Could not find VsDevCmd.bat nor vcvarsall.bat")
        endif ()
    endif ()
endif ()