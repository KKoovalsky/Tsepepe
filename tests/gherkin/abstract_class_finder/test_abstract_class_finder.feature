Feature: Header file with abstract class definition is found

  Background:
    Given Some dummy abstract class "AbstractClass" under path "some/dir1/abs_tract.h"
    Given Some dummy abstract class "MastaBlasta" under path "dirz/blasta_masta.hxx"
    Given Some dummy abstract class "yoko_poko_loko" under path "yoko_poko_loko/dums.hpp"
    Given Some dummy class "buerekepe" under path "classes/buerekepe.hpp"
    Given Some dummy class "ligondo" under path "classes/ligondo.hpp"

  Scenario: Finds an abstract class in file with similar name to the class name
        Given Header file "some/dir2/the_class.hpp" with content
        """
        #include <string>
        struct TheClass
        {
            virtual void run(unsigned int time_ms) = 0;
            virtual std::string get_name() = 0;
            virtual ~TheClass() = default;
        };
        """
        When Abstract class "TheClass" is tried to be found
        Then Path "some/dir2/the_class.hpp" is returned

  Scenario: Finds an abstract class in file with unsimilar name to the class name

  Scenario: Finds an abstract class which is nested in a namespace

  Scenario Outline: Finds no match if abstract class put under path that shall be ignored

    Examples:
      | file                      |
      | .git/some_dir/yolo.hpp    |
      | __pycache__/yolo.hpp      |
      | build/makapaka/yolo.hpp   |
      | build_host/masta/yolo.hpp |
