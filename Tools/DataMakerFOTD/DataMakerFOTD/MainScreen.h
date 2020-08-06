#pragma once

#include <IGameScreen.h>
#include <InputManager.h>

#include <GUI.h>
#include <GLSLProgram.h>

#include <rapidxml/rapidxml.hpp>

#include "XMLData.h"

class EditWindow {
	public:
		EditWindow() {}

		void finalizeData(); // writes all m_data into XMLModule::XMLData for saving.

		// constructs numerous add buttons and a remove button, as well as the button to actually open this editWindow.
		void init(std::string name, GLEngine::GUI& gui, std::vector<CEGUI::PushButton*>& buttons, std::vector<std::string> types); // types is a vector of node names.
		void setActive(bool active);
		bool isActive() {
			return m_active;
		}

		void createNewEntry(unsigned int type); // `type` is the index of m_types to use to create a new Genericdata object and addEntry
		void addEntry(XMLModule::GenericData* obj, std::string& nodeName); // Adds some data to the left editbox, giving it ID, name, etc.
		void removeEntry(); // removes currently selected.
		void resetIDs(); // Resets all IDs so that they are in order and all-inclusive (all numbers from 0-max)
		void sortItems(); // Sorts all items, so that all of the items from the same nodeName type are together and in order from least-greatest ID

		bool onSelectionChanged(const CEGUI::EventArgs& evnt);
		int m_selection = -1;

		void addAttributes(std::vector<XMLModule::AttributeBase*> attributes); // calls addAttribute over and over.
		void addAttribute(XMLModule::AttributeBase* attr); // Adds formatted attribute edit dialogues to the right editbox

		std::vector<std::string> m_types;
		std::vector<XMLModule::GenericData*> m_data;

		GLEngine::GUI* m_gui = nullptr;

		CEGUI::FrameWindow* m_window = nullptr;
		CEGUI::ItemListbox* m_itemListBox0 = nullptr;
		CEGUI::ItemListbox* m_itemListBox1 = nullptr;
		std::vector<CEGUI::PushButton*> m_buttons;

		CEGUI::ItemEntry* m_head0 = nullptr;
		CEGUI::ItemEntry* m_head1 = nullptr;

		std::string m_name;
		bool m_active = false;
};

class MainScreen : public GLEngine::IGameScreen {
	public:
		MainScreen(GLEngine::Window* window);
		~MainScreen();

		virtual int getNextScreenIndex() const override {
			return 0;
		}
		virtual int getPreviousScreenIndex() const override {
			return 0;
		}
		virtual void build() override {}
		virtual void destroy() override {}
		virtual void onEntry() override {}
		virtual void onExit() override {}
		virtual void update() override;
		virtual void draw() override;

	private:
		void setAllInactive();

		GLEngine::Window* m_window = nullptr;

		void loadXMLFile(std::string filepath);
		void saveXMLData(std::string filepath);

		GLEngine::GLSLProgram m_uiTextureProgram;
		GLEngine::GUI m_gui;

		CEGUI::Editbox* m_loadFileEditbox = nullptr;
		CEGUI::PushButton* m_loadFileButton = nullptr;
		CEGUI::PushButton* m_saveFileButton = nullptr;

		int getNextID();
		EditWindow* getActiveWindow();

		std::vector<EditWindow*> m_editWindows;
		std::vector<CEGUI::PushButton*> m_windowButtons;
};
