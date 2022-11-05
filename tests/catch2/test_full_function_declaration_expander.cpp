/**
 * @file        test_full_function_declaration_expander.cpp
 * @brief       Tests the full expander of the method declaration.
 */
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <initializer_list>
#include <string>

#include "libclang_utils/full_function_declaration_expander.hpp"

#include "clang_ast_fixtures.hpp"
#include "clang/AST/Decl.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchersMacros.h"
#include "clang/Basic/SourceManager.h"

AST_MATCHER_P(clang::FunctionDecl, isDeclaredAtLine, unsigned, line)
{
    const auto& source_manager{Finder->getASTContext().getSourceManager()};
    auto actual_line_with_declaration{source_manager.getPresumedLoc(Node.getBeginLoc()).getLine()};
    return line == actual_line_with_declaration;
};

namespace FullFunctionDeclarationExpanderTest
{
struct SingleHeaderFileTestData
{
    std::string description;
    std::string header_file_content;
    unsigned line_with_declaration;
    std::string expected_result;
};

struct SingleHeaderTestFixture
{
    SingleHeaderTestFixture(const std::string& header_file_content, unsigned line_with_declaration) :
        ast_fixture{header_file_content}, line_with_declaration{line_with_declaration}
    {
    }

    const clang::FunctionDecl* get_function_declaration() const
    {
        using namespace clang;
        auto matcher{
            ast_matchers::functionDecl(isDeclaredAtLine(line_with_declaration)).bind("function_at_line_matcher")};

        auto function{ast_fixture.get_first_match<FunctionDecl>(matcher)};
        return function;
    }

    const clang::SourceManager& get_source_manager() const
    {
        return ast_fixture.get_source_manager();
    }

  private:
    Tsepepe::ClangSingleAstFixture ast_fixture;
    unsigned line_with_declaration;
};
}; // namespace FullFunctionDeclarationExpanderTest

