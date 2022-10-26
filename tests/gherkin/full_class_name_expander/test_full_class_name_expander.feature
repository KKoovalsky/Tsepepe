Feature: Expands the class name to a fully qualified name

    Scenario: For a basic class
        Given Header file with content
        """
        class Yolo
        {
        };
        """
        When Class name "Yolo" is expanded
        Then The result is "Yolo"

    Scenario: For a basic struct
        Given Header file with content
        """
        struct Basta
        {
        };
        """
        When Class name "Basta" is expanded
        Then The result is "Basta"

    Scenario: For a class in namespace
        Given Header file with content
        """
        namespace Namespace {
        struct Basta
        {
        };
        }
        """
        When Class name "Basta" is expanded
        Then The result is "Namespace::Basta"

    Scenario: For a class in a class in a namespace
        Given Header file with content
        """
        namespace Namespace {
        struct Basta
        {
            struct Record
            {
            };
        };
        }
        """
        When Class name "Record" is expanded
        Then The result is "Namespace::Basta::Record"
