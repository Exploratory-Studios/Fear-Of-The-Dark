#include "MainScreen.h"

#include "XMLData.h"
#include "XMLDataTypes.h"

#include "Categories.h"

#include <rapidxml/rapidxml_print.hpp>

#include <Window.h>

#include <stdio.h>
#ifdef WINDOWS
#include <direct.h>
#define GCD _getcwd
#else
#include <unistd.h>
#define GCD getcwd
#endif
#include<iostream>

#define BUTTONS_HORIZONTAL 8 // number of buttons that fit horizontally.

std::string GetCurrentDirectory(void) {
	char buff[FILENAME_MAX];
	GCD(buff, FILENAME_MAX);
	std::string current_working_dir(buff);
	return current_working_dir;
}

void EditWindow::init(std::string name, GLEngine::GUI& gui, std::vector<CEGUI::PushButton*>& buttons, std::vector<std::string> types) {
	m_types = types;

	m_gui = &gui;

	m_window = static_cast<CEGUI::FrameWindow*>(m_gui->createWidget("WindowsLook/FrameWindow", glm::vec4(0.2f, 0.05f, 0.75f, 0.7f), glm::vec4(0.0f), name + "Window"));
	m_window->setRollupEnabled(false);
	m_window->setTitleBarEnabled(false);
	m_window->setCloseButtonEnabled(false);
	m_window->setEnabled(false);
	m_window->setVisible(false);

	CEGUI::FrameWindow* f = static_cast<CEGUI::FrameWindow*>(m_gui->createWidget(m_window, "WindowsLook/Label", glm::vec4(0.025f, 0.025f, 0.15f, 0.05f), glm::vec4(0.0f), name + "Label"));
	f->setFont("DejaVuSans-14-NoScale");
	f->setText(name);

	m_itemListBox0 = static_cast<CEGUI::ItemListbox*>(m_gui->createWidget(m_window, "WindowsLook/ItemListbox", glm::vec4(0.01f, 0.1f, 0.49f, 0.88f), glm::vec4(0.0f), name + "LB0"));
	m_itemListBox0->setMultiSelectEnabled(false);
	m_itemListBox0->setSortMode(CEGUI::ItemListBase::SortMode::Ascending);
	m_itemListBox0->setSortEnabled(false);
	m_itemListBox0->subscribeEvent(CEGUI::ItemListbox::EventSelectionChanged, CEGUI::Event::Subscriber([ = ](const CEGUI::EventArgs & evnt)->bool{return onSelectionChanged(evnt);}));

	m_itemListBox1 = static_cast<CEGUI::ItemListbox*>(m_gui->createWidget(m_window, "WindowsLook/ItemListbox", glm::vec4(0.5f, 0.1f, 0.49f, 0.88f), glm::vec4(0.0f), name + "LB1"));
	m_itemListBox1->setMultiSelectEnabled(false);
	m_itemListBox1->setSortMode(CEGUI::ItemListBase::SortMode::Ascending);
	m_itemListBox1->setSortEnabled(false);
	//m_itemListBox0->subscribeEvent(CEGUI::ItemListbox::EventSelectionChanged, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs& evnt)->bool{return onSelectionChanged(evnt);}));

	// Make add buttons
	for(unsigned int i = 0; i < m_types.size(); i++) {
		CEGUI::PushButton* addButton = static_cast<CEGUI::PushButton*>(m_gui->createWidget("WindowsLook/Button", glm::vec4(0.025f, 0.1f * (i + 1) + 0.025f, 0.15f, 0.1f), glm::vec4(0.0f), name + "ADD_" + std::to_string(i)));
		addButton->setText("New\n" + m_types[i]);
		addButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([ = ](const CEGUI::EventArgs & evnt)->bool{
			createNewEntry(i);
		}));
		m_buttons.push_back(addButton);
	}
	// Make remove button
	CEGUI::PushButton* removeButton = static_cast<CEGUI::PushButton*>(m_gui->createWidget("WindowsLook/Button", glm::vec4(0.025f, 0.025f, 0.15f, 0.1f), glm::vec4(0.0f), name + "REMOVE"));
	removeButton->setText("Remove");
	removeButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([ = ](const CEGUI::EventArgs & evnt)->bool{
		removeEntry();
	}));
	m_buttons.push_back(removeButton);

	// Make EditBox button
	CEGUI::PushButton* openWindowButton = static_cast<CEGUI::PushButton*>(m_gui->createWidget("WindowsLook/Button", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f), name + "Window_Button"));
	openWindowButton->setText(name);
	buttons.push_back(openWindowButton);

	m_name = name;

	setActive(false);
}

