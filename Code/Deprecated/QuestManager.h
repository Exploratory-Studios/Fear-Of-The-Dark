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

class EntityPlayer;

class DialogueManager {
    public:
        DialogueManager(std::vector<Question*>* questionList, std::vector<Flag*>* flagList, std::vector<TradeTable*>* tradeTables) : m_questionList(questionList), m_flagList(flagList), m_tradeTables(tradeTables) { }
        ~DialogueManager();

        void draw();
        void update(GLEngine::InputManager& input, EntityPlayer* p);

        void initUI(GLEngine::GUI* gui) { m_gui = gui; }

        void startConversation(GLEngine::InputManager& input); // The id is the index of the initial question. This function sets focus and starts a loop to draw and accept input.

        bool isDialogueActive() const { return m_dialogueActive; }
        bool isDialogueStarted() const { return m_dialogueStarted; }

        bool isTradingActive() const { return m_tradingActive; }

        void setQuestionId(unsigned int id) { m_questionId = id; }
        void setTradeTableId(unsigned int id) { m_tradeTable = id; }

        void setDialogueStarted(bool setting) { m_dialogueStarted = setting; }
        void setDialogueActive(bool setting) { m_dialogueActive = setting; }
        void setTradingStarted(bool setting) { m_tradingStarted = setting; }
        void setTradingActive(bool setting) { m_tradingActive = setting; }

        bool onLeaveButtonClicked(const CEGUI::EventArgs& e);

    private:
        void initConversation();

        Logger* logger = Logger::getInstance();

        GLEngine::GUI* m_gui = nullptr;

        std::vector<Question*>* m_questionList = nullptr;
        std::vector<Flag*>* m_flagList = nullptr;
        std::vector<TradeTable*>* m_tradeTables = nullptr;

        std::vector<CEGUI::PushButton*> m_buttons;
        std::vector<CEGUI::Window*> m_otherWidgets;
        CEGUI::Scrollbar* m_scrollbar = nullptr;

        // Following variables are for actually talking
        Question* m_currentQuestion = nullptr;
        bool m_dialogueActive = false;
        bool m_dialogueStarted = false;
        bool m_tradingStarted = false;
        bool m_tradingActive = false;

        unsigned int m_questionId;
        unsigned int m_tradeTable;

        unsigned int m_activeQuestions; // Really should be activeAnswers
};

class QuestManager
{
    public:
        QuestManager(std::string questionListPath, std::string flagListPath, std::string tradeListPath);
        ~QuestManager();

        void update(GLEngine::InputManager& input, EntityPlayer* p);
        void draw();

        void initUI(GLEngine::GUI* gui) { m_gui = gui; m_dialogueManager->initUI(gui); }

        std::vector<Question*>* getQuestionList()              { return &m_questionList; }
        bool                    isDialogueActive()       const { return m_dialogueManager->isDialogueActive(); }
        bool                    isDialogueStarted()      const { return m_dialogueManager->isDialogueStarted(); }
        bool                    isTradingActive()        const { return m_dialogueManager->isTradingActive(); }

        void setDialogueActive(bool setting) { m_dialogueManager->setDialogueActive(setting); }
        void setDialogueStarted(bool setting) { m_dialogueManager->setDialogueStarted(setting); }

        void setTradingActive(bool setting) { m_dialogueManager->setTradingActive(setting); }
        void setTradingStarted(bool setting) { m_dialogueManager->setTradingStarted(setting); }

        void setQuestionId(unsigned int id) { m_dialogueManager->setQuestionId(id); }
        void setTradeTableId(unsigned int id) { m_dialogueManager->setTradeTableId(id); }

        void setFlag(unsigned int id, bool val) { if(m_flagList.size() <= id) m_flagList[id]->value = val; }
        bool getFlag(unsigned int id) { if(m_flagList.size() <= id) return m_flagList[id]->value; }

    private:
        void readDialogueFromList(std::string listPath);
        void readFlagsFromList(std::string listPath);
        void readTradesFromList(std::string listPath);

        Logger* logger = Logger::getInstance();

        DialogueManager* m_dialogueManager = nullptr; /// TODO make this private again please :)
        GLEngine::GUI* m_gui = nullptr;

        std::vector<Question*> getDialogue(std::ifstream& file);
        Question* readQuestion(std::vector<std::string> lines); // Forms question struct from given lines

        std::vector<TradeTable*> getTradeTables(std::ifstream& file);
        TradeTable* readTradeTable(std::vector<std::string> lines, unsigned int id);
        Trade* readTrade(std::string line);

        std::vector<Flag*> getFlags(std::ifstream& file);

        std::vector<Question*> m_questionList; // Full list of questions (dialogue "heads") in game
        std::vector<Flag*> m_flagList; // Full list of all flags that exist
        std::vector<TradeTable*> m_tradeTables; // Full list of all trade tables that exist
};
