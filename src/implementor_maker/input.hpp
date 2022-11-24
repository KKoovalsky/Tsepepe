/**
 * @file        input.hpp
 * @brief       Input for the pure virtual functions extractor.
 */
#ifndef INPUT_HPP
#define INPUT_HPP

#include <filesystem>
#include <string>

#include "implement_interface_code_action.hpp"

namespace Tsepepe::ImplementorMaker
{

struct Input
{
    std::unique_ptr<clang::tooling::CompilationDatabase> compilation_database_ptr;
    ImplementInterfaceCodeActionParameters parameters;
};

} // namespace Tsepepe::ImplementorMaker

#endif /* INPUT_HPP */
