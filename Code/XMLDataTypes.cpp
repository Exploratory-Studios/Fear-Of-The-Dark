#include "XMLDataTypes.h"

#include "Categories.h"

#include "MetaData.h"
#include "XMLData.h"
#include "ScriptQueue.h"

unsigned int ScriptData::getID() {
	if(m_id != (unsigned int) - 1) return m_id;
	if(stringData.length() <= 0) return (unsigned int) - 1;
	ScriptingModule::Script scr(ASSETS_FOLDER_PATH + "/Scripts/" + stringData, isFile);
	m_id = ScriptingModule::ScriptQueue::addScript(scr);
	return m_id;
}

namespace XMLModule {

	template<typename T>
	T GenericData::getAttributeByName(::std::string name) { /// TODO: Error checking and cleaning!
		T t = m_attributes[name]->getData<T>();
		return t;
	}

	template<typename T> // Templating acrobatics
	void AttributeBase::setData(T data) {
		dynamic_cast<Attribute<T>&>(*this).setData(data);
	}
	template<typename T>
	T AttributeBase::getData() {
		return dynamic_cast<Attribute<T>&>(*this).getData();
	}
	template<typename T>
	T* AttributeBase::getDataPtr() {
		return dynamic_cast<Attribute<T>&>(*this).getDataPtr();
	}

	template<>
	void Attribute<glm::vec2>::setDefault() {
		setData(glm::vec2(0.0f, 0.0f));
	}

	template<>
	void Attribute<std::vector<unsigned int>>::setDefault() {
		setData(std::vector<unsigned int> {});
	}

	template<>
	void Attribute<std::vector<float>>::setDefault() {
		setData(std::vector<float> {});
	}

	template<>
	void Attribute<std::vector<glm::vec2>>::setDefault() {
		setData(std::vector<glm::vec2> {});
	}

	template<>
	void Attribute<std::string>::setDefault() {
		setData("UNDEF");
	}

	template<>
	void Attribute<ScriptData>::setDefault() {
		ScriptData scr;
		setData(scr);
	}

	template<>
	std::string Attribute<glm::vec2>::getDataString() {
		return std::string(std::to_string(m_data->x) + "," + std::to_string(m_data->y));
	}
	template<>
	std::string Attribute<std::vector<glm::vec2>>::getDataString() {
		return "";
	}
	template<>
	std::string Attribute<std::vector<float>>::getDataString() {
		return "";
	}
	template<>
	std::string Attribute<std::vector<unsigned int>>::getDataString() {
		return "";
	}
	template<>
	std::string Attribute<ScriptData>::getDataString() {
		return "";
	}

	GenericData::GenericData() : name(""), id(0) {
		addAttribute("name", AttributeType::STRING, &name);
		addAttribute("id", AttributeType::UNSIGNED_INT, &id);
	}

	GenericData::GenericData(std::vector<AttributeBase*> attrs) : name(""), id(0) {
		addAttribute("name", AttributeType::STRING, &name);
		addAttribute("id", AttributeType::UNSIGNED_INT, &id);

		addAttributes(attrs);
	}

