#pragma once

#include <IGameScreen.h>
#include <InputManager.h>

#include <GUI.h>
#include <GLSLProgram.h>

#include <rapidxml/rapidxml.hpp>

enum class Types {
	// these represent the different maps, not actual specific types
	ANIMATION,
	SKELETAL_ANIMATION,
	MELEE_ATTACK,
	RANGED_ATTACK,
	MAGIC_ATTACK,
	BIOME,
	BLOCK,
	QUESTION,
	RESPONSE,
	ERA,
	ITEM_ENTITY,
	NPC_ENTITY,
	PROJECTILE_ENTITY,
	ITEM,
	LOOT_DROP,
	LOOT_TABLE,
	QUEST,
	QUEST_OBJECTIVE,
	STRUCTURE,
	PARTICLE
};

std::string getNodenameFromType(Type type);

class EditWindow {
    public:
        EditWindow() {}

        void init(std::string name, GLEngine::GUI& gui);
        void setActive(bool active);
        bool isActive() { return m_active; }

        bool onSelectionChanged(const CEGUI::EventArgs& evnt);
        int m_selection = -1;
		
		void addAttribute(XMLModule::Attribute* attr); // Adds an attribute with proper formatting and stuff to the editbox.

		Type m_type;
        std::vector<XMLModule::GenericData*> m_data;

        GLEngine::GUI* m_gui = nullptr;

        CEGUI::FrameWindow* m_window = nullptr;
        CEGUI::ItemListbox* m_itemListBox0 = nullptr;
        CEGUI::ItemListbox* m_itemListBox1 = nullptr;

        CEGUI::ItemEntry* m_head0 = nullptr;
        CEGUI::ItemEntry* m_head1 = nullptr;

        std::string m_name;
        bool m_active = false;
};

class MainScreen : public GLEngine::IGameScreen
{
    public:
        MainScreen(GLEngine::Window* window);
        ~MainScreen();

        virtual int getNextScreenIndex() const override { return 0; }
        virtual int getPreviousScreenIndex() const override { return 0; }
        virtual void build() override {}
        virtual void destroy() override {}
        virtual void onEntry() override {}
        virtual void onExit() override {}
        virtual void update() override;
        virtual void draw() override;

    private:
        void setItemEntitiesActive();
        void setProjectilesActive();
        void setParticlesActive();
        void setBlocksActive();
        void setItemsActive();
        void setNPCsActive();
        void setBiomesActive();
        void setErasActive();
        void setLootDropsActive();
        void setLootTablesActive();
        void setStructuresActive();
		void setAllInactive();

        GLEngine::Window* m_window = nullptr;

        void loadXMLFile(std::string filepath);
        void saveXMLData(std::string filepath);

        GLEngine::GLSLProgram m_uiTextureProgram;
        GLEngine::GUI m_gui;

        CEGUI::Editbox* m_loadFileEditbox = nullptr;
        CEGUI::PushButton* m_loadFileButton = nullptr;
        CEGUI::PushButton* m_saveFileButton = nullptr;

        void addNewObject();
        void removeObject();

        int getNextID();
        EditWindow* getActiveWindow();

        CEGUI::PushButton* m_addButton = nullptr;
        CEGUI::PushButton* m_removeButton = nullptr;
		
		const unsigned int m_numTypes = 20;
        EditWindow* m_editWindows[m_numTypes] = {nullptr}; // Use the Types enum to access each element
		CEGUI::PushButton* m_windowButtons[m_numTypes] = {nullptr}; // Use the Types enum to access each element
		std::string m_nodenames[m_numTypes];
};