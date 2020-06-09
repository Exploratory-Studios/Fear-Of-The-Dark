#include "QuestManager.h"

#include "Inventory.h"

#include "EntityPlayer.h"

#include <GUI.h>

DialogueManager::~DialogueManager() {

}

void DialogueManager::startConversation(GLEngine::InputManager& input) {
    if(!m_tradingStarted) {
        Question* initialQuestion = (*m_questionList)[m_questionId];
        m_currentQuestion = initialQuestion;
        initConversation();
        m_dialogueActive = true;
        m_dialogueStarted = false;
    } else if(m_tradingStarted) {
        //TradeTable* currentTable = (*m_tradeTables)[m_tradeTable];
        initConversation();
        m_tradingActive = true;
        m_tradingStarted = false;
    }
}

void DialogueManager::initConversation() { // Sets gui for conversation
    if(!m_dialogueActive && !m_tradingStarted) {
        m_dialogueActive = true;

        CEGUI::FrameWindow* window = static_cast<CEGUI::FrameWindow*>(m_gui->createWidget("FOTDSkin/FrameWindow", glm::vec4(0.05f, 0.7f, 0.9f, 0.25f), glm::vec4(0.0f), "FrameWindowConversation"));

        window->setTitleBarEnabled(false);
        window->setCloseButtonEnabled(false);
        window->setRollupEnabled(false);
        window->setDragMovingEnabled(false);
        window->setSizingEnabled(false);

        CEGUI::FrameWindow* textFrame = static_cast<CEGUI::FrameWindow*>(m_gui->createWidget(window, "FOTDSkin/FrameWindow", glm::vec4(0.02f, 0.05f, 0.96f, 0.25f), glm::vec4(0.0f), "TextFrameConversation"));

        textFrame->setTitleBarEnabled(false);
        textFrame->setCloseButtonEnabled(false);
        textFrame->setRollupEnabled(false);
        textFrame->setDragMovingEnabled(false);
        textFrame->setSizingEnabled(false);

        CEGUI::DefaultWindow* textWindow = static_cast<CEGUI::DefaultWindow*>(m_gui->createWidget(textFrame, "FOTDSkin/Label", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f), "TextBoxConversation"));\
        textWindow->setText("[colour='FF000000']" + m_currentQuestion->str);

        CEGUI::DefaultWindow* answersWindow = static_cast<CEGUI::DefaultWindow*>(m_gui->createWidget(window, "FOTDSkin/Label", glm::vec4(0.02f, 0.325f, 0.96f, 0.7f), glm::vec4(0.0f), "OptionsBoxConversation"));

        CEGUI::ScrollablePane* answersScroller = static_cast<CEGUI::ScrollablePane*>(m_gui->createWidget(answersWindow, "FOTDSkin/ScrollablePane", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f), "OptionsScrollPaneConversation"));
        answersScroller->setVerticalStepSize(0.01f);
        m_scrollbar = answersScroller->getVertScrollbar();

        m_otherWidgets.push_back(answersScroller);
        m_otherWidgets.push_back(answersWindow);
        m_otherWidgets.push_back(textWindow);
        m_otherWidgets.push_back(textFrame);
        m_otherWidgets.push_back(window);
        // We must push them in the order that they are in (children first, then parents)

        float paddingIncrementer = 0;
        m_activeQuestions = 0;
        for(unsigned int i = 0; i < m_currentQuestion->answers.size(); i++) {
            CEGUI::PushButton* answerButton = static_cast<CEGUI::PushButton*>(m_gui->createWidget(answersScroller, "FOTDSkin/Button", glm::vec4(0.0f, 0.025f + ((0.03f + 0.425f) * paddingIncrementer), 1.0f, 0.425f), glm::vec4(0.0f), std::string("Option" + std::to_string(i) + "Conversation")));
            answerButton->setText(m_currentQuestion->answers[i].str);
            m_buttons.push_back(answerButton);

            paddingIncrementer++;
            m_activeQuestions++;
            for(unsigned int j = 0; j < m_currentQuestion->answers[i].requiredFlags.arrangement.size(); j++) {
                unsigned int flagId = m_currentQuestion->answers[i].requiredFlags.arrangement[j].id;
                if((*m_flagList)[flagId]->value != m_currentQuestion->answers[i].requiredFlags.arrangement[j].value) {
                    paddingIncrementer--;
                    m_activeQuestions--;
                    answerButton->disable();
                    answerButton->hide();

                    answerButton->setMouseInputPropagationEnabled(true);
                    break;
                }
            }
        }
    } else if(!m_tradingActive && !m_dialogueStarted) {
        m_tradingActive = true;

        CEGUI::FrameWindow* window = static_cast<CEGUI::FrameWindow*>(m_gui->createWidget("FOTDSkin/FrameWindow", glm::vec4(0.05f, 0.05f, 0.9f, 0.9f), glm::vec4(0.0f), "FrameWindowTrading"));

        window->setTitleBarEnabled(false);
        window->setCloseButtonEnabled(false);
        window->setRollupEnabled(false);
        window->setDragMovingEnabled(false);
        window->setSizingEnabled(false);

        CEGUI::PushButton* leaveButton = static_cast<CEGUI::PushButton*>(m_gui->createWidget(window, "FOTDSkin/Button", glm::vec4(0.8f, 0.9f, 0.15f, 0.08f), glm::vec4(0.0f), "LeaveButtonTrading"));

        leaveButton->setText("Leave");

        leaveButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&DialogueManager::onLeaveButtonClicked, this));

        CEGUI::FrameWindow* textFrame = static_cast<CEGUI::FrameWindow*>(m_gui->createWidget(window, "FOTDSkin/FrameWindow", glm::vec4(0.02f, 0.02f, 0.96f, 0.08f), glm::vec4(0.0f), "TextFrameTrading"));

        textFrame->setTitleBarEnabled(false);
        textFrame->setCloseButtonEnabled(false);
        textFrame->setRollupEnabled(false);
        textFrame->setDragMovingEnabled(false);
        textFrame->setSizingEnabled(false);

        CEGUI::DefaultWindow* textWindow = static_cast<CEGUI::DefaultWindow*>(m_gui->createWidget(textFrame, "FOTDSkin/Label", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f), "TextBoxTrading"));
        textWindow->setText("[colour='FF000000']I'll give you...");

        CEGUI::DefaultWindow* answersWindow = static_cast<CEGUI::DefaultWindow*>(m_gui->createWidget(window, "FOTDSkin/Label", glm::vec4(0.02f, 0.11f, 0.96f, 0.78f), glm::vec4(0.0f), "OptionsBoxTrading"));

        CEGUI::ScrollablePane* answersScroller = static_cast<CEGUI::ScrollablePane*>(m_gui->createWidget(answersWindow, "FOTDSkin/ScrollablePane", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f), "OptionsScrollPaneTrading"));
        answersScroller->setVerticalStepSize(0.01f);
        m_scrollbar = answersScroller->getVertScrollbar();

        m_otherWidgets.push_back(leaveButton);
        m_otherWidgets.push_back(answersScroller);
        m_otherWidgets.push_back(answersWindow);
        m_otherWidgets.push_back(textWindow);
        m_otherWidgets.push_back(textFrame);
        m_otherWidgets.push_back(window);
        // We must push them in the order that they are in (children first, then parents)

        float paddingIncrementer = 0;
        for(unsigned int i = 0; i < (*m_tradeTables)[m_tradeTable]->trades.size(); i++) {
            Trade* t = (*m_tradeTables)[m_tradeTable]->trades[i];

            CEGUI::PushButton* answerButton = static_cast<CEGUI::PushButton*>(m_gui->createWidget(answersScroller, "FOTDSkin/Button", glm::vec4(0.0f, 0.05f + ((0.02f + 0.1f) * paddingIncrementer), 1.0f, 0.1f), glm::vec4(0.0f), std::string("Option" + std::to_string(i) + "Trade")));

            std::string tradeString = std::to_string(t->rewardedItem->getQuantity());
            tradeString += " " + t->rewardedItem->getName();
            tradeString += " for ";
            tradeString += std::to_string(t->requiredItem->getQuantity());
            tradeString += " " + t->requiredItem->getName();

            answerButton->setText(tradeString);
            m_buttons.push_back(answerButton);

            answerButton->setMouseInputPropagationEnabled(true);

            paddingIncrementer++;
        }
    }
}