void EditWindow::setActive(bool active) {
	m_window->setEnabled(active);
	m_window->setVisible(active);
	for(unsigned int i = 0; i < m_buttons.size(); i++) {
		m_buttons[i]->setEnabled(active);
		m_buttons[i]->setVisible(active);
	}
	m_active = active;
}

void EditWindow::createNewEntry(unsigned int type) {
	std::string nodeName = m_types[type];
	unsigned int id = XMLModule::XMLData::getMapFromNodename(nodeName)->size();

	XMLModule::GenericData* obj = XMLModule::XMLData::createDataFromNodename(nodeName);
	obj->id = id;
	obj->name = "New " + nodeName;

	addEntry(obj, nodeName);

	XMLModule::XMLData::addData(obj, nodeName);
}

void EditWindow::addEntry(XMLModule::GenericData* obj, std::string& nodeName) {
	CEGUI::ItemEntry* l = static_cast<CEGUI::ItemEntry*>(m_gui->createWidget(m_itemListBox0, "WindowsLook/ListboxItem", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f), obj->name + m_name + std::to_string(obj->id) + "Entry"));

	std::ostringstream address;
	address << (void const *)l;
	std::string entryAddress = address.str();

	l->setName(obj->name + m_name + std::to_string(obj->id) + "Entry" + entryAddress);

	l->setUserString("ID", std::to_string(m_data.size()));

	l->setSelectable(true);
	std::string a = std::string(nodeName + ":" + std::to_string(obj->id));
	l->setText(a);
	l->setProperty("TextColour", "00000000");
	l->setProperty("SelectedTextColour", "00000000");

	CEGUI::DefaultWindow* label = static_cast<CEGUI::DefaultWindow*>(m_gui->createWidget(l, "WindowsLook/Label", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f), obj->name + m_name + std::to_string(obj->id) + "EntryLabel"));

	label->setName(obj->name + m_name + std::to_string(obj->id) + "EntryLabel" + entryAddress);

	label->setText(obj->name + " (ID: " + std::to_string(obj->id) + ", Type: " + nodeName + ")");
	label->setMousePassThroughEnabled(true);
	label->setInheritsAlpha(false);

	m_itemListBox0->insertItem(l, nullptr);
	m_itemListBox0->ensureItemIsVisibleVert(*l);

	m_data.push_back(obj);

	m_itemListBox0->sortList();
}

void EditWindow::removeAllEntries() {
	// Clear XML
	for(unsigned int i = 0; i < m_data.size(); i++) {
		std::string nodeName = m_data[i]->getNodeName();
		XMLModule::XMLData::removeData(m_data[i], nodeName);
	}

	// Clear local data (m_data)
	m_data.clear();

	// Clear CEGUI data (listboxes)
	m_itemListBox0->resetList();
	m_itemListBox1->resetList();
}

