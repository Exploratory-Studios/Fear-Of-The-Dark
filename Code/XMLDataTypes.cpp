#include "XMLDataTypes.h"

#include "XMLData.h"

namespace XMLModule {

    void Attribute::setDefault() {
        switch((unsigned int)type) {
            case (unsigned int)AttributeType::STRING: {
                data = (std::string)"UNDEFINED";
                break;
            }
            case (unsigned int)AttributeType::UNSIGNED_INT: {
                data = (unsigned int)-1;
                break;
            }
            case (unsigned int)AttributeType::INT: {
                data = (int)-1;
                break;
            }
            case (unsigned int)AttributeType::FLOAT: {
                data = (float)-1.0f;
                break;
            }
            case (unsigned int)AttributeType::VEC2: {
                glm::vec2 v2(69.0f, 69.0f);
                data = v2;
                break;
            }
            case (unsigned int)AttributeType::BOOL: {
                data = (bool)false;
                break;
            }
            case (unsigned int)AttributeType::FILEPATH_TEXTURE: {
                data = (std::string)(ASSETS_FOLDER_PATH + "/Textures/UNDEFINED.png");
                break;
            }
            case (unsigned int)AttributeType::FILEPATH_BUMPMAP: {
                data = (std::string)(ASSETS_FOLDER_PATH + "/Textures/UNDEFINED.png");
                break;
            }
            case (unsigned int)AttributeType::SCRIPT: {
                data = (unsigned int)-1;
                break;
            }
            case (unsigned int)AttributeType::STRING_FACTION: {
                data = (unsigned int)Categories::Faction::NEUTRAL;
                break;
            }
            case (unsigned int)AttributeType::VECTOR_UNSIGNED_INT: {
                data = std::vector<unsigned int>{};
                break;
            }
            default: {
                Logger::getInstance()->log("ERROR: XML Data type not supported!", true);
                break;
            }
        }
    }

    GenericData::GenericData(std::vector<Attribute> attrs) {
        addAttribute("name", AttributeType::STRING);
        addAttribute("id", AttributeType::UNSIGNED_INT);

        for(auto e : attrs) {
            addAttribute(e.name, e.type);
        }
    }

    void GenericData::init(rapidxml::xml_node<>* node) {
        /// The node must be the node of the Data. For example, if one were to load a Block item, we'd need to get the Block node, and pass it to this.


        m_attributes["id"].data = (unsigned int)std::stoi(node->first_attribute("id")->value());
        m_attributes["name"].data = (std::string)node->first_attribute("name")->value();

        for(auto& attr : m_attributes) { // Retrieved by reference, so we can change them!
            // For each attribute, load the stuff.
            if(attr.first == "name" || attr.first == "id") continue; // These are not regular child nodes, but rather special ones, taken care of beforehand!

            switch((unsigned int)attr.second.type) { /// TODO: Smarten this up
                case (unsigned int)AttributeType::STRING: {
                    std::string data;
                    getValue(node, attr.first, data);
                    attr.second.data = data;
                    break;
                }
                case (unsigned int)AttributeType::UNSIGNED_INT: {
                    unsigned int data;
                    getValue(node, attr.first, data);
                    attr.second.data = data;
                    break;
                }
                case (unsigned int)AttributeType::INT: {
                    int data;
                    getValue(node, attr.first, data);
                    attr.second.data = data;
                    break;
                }
                case (unsigned int)AttributeType::FLOAT: {
                    float data;
                    getValue(node, attr.first, data);
                    attr.second.data = data;
                    break;
                }
                case (unsigned int)AttributeType::VEC2: {
                    glm::vec2 v2;
                    getValue(node, attr.first + "X", v2.x);
                    getValue(node, attr.first + "Y", v2.y);

                    attr.second.data = v2;
                    break;
                }
                case (unsigned int)AttributeType::BOOL: {
                    bool data;
                    getValue(node, attr.first, data);
                    attr.second.data = data;
                    break;
                }
                case (unsigned int)AttributeType::FILEPATH_TEXTURE: {
                    std::string data;
                    getValue(node, attr.first, data);
                    attr.second.data = data;
                    attr.second.data = ASSETS_FOLDER_PATH + "/Textures/" + boost::get<std::string>(attr.second.data);
                    break;
                }
                case (unsigned int)AttributeType::FILEPATH_BUMPMAP: {
                    std::string data;
                    getValue(node, attr.first, data);
                    attr.second.data = data;
                    attr.second.data = ASSETS_FOLDER_PATH + "/Textures/BumpMaps/" + boost::get<std::string>(attr.second.data);
                    break;
                }
                case (unsigned int)AttributeType::SCRIPT: {
                    rapidxml::xml_node<>* scriptNode = node->first_node((char*)attr.first.c_str());
                    // This scriptNode has a bool (whether its a file or not) and the script filepath or literal

                    if(scriptNode) {
                        bool isFile = false;
                        getValue(scriptNode, "isFile", isFile); // Default false if its not found
                        std::string script;
                        getValue(scriptNode, "script", script);

                        if(isFile) {
                            script = ASSETS_FOLDER_PATH + "/Scripts/" + script;
                        }

                        ScriptingModule::Script scr(script, isFile);
                        attr.second.data = ScriptingModule::ScriptQueue::addScript(scr);
                    }
                    break;
                }
                case (unsigned int)AttributeType::STRING_FACTION: {
                    std::string factionString;
                    Categories::Faction faction;

                    getValue(node, attr.first, factionString);

                    if(factionString == "evil") {
                        faction = Categories::Faction::EVIL;
                    } else if(factionString == "bad") {
                        faction = Categories::Faction::BAD;
                    } else if(factionString == "neutral") {
                        faction = Categories::Faction::NEUTRAL;
                    } else if(factionString == "good") {
                        faction = Categories::Faction::GOOD;
                    } else if(factionString == "benign") {
                        faction = Categories::Faction::BENIGN;
                    } else {
                        faction = Categories::Faction::NEUTRAL;
                    }

                    attr.second.data = (unsigned int)faction;

                    break;
                }
                case (unsigned int)AttributeType::VECTOR_UNSIGNED_INT: {
                    std::string nodeName, valueName;
                    unsigned int slashIndex = attr.first.find("/");

                    nodeName = attr.first.substr(0, slashIndex); // Left half (excluding the slash)
                    valueName = attr.first.substr(slashIndex+1); // Right half (excluding the slash)

                    std::vector<unsigned int> vec;
                    getVector(node, nodeName, valueName, vec);
                    attr.second.data = vec;

                    break;
                }
                default: {
                    Logger::getInstance()->log("ERROR: XML Data type not supported!", true);

                    break;
                }

            }
        }

        ::XMLModule::getMetaData(node, m_metadata); //
    }