void DialogueManager::draw() {

}

void DialogueManager::update(GLEngine::InputManager& input, EntityPlayer* p) {
    if(m_dialogueStarted || m_tradingStarted) {
        startConversation(input);
        return;
    }

    if(m_dialogueActive) {
        if(m_activeQuestions > 0) m_scrollbar->setScrollPosition(m_scrollbar->getScrollPosition() + (-input.getMouseScrollPosition() * ((m_scrollbar->getDocumentSize() - m_scrollbar->getPageSize())/(m_activeQuestions * 2))));
        if(input.isKeyPressed(SDL_BUTTON_LEFT)) { // Answer clicked, check stuff...
            int optionChosen = -1;
            // Set new values of flags
            for(unsigned int i = 0; i < m_buttons.size(); i++) {
                if(m_buttons[i]->isPushed() && m_buttons[i]->isActive()) {
                    optionChosen = i;
                    // Actual answer clicked! Start doing stuff!
                    for(unsigned int j = 0; j < m_currentQuestion->answers[i].followingFlags.arrangement.size(); j++) {
                        unsigned int flagId = m_currentQuestion->answers[i].followingFlags.arrangement[j].id;
                        if(flagId >= m_flagList->size()) {
                            logger->log("ERROR: Flags list is not long enough. please add more entries or fix the dialogue's flags (following)\nIllegal ID: " + std::to_string(flagId), true);
                            break;
                        }
                        (*m_flagList)[flagId]->value = m_currentQuestion->answers[i].followingFlags.arrangement[j].value;
                    }
                    // Start script (if applicable)
                    if(m_currentQuestion->answers[i].followingScriptId != (unsigned int)-1) {
                        unsigned int id = m_currentQuestion->answers[i].followingScriptId;
                        ScriptQueue::activateScript(id);
                    }
                    break;
                }
            }

            if(optionChosen != -1) {
                m_activeQuestions = 0;
                for(unsigned int i = 0; i < m_buttons.size(); i++) {
                    CEGUI::WindowManager::getSingleton().destroyWindow(m_buttons[i]);
                }
                m_buttons.clear();
                m_buttons.resize(0);
                for(unsigned int i = 0; i < m_otherWidgets.size(); i++) {
                    CEGUI::WindowManager::getSingleton().destroyWindow(m_otherWidgets[i]);
                }
                m_otherWidgets.clear();
                m_otherWidgets.resize(0);
                m_dialogueActive = false;
                if(m_currentQuestion->answers[optionChosen].followingQuestion) {
                    m_currentQuestion = m_currentQuestion->answers[optionChosen].followingQuestion;
                    initConversation();
                }
            }
        }
    } else if(m_tradingActive) {
        if(input.isKeyPressed(SDL_BUTTON_LEFT)) { // Answer clicked, check stuff...
            // Set new values of flags
            for(unsigned int i = 0; i < m_buttons.size(); i++) {
                if(m_buttons[i]->isPushed() && m_buttons[i]->isActive()) {
                    Trade* t = (*m_tradeTables)[m_tradeTable]->trades[i];

                    // Check player's inventory to make sure they can buy the item
                    Inventory* inv = p->getInventory();

                    if(inv) {
                        unsigned int index = inv->getItemIndex(t->requiredItem);
                        if(index != (unsigned int)-1) {
                            inv->subtractItem(t->requiredItem);
                            inv->addItem(t->rewardedItem);
                        }
                    }
                }
            }
        }
    }
}