void EditWindow::removeEntry() {
	CEGUI::ItemEntry* selected = m_itemListBox0->getFirstSelectedItem();
	if(selected) {
		// Plan:
		// 1. Remove entry from m_data, and update IDs
		// 2. Get all nodenames, from itemEntry texts
		// 3. Remove & destroy all itemEntries and XMLData
		// 4. Loop through new m_data, adding entries (addEntry), using the nodenames from step 2

		// 1.
		std::vector<std::string> nodeNames;

		// Remove entry from m_data & XML Data
		std::string userString = selected->getUserString("ID").c_str();
		unsigned int index = std::stoi(userString);

		std::string type = std::string(selected->getText().c_str()).substr(0, std::string(selected->getText().c_str()).find(":"));
		XMLModule::XMLData::removeData(m_data[index], type);

		//delete m_data[index];
		m_data[index] = nullptr;
		for(int i = index; i < m_data.size() - 1; i++) {
			m_data[i] = m_data[i + 1];
		}
		m_data.pop_back();

		// Entry removed and IDs updated. Step 1 complete
		// 2. get all nodenames
		nodeNames.resize(m_data.size(), "");
		for(unsigned int i = 0; i < m_itemListBox0->getItemCount(); i++) {
			// If the i is less than the removed index, the index is the same. If equal, ignore. If more, minus 1
			unsigned int dataIndex = std::stoi(m_itemListBox0->getItemFromIndex(i)->getUserString("ID").c_str());

			std::string text = std::string(m_itemListBox0->getItemFromIndex(i)->getText().c_str());
			std::string nodeName = text.substr(0, text.find(":"));

			if(dataIndex < index) {
				// Less than removed index
				nodeNames[dataIndex] = nodeName;
			} else if(dataIndex > index) {
				// More than removed index, subtract 1
				nodeNames[dataIndex - 1] = nodeName;
			}
		}

		// Step 2 is done, we have all node names

		std::vector<XMLModule::GenericData*> dataCopy;

		// 3. remove and destroy all entries and XML, as well as m_data
		for(unsigned int i = 0; i < m_data.size(); i++)
			XMLModule::XMLData::removeData(m_data[i], nodeNames[i]);

		// Reset IDs
		unsigned int curID = 0;
		for(unsigned int i = 0; i < m_itemListBox0->getItemCount(); i++) {
			unsigned int dataIndex = std::stoi(m_itemListBox0->getItemFromIndex(i)->getUserString("ID").c_str());

			if(dataIndex == index) continue;
			if(dataIndex > index) dataIndex--;

			if(m_data[dataIndex]->id < curID) {
				curID = 0;
			}
			m_data[dataIndex]->id = curID;

			curID++;
		}

		for(unsigned int i = 0; i < m_data.size(); i++) {
			dataCopy.push_back(m_data[i]);
		}

		m_data.clear();

		m_itemListBox0->resetList();
		m_itemListBox1->resetList();

		// Step 3 is done, everything is gone. Reduced to atoms

		// 4. Reconstruct
		for(unsigned int i = 0; i < dataCopy.size(); i++) {
			addEntry(dataCopy[i], nodeNames[i]);
			XMLModule::XMLData::addData(dataCopy[i], nodeNames[i]);
			m_data.push_back(dataCopy[i]);
			m_itemListBox0->sortList();
		}
		// All should be reconstructed now.
	}
}

void EditWindow::resetIDs() {
	// Check if the current ID is less than the last. If it is, restart; we're onto a new map (new set of IDs).
	unsigned int curID = 0;
	for(unsigned int i = 0; i < m_itemListBox0->getItemCount(); i++) {
		unsigned int dataIndex = std::stoi(m_itemListBox0->getItemFromIndex(i)->getUserString("ID").c_str());

		if(m_data[dataIndex]->id < curID) {
			curID = 0;
		}
		m_data[dataIndex]->id = curID;

		curID++;
	}
}

void EditWindow::addAttributes(std::vector<XMLModule::AttributeBase*> attributes) {
	m_itemListBox1->resetList(); // remove all children and delete them

	for(int i = 0; i < attributes.size(); i++) {
		addAttribute(attributes[i]);
	}

	m_itemListBox1->sortList();
}

