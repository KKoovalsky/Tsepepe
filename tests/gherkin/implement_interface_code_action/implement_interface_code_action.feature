Feature: Code action allowing to generate code that makes a class implement an interface

  Scenario Outline: Implements a basic interface
    Given Header file "runnable.hpp" with content
      """
      struct Runnable
      {
          virtual void run() = 0;
          virtual int stop(unsigned timeout_ms) = 0;
      };
      """
    And Header file "maker.hpp" with content
      """
      struct Maker
      {
      };
      """
    When Implement interface code action is invoked for file "maker.hpp" with line at cursor <line>
    Then The result is
      """
      #include "runnable.hpp"
      
      struct Maker : Runnable
      {
          void run() override;
          int stop(unsigned timeout_ms) override;
      };
      """
    Examples:
      | line |
      | 1    |
      | 2    |
      | 3    |

  Scenario: Implements a compound interface
  Scenario: Properly resolves types nested within the interface
  Scenario: Properly resolves types nested within the same namespace the interface is defined, but outside of the interface
  Scenario: Properly resolves types nested within the same namespace the interface is defined, but outside of the interface, when the implementor is in the same namespace
  Scenario: Error when interface not found
  Scenario: Error when interface not found, but a normal class exists with the given name
  Scenario: Does not load file from filesystem, but from standard input
  Scenario: Implementor is a class with no base classes
  Scenario: Implementor is a class with a single base class
  Scenario: Implementor is a class with multiple base classes span over multiple lines
  Scenario: Implementor is an empty class
  Scenario: Implementor is an empty struct
  Scenario: Implementor is a struct with mutliple methods
  Scenario: Implementor is a class with mutliple public methods
  Scenario: Implementor is a class with only a private section
  Scenario: Implementor is a class with a single public section
  Scenario: Does not add include if the interface already included
  Scenario: Does not add base class specifier if already exists
