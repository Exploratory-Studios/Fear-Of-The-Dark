#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <GUI.h>

#include "EntityManager.h"
#include "QuestClasses.h"

class DialogueManager;
class QuestManager;

class DialogueManager {
    public:
        DialogueManager(std::vector<Question*>* questionList, std::vector<Flag*>* flagList) : m_questionList(questionList), m_flagList(flagList) { }
        ~DialogueManager();

        void draw(GLEngine::GUI& gui);
        void update(GLEngine::InputManager& input, GLEngine::GUI& gui);

        void startConversation(unsigned int id, GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, GLEngine::InputManager& input, GLEngine::GUI& gui); // The id is the index of the initial question. This function sets focus and starts a loop to draw and accept input.

        bool isInConversation() { return m_inConversation; }

    private:
        void initConversation(Question* initialQuestion, GLEngine::GUI& gui);

        std::vector<Question*>* m_questionList = nullptr;
        std::vector<Flag*>* m_flagList = nullptr;

        std::vector<CEGUI::PushButton*> m_buttons;
        std::vector<CEGUI::Window*> m_otherWidgets;

        // Following variables are for actually talking
        Question* m_currentQuestion = nullptr;
        bool m_inConversation = false;
};

class QuestManager
{
    public:
        QuestManager(std::string questionListPath, std::string flagListPath);
        ~QuestManager();

        std::vector<Question*>* getQuestionList() { return &m_questionList; }

        DialogueManager* m_dialogueManager = nullptr; /// TODO make this private again please :)

    private:
        void readDialogueFromList(std::string listPath);
        void readFlagsFromList(std::string listPath);

        std::vector<Question*> getDialogue(std::ifstream& file);
        Question* readQuestion(std::vector<std::string> lines); // Forms question struct from given lines

        std::vector<Flag*> getFlags(std::ifstream& file);

        std::vector<Question*> m_questionList; // Full list of questions (dialogue "heads") in game
        std::vector<Flag*> m_flagList; // Full list of all flags that exist
};