void EditWindow::addAttribute(XMLModule::AttributeBase* attr) {
	if(attr->name == "id") return;

	CEGUI::ItemEntry* l = static_cast<CEGUI::ItemEntry*>(m_gui->createWidget(m_itemListBox1, "WindowsLook/ListboxItem", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f), attr->name + m_name + "AttributeEntry"));
	l->setSelectable(false);
	std::string sortT = std::string(m_itemListBox1->getItemCount(), 1);
	l->setText(sortT);

	CEGUI::DefaultWindow* label = static_cast<CEGUI::DefaultWindow*>(m_gui->createWidget(l, "WindowsLook/Label", glm::vec4(0.0f, 0.0f, 0.5f, 1.0f), glm::vec4(0.0f), attr->name + m_name + "AttributeEntryLabel"));
	label->setText(attr->name);

	if(attr->type == XMLModule::AttributeType::STRING || attr->type == XMLModule::AttributeType::FILEPATH_TEXTURE || attr->type == XMLModule::AttributeType::FILEPATH_BUMPMAP) {
		CEGUI::Editbox* eb = static_cast<CEGUI::Editbox*>(m_gui->createWidget(l, "WindowsLook/Editbox", glm::vec4(0.5f, 0.0f, 0.5f, 1.0f), glm::vec4(0.0f), attr->name + m_name + "AttributeEntryEBSTRING"));
		eb->setText(attr->getData<std::string>());
		eb->subscribeEvent(CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber([ = ](const CEGUI::EventArgs & args)->bool{
			attr->setData((std::string)eb->getText().c_str());
			if(attr->name == "name") {
				CEGUI::DefaultWindow* thisLabel = static_cast<CEGUI::DefaultWindow*>(m_itemListBox0->getFirstSelectedItem()->getChildAtIdx(0));
				std::string tex = thisLabel->getText().c_str();
				tex = tex.substr(tex.rfind(" (ID:"));
				thisLabel->setText(eb->getText().c_str() + tex);
			}
		}));
	} else if(attr->type == XMLModule::AttributeType::STRING_FACTION) {
		CEGUI::Editbox* eb = static_cast<CEGUI::Editbox*>(m_gui->createWidget(l, "WindowsLook/Editbox", glm::vec4(0.5f, 0.0f, 0.5f, 1.0f), glm::vec4(0.0f), attr->name + m_name + "AttributeEntryEBSTRINGFACTION"));
		unsigned int fac = attr->getData<unsigned int>();
		std::string factionString = "";

		if(fac == (unsigned int)Categories::Faction::EVIL) {
			factionString = "evil";
		} else if(fac == (unsigned int)Categories::Faction::BAD) {
			factionString = "bad";
		} else if(fac == (unsigned int)Categories::Faction::NEUTRAL) {
			factionString = "neutral";
		} else if(fac == (unsigned int)Categories::Faction::GOOD) {
			factionString = "good";
		} else if(fac == (unsigned int)Categories::Faction::BENIGN) {
			factionString = "benign";
		} else {
			factionString = "neutral";
		}

		eb->setText(factionString);
		eb->subscribeEvent(CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber([ = ](const CEGUI::EventArgs & args)->bool{
			std::string factionString = eb->getText().c_str();
			unsigned int faction = 0;

			if(factionString == "evil") {
				faction = (unsigned int)Categories::Faction::EVIL;
			} else if(factionString == "bad") {
				faction = (unsigned int)Categories::Faction::BAD;
			} else if(factionString == "neutral") {
				faction = (unsigned int)Categories::Faction::NEUTRAL;
			} else if(factionString == "good") {
				faction = (unsigned int)Categories::Faction::GOOD;
			} else if(factionString == "benign") {
				faction = (unsigned int)Categories::Faction::BENIGN;
			} else {
				faction = (unsigned int)Categories::Faction::NEUTRAL;
			}

			attr->setData(faction);
		}));
	} else if(attr->type == XMLModule::AttributeType::SCRIPT) {
		// isFile(bool) and actual script/path(string)






		/*
		 * I DONT KNOW WHY THE REGULAR WAY DOESNT WORK BUT THIS DOES.
		 * FIX THIS ISSUE SOMEDAY
		 */
		XMLModule::Attribute<ScriptData>* at = static_cast<XMLModule::Attribute<ScriptData>*>(attr);
		ScriptData script = at->getData();






		CEGUI::MultiLineEditbox* eb = static_cast<CEGUI::MultiLineEditbox*>(m_gui->createWidget(l, "WindowsLook/MultiLineEditbox", glm::vec4(0.5f, 0.0f, 0.4f, 2.0f), glm::vec4(0.0f), attr->name + m_name + "AttributeEntryEBSCRIPT"));
		eb->setText(script.stringData);
		eb->subscribeEvent(CEGUI::MultiLineEditbox::EventTextChanged, CEGUI::Event::Subscriber([ = ](const CEGUI::EventArgs & args)->bool{
			ScriptData scr = at->getData();
			scr.stringData = eb->getText().c_str();
			at->setData(scr);
		}));

		CEGUI::ToggleButton* cb = static_cast<CEGUI::ToggleButton*>(m_gui->createWidget(l, "WindowsLook/Checkbox", glm::vec4(0.94f, 0.0f, 0.1f, 1.0f), glm::vec4(0.0f), attr->name + m_name + "AttributeEntryCBSCRIPT"));
		cb->setSelected(script.isFile);
		cb->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber([ = ](const CEGUI::EventArgs & args)->bool{
			ScriptData scr = at->getData();
			scr.isFile = cb->isSelected();
			at->setData(scr);
		}));
	} else if(attr->type == XMLModule::AttributeType::BOOL) {
		CEGUI::ToggleButton* eb = static_cast<CEGUI::ToggleButton*>(m_gui->createWidget(l, "WindowsLook/Checkbox", glm::vec4(0.735f, 0.0f, 1.2f, 1.2f), glm::vec4(0.0f), attr->name + m_name + "AttributeEntryEBBOOL"));
		bool b = attr->getData<bool>();
		eb->setSelected(b);
		eb->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber([ = ](const CEGUI::EventArgs & args)->bool{
			bool val = eb->isSelected();
			attr->setData(val);
		}));
	} else if(attr->type == XMLModule::AttributeType::FLOAT) {
		CEGUI::Editbox* eb = static_cast<CEGUI::Editbox*>(m_gui->createWidget(l, "WindowsLook/Editbox", glm::vec4(0.5f, 0.0f, 0.5f, 1.0f), glm::vec4(0.0f), attr->name + m_name + "AttributeEntryEBFLOAT"));
		eb->setText(std::to_string(attr->getData<float>()));
		eb->setValidationString("((\\d*)(.?)(\\d*))$");
		eb->subscribeEvent(CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber([ = ](const CEGUI::EventArgs & args)->bool{
			if(eb->getText().length() > 0)
				attr->setData(std::stof(eb->getText().c_str()));
			return false;
		}));
	} else if(attr->type == XMLModule::AttributeType::INT) {
		CEGUI::Editbox* eb = static_cast<CEGUI::Editbox*>(m_gui->createWidget(l, "WindowsLook/Editbox", glm::vec4(0.5f, 0.0f, 0.5f, 1.0f), glm::vec4(0.0f), attr->name + m_name + "AttributeEntryEBINT"));
		eb->setText(std::to_string(attr->getData<int>()));
		eb->setValidationString("[\-]?[\\d]*");
		eb->subscribeEvent(CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber([ = ](const CEGUI::EventArgs & args)->bool{
			if(eb->getText().length() > 0 && eb->getText() != "-") attr->setData(std::stoi(eb->getText().c_str()));
		}));
	} else if(attr->type == XMLModule::AttributeType::UNSIGNED_INT) {
		CEGUI::Editbox* eb = static_cast<CEGUI::Editbox*>(m_gui->createWidget(l, "WindowsLook/Editbox", glm::vec4(0.5f, 0.0f, 0.5f, 1.0f), glm::vec4(0.0f), attr->name + m_name + "AttributeEntryEBINT"));
		eb->setText(std::to_string(attr->getData<unsigned int>()));
		eb->setValidationString("[\-]?[\\d]*");
		eb->subscribeEvent(CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber([ = ](const CEGUI::EventArgs & args)->bool{
			if(eb->getText().length() > 0 && eb->getText() != "-") attr->setData((unsigned int)std::stoi(eb->getText().c_str()));
		}));
	} else if(attr->type == XMLModule::AttributeType::VECTOR_UNSIGNED_INT) {
		CEGUI::Editbox* eb = static_cast<CEGUI::Editbox*>(m_gui->createWidget(l, "WindowsLook/Editbox", glm::vec4(0.5f, 0.0f, 0.5f, 1.0f), glm::vec4(0.0f), attr->name + m_name + "AttributeEntryEBVEC_INT"));

		std::vector<unsigned int> data = attr->getData<std::vector<unsigned int>>(); // last type of item
		std::string dataString = "";

		if(data.size() > 0) {
			dataString += std::to_string(data[0]);
			for(unsigned int i = 1; i < data.size(); i++) {
				dataString += "," + std::to_string(data[i]);
			}
		}

		eb->setText(dataString);
		eb->setValidationString("(\\d+)(,?\\d*)*$"); // One or 0 items, it must have a number, then 0 or more commas followed by a number.
		eb->subscribeEvent(CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber([ = ](const CEGUI::EventArgs & args)->bool{
			std::string dataStr = eb->getText().c_str();
			std::vector<unsigned int> data;

			bool found = true;
			while(found) {
				if(dataStr.length() <= 0) {
					break;
				}
				int pos = dataStr.find(",");

				if(pos == std::string::npos) {
					// We've reached the end
					data.push_back(std::stoi(dataStr));
					found = false;
				} else {
					data.push_back(std::stoi(dataStr.substr(0, pos)));
					dataStr.erase(0, pos + 1);
				}
			}

			attr->setData(data);
		}));
	} else if(attr->type == XMLModule::AttributeType::VECTOR_FLOAT) {
		CEGUI::Editbox* eb = static_cast<CEGUI::Editbox*>(m_gui->createWidget(l, "WindowsLook/Editbox", glm::vec4(0.5f, 0.0f, 0.5f, 1.0f), glm::vec4(0.0f), attr->name + m_name + "AttributeEntryEBVEC_FLOAT"));

		std::vector<float> data = attr->getData<std::vector<float>>(); // last type of item
		std::string dataString = "";

		if(data.size() > 0) {
			dataString += std::to_string(data[0]);
			for(unsigned int i = 1; i < data.size(); i++) {
				dataString += "," + std::to_string(data[i]);
			}
		}

		eb->setText(dataString);
		eb->setValidationString("(([\\d]*([.][\\d]+)?$\\D{0})(,{1}[\\d]+([.][\\d]+)?)*)?$"); // One or 0 items, it must have a number, then 0 or more commas followed by a number.
		eb->subscribeEvent(CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber([ = ](const CEGUI::EventArgs & args)->bool{
			std::string dataStr = eb->getText().c_str();
			std::vector<float> data;

			bool found = true;
			while(found) {
				if(dataStr.length() <= 0) {
					break;
				}
				int pos = dataStr.find(",");

				if(pos == std::string::npos) {
					// We've reached the end
					data.push_back(std::stof(dataStr));
					found = false;
				} else {
					data.push_back(std::stoi(dataStr.substr(0, pos)));
					dataStr.erase(0, pos + 1);
				}
			}

			attr->setData(data);
		}));
	} else if(attr->type == XMLModule::AttributeType::VECTOR_VEC2) {
		/*CEGUI::Editbox* eb = static_cast<CEGUI::Editbox*>(m_gui->createWidget(l, "WindowsLook/Editbox", glm::vec4(0.5f, 0.0f, 0.5f, 1.0f), glm::vec4(0.0f), attr->name + m_name + "AttributeEntryEBVEC_VEC2"));

		std::vector<glm::vec2> data = attr->getData<std::vector<glm::vec2>>(); // last type of item
		std::string dataString = "";

		if(data.size() > 0) {
			dataString += std::to_string(data[0]);
			for(unsigned int i = 1; i < data.size(); i++) {
				dataString += "," + std::to_string(data[i]);
			}
		}

		eb->setText(dataString);
		eb->setValidationString("((\\D{0}^[\\d]*([.][\\d]*)?$\\D{0})(,{1}\\D{0}^[\\d]*([.][\\d]*)?$\\D{0})*)?$"); // One or 0 items, it must have a number, then 0 or more commas followed by a number.
		eb->subscribeEvent(CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs& args)->bool{
			std::string dataStr = eb->getText().c_str();
			std::vector<float> data;

			bool found = true;
			while(found) {
				if(dataStr.length() <= 0) {
					break;
				}
				int pos = dataStr.find(",");

				if(pos == std::string::npos) {
					// We've reached the end
					data.push_back(std::stof(dataStr));
					found = false;
				} else {
					data.push_back(std::stoi(dataStr.substr(0, pos)));
					dataStr.erase(0, pos+1);
				}
			}

			attr->setData(data);
		}));*/
	} else if(attr->type == XMLModule::AttributeType::VEC2) {
		CEGUI::Editbox* eb_x = static_cast<CEGUI::Editbox*>(m_gui->createWidget(l, "WindowsLook/Editbox", glm::vec4(0.5f, 0.0f, 0.25f, 1.0f), glm::vec4(0.0f), attr->name + m_name + "AttributeEntryEBVEC2_X"));
		CEGUI::Editbox* eb_y = static_cast<CEGUI::Editbox*>(m_gui->createWidget(l, "WindowsLook/Editbox", glm::vec4(0.75f, 0.0f, 0.25f, 1.0f), glm::vec4(0.0f), attr->name + m_name + "AttributeEntryEBVEC2_Y"));

		glm::vec2 data = attr->getData<glm::vec2>();

		std::string dataString_x = std::to_string(data.x);
		std::string dataString_y = std::to_string(data.y);

		eb_x->setText(dataString_x);
		eb_y->setText(dataString_y);

		eb_x->setValidationString("((\\d*)(.?)(\\d*))$");
		eb_y->setValidationString("((\\d*)(.?)(\\d*))$");

		eb_x->subscribeEvent(CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber([ = ](const CEGUI::EventArgs & args)->bool{
			std::string dataStr = eb_x->getText().c_str();
			if(dataStr.length() == 0) return false;
			float x = std::stof(dataStr);

			glm::vec2 finalData = attr->getData<glm::vec2>();
			finalData.x = x;

			attr->setData(finalData);
			return false;
		}));

		eb_y->subscribeEvent(CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber([ = ](const CEGUI::EventArgs & args)->bool{
			std::string dataStr = eb_y->getText().c_str();
			if(dataStr.length() == 0) return false;
			float y = std::stof(dataStr);

			glm::vec2 finalData = attr->getData<glm::vec2>();
			finalData.y = y;

			attr->setData(finalData);
			return false;
		}));
	}

	m_itemListBox1->addItem(l);
}

