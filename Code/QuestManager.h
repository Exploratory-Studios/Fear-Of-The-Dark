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
        QuestManager(std::string questListPath, std::string flagListPath);
        ~QuestManager();

    private:
        void readDialogueFromList(std::string listPath);
        void readFlagsFromList(std::string listPath);

        std::vector<Question> getDialogue(std::ifstream& file);
        Question readQuestion(std::vector<std::string> lines); // Forms question struct from given lines

        std::vector<Flag> getFlags(std::ifstream& file);

        std::vector<Question> m_questList; // Full list of quests in game
        std::vector<Flag> m_flagList; // Full list of all flags that exist
};

class DialogueManager {
    public:
        DialogueManager(std::vector<Question>* questList, std::vector<Flag>* flagList) : m_questList(questList), m_flagList(flagList) { }

    private:
        std::vector<Question>* m_questList = nullptr;
        std::vector<Flag>* m_flagList = nullptr;

        // Following variables are for actually talking
        bool m_inConversation = false;
        unsigned int m_curQuestionIndex = 0;

};
