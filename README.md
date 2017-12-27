# Drupal 7 export QUIZ 7.x-4.0 to excel file

Quiz master and Administrator be able to download any quiz from the list, pressing download link. It should generate xlsx file, named by Quiz title.

List of supported question sets:

    Long answer question
    Matching
    Multiple choice question
    Quiz directions
    Scale question
    Short answer question
    True/false question
    
Excel contain all the fields for different question sets and skip not relevant values for specific question set.
List of fields in xlsx file:

    Type
    Question Title
    Question description
    Answer as:
        True/False question - 1 value: yes/no
        Short answer question - 1 value
        Scale question - up to 10 values
        Multiple choice question - up to 10 values, matching right one by star
    Matching answers (Matching, up to 5 values for each):
        Question
        Correct answer