	void GenericData::init(rapidxml::xml_node<>* node) {
		/// The node must be the node of the Data. For example, if one were to load a Block item, we'd need to get the Block node, and pass it to this.
		m_attributes["id"]->setData((unsigned int)std::stoi(node->first_attribute("id")->value()));
		m_attributes["name"]->setData((std::string)node->first_attribute("name")->value());

		for(auto& attr : m_attributes) { // Retrieved by reference, so we can change them!
			// For each attribute, load the stuff.
			if(attr.first == "name" || attr.first == "id") continue; // These are not regular child nodes, but rather special ones, taken care of beforehand!

			switch((unsigned int)attr.second->type) { /// TODO: Smarten this up
				case(unsigned int)AttributeType::STRING: {
					std::string data;
					getValue(node, attr.first, data);
					attr.second->setData(data);
					break;
				}
				case(unsigned int)AttributeType::UNSIGNED_INT: {
					unsigned int data;
					getValue(node, attr.first, data);
					attr.second->setData(data);
					break;
				}
				case(unsigned int)AttributeType::INT: {
					int data;
					getValue(node, attr.first, data);
					attr.second->setData(data);
					break;
				}
				case(unsigned int)AttributeType::FLOAT: {
					float data;
					getValue(node, attr.first, data);
					attr.second->setData(data);
					break;
				}
				case(unsigned int)AttributeType::VEC2: {
					glm::vec2 v2;
					std::string str;
					getValue(node, attr.first, str);

					unsigned int index = str.find(",");
					std::string x = str.substr(0, index);
					std::string y = str.substr(index + 1);

					v2 = glm::vec2(std::stof(x), std::stof(y));

					attr.second->setData(v2);
					break;
				}
				case(unsigned int)AttributeType::BOOL: {
					bool data;
					getValue(node, attr.first, data);
					attr.second->setData(data);
					break;
				}
				case(unsigned int)AttributeType::FILEPATH_TEXTURE: {
					std::string data;
					getValue(node, attr.first, data);
					attr.second->setData((std::string)(data));
					break;
				}
				case(unsigned int)AttributeType::FILEPATH_BUMPMAP: {
					std::string data;
					getValue(node, attr.first, data);
					attr.second->setData((std::string)(data));
					break;
				}
				case(unsigned int)AttributeType::SCRIPT: {
					rapidxml::xml_node<>* scriptNode = node->first_node((char*)(attr.first.c_str()));
					// This scriptNode has a bool (whether its a file or not) and the script filepath or literal

					if(scriptNode) {
						bool isFile = false;
						if(!getValue(scriptNode, "isFile", isFile)) {
							Logger::getInstance()->log("XML ERROR: Script definition not formed correctly! Missing flag \"isFile\": ID=" + std::to_string(id), true);
							break;
						}
						std::string script;
						if(!getValue(scriptNode, "script", script)) {
							Logger::getInstance()->log("XML ERROR: Script definition not formed correctly! Missing attribute \"script\": ID=" + std::to_string(id), true);
							break;
						}

						ScriptData scr;
						scr.isFile = isFile;
						scr.stringData = script;

						attr.second->setData(scr);
					}
					break;
				}
				case(unsigned int)AttributeType::STRING_FACTION: {
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

					attr.second->setData((unsigned int)faction);

					break;
				}
				case(unsigned int)AttributeType::VECTOR_UNSIGNED_INT: {
					std::string nodeName, valueName;
					unsigned int slashIndex = attr.first.find("/");

					nodeName = attr.first.substr(0, slashIndex); // Left half (excluding the slash)
					valueName = attr.first.substr(slashIndex + 1); // Right half (excluding the slash)

					std::vector<unsigned int> vec;
					getVector(node, nodeName, valueName, vec);
					attr.second->setData(vec);

					break;
				}
				case(unsigned int)AttributeType::VECTOR_FLOAT: {
					std::string nodeName, valueName;
					unsigned int slashIndex = attr.first.find("/");

					nodeName = attr.first.substr(0, slashIndex);
					valueName = attr.first.substr(slashIndex + 1);

					std::vector<float> vec;
					getVector(node, nodeName, valueName, vec);
					attr.second->setData(vec);

					break;
				}
				case(unsigned int)AttributeType::VECTOR_VEC2: {
					std::string nodeName, valueName;
					unsigned int slashIndex = attr.first.find("/");

					nodeName = attr.first.substr(0, slashIndex);
					valueName = attr.first.substr(slashIndex + 1);

					std::vector<glm::vec2> vec;
					getVector(node, nodeName, valueName, vec);
					attr.second->setData(vec);

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

	void GenericData::write(::rapidxml::xml_document<>* doc) {

		char* nodeName_allocated = doc->allocate_string(nodeName.c_str());

		rapidxml::xml_node<>* node = doc->allocate_node(rapidxml::node_element, nodeName_allocated);
		doc->append_node(node);

		{
			const char* temp = std::to_string(id).c_str();
			char* val = node->document()->allocate_string(temp);
			rapidxml::xml_attribute<>* id = node->document()->allocate_attribute("id", val);
			node->append_attribute(id);

			const char* temp0 = name.c_str();
			val = node->document()->allocate_string(temp0);
			rapidxml::xml_attribute<>* name = node->document()->allocate_attribute("name", val);
			node->append_attribute(name);
		}

		for(auto& attr : m_attributes) { // Retrieved by reference, so we can change them!
			// For each attribute, load the stuff.
			if(attr.first == "name" || attr.first == "id") continue; // These are not regular child nodes, but rather special ones, taken care of beforehand!

			switch((unsigned int)attr.second->type) { /// TODO: Smarten this up
				case(unsigned int)AttributeType::STRING: {
					const char* temp = attr.second->getData<std::string>().c_str();
					char* val = node->document()->allocate_string(temp);
					const char* temp0 = attr.first.c_str();
					rapidxml::xml_node<>* newNode = node->document()->allocate_node(rapidxml::node_element, temp0, val);
					node->append_node(newNode);
					break;
				}
				case(unsigned int)AttributeType::UNSIGNED_INT: {
					const char* temp = std::to_string(attr.second->getData<unsigned int>()).c_str();
					char* val = node->document()->allocate_string(temp);
					const char* temp0 = attr.first.c_str();
					rapidxml::xml_node<>* newNode = node->document()->allocate_node(rapidxml::node_element, temp0, val);
					node->append_node(newNode);
					break;
				}
				case(unsigned int)AttributeType::INT: {
					const char* temp = std::to_string(attr.second->getData<int>()).c_str();
					char* val = node->document()->allocate_string(temp);
					const char* temp0 = attr.first.c_str();
					rapidxml::xml_node<>* newNode = node->document()->allocate_node(rapidxml::node_element, temp0, val);
					node->append_node(newNode);
					break;
				}
				case(unsigned int)AttributeType::FLOAT: {
					const char* temp = std::to_string(attr.second->getData<float>()).c_str();
					char* val = node->document()->allocate_string(temp);
					const char* temp0 = attr.first.c_str();
					rapidxml::xml_node<>* newNode = node->document()->allocate_node(rapidxml::node_element, temp0, val);
					node->append_node(newNode);
					break;
				}
				case(unsigned int)AttributeType::VEC2: {
					glm::vec2 value = attr.second->getData<glm::vec2>();
					std::string XYValueString = std::to_string(value.x) + "," + std::to_string(value.y);

					const char* temp = XYValueString.c_str();
					char* val = node->document()->allocate_string(temp);
					const char* temp0 = attr.first.c_str();
					rapidxml::xml_node<>* newNode = node->document()->allocate_node(rapidxml::node_element, temp0, val);
					node->append_node(newNode);
					break;
				}
				case(unsigned int)AttributeType::BOOL: {
					const char* temp = std::to_string(attr.second->getData<bool>()).c_str();
					char* val = node->document()->allocate_string(temp);
					const char* temp0 = attr.first.c_str();
					rapidxml::xml_node<>* newNode = node->document()->allocate_node(rapidxml::node_element, temp0, val);
					node->append_node(newNode);
					break;
				}
				case(unsigned int)AttributeType::FILEPATH_TEXTURE: {
					std::string valueString = attr.second->getData<std::string>();
					if(attr.second->getData<std::string>().length() == 0) valueString = "";
					const char* temp = valueString.c_str();
					char* val = node->document()->allocate_string(temp);
					const char* temp0 = attr.first.c_str();
					rapidxml::xml_node<>* newNode = node->document()->allocate_node(rapidxml::node_element, temp0, val);
					node->append_node(newNode);
					break;
				}
				case(unsigned int)AttributeType::FILEPATH_BUMPMAP: {
					std::string valueString = attr.second->getData<std::string>();
					if(attr.second->getData<std::string>().length() == 0) valueString = "";
					const char* temp = valueString.c_str();
					char* val = node->document()->allocate_string(temp);
					const char* temp0 = attr.first.c_str();
					rapidxml::xml_node<>* newNode = node->document()->allocate_node(rapidxml::node_element, temp0, val);
					node->append_node(newNode);
					break;
				}
				case(unsigned int)AttributeType::SCRIPT: {
					if(attr.first.length() == 0) break;

					const char* temp = attr.first.c_str();
					char* val = node->document()->allocate_string(temp);
					rapidxml::xml_node<>* newNode = node->document()->allocate_node(rapidxml::node_element, val);
					node->append_node(newNode);

					char* name1 = node->document()->allocate_string("isFile");
					const char* temp1 = std::to_string(attr.second->getData<ScriptData>().isFile).c_str();
					char* val1 = node->document()->allocate_string(temp1);
					rapidxml::xml_node<>* newNode1 = newNode->document()->allocate_node(rapidxml::node_element, name1, val1);
					newNode->append_node(newNode1);

					char* name2 = node->document()->allocate_string("script");
					const char* temp2 = std::string(attr.second->getData<ScriptData>().stringData).c_str();
					char* val2 = node->document()->allocate_string(temp2);
					rapidxml::xml_node<>* newNode2 = newNode->document()->allocate_node(rapidxml::node_element, name2, val2);
					newNode->append_node(newNode2);
					break;
				}
				case(unsigned int)AttributeType::STRING_FACTION: {
					std::string factionString;
					unsigned int faction = attr.second->getData<unsigned int>();

					if(faction == (unsigned int)Categories::Faction::EVIL) {
						factionString = "evil";
					} else if(faction == (unsigned int)Categories::Faction::BAD) {
						factionString = "bad";
					} else if(faction == (unsigned int)Categories::Faction::NEUTRAL) {
						factionString = "neutral";
					} else if(faction == (unsigned int)Categories::Faction::GOOD) {
						factionString = "good";
					} else if(faction == (unsigned int)Categories::Faction::BENIGN) {
						factionString = "benign";
					}

					// Allocate faction string
					const char* tempName = attr.first.c_str();
					const char* tempVal = factionString.c_str();
					char* nameStringAllocated = node->document()->allocate_string(tempName);
					char* facStringAllocated = node->document()->allocate_string(tempVal);
					rapidxml::xml_node<>* newNode = node->document()->allocate_node(rapidxml::node_element, nameStringAllocated, facStringAllocated);
					node->append_node(newNode);

					break;
				}
				case(unsigned int)AttributeType::VECTOR_UNSIGNED_INT: {
					std::string nodeName, valueName;
					unsigned int slashIndex = attr.first.find("/");

					nodeName = attr.first.substr(0, slashIndex);
					valueName = attr.first.substr(slashIndex + 1);

					std::vector<unsigned int> value = attr.second->getData<std::vector<unsigned int>>();

					const char* temp = nodeName.c_str();
					char* val = node->document()->allocate_string(temp);
					rapidxml::xml_node<>* newNode = node->document()->allocate_node(rapidxml::node_element, val);
					node->append_node(newNode);

					const char* temp0 = valueName.c_str();
					char* nodeNameV = node->document()->allocate_string(temp0);

					for(unsigned int i = 0; i < value.size(); i++) {
						const char* temp1 = std::to_string(value[i]).c_str();
						char* val1 = node->document()->allocate_string(temp1);
						rapidxml::xml_node<>* newNode1 = node->document()->allocate_node(rapidxml::node_element, nodeNameV, val1);
						newNode->append_node(newNode1);
					}

					break;
				}
				case(unsigned int)AttributeType::VECTOR_FLOAT: {
					std::string nodeName, valueName;
					unsigned int slashIndex = attr.first.find("/");

					nodeName = attr.first.substr(0, slashIndex);
					valueName = attr.first.substr(slashIndex + 1);

					std::vector<float> value = attr.second->getData<std::vector<float>>();

					const char* temp = nodeName.c_str();
					char* val = node->document()->allocate_string(temp);
					rapidxml::xml_node<>* newNode = node->document()->allocate_node(rapidxml::node_element, val);
					node->append_node(newNode);

					const char* temp0 = valueName.c_str();
					char* nodeNameV = node->document()->allocate_string(temp0);

					for(unsigned int i = 0; i < value.size(); i++) {
						const char* temp1 = std::to_string(value[i]).c_str();
						char* val1 = node->document()->allocate_string(temp1);
						rapidxml::xml_node<>* newNode1 = node->document()->allocate_node(rapidxml::node_element, nodeNameV, val1);
						newNode->append_node(newNode1);
					}

					break;
				}
				case(unsigned int)AttributeType::VECTOR_VEC2: {
					std::string nodeName, valueName;
					unsigned int slashIndex = attr.first.find("/");

					nodeName = attr.first.substr(0, slashIndex);
					valueName = attr.first.substr(slashIndex + 1);

					std::vector<glm::vec2> value = attr.second->getData<std::vector<glm::vec2>>();

					const char* temp = nodeName.c_str();
					char* val = node->document()->allocate_string(temp);
					rapidxml::xml_node<>* newNode = node->document()->allocate_node(rapidxml::node_element, val);
					node->append_node(newNode);

					const char* temp1 = valueName.c_str();
					char* nodeNameV = node->document()->allocate_string(temp1);

					for(unsigned int i = 0; i < value.size(); i++) {
						std::string valueString = std::string(std::to_string(value[i].x) + "," + std::to_string(value[i].y));
						const char* temp2 = valueString.c_str();
						char* val1 = node->document()->allocate_string(temp2);
						rapidxml::xml_node<>* newNode1 = node->document()->allocate_node(rapidxml::node_element, nodeNameV, val1);
						newNode->append_node(newNode1);
					}

					break;
				}
				default: {
					Logger::getInstance()->log("ERROR: XML Data type not supported!", true);

					break;
				}

			}
		}
	}

	template<class T>
	void GenericData::addAttribute(std::string name, AttributeType type, T* data) {
		Attribute<T>* attr = new Attribute<T>(name, type, data);

		m_attributes[name] = attr;
	}

	/*void GenericData::addAttribute(std::string name, AttributeType type, void* data) {
		Attribute attr;
		attr.name = name;
		attr.m_type = type;

		attr.m_data = std::any_cast<std::any*>(data); // Big ol switch statement was here

		addAttribute(attr);
	}*/

	void GenericData::addAttribute(AttributeBase* a) {
		m_attributes[a->name] = a;
	}

	void GenericData::addAttributes(std::vector<AttributeBase*> attrs) {
		/**
		 * @brief Adds a vector of attributes
		 * @param attrs
		 */

		for(auto* e : attrs) {
			addAttribute(e);
		}
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