    void GenericData::addAttribute(std::string name, AttributeType type) {
        Attribute attr(name, type);

        m_attributes[name] = attr;
    }

    // basic
    /*void GenericData::getAttribute(std::string name, void* dataPtr) {
        // 1. Find the element:
        auto it = m_attributes.find(name);
        // 2. Check if it exists:
        if(it == m_attributes.end()) {
            Logger::getInstance()->log("ERROR: XML Attribute not found!: " + name, true);
            dataPtr = nullptr;
        }
        // 3. Return the attribute (it->second.data)'s value
        switch((unsigned int)it->second.type) {
            case (unsigned int)AttributeType::FILEPATH_TEXTURE:
            case (unsigned int)AttributeType::FILEPATH_BUMPMAP:
            case (unsigned int)AttributeType::STRING: {
                *dataPtr = it->second.data.s;
                break;
            }
            case (unsigned int)AttributeType::SCRIPT:
            case (unsigned int)AttributeType::STRING_FACTION:
            case (unsigned int)AttributeType::UNSIGNED_INT: {
                *dataPtr = it->second.data.ui;
                break;
            }
            case (unsigned int)AttributeType::INT: {
                *dataPtr = it->second.data.i;
                break;
            }
            case (unsigned int)AttributeType::FLOAT: {
                *dataPtr = it->second.data.f;
                break;
            }
            case (unsigned int)AttributeType::BOOL: {
                *dataPtr = it->second.data.b;
                break;
            }
            case (unsigned int)AttributeType::VECTOR_UNSIGNED_INT: {
                *dataPtr = it->second.data.v_ui;
                break;
            }
            case (unsigned int)AttributeType::VEC2: {
                *dataPtr = it->second.data.v2;
                break;
            }
            default: {
                Logger::getInstance()->log("ERROR: XML Data type not supported!", true);
                dataPtr = nullptr;
            }
        }
    }*/

