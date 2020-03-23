#include "ScriptQueue.h"

std::vector<Script> ScriptQueue::m_scriptCache;
std::vector<Script> ScriptQueue::m_activeScripts;

unsigned int ScriptQueue::addScript(Script& script) // returns id of script, puts into cache
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
unsigned int ScriptQueue::addScript(std::string filePath) // same here
{
    filePath = ASSETS_FOLDER_PATH + "Scripts/" + filePath;

    Script s(filePath, true);

    unsigned int id = addScript(s);

    Logger::getInstance()->log("Successfully Loaded Script File: " + filePath + ", and given id of: " + std::to_string(id));

    return id;
}

void ScriptQueue::activateScript(unsigned int id, std::string preCommand) // Starts a script using the id given earlier
{
    m_activeScripts.push_back(m_scriptCache[id]);
    if(preCommand != "") m_activeScripts[m_activeScripts.size()-1].preCommand = preCommand;
    //Logger::getInstance()->log("Activated script with id of: " + std::to_string(id));
}

void ScriptQueue::deactivateScripts() // Clears active scripts
{
    m_activeScripts.clear();
}

void ScriptQueue::deactivateScript(unsigned int index)
{
    if(m_activeScripts.size() > 1) {
        for(unsigned int i = index; i < m_activeScripts.size()-1; i++) {
            m_activeScripts[i] = m_activeScripts[i+1];
        }
    }
    m_activeScripts.pop_back();
}