bool EditWindow::onSelectionChanged(const CEGUI::EventArgs& evnt) {
	CEGUI::WindowEventArgs a = static_cast<const CEGUI::WindowEventArgs&>(evnt);
	CEGUI::ItemListbox* listbox = static_cast<CEGUI::ItemListbox*>(a.window);
	CEGUI::ItemEntry* i = listbox->getFirstSelectedItem();

	if(i) {
		m_selection = std::stoi(i->getUserString("ID").c_str());

		std::unordered_map<std::string, XMLModule::AttributeBase*> attrsMap = m_data[m_selection]->getAttributes();
		std::vector<XMLModule::AttributeBase*> attrs;

		for(auto& item : attrsMap) {
			attrs.push_back(item.second);
		}

		addAttributes(attrs);
	}
}

MainScreen::MainScreen(GLEngine::Window* window) {
	/// Initialize GUI components.
	m_window = window;

	m_gui.init("GUI", 1);
	m_gui.loadScheme("WindowsLook.scheme");
	m_gui.setFont("DejaVuSans-14-NoScale");
	m_gui.setFont("DejaVuSans-10-NoScale");

	m_gui.hideMouseCursor();

	m_loadFileEditbox = static_cast<CEGUI::Editbox*>(m_gui.createWidget("WindowsLook/Editbox", glm::vec4(0.05f, 0.9f, 0.2f, 0.075f), glm::vec4(0.0f), "loadFileEditbox"));
	m_loadFileButton  = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.275f, 0.9f, 0.1f, 0.075f), glm::vec4(0.0f), "loadFileButton"));
	m_loadFileButton->setText("Load Files");
	m_loadFileButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([ = ](const CEGUI::EventArgs & args)->bool{
		clearData();
		std::string path = GetCurrentDirectory() + "/" + (std::string)m_loadFileEditbox->getText().c_str();
		loadXMLFile(path);
	}));

	m_saveFileButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.4f, 0.9f, 0.1f, 0.075f), glm::vec4(0.0f), "saveFileButton"));
	m_saveFileButton->setText("Save Files");
	m_saveFileButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([ = ](const CEGUI::EventArgs&)->bool{
		std::string path = GetCurrentDirectory() + "/" + (std::string)m_loadFileEditbox->getText().c_str();
		saveXMLData(path);
	}));

	for(unsigned int i = 0; i < XMLModule::XMLData::m_fileNames.size(); i++) {
		m_editWindows.push_back(new EditWindow());
		m_editWindows[i]->init(XMLModule::XMLData::m_fileNames[i].m_filename, m_gui, m_windowButtons, XMLModule::XMLData::m_fileNames[i].m_nodeNames);
	}

	for(unsigned int i = 0; i < m_windowButtons.size(); i++) {
		m_windowButtons[i]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([ = ](const CEGUI::EventArgs & args)->bool{
			setAllInactive();
			m_editWindows[i]->setActive(true);
		}));
	}

	// Set the size and position because now we have all of them
	unsigned int rows = std::ceil((float)m_windowButtons.size() / (float)BUTTONS_HORIZONTAL);

	for(unsigned int y = 0; y < rows; y++) {
		for(unsigned int x = 0; x < BUTTONS_HORIZONTAL && y * BUTTONS_HORIZONTAL + x < m_windowButtons.size(); x++) {
			m_windowButtons[y * BUTTONS_HORIZONTAL + x]->setSize(CEGUI::USize(cegui_reldim(0.9f / BUTTONS_HORIZONTAL), cegui_reldim(0.05f)));
			m_windowButtons[y * BUTTONS_HORIZONTAL + x]->setPosition(CEGUI::UVector2(cegui_reldim(x * 0.9f / BUTTONS_HORIZONTAL + 0.05f), cegui_reldim(0.825f - 0.05f * y)));
		}
	}
}

