#include "QuestManager.h"

#include <iostream>

#include <GUI.h>

DialogueManager::~DialogueManager() {
    delete m_questionList;
    delete m_flagList;
}

void DialogueManager::startConversation(unsigned int id, GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, GLEngine::InputManager& input, GLEngine::GUI& gui) {
    Question* initialQuestion = (*m_questionList)[id];
    m_currentQuestion = initialQuestion;
    initConversation(initialQuestion, gui);
    m_inConversation = true;
}

void DialogueManager::initConversation(Question* initialQuestion, GLEngine::GUI& gui) { // Sets gui for conversation
    if(!m_inConversation) {

        CEGUI::FrameWindow* window = static_cast<CEGUI::FrameWindow*>(gui.createWidget("FOTDSkin/FrameWindow", glm::vec4(0.05f, 0.7f, 0.9f, 0.25f), glm::vec4(0.0f), "FrameWindowConversation"));

        window->setTitleBarEnabled(false);
        window->setCloseButtonEnabled(false);
        window->setRollupEnabled(false);
        window->setDragMovingEnabled(false);

        CEGUI::FrameWindow* textFrame = static_cast<CEGUI::FrameWindow*>(gui.createWidget(window, "FOTDSkin/FrameWindow", glm::vec4(0.02f, 0.05f, 0.96f, 0.25f), glm::vec4(0.0f), "TextFrameConversation"));

        textFrame->setTitleBarEnabled(false);
        textFrame->setCloseButtonEnabled(false);
        textFrame->setRollupEnabled(false);
        textFrame->setDragMovingEnabled(false);

        CEGUI::DefaultWindow* textWindow = static_cast<CEGUI::DefaultWindow*>(gui.createWidget(textFrame, "FOTDSkin/Label", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f), "TextBoxConversation"));\
        textWindow->setText("[colour='FF000000']" + m_currentQuestion->str);

        CEGUI::DefaultWindow* answersWindow = static_cast<CEGUI::DefaultWindow*>(gui.createWidget(window, "FOTDSkin/Label", glm::vec4(0.02f, 0.325f, 0.96f, 0.7f), glm::vec4(0.0f), "OptionsBoxConversation"));

        CEGUI::ScrollablePane* answersScroller = static_cast<CEGUI::ScrollablePane*>(gui.createWidget(answersWindow, "FOTDSkin/ScrollablePane", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f), "OptionsScrollPaneConversation"));
        m_otherWidgets.push_back(answersScroller);
        m_otherWidgets.push_back(answersWindow);
        m_otherWidgets.push_back(textWindow);
        m_otherWidgets.push_back(textFrame);
        m_otherWidgets.push_back(window);
        // We must push them in the order that they are in (children first, then parents)

        float paddingIncrementer = 0;
        for(int i = 0; i < initialQuestion->answers.size(); i++) {
            CEGUI::PushButton* answerButton = static_cast<CEGUI::PushButton*>(gui.createWidget(answersScroller, "FOTDSkin/Button", glm::vec4(0.0f, 0.025f + ((0.03f + 0.425f) * paddingIncrementer), 1.0f, 0.425f), glm::vec4(0.0f), std::string("Option" + std::to_string(i) + "Conversation")));
            answerButton->setText(initialQuestion->answers[i].str);
            m_buttons.push_back(answerButton);

            paddingIncrementer++;
            for(int j = 0; j < m_currentQuestion->answers[i].requiredFlags.arrangement.size(); j++) {
                unsigned int flagId = m_currentQuestion->answers[i].requiredFlags.arrangement[j].id;
                if((*m_flagList)[flagId]->value != m_currentQuestion->answers[i].requiredFlags.arrangement[j].value) {
                    paddingIncrementer--;
                    m_buttons[i]->setVisible(false);
                    break;
                }
            }
        }
    }
}

void DialogueManager::draw(GLEngine::GUI& gui) {

}

void DialogueManager::update(GLEngine::InputManager& input, GLEngine::GUI& gui) {
    if(m_inConversation) {
        if(input.isKeyPressed(SDL_BUTTON_LEFT)) {

            int optionChosen = -1;

            // Set value of flags
            for(int i = 0; i < m_buttons.size(); i++) {
                if(m_buttons[i]->isPushed() && m_buttons[i]->isVisible()) {
                    optionChosen = i;
                    for(int j = 0; j < m_currentQuestion->answers[i].followingFlags.arrangement.size(); j++) {
                        unsigned int flagId = m_currentQuestion->answers[i].followingFlags.arrangement[j].id;

                        if(flagId >= m_flagList->size()) {
                            std::cout << "ERROR: Flags list is not long enough. please add more entries or fix the dialogue's flags (following)\n";
                            break;
                        }

                        (*m_flagList)[flagId]->value = m_currentQuestion->answers[i].followingFlags.arrangement[j].value;
                    }
                }
            }

            if(optionChosen != -1) {
                for(int i = 0; i < m_buttons.size(); i++) {
                    m_buttons[i]->destroy();
                    delete m_buttons[i];
                    m_buttons.pop_back();
                }
                m_buttons.clear();
                for(int i = 0; i < m_otherWidgets.size(); i++) {
                    m_otherWidgets[i]->destroy();
                    delete m_otherWidgets[i];
                }
                m_otherWidgets.clear();
                m_inConversation = false;
                if(m_currentQuestion->answers[optionChosen].followingQuestion) {
                    initConversation(m_currentQuestion->answers[optionChosen].followingQuestion, gui);
                }
            }
        }
    }
}

