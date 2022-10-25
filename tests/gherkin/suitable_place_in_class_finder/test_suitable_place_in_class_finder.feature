Feature: Finds suitable place in classes to put a new public function declaration

    Scenario: In a class with a single public section
        Given Header file with content
        """
        class Yolo
        {
        public:
            Yolo() = default;
            void gimme();
        };
        """
        When Suitable place in class is searched for class "Yolo"
        Then Line number 5 is returned

    Scenario: In a class with private methods
        Given Header file with content
        """
        class Maka
        {
        private:
            void gimme();
            void sijek();
        };
        """
        When Suitable place in class is searched for class "Maka"
        Then Line number 2 is returned with indication of inserting a public section

    Scenario: In a class with public and private section, in that order
    Scenario: In a class with private and public section, in that order
    Scenario: In a class with public, private and one more public section, in that order
    Scenario: In an empty struct
    Scenario: In a struct with properties only
    Scenario: In a struct with few public methods
    Scenario: In an empty class
    Scenario: In an empty class with the opening bracket on the same line as the class name
    Scenario: In a class with private section only
