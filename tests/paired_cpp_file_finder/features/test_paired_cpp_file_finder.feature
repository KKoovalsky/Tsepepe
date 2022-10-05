Feature: Finds paired C++ file

  Scenario Outline: When put in the same directory

    Given C++ file under path <input_file>
    And C++ file under path <expected_result>
    When Paired file for <input_file> is searched
    Then Finding result is <expected_result>
    
    Examples:
      | input_file    | expected_result |
      | dir/file1.hpp | dir/file1.cpp   |
      | dir/file2.cpp | dir/file2.hpp   |

  Scenario: When put in the child directory

  Scenario: When put in the parent directory

  Scenario: File in the current directory takes precedence

  Scenario: Takes multiple files if found with the same name, when paired file not found in the current directory

  Scenario: Error is raised when the paired file is not found

