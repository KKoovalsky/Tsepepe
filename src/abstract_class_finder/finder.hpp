/**
 * @file        finder.hpp
 * @brief       Declares the abstract class finder, which is a clang AST matcher.
 */
#ifndef FINDER_HPP
#define FINDER_HPP

#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>

#include "input.hpp"

#include "clang_ast_utils.hpp"

namespace Tsepepe::AbstractClassFinder
{

using ReturnCode = int;
ReturnCode find(const Input& input);

} // namespace Tsepepe::AbstractClassFinder

#endif /* FINDER_HPP */
