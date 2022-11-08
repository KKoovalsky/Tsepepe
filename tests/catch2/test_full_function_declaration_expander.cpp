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

#include "directory_tree.hpp"

#include <clang/AST/Decl.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchersMacros.h>
#include <clang/Basic/SourceManager.h>

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

struct TestWithOptionsTestData
{
    std::string description;
    Tsepepe::FullFunctionDeclarationExpanderOptions options;
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
            SingleHeaderFileTestData{.description = "From operator overload which has no explicit return type",
                                     .header_file_content = "struct Class\n"
                                                            "{\n"
                                                            "    operator bool();\n"
                                                            "};\n",
                                     .line_with_declaration = 3,
                                     .expected_result = "Class::operator bool()"},
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
            SingleHeaderFileTestData{.description = "From method returning sugarized std:: type",
                                     .header_file_content = "#include <string>\n"
                                                            "struct Class\n"
                                                            "{\n"
                                                            "    std::string get();\n"
                                                            "};\n",
                                     .line_with_declaration = 4,
                                     .expected_result = "std::string Class::get()"},
            SingleHeaderFileTestData{.description = "From method taking sugarized std:: type",
                                     .header_file_content = "#include <string>\n"
                                                            "struct Class\n"
                                                            "{\n"
                                                            "    void get(std::string);\n"
                                                            "};\n",
                                     .line_with_declaration = 4,
                                     .expected_result = "void Class::get(std::string)"},
            SingleHeaderFileTestData{.description = "From method returning an aliased type",
                                     .header_file_content = "#include <string>\n"
                                                            "using String = std::string;\n"
                                                            "struct Class\n"
                                                            "{\n"
                                                            "    String get();\n"
                                                            "};\n",
                                     .line_with_declaration = 5,
                                     .expected_result = "String Class::get()"},
            SingleHeaderFileTestData{.description = "From method taking an aliased type",
                                     .header_file_content = "#include <string>\n"
                                                            "using String = std::string;\n"
                                                            "struct Class\n"
                                                            "{\n"
                                                            "    void get(String);\n"
                                                            "};\n",
                                     .line_with_declaration = 5,
                                     .expected_result = "void Class::get(String)"},
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
            SingleHeaderFileTestData{.description = "Ignores virtual and override keywords",
                                     .header_file_content = "struct Base\n"
                                                            "{\n"
                                                            "    virtual void do_stuff() = 0;\n"
                                                            "};\n"
                                                            "class Derived : public Base\n"
                                                            "{\n"
                                                            "    virtual void do_stuff() override;\n"
                                                            "};\n",
                                     .line_with_declaration = 7,
                                     .expected_result = "void Derived::do_stuff()"},
            SingleHeaderFileTestData{.description = "Ignores pure virtual specifier",
                                     .header_file_content = "struct Base\n"
                                                            "{\n"
                                                            "    virtual void do_stuff() = 0;\n"
                                                            "};\n",
                                     .line_with_declaration = 3,
                                     .expected_result = "void Base::do_stuff()"},
            SingleHeaderFileTestData{.description = "Ignores static keyword",
                                     .header_file_content = "class Class\n"
                                                            "{\n"
                                                            "public:\n"
                                                            "\n"
                                                            "    static Class make();\n"
                                                            "};\n",
                                     .line_with_declaration = 5,
                                     .expected_result = "Class Class::make()"},
            SingleHeaderFileTestData{.description = "Preserves const specifier",
                                     .header_file_content = "class Benc\n"
                                                            "{\n"
                                                            "    void do_stuff() const;\n"
                                                            "};\n",
                                     .line_with_declaration = 3,
                                     .expected_result = "void Benc::do_stuff() const"},
            SingleHeaderFileTestData{.description = "Preserves noexcept specifier",
                                     .header_file_content = "class Benc\n"
                                                            "{\n"
                                                            "    void do_stuff() noexcept;\n"
                                                            "};\n",
                                     .line_with_declaration = 3,
                                     .expected_result = "void Benc::do_stuff() noexcept"},
            SingleHeaderFileTestData{.description = "Preserves noexcept expression",
                                     .header_file_content = "class Benc\n"
                                                            "{\n"
                                                            "    void do_stuff() noexcept(false);\n"
                                                            "};\n",
                                     .line_with_declaration = 3,
                                     .expected_result = "void Benc::do_stuff() noexcept(false)"},
            SingleHeaderFileTestData{.description = "Preserves ref-qualifier",
                                     .header_file_content = "struct Class\n"
                                                            "{\n"
                                                            "    Class& get() &;\n"
                                                            "    Class&& get() &&;\n"
                                                            "    const Class& get() const&;\n"
                                                            "};\n",
                                     .line_with_declaration = 4,
                                     .expected_result = "Class && Class::get() &&"},
            SingleHeaderFileTestData{
                .description = "From multiline declaration",
                .header_file_content = "#include <string>\n"
                                       "#include <vector>\n"
                                       "\n"
                                       "struct Class\n"
                                       "{\n"
                                       "    void foo(unsigned int number,\n"
                                       "             std::string,\n"
                                       "             std::vector<std::string> strings);\n"
                                       "};\n",
                .line_with_declaration = 6,
                .expected_result =
                    "void Class::foo(unsigned int number, std::string, std::vector<std::string> strings)"},
            SingleHeaderFileTestData{.description = "Skips default parameter",
                                     .header_file_content = "struct Class\n"
                                                            "{\n"
                                                            "    void foo(unsigned int i, bool do_yolo = true);\n"
                                                            "};\n",
                                     .line_with_declaration = 3,
                                     .expected_result = "void Class::foo(unsigned int i, bool do_yolo)"},
            SingleHeaderFileTestData{
                .description = "Skips multiple default parameters",
                .header_file_content = "#include <string>\n"
                                       "struct Class\n"
                                       "{\n"
                                       "    struct Nested\n"
                                       "    {\n"
                                       "    };\n"
                                       "\n"
                                       "    void foo(Nested, unsigned i, bool do_yolo = false, "
                                       "std::string s= \"bang\", float val =2.4f);\n"
                                       "};\n",
                .line_with_declaration = 8,
                .expected_result =
                    "void Class::foo(Class::Nested, unsigned int i, bool do_yolo, std::string s, float val)"},
        };

        auto [description, header_file_content, line_with_declaration, expected_result] = GENERATE(values(test_data));

        INFO(description);

        SingleHeaderTestFixture fixture{header_file_content, line_with_declaration};
        CHECK(fully_expand_function_declaration(fixture.get_function_declaration(), fixture.get_source_manager())
              == expected_result);
    }

    SECTION("From function declaration with attribute specifiers")
    {
        GIVEN("Method declaration with attribute specifiers")
        {
            std::string header_file_content{
                "struct Class\n"
                "{\n"
                "[[nodiscard]] [[gnu::const]] int get();\n"
                "};\n"};
            unsigned line_with_declaration{3};

            WHEN("Attribute specifiers are ignored")
            {
                FullFunctionDeclarationExpanderOptions options{.ignore_attribute_specifiers = 1};

                THEN("No attribute specifier appears in the full declaration")
                {
                    SingleHeaderTestFixture fixture{header_file_content, line_with_declaration};
                    CHECK(fully_expand_function_declaration(
                              fixture.get_function_declaration(), fixture.get_source_manager(), options)
                          == "int Class::get()");
                }
            }

            WHEN("Attribute specifiers are not ignored")
            {
                FullFunctionDeclarationExpanderOptions options{.ignore_attribute_specifiers = 0};

                THEN("Attribute specifiers do appear in the full declaration")
                {
                    SingleHeaderTestFixture fixture{header_file_content, line_with_declaration};
                    CHECK(fully_expand_function_declaration(
                              fixture.get_function_declaration(), fixture.get_source_manager(), options)
                          == "[[nodiscard]] [[gnu::const]] int Class::get()");
                }
            }
        }
    }

    SECTION("From function returning a nested type defined in another header")
    {
        GIVEN("Header file with a type definition")
        {
            DirectoryTree dir_tree{"temp"};
            dir_tree.create_file("header1.hpp",
                                 "namespace Namespace {\n"
                                 "struct External\n"
                                 "{\n"
                                 "    struct Nested\n"
                                 "    {\n"
                                 "    };\n"
                                 "};\n"
                                 "}\n");

            AND_GIVEN("Header file with function declaration which uses that type")
            {
                auto path{dir_tree.create_file("yolo.hpp",
                                               "#include \"header1.hpp\"\n"
                                               "namespace Namespace {\n"
                                               "class Caller\n"
                                               "{\n"
                                               "    External::Nested gimme(External::Nested);\n"
                                               "};\n"
                                               "}\n")};
                unsigned line_with_declaration{5};

                WHEN("Full function declaration is expanded")
                {
                    ClangAstFixture fixture{COMPILATION_DATABASE_DIR, {path}};

                    using namespace clang;
                    auto matcher{ast_matchers::functionDecl(isDeclaredAtLine(line_with_declaration))
                                     .bind("function_at_line_matcher")};
                    auto function{fixture.get_first_match<FunctionDecl>(matcher)};
                    auto result{fully_expand_function_declaration(function, fixture.get_source_manager())};

                    THEN("The external type is properly expanded")
                    {
                        CHECK(result
                              == "Namespace::External::Nested Namespace::Caller::gimme(Namespace::External::Nested)");
                    }
                }
            }
        }
    }
    SECTION("From function returning and taking an aliased type defined in another header")
    {
        GIVEN("An alias defined in one header")
        {
            DirectoryTree dir_tree{"temp"};

            dir_tree.create_file("hehe.hpp",
                                 "#include <string_view>\n"
                                 "#include <array>\n"
                                 "namespace Namespace {\n"
                                 "using StringViewArray = std::array<std::string_view, 51>;\n"
                                 "}\n");

            AND_GIVEN("Header file with function declaration which uses that type")
            {
                auto path{dir_tree.create_file("user.hpp",
                                               "#include \"hehe.hpp\"\n"
                                               "\n"
                                               "using namespace Namespace;\n"
                                               "StringViewArray transform(const StringViewArray&);\n")};
                unsigned line_with_declaration{4};

                WHEN("Full function declaration is expanded")
                {
                    ClangAstFixture fixture{COMPILATION_DATABASE_DIR, {path}};

                    using namespace clang;
                    auto matcher{ast_matchers::functionDecl(isDeclaredAtLine(line_with_declaration))
                                     .bind("function_at_line_matcher")};
                    auto function{fixture.get_first_match<FunctionDecl>(matcher)};
                    auto result{fully_expand_function_declaration(function, fixture.get_source_manager())};

                    THEN("The external type is properly expanded")
                    {
                        CHECK(result == "Namespace::StringViewArray transform(const Namespace::StringViewArray &)");
                    }
                }
            }
        }
    }
}
