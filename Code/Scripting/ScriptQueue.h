#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <GLEngineErrors.h>

#include "../Logging.h"
#include "../PresetValues.h"

struct Script {
    std::vector<std::string> commands;
};

class ScriptQueue {
    friend class Scripter;
    public:
        unsigned int addScript(Script& script) // returns id of script, puts into cache
        {
            if(!m_scriptCache.empty()) {
                for(unsigned int i = 0; i < m_scriptCache.size(); i++) {
                    if(m_scriptCache[i].commands == script.commands) {
                        return i;
                    }
                }
            }
            m_scriptCache.push_back(script);
            return m_scriptCache.size() - 1;
        }
        unsigned int addScript(std::string& filePath) // same here
        {
            filePath = ASSETS_FOLDER_PATH + "Scripts/" + filePath;
            std::ifstream file(filePath);

            if(file.fail()) {
                logger->log("Couldn't open script file: " + filePath + ". Quitting...", true);
                GLEngine::fatalError("Couldn't open script file: " + filePath);
            }

            Script s;

            std::string line;

            while(std::getline(file, line)) {
                s.commands.push_back(line);
            }

            m_scriptCache.push_back(s);

            logger->log("Successfully Loaded Script File: " + filePath);

            return m_scriptCache.size() - 1;
        }

        void activateScript(unsigned int id) // Starts a script using the id given earlier
        {
            m_activeScripts.push_back(&m_scriptCache[id]);
        }

        void deactivateScripts() // Clears active scripts
        {
            m_activeScripts.clear();
        }

    private:
        std::vector<Script> m_scriptCache; // Holds all scripts in memory to be used/reused later
        std::vector<Script*> m_activeScripts; // Is a list of active scripts' addresses

        Logger* logger = Logger::getInstance();
};
