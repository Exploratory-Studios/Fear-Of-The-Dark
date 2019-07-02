#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <GLEngineErrors.h>

#include "../Logging.h"
#include "../PresetValues.h"

class Script {
    public:
        Script() { commands.clear(); }
        std::vector<std::string> commands = {};
        void operator = (Script& s) {
            for(int i = 0; i < s.commands.size(); i++) {
                commands.emplace_back(s.commands[i]);
            }
        }
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

            unsigned int id = addScript(s);

            logger->log("Successfully Loaded Script File: " + filePath + ", and given id of: " + std::to_string(id));

            return id;
        }

        void activateScript(unsigned int id) // Starts a script using the id given earlier
        {
            m_activeScripts.push_back(id);
            logger->log("Activated script with id of: " + std::to_string(id));
        }

        void deactivateScripts() // Clears active scripts
        {
            m_activeScripts.clear();
            m_activeScripts.resize(0);
        }

    private:
        std::vector<Script> m_scriptCache; // Holds all scripts in memory to be used/reused later
        std::vector<unsigned int> m_activeScripts; // Is a list of active scripts' addresses

        Logger* logger = Logger::getInstance();
};
