Feature: Extracts pure virtual functions from abstract class and converts them to overriding declarations

  Scenario: Extracts and converts a single pure virtual function
    Given Header file with content
    """
    struct Interface
    {
        virtual void run(unsigned int) = 0;
        virtual ~Interface() = default;
    };
    """
    When Pure virtual functions are extracted
    Then Stdout contains
    """
    void run(unsigned int) override;
    """
    And No errors are emitted

  Scenario: Extracts and converts multiple pure virtual functions

  Scenario: Extracts and converts pure virtual functions from a compound interface

  Scenario: Extracts and converts pure virtual functions from a class in a namespace

  Scenario: Extracts and converts pure virtual functions from a nested class
