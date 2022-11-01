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

function(ProvideCucumberCpp)
    
    option(CUKE_ENABLE_BOOST_TEST   "Enable Boost.Test framework for Cucumber-CPP" OFF)
    option(CUKE_USE_STATIC_BOOST    "Statically link Boost (except boost::test) with Cucumber-CPP" OFF)
    option(CUKE_USE_STATIC_GTEST    "Statically link Google Test with Cucumber-CPP" OFF)
    option(CUKE_TESTS_E2E           "Enable end-to-end tests for Cucumber-CPP" OFF)
    option(CUKE_TESTS_UNIT          "Enable unit tests for Cucumber-CPP" OFF)
    option(CUKE_ENABLE_GTEST        "Enable Google Test framework for Cucumber-CPP" OFF)
    option(CUKE_ENABLE_QT           "Enable Qt framework for Cucumber-CPP" OFF)

    set(patch ${TSEPEPE_CMAKE_DIR}/0001_cucumber_cpp_fix_asio_std_exchange_error.patch)
    FetchContent_Declare(
        CucumberCpp
        GIT_REPOSITORY https://github.com/cucumber/cucumber-cpp.git
        GIT_TAG 15d73be1759d920206907793720d1e30902428df
        # We need this patch step, because we need Boost as well in Tsepepe project, and it leads to some weird
        # behavior when resolving includes.
        # The patch step will try to apply the patch, and in case it fails it will try to check if the reversed patch
        # applies. If it does, then we know that the patch is correctly applied, thus the step is successful.
        PATCH_COMMAND git apply ${patch} || git apply ${patch} --reverse --check
    )
    FetchContent_MakeAvailable(CucumberCpp)

endfunction()

include(FetchContent)
set(TSEPEPE_CMAKE_DIR ${CMAKE_CURRENT_LIST_DIR})
