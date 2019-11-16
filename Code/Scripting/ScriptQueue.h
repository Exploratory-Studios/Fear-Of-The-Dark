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
        unsigned int place = 0; // The current command, used when script is paused
        bool paused = false;
        float startTime = 0;
        float timerTime = 0;
        void operator = (Script& s) {
            commands.clear();
            for(unsigned int i = 0; i < s.commands.size(); i++) {
                commands.emplace_back(s.commands[i]);
            }
            place = s.place;
            paused = s.paused;
            startTime = s.startTime;
            timerTime = s.timerTime;
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
            m_activeScripts.push_back(m_scriptCache[id]);
            logger->log("Activated script with id of: " + std::to_string(id));
        }

        void deactivateScripts() // Clears active scripts
        {
            m_activeScripts.clear();
            m_activeScripts.resize(0);
        }

        void deactivateScript(unsigned int index)
        {
            if(m_activeScripts.size() > 1) {
                for(unsigned int i = index; i < m_activeScripts.size()-1; i++) {
                    m_activeScripts[i] = m_activeScripts[i+1];
                }
            }
            m_activeScripts.pop_back();
        }

    private:
        std::vector<Script> m_scriptCache; // Holds all scripts in memory to be used/reused later
        std::vector<Script> m_activeScripts; // Is a list of active scripts' copies, with blank data such as place, timer info, etc.

        Logger* logger = Logger::getInstance();
};