TEST_CASE("Function declarations are expanded fully", "[FullFunctionDeclarationExpander]")
{
    using namespace Tsepepe;
    using namespace FullFunctionDeclarationExpanderTest;

    SECTION("For a method within a single header file")
    {
        static std::initializer_list<SingleHeaderFileTestData> test_data{
            SingleHeaderFileTestData{.description = "From declaration returning a bool",
                                     .header_file_content = "struct Boo\n"
                                                            "{\n"
                                                            "    bool baz();\n"
                                                            "};\n",
                                     .line_with_declaration = 3,
                                     .expected_result = "bool Boo::baz()"},
            SingleHeaderFileTestData{.description = "From declaration inside a namespace",
                                     .header_file_content = "namespace Namespace\n"
                                                            "{\n"
                                                            "struct Yolo\n"
                                                            "{\n"
                                                            "    void foo();\n"
                                                            "};\n"
                                                            "}\n",
                                     .line_with_declaration = 5,
                                     .expected_result = "void Namespace::Yolo::foo()"},
            SingleHeaderFileTestData{.description = "From declaration returning a type from a nested namespace",
                                     .header_file_content = "namespace Namespace\n"
                                                            "{\n"
                                                            "\n"
                                                            "struct Nested\n"
                                                            "{\n"
                                                            "};\n"
                                                            "\n"
                                                            "struct Class\n"
                                                            "{\n"
                                                            "    Nested foo();\n"
                                                            "};\n"
                                                            "\n"
                                                            "}\n",
                                     .line_with_declaration = 10,
                                     .expected_result = "Namespace::Nested Namespace::Class::foo()"},
            SingleHeaderFileTestData{.description = "From declaration with a single named parameter",
                                     .header_file_content = "struct Yolo\n"
                                                            "{\n"
                                                            "    void gimme(unsigned int number);\n"
                                                            "};\n",
                                     .line_with_declaration = 3,
                                     .expected_result = "void Yolo::gimme(unsigned int number)"},
            SingleHeaderFileTestData{
                .description = "From declaration with multiple parameters, some are named, some not",
                .header_file_content = "struct Yolo\n"
                                       "{\n"
                                       "    struct Nested1\n"
                                       "    {\n"
                                       "    };\n"
                                       "\n"
                                       "    struct Nested2\n"
                                       "    {\n"
                                       "    };\n"
                                       "\n"
                                       "    void gimme(Nested1, unsigned int number, const Nested2&);\n"
                                       "};\n",
                .line_with_declaration = 11,
                .expected_result = "void Yolo::gimme(Yolo::Nested1, unsigned int number, const Yolo::Nested2 &)"},
            SingleHeaderFileTestData{.description = "From basic method declaration",
                                     .header_file_content = "class SomeClass\n"
                                                            "{\n"
                                                            "    unsigned int foo();\n"
                                                            "};\n",
                                     .line_with_declaration = 3,
                                     .expected_result = "unsigned int SomeClass::foo()"},
            SingleHeaderFileTestData{.description = "From declaration returning a nested type",
                                     .header_file_content = "class Foo\n"
                                                            "{\n"
                                                            "    class Bar\n"
                                                            "    {\n"
                                                            "    };\n"
                                                            "\n"
                                                            "    Bar yolo();\n"
                                                            "};\n",
                                     .line_with_declaration = 7,
                                     .expected_result = "Foo::Bar Foo::yolo()"},
            SingleHeaderFileTestData{.description = "From method returning multi-nested type",
                                     .header_file_content = "class Class\n"
                                                            "{\n"
                                                            "    class Nested\n"
                                                            "    {\n"
                                                            "        struct EvenMoreNested\n"
                                                            "        {\n"
                                                            "            EvenMoreNested create();\n"
                                                            "        };\n"
                                                            "    };\n"
                                                            "};\n",
                                     .line_with_declaration = 7,
                                     .expected_result =
                                         "Class::Nested::EvenMoreNested Class::Nested::EvenMoreNested::create()"},
            SingleHeaderFileTestData{.description = "From constructor declaration",
                                     .header_file_content = "struct Class\n"
                                                            "{\n"
                                                            "    explicit Class();\n"
                                                            "};\n",
                                     .line_with_declaration = 3,
                                     .expected_result = "Class::Class()"},

            SingleHeaderFileTestData{.description = "From destructor declaration",
                                     .header_file_content = "struct Class\n"
                                                            "{\n"
                                                            "    Class();\n"
                                                            "    ~Class();\n"
                                                            "};\n",
                                     .line_with_declaration = 4,
                                     .expected_result = "Class::~Class()"},
            SingleHeaderFileTestData{.description = "From method returning templated type",
                                     .header_file_content = "#include <variant>\n"
                                                            "namespace Context\n"
                                                            "{\n"
                                                            "    struct One {};\n"
                                                            "    struct Two {};\n"
                                                            "    struct Three {};\n"
                                                            "\n"
                                                            "    struct Class\n"
                                                            "    {\n"
                                                            "        struct Four {};\n"
                                                            "        std::variant<One, Two, Three, Four> gimme();\n"
                                                            "    };\n"
                                                            "};\n",
                                     .line_with_declaration = 11,
                                     .expected_result = "std::variant<Context::One, Context::Two, Context::Three, "
                                                        "Context::Class::Four> Context::Class::gimme()"},
            SingleHeaderFileTestData{.description = "From a plain function declaration",
                                     .header_file_content = "#include <vector>\n"
                                                            "#include <string>\n"
                                                            "unsigned long long foo(std::vector<std::string> names);\n",
                                     .line_with_declaration = 3,
                                     .expected_result = "unsigned long long foo(std::vector<std::string> names)"},
            SingleHeaderFileTestData{.description = "From plain declaration inside a namespace",
                                     .header_file_content = "namespace Namespace\n"
                                                            "{\n"
                                                            "void foo();\n"
                                                            "}\n",
                                     .line_with_declaration = 3,
                                     .expected_result = "void Namespace::foo()"},
        };

        auto [description, header_file_content, line_with_declaration, expected_result] = GENERATE(values(test_data));

        INFO(description);

        SingleHeaderTestFixture fixture{header_file_content, line_with_declaration};
        CHECK(fully_expand_function_declaration(fixture.get_function_declaration(), fixture.get_source_manager())
              == expected_result);
    }
}
/*
    Scenario: Ignores virtual and override keywords
        Given Header file with content
        """
        class Derived : public Base
        {
            virtual void do_stuff() override;
        };
        """
        When Method definition is generated from declaration at line 3
        Then Stdout contains
        """
        void Derived::do_stuff()
        """
        And No errors are emitted

    Scenario: Ignores static keyword
        Given Header file with content
        """
        class Class
        {
        public:
            static Class make();
        }
        """
        When Method definition is generated from declaration at line 4
        Then Stdout contains
        """
        Class Class::make()
        """
        And No errors are emitted

    Scenario: Ignores attributes
        Given Header file with content
        """
        class Yolo
        {
            [[nodiscard]] [[no_unique_address]] int get();
        }
        """
        When Method definition is generated from declaration at line 3
        Then Stdout contains
        """
        int Yolo::get()
        """
        And No errors are emitted

    Scenario: Preserves const specifier
        Given Header file with content
        """
        class Benc
        {
            void do_stuff() const;
        }
        """
        When Method definition is generated from declaration at line 3
        Then Stdout contains
        """
        void Benc::do_stuff() const
        """
        And No errors are emitted

    Scenario: Preserves noexcept specifier
        Given Header file with content
        """
        class Benc
        {
            void do_stuff() noexcept;
        }
        """
        When Method definition is generated from declaration at line 3
        Then Stdout contains
        """
        void Benc::do_stuff() noexcept
        """
        And No errors are emitted

    Scenario: Preserves noexcept expression
        Given Header file with content
        """
        class Benc
        {
            void do_stuff() noexcept(false);
        }
        """
        When Method definition is generated from declaration at line 3
        Then Stdout contains
        """
        void Benc::do_stuff() noexcept(false)
        """
        And No errors are emitted

    Scenario: Preserves ref-qualifier
        Given Header file with content
        """
        struct Class
        {
            Class& get() &;
            Class&& get() &&;
            const Class& get() const&;
        };
        """
        When Method definition is generated from declaration at line 4
        Then Stdout contains
        """
        Class && Class::get() &&
        """
        And No errors are emitted

    Scenario: From plain function declaration
        Given Header file with content
        """
        unsigned long long bar();
        """
        When Method definition is generated from declaration at line 1
        Then Stdout contains
        """
        unsigned long long bar()
        """
        And No errors are emitted

    Scenario: From multiline declaration
        Given Header file with content
        """
        #include <string>
        #include <vector>

        struct Class
        {
            void foo(unsigned int number,
                     std::string,
                     std::vector<std::string> strings);
        };
        """
        When Method definition is generated from declaration at line 6
        Then Stdout contains
        """
        void Class::foo(unsigned int number, std::string, std::vector<std::string> strings)
        """
        And No errors are emitted

    Scenario: From operator overload which has no explicit return type
        Given Header file with content
        """
        struct Class
        {
            operator bool();
        };
        """
        When Method definition is generated from declaration at line 3
        Then Stdout contains
        """
        Class::operator bool()
        """
        And No errors are emitted

    Scenario: Skips default parameter
        Given Header file with content
        """
        struct Class
        {
            void foo(unsigned int i, bool do_yolo = true);
        };
        """
        When Method definition is generated from declaration at line 3
        Then Stdout contains
        """
        void Class::foo(unsigned int i, bool do_yolo)
        """
        And No errors are emitted

    Scenario: Skips multiple default parameters
        Given Header file with content
        """
        struct Class
        {
            struct Nested
            {
            };

            void foo(Nested, unsigned i, bool do_yolo = false, std::string s= "bang", float val =2.4f);
        };
        """
        When Method definition is generated from declaration at line 7
        Then Stdout contains
        """
        void Class::foo(Nested, unsigned i, bool do_yolo, std::string s, float val)
        """
        And No errors are emitted

    Scenario: Raises error if no definition found
        Given Header file with content
        """
        struct Class
        {
            void foo();
        };
        """
        When Method definition is generated from declaration at line 1
        Then Error is raised

    Scenario: From function returning a nested type defined in another header
        Given Header File Called "header1.hpp" With Content
        """
        struct External
        {
            struct Nested
            {
            };
        };
        """
        And Header File called "caller.hpp" With Content
        """
        #include "header1.hpp"
        class Caller
        {
            External::Nested gimme();
        };
        """
        When Method definition is generated from declaration in file "caller.hpp" at line 4
        Then Stdout contains
        """
        External::Nested Caller::gimme()
        """
        And No errors are emitted

*/
