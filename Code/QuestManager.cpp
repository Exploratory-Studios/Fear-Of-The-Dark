#include "QuestManager.h"

#include <iostream>

QuestManager::QuestManager(std::string questListPath, std::string flagListPath)
{
    readDialogueFromList(questListPath);
    readFlagsFromList(flagListPath);
}

QuestManager::~QuestManager()
{
    //dtor
}

void QuestManager::readDialogueFromList(std::string listPath) {
    std::ifstream file(listPath);

    if(file.fail()) {
        std::cout << "\n!!! Failed to load quest list at " << listPath << " !!!\n";
    } else {
        m_questList = getDialogue(file);
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

Question QuestManager::readQuestion(std::vector<std::string> lines) {
    Question q;
    {
        int depth;

        for(unsigned int i = 0; i < lines.size(); i++) {
            if(q.str.empty() && lines[i] != "#BEGIN" && !lines[i].empty()) {
                q.str = lines[i];
            } else if (lines[i] == "#BEGIN") {
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
                    }
                    if(j < newLines.size()-1) j++;
                }

                if(j < newLines.size()-1) { // Lines left over
                    std::vector<std::string> extraLines;
                    for(unsigned int k = j; k < newLines.size(); k++) {
                        extraLines.push_back(newLines[k]);
                    }
                    extraLines.pop_back(); // Gets rid of extra "#END"
                    a.followingQuestion = readQuestion(extraLines);
                }
                q.answers.push_back(a);
            }
            return q;
        }
    }

}


std::vector<Question> QuestManager::getDialogue(std::ifstream& file) {
    int pos = file.tellg();
    file.seekg(0, std::ios::end);
    int filesize = file.tellg();
    file.seekg(pos);

    std::cout << "Loading dialogue. " << filesize << " bytes to read.\n";

    std::vector<std::string> lines;

    std::vector<Question> questions;

    std::string line;
    int depth = 0;
    while(std::getline(file, line)) {
        lines.push_back(line); // Simply read all lines into vector
        if(line == "#BEGIN") {
            depth++;
        } else if (line == "#END") {
            depth--;
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

std::vector<Flag> QuestManager::getFlags(std::ifstream& file) {
    int pos = file.tellg();
    file.seekg(0, std::ios::end);
    int filesize = file.tellg();
    file.seekg(pos);

    std::cout << "Loading flags. " << filesize << " bytes to read.\n";

    std::vector<Flag> flags;

    std::string line;
    while(std::getline(file, line)) {
        flags.push_back(Flag(line));
        std::cout << "Loaded flag with id of " << Flag(line).id << " and value of " << Flag(line).value << "\n";
    }

    std::cout << "Flags loaded! (" << filesize << " bytes)\n";

    return flags;
}
