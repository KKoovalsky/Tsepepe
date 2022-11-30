/**
 * @file        input.hpp
 * @brief       Input for the function definition generator
 */
#ifndef INPUT_HPP
#define INPUT_HPP

#include <filesystem>
#include <string>

#include "generate_function_definitions_code_action.hpp"

namespace Tsepepe::FunctionDefinitionGenerator
{

struct Input
{
    std::unique_ptr<clang::tooling::CompilationDatabase> compilation_database_ptr;
    GenerateFunctionDefinitionsCodeActionParameters parameters;
};

} // namespace Tsepepe::FunctionDefinitionGenerator

#endif /* INPUT_HPP */
