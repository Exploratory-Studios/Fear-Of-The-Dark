#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "EntityManager.h"
#include <iostream>

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

class Answer;

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
    Question followingQuestion;
};

class QuestManager
{
    public:
        QuestManager(std::string listPath);
        ~QuestManager();

    private:
        void readDialogueFromList(std::string listPath);
        std::vector<Question> getDialogue(std::ifstream& file);
        Question readQuestion(std::vector<std::string> lines);

        std::vector<Question> m_questList; // Full list of quests in game
};
