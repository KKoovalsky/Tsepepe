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
        Given Header file with content
        """

        class Bumm
        {
        public:
        private:
        };

        """
        When Suitable place in class is searched for class "Bumm"
        Then Line number 4 is returned

    Scenario: In a class with private and public section, in that order
        Given Header file with content
        """


        class Bumm
        {
        private:
        public:
        };

        """
        When Suitable place in class is searched for class "Bumm"
        Then Line number 6 is returned

    Scenario: In a class with public, private and one more public section, in that order
        Given Header file with content
        """

        class Masta
        {
        public:

        private:

        public:
        };

        """
        When Suitable place in class is searched for class "Masta"
        Then Line number 4 is returned

    Scenario: In an empty struct
        Given Header file with content
        """
        struct Yolo
        {
        };
        """
        When Suitable place in class is searched for class "Yolo"
        Then Line number 2 is returned

    Scenario: In a struct with properties only
        Given Header file with content
        """
        #include <string>
        
        struct Yolo
        {
            std::string s;
            unsigned n;
        };
        """
        When Suitable place in class is searched for class "Yolo"
        Then Line number 4 is returned

    Scenario: In a struct with few public methods
        Given Header file with content
        """
        
        struct Yolo
        {
            Yolo() {}

            ~Yolo()
            {
            }

            void gimme();
        };
        """
        When Suitable place in class is searched for class "Yolo"
        Then Line number 10 is returned

    Scenario: In an empty class
        Given Header file with content
        """

        class Yolo
        {
        };
        """
        When Suitable place in class is searched for class "Yolo"
        Then Line number 3 is returned with indication of inserting a public section

    Scenario: In an empty class with the opening bracket on the same line as the class name
        Given Header file with content
        """

        class Yolo {
        
        };
        """
        When Suitable place in class is searched for class "Yolo"
        Then Line number 2 is returned with indication of inserting a public section

    Scenario: In a class with private section only
        Given Header file with content
        """

        class Yolo 
        {
          private:
            unsigned i;
            float f;
        
        };
        """
        When Suitable place in class is searched for class "Yolo"
        Then Line number 3 is returned with indication of inserting a public section