MainScreen::~MainScreen() {
	//dtor
}

void MainScreen::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(.94f, .94f, 1.0f, 1.0f);

	m_gui.draw();
}

void MainScreen::update() {
	SDL_Event evnt;
	while(SDL_PollEvent(&evnt)) {
		m_game->onSDLEvent(evnt);
		m_gui.onSDLEvent(evnt);
		switch(evnt.type) {
			case SDL_QUIT:
				m_currentState = GLEngine::ScreenState::EXIT_APPLICATION;
				break;
		}
	}
}

void MainScreen::loadXMLFile(std::string filepath) {
	XMLModule::XMLData::init(filepath);

	// Get all data and enter it
	// Loop over all XMLDataFiles
	for(unsigned int i = 0; i < XMLModule::XMLData::m_fileNames.size(); i++) {
		// Loop over each map in each XMLDataFile
		for(unsigned int j = 0; j < XMLModule::XMLData::m_fileNames[i].m_maps.size(); j++) {
			// Loop over each piece of data in each map
			for(auto data : *XMLModule::XMLData::m_fileNames[i].m_maps[j]) {
				std::string nodeName = data.second->getNodeName();
				m_editWindows[i]->addEntry(data.second, nodeName);
			}
		}
	}
}

void MainScreen::saveXMLData(std::string filepath) {
	XMLModule::XMLData::write(filepath);
}

void MainScreen::setAllInactive() {
	for(unsigned int i = 0; i < m_editWindows.size(); i++) {
		m_editWindows[i]->setActive(false);
	}
}

void MainScreen::clearData() {
	for(unsigned int i = 0; i < m_editWindows.size(); i++) {
		m_editWindows[i]->removeAllEntries();
	}
}
