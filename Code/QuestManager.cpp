#include "QuestManager.h"

#include <iostream>

#include <GUI.h>

DialogueManager::~DialogueManager() {
    delete m_questionList;
    delete m_flagList;
}

void DialogueManager::startConversation(unsigned int id, GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, GLEngine::InputManager& input, GLEngine::GUI& gui) {
    Question* initialQuestion = (*m_questionList)[id];
    initConversation(id, gui);
    m_inConversation = true;
}

void DialogueManager::initConversation(unsigned int id, GLEngine::GUI& gui) { // Sets gui for conversation
    if(!m_inConversation) {

        CEGUI::FrameWindow* window = static_cast<CEGUI::FrameWindow*>(gui.createWidget("FOTDSkin/FrameWindow", glm::vec4(0.05f, 0.7f, 0.9f, 0.25f), glm::vec4(0.0f), "FrameWindowConversation"));

        window->setTitleBarEnabled(false);
        window->setCloseButtonEnabled(false);
        window->setRollupEnabled(false);
        window->setDragMovingEnabled(false);

        for(int i = 0; i < (*m_questionList)[id]->answers.size(); i++) {
            CEGUI::PushButton* answerButton = static_cast<CEGUI::PushButton*>(gui.createWidget(window, "FOTDSkin/Button", glm::vec4(0.02f, 0.05f + ((0.02f + 0.29f) * (float)i), 0.96f, 0.29f), glm::vec4(0.0f), std::string("Option" + std::to_string(i) + "Conversation")));
            answerButton->setText((*m_questionList)[id]->answers[i].str);
            m_buttons.push_back(answerButton);
        }

    }
}

void DialogueManager::draw(GLEngine::GUI& gui) {

}

void DialogueManager::update(GLEngine::InputManager& input) {
    if(m_inConversation) {
        if(input.isKeyPressed(SDL_BUTTON_LEFT)) {
            for(int i = 0; i < m_buttons.size(); i++) {
                m_buttons[i]->destroy();
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
                    if(extraLines.size() != 0) a.followingQuestion = readQuestion(extraLines);
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
