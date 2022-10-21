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
          virtual ~Iface() = default;
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
    Given Header file with name "runnable.hpp" and content
      """
      #include <string>
      struct Runnable
      {
          virtual void run(std::string name) = 0;
          virtual ~Runnable() = default;
      };
      """
    And Header file with name "printer.hpp" and content
      """
      #include <vector>
      #include <string>
      struct Printer
      {
          struct PrintingResult
          {
          };

          virtual PrintingResult print(std::vector<std::string> stuff) = 0;
          virtual ~Printer() = default;
      };
      """
    And Header file with name "runnable_printer.hpp" and content
      """
      #include "runnable.hpp"
      #include "printer.hpp"
      struct RunnablePrinter : Runnable, Printer
      {
          virtual ~RunnablePrinter() = default;
      };
      """
    When Pure virtual functions are extracted from class "RunnablePrinter" in file "runnable_printer.hpp"
    Then Stdout contains
      """
      void run(std::string name) override;
      PrintingResult print(std::vector<std::string> stuff) override;
      """
    And No errors are emitted

  Scenario: Extracts and converts pure virtual functions from a complex compound interface
    Given Header file with name "display.hpp" and content
      """
      #include <vector>
      struct Display
      {
          using Bytes = std::vector<unsigned char>;
          virtual void display(const Bytes&) = 0;
          virtual ~Display() = default;
      };
      """
    And Header file with name "printer.hpp" and content
      """
      #include <vector>
      #include <string>
      struct Printer
      {
          struct PrintingResult
          {
          };

          virtual PrintingResult print(std::vector<std::string> stuff) = 0;
          virtual ~Printer() = default;
      };
      """
    And Header file with name "displaying_printer.hpp" and content
      """
      #include "display.hpp"
      #include "printer.hpp"
      struct DisplayingPrinter : Display, Printer
      {
          virtual ~DisplayingPrinter() = default;
      };
      """
    And Header file with name "dumpable_displaying_printer.hpp" and content
      """
      #include "displaying_printer.hpp"
      struct DumpableDisplayingPrinter : DisplayingPrinter
      {
          virtual void dump() = 0;
          virtual ~DumpableDisplayingPrinter() = default;
      };
      """
    When Pure virtual functions are extracted from class "DumpableDisplayingPrinter" in file "dumpable_displaying_printer.hpp"
    Then Stdout contains
      """
      void display(const Bytes&) override;
      PrintingResult print(std::vector<std::string> stuff) override;
      void dump() override;
      """
    And No errors are emitted

  Scenario: Extracts and converts pure virtual functions from a class in a namespace

  Scenario: Extracts and converts pure virtual functions from a nested class
