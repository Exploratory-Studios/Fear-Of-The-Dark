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

        std::string preCommand = "";

    private:
        bool m_isFile = false;
        std::string m_text;
        std::string m_fileName;
};

class ScriptQueue {
    friend class Scripter;
    public:
        static unsigned int addScript(Script& script); // returns id of script, puts into cache
        static unsigned int addScript(std::string filePath); // same here

        static void activateScript(unsigned int id, std::string preCommand = ""); // Starts a script using the id given earlier

        static void deactivateScripts(); // Clears active scripts

        static void deactivateScript(unsigned int index);

    private:
        static std::vector<Script> m_scriptCache; // Holds all scripts in memory to be used/reused later
        static std::vector<Script> m_activeScripts; // Is a list of active scripts' copies, with blank data such as place, timer info, etc.
};
