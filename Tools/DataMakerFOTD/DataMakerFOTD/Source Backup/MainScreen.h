#pragma once

#include <IGameScreen.h>
#include <InputManager.h>

#include <GUI.h>
#include <GLSLProgram.h>

#include <rapidxml/rapidxml.hpp>

#include "Objects.h"

class EditWindow {
    public:
        EditWindow() {}

        void init(std::string name, GLEngine::GUI& gui);
        void setActive(bool active);
        bool isActive() { return m_active; }

        void addObject(Object* obj);
        void addAttributes(std::vector<Attribute*> attributes);
        void addAttribute(Attribute* a);

        bool onSelectionChanged(const CEGUI::EventArgs& evnt);
        int m_selection = -1;

        std::vector<Object*> m_objects;

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

        GLEngine::Window* m_window = nullptr;

        void loadXMLFile(std::string filepath);
        void saveXMLData(std::string filepath);

        void saveXMLFile(std::vector<Object*> objects, std::string itemTag, std::string filepath, bool append = false);
        void saveXMLObject(Object* obj, rapidxml::xml_document<>& doc, std::string itemTag);
        void saveXMLAttribute(Attribute* a, rapidxml::xml_document<>& doc, rapidxml::xml_node<>* parent);

        GLEngine::GLSLProgram m_uiTextureProgram;
        GLEngine::GUI m_gui;

        CEGUI::Editbox* m_loadFileEditbox = nullptr;
        CEGUI::PushButton* m_loadFileButton = nullptr;
        CEGUI::PushButton* m_saveFileButton = nullptr;

        CEGUI::PushButton* m_itemEntitiesWindowButton = nullptr;
        CEGUI::PushButton* m_projectilesWindowButton = nullptr;
        CEGUI::PushButton* m_particlesWindowButton = nullptr;
        CEGUI::PushButton* m_blocksWindowButton = nullptr;
        CEGUI::PushButton* m_itemsWindowButton = nullptr;
        CEGUI::PushButton* m_NPCsWindowButton = nullptr;
        CEGUI::PushButton* m_biomesWindowButton = nullptr;
        CEGUI::PushButton* m_erasWindowButton = nullptr;
        CEGUI::PushButton* m_lootDropsWindowButton = nullptr;
        CEGUI::PushButton* m_lootTablesWindowButton = nullptr;
        CEGUI::PushButton* m_structuresWindowButton = nullptr;

        void addNewObject();
        void removeObject();

        int getNextID();
        EditWindow* getActiveWindow();

        CEGUI::PushButton* m_addButton = nullptr;
        CEGUI::PushButton* m_removeButton = nullptr;

        std::vector<Object*> m_itemEntitiesObjects, m_projectilesObjects, m_particlesObjects, m_blocksObjects, m_itemsObjects, m_NPCsObjects, m_biomesObjects, m_erasObjects, m_lootDropsObjects, m_lootTablesObjects, m_structuresObjects;

        EditWindow m_itemEntities, m_projectiles, m_particles, m_blocks, m_items, m_NPCs, m_biomes, m_eras, m_lootDrops, m_lootTables, m_structures;
};
