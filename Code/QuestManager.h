#pragma once

#include <fstream>
#include <string>
#include <vector>

#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <GUI.h>
#include <InputManager.h>

#include "QuestClasses.h"
#include "Logging.h"
#include "Scripting/ScriptQueue.h"

#include "ExtraFunctions.h"

class DialogueManager;
class QuestManager;

class DialogueManager {
    public:
        DialogueManager(std::vector<Question*>* questionList, std::vector<Flag*>* flagList, ScriptQueue* sq) : m_questionList(questionList), m_flagList(flagList), m_sq(sq) { }
        ~DialogueManager();

        void draw();
        void update(GLEngine::InputManager& input);

        void initUI(GLEngine::GUI* gui) { m_gui = gui; }

        void startConversation(unsigned int id, GLEngine::InputManager& input); // The id is the index of the initial question. This function sets focus and starts a loop to draw and accept input.

        bool isDialogueActive() { return m_dialogueActive; }
        bool isDialogueStarted() { return m_dialogueStarted; }

        void setQuestionId(unsigned int id) { m_questionId = id; }
        void setDialogueStarted(bool setting) { m_dialogueStarted = setting; }
        void setDialogueActive(bool setting) { m_dialogueActive = setting; }

    private:
        void initConversation(Question* initialQuestion);

        Logger* logger = Logger::getInstance();

        GLEngine::GUI* m_gui = nullptr;

        std::vector<Question*>* m_questionList = nullptr;
        std::vector<Flag*>* m_flagList = nullptr;

        ScriptQueue* m_sq = nullptr;

        std::vector<CEGUI::PushButton*> m_buttons;
        std::vector<CEGUI::Window*> m_otherWidgets;
        CEGUI::Scrollbar* m_scrollbar = nullptr;

        // Following variables are for actually talking
        Question* m_currentQuestion = nullptr;
        bool m_dialogueActive = false;
        bool m_dialogueStarted = false;

        unsigned int m_questionId;

        unsigned int m_activeQuestions; // Really should be activeAnswers
};

class QuestManager
{
    public:
        QuestManager(std::string questionListPath, std::string flagListPath, ScriptQueue* sq);
        ~QuestManager();

        void update(GLEngine::InputManager& input);
        void draw();

        void initUI(GLEngine::GUI* gui) { m_gui = gui; m_dialogueManager->initUI(gui); }

        std::vector<Question*>* getQuestionList() { return &m_questionList; }
        bool                    isDialogueActive() {return m_dialogueManager->isDialogueActive(); }
        bool                    isDialogueStarted(){return m_dialogueManager->isDialogueStarted(); }

        void setDialogueActive(bool setting) { m_dialogueManager->setDialogueActive(setting); }
        void setDialogueStarted(bool setting) { m_dialogueManager->setDialogueStarted(setting); }

        void setQuestionId(unsigned int id) { m_dialogueManager->setQuestionId(id); }

        void setFlag(unsigned int id, bool val) { if(m_flagList.size() <= id) m_flagList[id]->value = val; }
        bool getFlag(unsigned int id) { if(m_flagList.size() <= id) return m_flagList[id]->value; }

    private:
        void readDialogueFromList(std::string listPath);
        void readFlagsFromList(std::string listPath);

        Logger* logger = Logger::getInstance();

        DialogueManager* m_dialogueManager = nullptr; /// TODO make this private again please :)
        GLEngine::GUI* m_gui = nullptr;
        ScriptQueue* m_sq = nullptr;

        std::vector<Question*> getDialogue(std::ifstream& file);
        Question* readQuestion(std::vector<std::string> lines); // Forms question struct from given lines

        std::vector<Flag*> getFlags(std::ifstream& file);

        std::vector<Question*> m_questionList; // Full list of questions (dialogue "heads") in game
        std::vector<Flag*> m_flagList; // Full list of all flags that exist
};
