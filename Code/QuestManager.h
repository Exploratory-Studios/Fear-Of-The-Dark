#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <GUI.h>

#include "EntityManager.h"

class Flag;
struct FlagArrangement;
class Answer;
class Question;
class DialogueManager;
class QuestManager;

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
    Question followingQuestion;
};

class DialogueManager {
    public:
        DialogueManager(std::vector<Question>* questionList, std::vector<Flag>* flagList) : m_questionList(questionList), m_flagList(flagList) { }
        ~DialogueManager();

        void draw(GLEngine::GUI& gui);
        void update(GLEngine::InputManager& input);

        void startConversation(unsigned int id, GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, GLEngine::InputManager& input, GLEngine::GUI& gui); // The id is the index of the initial question. This function sets focus and starts a loop to draw and accept input.

        bool isInConversation() { return m_inConversation; }

    private:
        void initConversation(unsigned int id, GLEngine::GUI& gui);

        std::vector<Question>* m_questionList = nullptr;
        std::vector<Flag>* m_flagList = nullptr;

        std::vector<CEGUI::PushButton*> m_buttons;

        // Following variables are for actually talking
        bool m_inConversation = false;
};

class QuestManager
{
    public:
        QuestManager(std::string questionListPath, std::string flagListPath);
        ~QuestManager();

        DialogueManager* m_dialogueManager = nullptr; /// TODO make this private again please :)

    private:


        void readDialogueFromList(std::string listPath);
        void readFlagsFromList(std::string listPath);

        std::vector<Question> getDialogue(std::ifstream& file);
        Question readQuestion(std::vector<std::string> lines); // Forms question struct from given lines

        std::vector<Flag> getFlags(std::ifstream& file);

        std::vector<Question> m_questionList; // Full list of questions (dialogue "heads") in game
        std::vector<Flag> m_flagList; // Full list of all flags that exist
};
