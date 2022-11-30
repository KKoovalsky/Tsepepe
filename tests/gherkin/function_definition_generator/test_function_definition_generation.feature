Feature: Generates function definitions

    Scenario: From basic method declaration
        Given Header file with content
        """
        class SomeClass
        {
            unsigned int foo();
        };
        """
        When Method definition is generated from declaration at line 3
        Then Stdout contains
        """
        unsigned int SomeClass::foo()
        """
        And No errors are emitted

    Scenario: From declaration returning a nested type
        Given Header file with content
        """
        class Foo
        {
            class Bar
            {
            };

            Bar yolo();
        };
        """
        When Method definition is generated from declaration at line 7
        Then Stdout contains
        """
        Foo::Bar Foo::yolo()
        """
        And No errors are emitted

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


    Scenario: From method returning multi-nested type
        Given Header File With Content
        """
        class Class
        {
            class Nested
            {
                struct EvenMoreNested
                {
                    EvenMoreNested create();
                };
            };
        };
        """
        When Method definition is generated from declaration at line 7
        Then Stdout contains
        """
        Class::Nested::EvenMoreNested Class::Nested::EvenMoreNested::create()
        """
        And No errors are emitted

    Scenario: Ignores virtual and override keywords
        Given Header file with content
        """
        class Base
        {
            virtual void do_stuff();
        };
        class Derived : public Base
        {
            virtual void do_stuff() override;
        };
        """
        When Method definition is generated from declaration at line 7
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
        };
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
            [[nodiscard]] [[gnu::const]] int get();
        };
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
        };
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
        };
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
        };
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


    Scenario: From declaration returning a bool
        Given Header file with content
        """
        bool baz();
        """
        When Method definition is generated from declaration at line 1
        Then Stdout contains
        """
        bool baz()
        """
        And No errors are emitted


    Scenario: From declaration inside a namespace
        Given Header file with content
        """
        namespace Namespace
        {
        void foo();
        };
        """
        When Method definition is generated from declaration at line 3
        Then Stdout contains
        """
        void Namespace::foo()
        """
        And No errors are emitted

    Scenario: From declaration returning a type from a nested namespace
        Given Header file with content
        """
        namespace Namespace
        {

        struct Nested
        {
        };

        Nested foo();

        }
        """
        When Method definition is generated from declaration at line 8
        Then Stdout contains
        """
        Namespace::Nested Namespace::foo()
        """
        And No errors are emitted

    Scenario: From declaration with a single named parameter
        Given Header file with content
        """
        struct Yolo
        {
            void gimme(unsigned int number);
        };
        """
        When Method definition is generated from declaration at line 3
        Then Stdout contains
        """
        void Yolo::gimme(unsigned int number)
        """
        And No errors are emitted

    Scenario: From declaration with multiple parameters, some are named, some not
        Given Header file with content
        """
        struct Yolo
        {
            struct Nested1
            {
            };

            struct Nested2
            {
            };

            void gimme(Nested1, unsigned int number, const Nested2&);
        };
        """
        When Method definition is generated from declaration at line 11
        Then Stdout contains
        """
        void Yolo::gimme(Nested1, unsigned int number, const Nested2 &)
        """
        And No errors are emitted


    Scenario: From constructor declaration
        Given Header file with content
        """
        struct Class
        {
            explicit Class();
        };
        """
        When Method definition is generated from declaration at line 3
        Then Stdout contains
        """
        Class::Class()
        """
        And No errors are emitted

    Scenario: From destructor declaration
        Given Header file with content
        """
        struct Class
        {
            Class();
            ~Class();
        };
        """
        When Method definition is generated from declaration at line 4
        Then Stdout contains
        """
        Class::~Class()
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
        #include <string>
        struct Class
        {
            struct Nested
            {
            };

            void foo(Nested, unsigned i, bool do_yolo = false, std::string s= "bang", float val =2.4f);
        };
        """
        When Method definition is generated from declaration at line 8
        Then Stdout contains
        """
        void Class::foo(Nested, unsigned int i, bool do_yolo, std::string s, float val)
        """
        And No errors are emitted

    Scenario: Does not include scope in parameters nested in the same class scope, for a deeply nested method
        Given Header file with content
        """
        #include <utility>
        struct Parent
        {
            struct ParentType {} ;

            struct Nested
            {
                struct NestedType { };

                struct DeeplyNested
                {
                    std::pair<ParentType, NestedType> gimme(ParentType, NestedType p2);
                };
            };
        };
        """
        When Method definition is generated from declaration at line 12
        Then Stdout contains
        """
        std::pair<Parent::ParentType, Parent::Nested::NestedType> Parent::Nested::DeeplyNested::gimme(ParentType, NestedType p2)
        """
        And No errors are emitted

    Scenario: Does not include scope in parameters nested in the same namespace, for a deeply nested free function 
        Given Header file with content
        """
        #include <utility>

        namespace Ns1
        {
            struct Ns1Type {};

            namespace Ns2
            {
                struct Ns2Type {};

                namespace Ns3
                {
                    std::pair<Ns1Type, Ns2Type> gimme(Ns1Type p1, Ns2Type);
                };
            };
        };
        """
        When Method definition is generated from declaration at line 13
        Then Stdout contains
        """
        std::pair<Ns1::Ns1Type, Ns1::Ns2::Ns2Type> Ns1::Ns2::Ns3::gimme(Ns1Type p1, Ns2Type)
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

