#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <GLEngineErrors.h>

#include "../Logging.h"
#include "../PresetValues.h"

class Script {
    public:
        Script(std::string filename_text, bool isFile) {
            m_isFile = isFile;
            if(isFile) {
                m_fileName = filename_text;
            } else {
                m_text = filename_text;
            }
        }

        bool isFile() { return m_isFile; }
        std::string getFileName() { return m_fileName; }
        std::string getText() { return m_text; }

        bool operator==(Script r) {
            if(isFile() != r.isFile()) {
                return false;
            }
            if(getFileName() != r.getFileName()) {
                return false;
            }
            if(getText() != r.getText()) {
                return false;
            }
            return true;
        }

    private:
        bool m_isFile = false;
        std::string m_text;
        std::string m_fileName;
};

class ScriptQueue {
    friend class Scripter;
    public:
        unsigned int addScript(Script& script) // returns id of script, puts into cache
        {
            if(!m_scriptCache.empty()) {
                for(unsigned int i = 0; i < m_scriptCache.size(); i++) {
                    if(m_scriptCache[i] == script) {
                        return i;
                    }
                }
            }
            m_scriptCache.push_back(script);
            return m_scriptCache.size() - 1;
        }
        unsigned int addScript(std::string filePath) // same here
        {
            filePath = ASSETS_FOLDER_PATH + "Scripts/" + filePath;

            Script s(filePath, true);

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
