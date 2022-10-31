function(ProvideRangesV3)

    FetchContent_Declare(
        range_v3
        URL https://github.com/ericniebler/range-v3/archive/refs/tags/0.12.0.tar.gz
        URL_HASH MD5=373a795e450fd1c8251df624be784710
    )
    FetchContent_MakeAvailable(range_v3)

endfunction()

function(ProvideNamedType)

    FetchContent_Declare(
        NamedType
        GIT_REPOSITORY https://github.com/joboccara/NamedType.git
        GIT_TAG 76668abe09807f92a695ee5e868f9719e888e65f
    )
    FetchContent_MakeAvailable(NamedType)

function(ProvideUniversalCodeGrep)

    find_library(libpcre NAMES pcre2-8 pcre2-16 pcre2-32 REQUIRED)
    find_program(MAKE make REQUIRED)
    find_program(LIBTOOLIZE libtoolize REQUIRED)
    find_program(ACLOCAL aclocal REQUIRED)
    find_program(AUTOHEADER autoheader REQUIRED)
    find_program(AUTOMAKE automake REQUIRED)
    find_program(AUTOCONF autoconf REQUIRED)
    
    ExternalProject_Add(
        UniversalCodeGrep
        GIT_REPOSITORY https://github.com/gvansickle/ucg.git
        GIT_TAG cbb185e8adad6546b7e1c5e9ca59a81f98dca49f
        INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/bin
        CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
        BUILD_COMMAND make
        BUILD_IN_SOURCE True
        INSTALL_COMMAND make install 
        STEP_TARGETS build
    )

    ExternalProject_Add_Step(
        UniversalCodeGrep
        bootstrap 
        COMMAND libtoolize &&
                    aclocal &&
                    autoheader &&
                    automake --add-missing &&
                    autoconf
        DEPENDEES download
        DEPENDERS configure
        WORKING_DIRECTORY <SOURCE_DIR>
    )


    add_library(UniversalCodeGrepLibrary INTERFACE)
    add_library(UniversalCodeGrep::UniversalCodeGrep ALIAS UniversalCodeGrepLibrary)
    add_dependencies(UniversalCodeGrepLibrary UniversalCodeGrep-build)

endfunction()

include(FetchContent)
include(ExternalProject)
