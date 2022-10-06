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

  Scenario Outline: When put in the child directory

    Given C++ file under path <input_file>
    And C++ file under path <expected_result>
    When Paired file for <input_file> is searched
    Then Finding result is <expected_result>

    Examples:
      | input_file    | expected_result     |
      | dir/file1.hpp | dir/child/file1.cpp |
      | dir/file2.cpp | dir/child/file2.hpp |


  Scenario Outline: When put in a parallel directory

    Given C++ file under path <input_file>
    And C++ file under path <expected_result>
    When Paired file for <input_file> is searched
    Then Finding result is <expected_result>

    Examples:
      | input_file     | expected_result |
      | dir1/file1.hxx | dir2/file1.cc   |
      | dir1/file2.cxx | dir2/file2.h    |

  Scenario Outline: File in the current directory takes precedence

    Given C++ file under path <input_file>
    And C++ file under path <another_file1>
    And C++ file under path <another_file2>
    And C++ file under path <expected_result>
    When Paired file for <input_file> is searched
    Then Finding result is <expected_result>

    Examples:
      | input_file        | another_file1       | another_file2       | expected_result  |
      | dir_main/file.hxx | dir_other1/file.cpp | dir_other2/file.cxx | dir_main/file.cc |
      | dir_main/file.cxx | dir_other1/file.hpp | dir_other2/file.hxx | dir_main/file.hh |

  Scenario Outline: Takes multiple files if found with the same name, when paired file not found in the current directory

    Given C++ file under path <input_file>
    And C++ file under path <another_file1>
    And C++ file under path <another_file2>
    When Paired file for <input_file> is searched
    Then Finding results are <another_file1> and <another_file2>

    Examples:
      | input_file   | another_file1       | another_file2       |
      | dir/file.hxx | dir_other1/file.cpp | dir_other2/file.cxx |
      | dir/file.cxx | dir_other1/file.hpp | dir_other2/file.hxx |


  Scenario: Error is raised when the paired file is not found

        Given C++ file under path dir/file.cpp
        When Paired file for dir/file.cpp is searched
        Then No paired file found error is raised
