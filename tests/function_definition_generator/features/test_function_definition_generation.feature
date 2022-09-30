Feature: Generates function definitions

    Scenario: From basic method declaration

        Given Header File With Content
            """
            class SomeClass
            {
                unsigned int foo();
            };
            """
        When Method Definition Is Generated From Declaration At Line 3
        Then Stdout contains
            """
            unsigned int SomeClass::foo()
            """
        And No errors are emitted

    Scenario: From declaration returning a nested type

        Given Header File With Content
            """
            class Foo
            {
                class Bar
                {
                };

                Bar yolo();
            };
            """
        When Method Definition Is Generated From Declaration At Line 7
        Then Stdout contains
            """
            Foo::Bar SomeClass::yolo()
            """
        And No errors are emitted