    // Strings
    /*void GenericData::getAttribute(std::string name, std::string& dataPtr) {
        // 1. Find the element:
        auto it = m_attributes.find(name);
        // 2. Check if it exists:
        if(it == m_attributes.end()) {
            Logger::getInstance()->log("ERROR: XML Attribute not found!: " + name, true);
        }
        // 3. Return the attribute (it->second.data)'s value
        switch((unsigned int)it->second.type) {
            case (unsigned int)AttributeType::FILEPATH_TEXTURE:
            case (unsigned int)AttributeType::FILEPATH_BUMPMAP:
            case (unsigned int)AttributeType::STRING: {
                dataPtr = boost::get<std::string>(it->second.data);
                break;
            }
            default: {
                Logger::getInstance()->log("ERROR: XML Data type not supported! (Get: STR)", true);
            }
        }
    }

    // Unsigned int
    void GenericData::getAttribute(std::string name, unsigned int& dataPtr) {
        // 1. Find the element:
        auto it = m_attributes.find(name);
        // 2. Check if it exists:
        if(it == m_attributes.end()) {
            Logger::getInstance()->log("ERROR: XML Attribute not found!: " + name, true);
        }
        // 3. Return the attribute (it->second.data)'s value
        switch((unsigned int)it->second.type) {
            case (unsigned int)AttributeType::SCRIPT:
            case (unsigned int)AttributeType::STRING_FACTION:
            case (unsigned int)AttributeType::UNSIGNED_INT: {
                dataPtr = boost::get<unsigned int>(it->second.data);
                break;
            }
            default: {
                Logger::getInstance()->log("ERROR: XML Data type not supported (Get: U_I)!", true);
            }
        }
    }

    // Signed int
    void GenericData::getAttribute(std::string name, int& dataPtr) {
        // 1. Find the element:
        auto it = m_attributes.find(name);
        // 2. Check if it exists:
        if(it == m_attributes.end()) {
            Logger::getInstance()->log("ERROR: XML Attribute not found!: " + name, true);
        }
        // 3. Return the attribute (it->second.data)'s value
        switch((unsigned int)it->second.type) {
            case (unsigned int)AttributeType::INT: {
                dataPtr = boost::get<int>(it->second.data);
                break;
            }
            default: {
                Logger::getInstance()->log("ERROR: XML Data type not supported! (Get: INT)", true);
            }
        }
    }

    // float
    void GenericData::getAttribute(std::string name, float& dataPtr) {
        // 1. Find the element:
        auto it = m_attributes.find(name);
        // 2. Check if it exists:
        if(it == m_attributes.end()) {
            Logger::getInstance()->log("ERROR: XML Attribute not found!: " + name, true);
        }
        // 3. Return the attribute (it->second.data)'s value
        switch((unsigned int)it->second.type) {
            case (unsigned int)AttributeType::FLOAT: {
                dataPtr = boost::get<float>(it->second.data);
                break;
            }
            default: {
                Logger::getInstance()->log("ERROR: XML Data type not supported! (Get: FLO)", true);
            }
        }
    }

    // bool
    void GenericData::getAttribute(std::string name, bool& dataPtr) {
        // 1. Find the element:
        auto it = m_attributes.find(name);
        // 2. Check if it exists:
        if(it == m_attributes.end()) {
            Logger::getInstance()->log("ERROR: XML Attribute not found!: " + name, true);
        }
        // 3. Return the attribute (it->second.data)'s value
        switch((unsigned int)it->second.type) {
            case (unsigned int)AttributeType::BOOL: {
                dataPtr = boost::get<bool>(it->second.data);
                break;
            }
            default: {
                Logger::getInstance()->log("ERROR: XML Data type not supported! (Get: BOO)", true);
            }
        }
    }

    // vector (unsigned int)
    void GenericData::getAttribute(std::string name, std::vector<unsigned int>& dataPtr) {
        // 1. Find the element:
        auto it = m_attributes.find(name);
        // 2. Check if it exists:
        if(it == m_attributes.end()) {
            Logger::getInstance()->log("ERROR: XML Attribute not found!: " + name, true);
        }
        // 3. Return the attribute (it->second.data)'s value
        switch((unsigned int)it->second.type) {
            case (unsigned int)AttributeType::VECTOR_UNSIGNED_INT: {
                dataPtr = boost::get<std::vector<unsigned int>>(it->second.data);
                break;
            }
            default: {
                Logger::getInstance()->log("ERROR: XML Data type not supported! (Get: V_U_I)", true);
            }
        }
    }

    // glm::vec2
    void GenericData::getAttribute(std::string name, glm::vec2& dataPtr) {
        // 1. Find the element:
        auto it = m_attributes.find(name);
        // 2. Check if it exists:
        if(it == m_attributes.end()) {
            Logger::getInstance()->log("ERROR: XML Attribute not found!: " + name, true);
        }
        // 3. Return the attribute (it->second.data)'s value
        switch((unsigned int)it->second.type) {
            case (unsigned int)AttributeType::VEC2: {
                dataPtr = boost::get<glm::vec2>(it->second.data);
                break;
            }
            default: {
                Logger::getInstance()->log("ERROR: XML Data type not supported! (Get: V_2)", true);
            }
        }
    }*/

}