bool DialogueManager::onLeaveButtonClicked(const CEGUI::EventArgs& e) {
    for(unsigned int i = 0; i < m_buttons.size(); i++) {
        CEGUI::WindowManager::getSingleton().destroyWindow(m_buttons[i]);
    }
    m_buttons.clear();
    m_buttons.resize(0);
    for(unsigned int i = 0; i < m_otherWidgets.size(); i++) {
        CEGUI::WindowManager::getSingleton().destroyWindow(m_otherWidgets[i]);
    }
    m_otherWidgets.clear();
    m_otherWidgets.resize(0);
    m_tradingActive = false;

    return true;
}

QuestManager::QuestManager(std::string questionListPath, std::string flagListPath, std::string tradeListPath)
{
    readDialogueFromList(questionListPath);
    readFlagsFromList(flagListPath);
    readTradesFromList(tradeListPath);

    m_dialogueManager = new DialogueManager(&m_questionList, &m_flagList, &m_tradeTables);
}

QuestManager::~QuestManager()
{
    delete m_dialogueManager;
}

void QuestManager::update(GLEngine::InputManager& input, EntityPlayer* player) {
    m_dialogueManager->update(input, player);
}

void QuestManager::draw() {
    m_dialogueManager->draw();
}

void QuestManager::readDialogueFromList(std::string listPath) {
    std::ifstream file(listPath);

    if(file.fail()) {
        logger->log("Failed to load quest list at " + listPath, true);
    } else {
        m_questionList = getDialogue(file);
    }
}

