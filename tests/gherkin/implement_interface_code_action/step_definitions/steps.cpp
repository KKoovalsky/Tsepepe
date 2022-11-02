/**
 * @file	steps.cpp
 * @brief	Cucumber step definitions for the implement interface code action tests.
 */

#include <boost/test/tools/old/interface.hpp>
#include <memory>
#include <stdexcept>

#include <boost/test/unit_test.hpp>
#include <cucumber-cpp/autodetect.hpp>

#include <clang/Tooling/CompilationDatabase.h>

#include "directory_tree.hpp"
#include "implement_interface_code_action.hpp"

using cucumber::ScenarioScope;
using namespace Tsepepe;

struct Context
{
    Context() :
        directory_tree{"temp"},
        compilation_database{clang::tooling::CompilationDatabase::loadFromDirectory(
            COMPILATION_DATABASE_DIR, // Supplied within CMakeLists.txt
            error_message)}
    {
        if (compilation_database == nullptr)
            throw std::runtime_error{"Failed to load compilation database from: " COMPILATION_DATABASE_DIR ": "
                                     + error_message};
    }

    DirectoryTree directory_tree;
    std::string error_message;
    std::shared_ptr<clang::tooling::CompilationDatabase> compilation_database;
    std::string result;
};

GIVEN("^Header file \"([^\"]*)\" with content$", (std::string file, std::string content))
{
    cucumber::ScenarioScope<Context> context;
    context->directory_tree.create_file(std::move(file), std::move(content));
}

WHEN("We want to implement \"(.*)\" interface for file \"(.*)\" with line at cursor (\\d+)$",
     (std::string iface_name, std::string file, unsigned line_number))
{
    cucumber::ScenarioScope<Context> context;
    ImplementIntefaceCodeActionLibclangBased action{context->compilation_database};
    auto file_content{context->directory_tree.load_file(file)};
    context->result = action.apply(RootDirectory{"temp"},
                                   FileContentConstRef{file_content},
                                   InterfaceName{iface_name},
                                   CursorPositionLine{line_number});
}

THEN("^The result is$", (std::string expected_result))
{
    cucumber::ScenarioScope<Context> context;
    BOOST_REQUIRE(context->result == expected_result);
}

