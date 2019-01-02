#pragma once

#include <string>
#include <vector>

class Flag;
struct FlagArrangement;
class Answer;
class Question;

class Flag {
public:
    Flag(std::string strForm) { // should be in this form "intForID intForValue". Ex: "7 1", "92 0", etc.
        id = std::stoi(strForm.substr(0, strForm.find(" ")));
        value = std::stoi(strForm.substr(strForm.find(" ")+1, (int)strForm.size()));
    }
    int id;
    bool value;
};

struct FlagArrangement {
    std::vector<Flag> arrangement;
};

class Question {
public:
    int id;
    std::vector<Answer> answers;
    std::string str;
};

class Answer {
public:
    int id;
    std::string str;
    FlagArrangement requiredFlags;
    FlagArrangement followingFlags;
    Question* followingQuestion = nullptr;
};