void QuestManager::readFlagsFromList(std::string listPath) {
    std::ifstream file(listPath);

    if(file.fail()) {
        logger->log("Failed to load flag list at " + listPath, true);
    } else {
        m_flagList = getFlags(file);
    }
}

void QuestManager::readTradesFromList(std::string listPath) {
    std::ifstream file(listPath);

    if(file.fail()) {
        logger->log("Failed to load trades list at " + listPath, true);
    } else {
        m_tradeTables = getTradeTables(file);
    }
}

Question* QuestManager::readQuestion(std::vector<std::string> lines) {
    Question* q = new Question();
    {
        //int depth;
        q->str = lines[0];

        for(unsigned int i = 0; i < lines.size(); i++) {
            if (lines[i] == "#BEGIN") {
                Answer a;
                int answerDepth = 1;
                std::vector<std::string> newLines;
                for(unsigned int j = i+1; j < lines.size(); j++) {
                    newLines.push_back(lines[j]); // Gets all the lines for the answer
                    if(lines[j] == "#BEGIN") {
                        answerDepth++;
                    } else if(lines[j] == "#END") {
                        answerDepth--;
                        if(answerDepth == 0) {
                            i = j; // Make sure we don't read twice
                            break;
                        }
                    }
                }

                a.str = newLines[0]; // Get string. First is #BEGIN

                unsigned int j = 1;
                for(int k = 0; k < 2; k++) { // Get required flags, then following flags
                    while(newLines[j] != "#FLAGS_END" && newLines[j] != "#END") {
                        Flag f = Flag(newLines[j]);
                        if(k == 0) a.requiredFlags.arrangement.push_back(f);
                        if(k == 1) a.followingFlags.arrangement.push_back(f);
                        if(j < newLines.size()-1) j++;
                    }
                    if(j < newLines.size()-1) j++;
                }

                { // Get filepath of script(s) to be executed on answer
                    while(newLines[j] != "#SCRIPTS_END" && newLines[j] != "END") {
                        a.followingScriptId = ScriptQueue::addScript(newLines[j]); // Adds script to cache with filepath of (Assets path)/Scripts/...
                        if(j < newLines.size()-1) j++;
                    }
                    if(j < newLines.size()-1) j++;
                }

                if(j < newLines.size()-1) { // Lines left over
                    std::vector<std::string> extraLines;
                    for(unsigned int k = j; k < newLines.size()-1; k++) {
                        extraLines.push_back(newLines[k]);
                    }
                    if(extraLines.size() != 0) {
                        Question* q = new Question();
                        *q = *readQuestion(extraLines);
                        a.followingQuestion = q;
                    }
                }

                q->answers.push_back(a);
            }
        }
        return q;
    }
}


std::vector<Question*> QuestManager::getDialogue(std::ifstream& file) {
    int pos = file.tellg();
    file.seekg(0, std::ios::end);
    int filesize = file.tellg();
    file.seekg(pos);

    logger->log("Loading dialogue. " + std::to_string(filesize) + " bytes to read.");

    std::vector<std::string> lines;

    std::vector<Question*> questions;

    std::string line;
    int depth = 0;
    while(std::getline(file, line)) {
        lines.push_back(extras::removeWhitespace(line)); // Simply read all lines into vector, without tabs or spaces
        if(line == "#BEGIN") {
            depth++;
            if(depth == 1) lines.pop_back();
        } else if (line == "#END") {
            depth--;
            if(depth == 0) lines.pop_back();
            if(depth <= 0) {
                // Got one question's lines completely
                Question* q = nullptr;
                q = readQuestion(lines);
                questions.push_back(q);
                lines.clear();
            }
        }
    }

    logger->log("Dialogue loaded! (" + std::to_string(sizeof(questions)) + " bytes)");

    return questions;
}

