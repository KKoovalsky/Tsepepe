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
    When Pure virtual functions are extracted from class "Interface"
    Then Stdout contains
    """
    void run(unsigned int) override;
    """
    And No errors are emitted

  Scenario: Extracts and converts multiple pure virtual functions
    Given Header file with content
    """
    #include <string>
    #include <vector>
    struct Iface
    {
        virtual int run(double d) = 0;
        virtual std::string gimme(const std::vector<std::string>& some_list) = 0;
        virtual ~Interface() = default;
    };
    """
    When Pure virtual functions are extracted from class "Iface"
    Then Stdout contains
    """
    int run(double d) override;
    std::string gimme(const std::vector<std::string>& some_list) override;
    """
    And No errors are emitted

  Scenario: Extracts and converts pure virtual functions from a compound interface

  Scenario: Extracts and converts pure virtual functions from a class in a namespace

  Scenario: Extracts and converts pure virtual functions from a nested class
