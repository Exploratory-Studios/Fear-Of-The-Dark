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
        FlagArrangement followingFlags; // Equivalent to the followingScript saying "setFlag XXX 1/0" but saves on script cache size
        unsigned int followingScriptId = (unsigned int)-1; // The id of the script that's executed when the answer is clicked (-1 for no script)
        Question* followingQuestion = nullptr;
};

class Item;

struct Trade {
    Item* requiredItem = nullptr;
    Item* rewardedItem = nullptr;
};

struct TradeTable {
    unsigned int id;
    std::vector<Trade*> trades;
};