std::vector<Flag*> QuestManager::getFlags(std::ifstream& file) {
    int pos = file.tellg();
    file.seekg(0, std::ios::end);
    int filesize = file.tellg();
    file.seekg(pos);

    logger->log("Loading flags. " + std::to_string(filesize) + " bytes to read.");

    std::vector<Flag*> flags;

    std::string line;
    while(std::getline(file, line)) {
        Flag* f = new Flag(line);
        flags.push_back(f);
        logger->log("Loaded flag with id of " + std::to_string(Flag(line).id) + " and value of " + std::to_string(Flag(line).value));
    }

    logger->log("Flags loaded! (" + std::to_string(sizeof(flags)) + " bytes)");

    return flags;
}

std::vector<TradeTable*> QuestManager::getTradeTables(std::ifstream& file) {
    int pos = file.tellg();
    file.seekg(0, std::ios::end);
    int filesize = file.tellg();
    file.seekg(pos);

    logger->log("Loading trade tables. " + std::to_string(filesize) + " bytes to read.");

    std::vector<std::string> lines;

    std::vector<TradeTable*> tables;

    std::string line;

    bool reading = false;

    unsigned int id = 0;
    while(std::getline(file, line)) {
        if(reading) lines.push_back(extras::removeWhitespace(line)); // Simply read all lines into vector, without tabs or spaces
        if(line == "#BEGIN") {
            reading = true;
        } else if (line == "#END") {
            reading = false;
            lines.pop_back();

            // Got one trade table's lines completely
            TradeTable* table = nullptr;
            table = readTradeTable(lines, id++);
            tables.push_back(table);
            lines.clear();
        }
    }

    logger->log("Loaded trade tables! (" + std::to_string(sizeof(tables)) + " bytes)");

    return tables;
}

TradeTable* QuestManager::readTradeTable(std::vector<std::string> lines, unsigned int id) {
    TradeTable* table = new TradeTable;

    table->id = id;

    for(unsigned int i = 0; i < lines.size(); i++) {
        table->trades.push_back(readTrade(lines[i]));
    }

    return table;
}

Trade* QuestManager::readTrade(std::string line) {
    // Format is as follows: [required item id w/o brackets]:"metadata here in quotes"*[quantity w/o brackets]>[rewarded item id w/o brackets]:"metadata here in quotes"*[quantity w/o brackets]
    // So, int:string*int>int:string*int

    unsigned int reqId = -1, rewId = -1;
    std::string reqMeta, rewMeta;
    unsigned int reqQuan = -1, rewQuan = -1;

    bool requiredRead = false; // Has the required Item been completely read?

    std::string chars;

    for(unsigned int i = 0; i < line.size(); i++) {
        if(line[i] == ':') { // After an id
            if(requiredRead) {
                rewId = std::stoi(chars);
            } else {
                reqId = std::stoi(chars);
            }
            chars.clear();
        } else if(line[i] == '\"') {
            i++;
            while(line[i] != '\"') {
                chars += line[i];
                i++;
            }
            if(requiredRead) {
                //rewMeta = chars; /// TODO: Metadata
            } else {
                //reqMeta = chars;
            }
            chars.clear();
            i += 1;
            continue;
        } else if(line[i] == '>') {
            reqQuan = std::stoi(chars);
            requiredRead = true;
            chars.clear();
            continue;
        }
        chars += line[i];
    }
    rewQuan = std::stoi(chars);

    Trade* t = new Trade;
    Item* reqItem = nullptr;
    Item* rewItem = nullptr;

    reqItem = new Item(reqQuan, reqId, false);
    /// TODO: Metadata
    rewItem = new Item(rewQuan, rewId, false);
    /// TODO: Metadata

    t->requiredItem = reqItem;
    t->rewardedItem = rewItem;

    return t;
}
