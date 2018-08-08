cmake_minimum_required(VERSION 3.0)


set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)


project(torrebis_bencode)


set(CMAKE_CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED ON)

set(torrebis_bencode_VERSION_MAJOR 1)
set(torrebis_bencode_VERSION_MINOR 0)
set(torrebis_bencode_VERSION_PATCH 0)

add_library(torrebis_bencode MODULE "${PROJECT_SOURCE_DIR}/include/torrebis_bencode/torrebis_bencode.h")

target_include_directories(torrebis_bencode PRIVATE "${PROJECT_SOURCE_DIR}/include")

include(CheckCXXCompilerFlag)

function(enable_cxx_compiler_flag_if_supported flags)
    foreach(flag "${flags}")
        string(FIND "${CMAKE_CXX_FLAGS}" "${flag}" flag_already_set)
        if(flag_already_set EQUAL -1)
            check_cxx_compiler_flag("${flag}" flag_supported)
            if(flag_supported)
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
            endif()
            unset(flag_supported CACHE)
        endif()
    endforeach(flag)
endfunction()


# g++ strict flags
set(gpp_strict_flags "-pedantic" "-Wall" "-Wextra" "-Wcast-align" "-Wcast-qual" "-Wctor-dtor-privacy" "-Wdisabled-optimization" "-Wformat=2" "-Winit-self" "-Wlogical-op" "-Wmissing-declarations" "-Wmissing-include-dirs" "-Wnoexcept" "-Wold-style-cast" "-Woverloaded-virtual" "-Wredundant-decls" "-Wshadow" "-Wsign-conversion" "-Wsign-promo" "-Wstrict-null-sentinel" "-Wstrict-overflow=5" "-Wswitch-default" "-Wundef" "-Werror" "-Wno-unused" "-Wzero-as-null-pointer-constant" "-Wuseless-cast")

# VC++ warning flags
set(vcpp_strict_flags "/W4" "/Wall" "/Wp64" "/WX")

if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
    enable_cxx_compiler_flag_if_supported("${gpp_strict_flags}")
    enable_cxx_compiler_flag_if_supported("${vcpp_strict_flags}")
endif()
