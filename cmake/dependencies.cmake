function(ProvideRangesV3)

    FetchContent_Declare(
        range_v3
        URL https://github.com/ericniebler/range-v3/archive/refs/tags/0.12.0.tar.gz
        URL_HASH MD5=373a795e450fd1c8251df624be784710
    )
    FetchContent_MakeAvailable(range_v3)

endfunction()

include(FetchContent)