QuestManager::QuestManager(std::string questionListPath, std::string flagListPath)
{
    readDialogueFromList(questionListPath);
    readFlagsFromList(flagListPath);

    m_dialogueManager = new DialogueManager(&m_questionList, &m_flagList);
}

QuestManager::~QuestManager()
{
    delete m_dialogueManager;
}

void QuestManager::readDialogueFromList(std::string listPath) {
    std::ifstream file(listPath);

    if(file.fail()) {
        std::cout << "\n!!! Failed to load quest list at " << listPath << " !!!\n";
    } else {
        m_questionList = getDialogue(file);
    }
}

void QuestManager::readFlagsFromList(std::string listPath) {
    std::ifstream file(listPath);

    if(file.fail()) {
        std::cout << "\n!!! Failed to load flag list at " << listPath << " !!!\n";
    } else {
        m_flagList = getFlags(file);
    }
}

Question* QuestManager::readQuestion(std::vector<std::string> lines) {
    Question* q = new Question();
    {
        int depth;
        q->str = lines[0];

        for(unsigned int i = 1; i < lines.size(); i++) {
            if (lines[i] == "#BEGIN") {
                Answer a;
                int answerDepth = 0;
                std::vector<std::string> newLines;
                for(unsigned int j = i; j < lines.size(); j++) {
                    newLines.push_back(lines[j]); // Gets all the lines for the answer
                    if(lines[j] == "#BEGIN") {
                        answerDepth++;
                    } else if(lines[j] == "#END") {
                        answerDepth--;
                        if(answerDepth <= 0) {
                            break;
                        }
                    }
                }

                a.str = newLines[1]; // Get string. First is #BEGIN

                unsigned int j = 2;
                for(int k = 0; k < 2; k++) {
                    while(newLines[j] != "#FLAGS_END" && newLines[j] != "#END") {
                        Flag f = Flag(newLines[j]);
                        if(k == 0) a.requiredFlags.arrangement.push_back(f);
                        if(k == 1) a.followingFlags.arrangement.push_back(f);
                        if(j < newLines.size()-1) j++;

                        if(newLines[j] == "#END");
                    }
                    if(j < newLines.size()-1) j++;
                }

                if(j < newLines.size()-1) { // Lines left over
                    std::vector<std::string> extraLines;
                    for(unsigned int k = j; k < newLines.size(); k++) {
                        extraLines.push_back(newLines[k]);
                    }
                    extraLines.pop_back(); // Gets rid of extra "#END"
                    if(extraLines.size() != 0) {
                        a.followingQuestion = readQuestion(extraLines);
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

    std::cout << "Loading dialogue. " << filesize << " bytes to read.\n";

    std::vector<std::string> lines;

    std::vector<Question*> questions;

    std::string line;
    int depth = 0;
    while(std::getline(file, line)) {
        lines.push_back(line); // Simply read all lines into vector
        if(line == "#BEGIN") {
            depth++;
            if(depth == 1) lines.pop_back();
        } else if (line == "#END") {
            depth--;
            if(depth == 0) lines.pop_back();
            if(depth <= 0) {
                // Got one question's lines completely
                questions.push_back(readQuestion(lines));
                lines.clear();
            }
        }
    }

    std::cout << "Dialogue loaded! (" << filesize << " bytes)\n";

    return questions;
}

std::vector<Flag*> QuestManager::getFlags(std::ifstream& file) {
    int pos = file.tellg();
    file.seekg(0, std::ios::end);
    int filesize = file.tellg();
    file.seekg(pos);

    std::cout << "Loading flags. " << filesize << " bytes to read.\n";

    std::vector<Flag*> flags;

    std::string line;
    while(std::getline(file, line)) {
        Flag* f = new Flag(line);
        flags.push_back(f);
        std::cout << "Loaded flag with id of " << Flag(line).id << " and value of " << Flag(line).value << "\n";
    }

    std::cout << "Flags loaded! (" << filesize << " bytes)\n";

    return flags;
}
