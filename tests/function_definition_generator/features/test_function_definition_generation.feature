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
