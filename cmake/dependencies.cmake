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

endfunction()

include(FetchContent)
