#include "MainScreen.h"

#include <XMLData.h>

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

std::string GetCurrentDirectory( void ) {
  char buff[FILENAME_MAX];
  GCD( buff, FILENAME_MAX );
  std::string current_working_dir(buff);
  return current_working_dir;
}

std::string getNodenameFromType(Types type) {
	switch (unsigned int)type {
		case (unsigned int)Types::ANIMATION: {
			return "animation";
			break;
		}
		case (unsigned int)Types::SKELETAL_ANIMATION: {
			return "skeletalAnimation";
			break;
		}
		case (unsigned int)Types::MELEE_ATTACK: {
			return "meleeAttack";
			break;
		}
		case (unsigned int)Types::RANGED_ATTACK: {
			return "rangedAttack";
			break;
		}
		case (unsigned int)Types::MAGIC_ATTACK: {
			return "magicAttack";
			break;
		}
		case (unsigned int)Types::BIOME: {
			return "biome";
			break;
		}
		case (unsigned int)Types::BLOCK: {
			return "tile";
			break;
		}
		case (unsigned int)Types::QUESTION: {
			return "question";
			break;
		}
		case (unsigned int)Types::RESPONSE: {
			return "response";
			break;
		}
		case (unsigned int)Types::ERA: {
			return "era";
			break;
		}
		case (unsigned int)Types::ITEM_ENTITY: {
			return "itemEntity";
			break;
		}
		case (unsigned int)Types::NPC_ENTITY: {
			return "npc";
			break;
		}
		case (unsigned int)Types::PROJECTILE_ENTITY: {
			return "projectile";
			break;
		}
		case (unsigned int)Types::ITEM: {
			return "item";
			break;
		}
		case (unsigned int)Types::LOOT_DROP: {
			return "lootDrop";
			break;
		}
		case (unsigned int)Types::LOOT_TABLE: {
			return "lootTable";
			break;
		}
		case (unsigned int)Types::QUEST: {
			return "quest";
			break;
		}
		case (unsigned int)Types::QUEST_OBJECTIVE: {
			return "objective";
			break;
		}
		case (unsigned int)Types::STRUCTURE: {
			return "structure";
			break;
		}
		case (unsigned int)Types::PARTICLE: {
			return "particle";
			break;
		}
	}
}

void EditWindow::init(std::string name, GLEngine::GUI& gui) {
    m_gui = &gui;

    m_window = static_cast<CEGUI::FrameWindow*>(m_gui->createWidget("WindowsLook/FrameWindow", glm::vec4(0.05f, 0.05f, 0.9f, 0.7f), glm::vec4(0.0f), name + "Window"));
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
    m_itemListBox0->setSortEnabled(true);
    m_itemListBox0->subscribeEvent(CEGUI::ItemListbox::EventSelectionChanged, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs& evnt)->bool{return onSelectionChanged(evnt);}));

    m_itemListBox1 = static_cast<CEGUI::ItemListbox*>(m_gui->createWidget(m_window, "WindowsLook/ItemListbox", glm::vec4(0.5f, 0.1f, 0.49f, 0.88f), glm::vec4(0.0f), name + "LB1"));
    m_itemListBox1->setMultiSelectEnabled(false);
    m_itemListBox1->setSortMode(CEGUI::ItemListBase::SortMode::Ascending);
    m_itemListBox1->setSortEnabled(true);
    //m_itemListBox0->subscribeEvent(CEGUI::ItemListbox::EventSelectionChanged, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs& evnt)->bool{return onSelectionChanged(evnt);}));

    m_name = name;
}

void EditWindow::setActive(bool active) {
    m_window->setEnabled(active);
    m_window->setVisible(active);
    m_active = active;
}

void EditWindow::addObject(Object* obj) {
    CEGUI::ItemEntry* l = static_cast<CEGUI::ItemEntry*>(m_gui->createWidget(m_itemListBox0, "WindowsLook/ListboxItem", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f), obj->m_name + m_name + std::to_string(obj->m_id) + "Entry"));
    l->setSelectable(true);
    std::string a = std::string(m_itemListBox0->getItemCount(), 1);
    l->setText(a);

    CEGUI::DefaultWindow* label = static_cast<CEGUI::DefaultWindow*>(m_gui->createWidget(l, "WindowsLook/Label", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f), obj->m_name + m_name + std::to_string(obj->m_id) + "EntryLabel"));
    label->setText(obj->m_name + " (ID: " + std::to_string(obj->m_id) + ")");
    label->setMousePassThroughEnabled(true);
    label->setInheritsAlpha(false);

    m_itemListBox0->addItem(l);
    m_itemListBox0->sortList();

    m_objects.push_back(obj);
}

void EditWindow::addAttributes(std::vector<Attribute*> attributes) {
    m_itemListBox1->resetList(); // remove all children and delete them

    for(int i = 0; i < attributes.size(); i++) {
        addAttribute(attributes[i]);
    }

    m_itemListBox1->sortList();
}

void EditWindow::addAttribute(XMLModule::Attribute* attr) {
    CEGUI::ItemEntry* l = static_cast<CEGUI::ItemEntry*>(m_gui->createWidget(m_itemListBox1, "WindowsLook/ListboxItem", glm::vec4(0.0f, 0.0f, 1.0f, 0.1f), glm::vec4(0.0f), attr->name + m_name + "AttributeEntry"));
    l->setSelectable(false);
    std::string sortT = std::string(m_itemListBox1->getItemCount(), 1);
    l->setText(sortT);

    CEGUI::DefaultWindow* label = static_cast<CEGUI::DefaultWindow*>(m_gui->createWidget(l, "WindowsLook/Label", glm::vec4(0.0f, 0.0f, 0.5f, 1.0f), glm::vec4(0.0f), attr->name + m_name + "AttributeEntryLabel"));
    label->setText(attr->name);

    if(attr->type == XMLModule::AttributeType::STRING) {
        CEGUI::Editbox* eb = static_cast<CEGUI::Editbox*>(m_gui->createWidget(l, "WindowsLook/Editbox", glm::vec4(0.5f, 0.0f, 0.5f, 1.0f), glm::vec4(0.0f), attr->name + m_name + "AttributeEntryEBSTRING"));
        eb->setText(static_cast<std::string>(attr->getData());
        eb->subscribeEvent(CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs& args)->bool{
            attr->setData((std::string)eb->getText().c_str());
        }));
    } else if(attr->type == XMLModule::AttributeType::BOOL) {
        CEGUI::ToggleButton* eb = static_cast<CEGUI::ToggleButton*>(m_gui->createWidget(l, "WindowsLook/Checkbox", glm::vec4(0.735f, 0.0f, 1.2f, 1.2f), glm::vec4(0.0f), attr->name + m_name + "AttributeEntryEBBOOL"));
        bool b = static_cast<bool>(attr->getData()) == "1";
        eb->setSelected(b);
        eb->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs& args)->bool{
            std::string val = std::to_string(eb->isSelected());
            attr->setData(val);
        }));
    } else if(attr->type == XMLModule::AttributeType::FLOAT) {
        CEGUI::Editbox* eb = static_cast<CEGUI::Editbox*>(m_gui->createWidget(l, "WindowsLook/Editbox", glm::vec4(0.5f, 0.0f, 0.5f, 1.0f), glm::vec4(0.0f), attr->name + m_name + "AttributeEntryEBFLOAT"));
        eb->setText(static_cast<float>(attr->getData());
        eb->setValidationString("\\D{0}^[\\d]*([.][\\d]*)?$\\D{0}");
        eb->subscribeEvent(CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs& args)->bool{
            attr->setData((std::string)eb->getText().c_str());
        }));
    } else if(attr->type == XMLModule::AttributeType::INT) {
        CEGUI::Editbox* eb = static_cast<CEGUI::Editbox*>(m_gui->createWidget(l, "WindowsLook/Editbox", glm::vec4(0.5f, 0.0f, 0.5f, 1.0f), glm::vec4(0.0f), attr->name + m_name + "AttributeEntryEBINT"));
        eb->setText(static_cast<std::string>(attr->getData()));
        eb->setValidationString("[-+]?\\D{0}^[\\d]*$\\D{0}");
        eb->subscribeEvent(CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs& args)->bool{
            attr->setData((std::string)eb->getText().c_str());
        }));
    } else if(attr->type == XMLModule::AttributeType::VECTOR_INT) {
        CEGUI::Editbox* eb = static_cast<CEGUI::Editbox*>(m_gui->createWidget(l, "WindowsLook/Editbox", glm::vec4(0.5f, 0.0f, 0.5f, 1.0f), glm::vec4(0.0f), attr->name + m_name + "AttributeEntryEBVEC_INT"));
        eb->setText(attr->value);
        eb->setValidationString("(((\\d)*)(,{1}(\\d)*)*)?$"); // One or 0 items, it must have a number, then 0 or more commas followed by a number.
        eb->subscribeEvent(CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs& args)->bool{
            attr->setData((std::string)eb->getText().c_str());
        }));
    }

    m_itemListBox1->addItem(l);
}

bool EditWindow::onSelectionChanged(const CEGUI::EventArgs& evnt) {
    CEGUI::WindowEventArgs a = static_cast<const CEGUI::WindowEventArgs&>(evnt);
    CEGUI::ItemListbox* listbox = static_cast<CEGUI::ItemListbox*>(a.window);
    CEGUI::ItemEntry* i = listbox->getFirstSelectedItem();

    if(i) {
        m_selection = listbox->getItemIndex(i);
        addAttributes(m_objects[m_selection]->getAttributes());
    }
}

MainScreen::MainScreen(GLEngine::Window* window)
{
    /// Initialize GUI components.
    m_window = window;

    m_gui.init("GUI");
    m_gui.loadScheme("WindowsLook.scheme");
    m_gui.setFont("DejaVuSans-14-NoScale");
    m_gui.setFont("DejaVuSans-10-NoScale");

    m_gui.hideMouseCursor();

	m_editWindows[(unsigned int)Types::ITEM_ENTITY].init("ItemEntities", m_gui);
	m_editWindows[(unsigned int)Types::PROJECTILE_ENTITY].init("Projectiles", m_gui);
	m_editWindows[(unsigned int)Types::PARTICLE].init("Particles", m_gui);
	m_editWindows[(unsigned int)Types::BLOCK].init("Blocks", m_gui);
	m_editWindows[(unsigned int)Types::ITEM].init("Items", m_gui);
	m_editWindows[(unsigned int)Types::NPC_ENTITY].init("NPCs", m_gui);
	m_editWindows[(unsigned int)Types::BIOME].init("Biomes", m_gui);
	m_editWindows[(unsigned int)Types::ERA].init("Eras", m_gui);
	m_editWindows[(unsigned int)Types::LOOT_DROP].init("Loot Drops", m_gui);
	m_editWindows[(unsigned int)Types::LOOT_TABLE].init("Loot Tables", m_gui);
	m_editWindows[(unsigned int)Types::STRUCTURE].init("Structures", m_gui);

    m_loadFileEditbox = static_cast<CEGUI::Editbox*>(m_gui.createWidget("WindowsLook/Editbox", glm::vec4(0.05f, 0.85f, 0.2f, 0.075f), glm::vec4(0.0f), "loadFileEditbox"));
    m_loadFileButton  = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.275f, 0.85f, 0.1f, 0.075f), glm::vec4(0.0f), "loadFileButton"));
    m_loadFileButton->setText("Load Files");
    m_loadFileButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs& args)->bool{
        std::string path = GetCurrentDirectory() + "/" + (std::string)m_loadFileEditbox->getText().c_str();
        loadXMLFile(path);
    }));

    m_saveFileButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.4f, 0.85f, 0.1f, 0.075f), glm::vec4(0.0f), "saveFileButton"));
    m_saveFileButton->setText("Save Files");
    m_saveFileButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs&)->bool{
        std::string path = GetCurrentDirectory() + "/" + (std::string)m_loadFileEditbox->getText().c_str();
        saveXMLData(path);
    }));

    m_addButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.01f, 0.94f, 0.05f, 0.05f), glm::vec4(0.0f), "AddButton"));
    m_addButton->setText("+");
    m_addButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs& args)->bool{
        addNewObject();
    }));

    m_removeButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.06f, 0.94f, 0.05f, 0.05f), glm::vec4(0.0f), "RemoveButton"));
    m_removeButton->setText("-");
    m_removeButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs& args)->bool{
        removeObject();
    }));

    m_windowButtons[(unsigned int)Types::ITEM_ENTITY] = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.05f, 0.76f, 0.125f, 0.0375f), glm::vec4(0.0f), "ItemEntitiesWindowButton"));
    m_windowButtons[(unsigned int)Types::ITEM_ENTITY]->setText("ItemEntities");
    m_windowButtons[(unsigned int)Types::ITEM_ENTITY]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs &Arg)->bool{setItemEntitiesActive();}));


    m_windowButtons[(unsigned int)Types::PROJECTILE_ENTITY] = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.05f + 0.13f, 0.76f, 0.125f, 0.0375f), glm::vec4(0.0f), "ProjectilesWindowButton"));
    m_windowButtons[(unsigned int)Types::PROJECTILE_ENTITY]->setText("Projectiles");
    m_windowButtons[(unsigned int)Types::PROJECTILE_ENTITY]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs &Arg)->bool{setProjectilesActive();}));


    m_windowButtons[(unsigned int)Types::PARTICLE] = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.05f + 0.13f * 2.0f, 0.76f, 0.125f, 0.0375f), glm::vec4(0.0f), "m_particlesWindowButton"));
    m_windowButtons[(unsigned int)Types::PARTICLE]->setText("Particles");
    m_windowButtons[(unsigned int)Types::PARTICLE]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs &Arg)->bool{setParticlesActive();}));


    m_windowButtons[(unsigned int)Types::BLOCK] = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.05f + 0.13f * 3.0f, 0.76f, 0.125f, 0.0375f), glm::vec4(0.0f), "m_blocksWindowButton"));
    m_windowButtons[(unsigned int)Types::BLOCK]->setText("Blocks");
    m_windowButtons[(unsigned int)Types::BLOCK]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs &Arg)->bool{setBlocksActive();}));


    m_windowButtons[(unsigned int)Types::ITEM] = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.05f + 0.13f * 4.0f, 0.76f, 0.125f, 0.0375f), glm::vec4(0.0f), "m_itemsWindowButton"));
    m_windowButtons[(unsigned int)Types::ITEM]->setText("Items");
    m_windowButtons[(unsigned int)Types::ITEM]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs &Arg)->bool{setItemsActive();}));


    m_windowButtons[(unsigned int)Types::NPC_ENTITY] = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.05f + 0.13f * 5.0f, 0.76f, 0.125f, 0.0375f), glm::vec4(0.0f), "m_NPCsWindowButton"));
    m_windowButtons[(unsigned int)Types::NPC_ENTITY]->setText("NPCs");
    m_windowButtons[(unsigned int)Types::NPC_ENTITY]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs &Arg)->bool{setNPCsActive();}));


    m_windowButtons[(unsigned int)Types::BIOME] = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.05f, 0.76f + 0.0375f, 0.125f, 0.0375f), glm::vec4(0.0f), "m_BiomesWindowButton"));
    m_windowButtons[(unsigned int)Types::BIOME]->setText("Biomes");
    m_windowButtons[(unsigned int)Types::BIOME]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs &Arg)->bool{setBiomesActive();}));


    m_windowButtons[(unsigned int)Types::ERA] = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.05f + 0.13f, 0.76f + 0.0375f, 0.125f, 0.0375f), glm::vec4(0.0f), "m_erasWindowButton"));
    m_windowButtons[(unsigned int)Types::ERA]->setText("Eras");
    m_windowButtons[(unsigned int)Types::ERA]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs &Arg)->bool{setErasActive();}));


    m_windowButtons[(unsigned int)Types::LOOT_DROP] = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.05f + 0.13f * 2.0f, 0.76f + 0.0375f, 0.125f, 0.0375f), glm::vec4(0.0f), "m_lootDropsWindowButton"));
    m_windowButtons[(unsigned int)Types::LOOT_DROP]->setText("Loot Drops");
    m_windowButtons[(unsigned int)Types::LOOT_DROP]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs &Arg)->bool{setLootDropsActive();}));


    m_windowButtons[(unsigned int)Types::LOOT_TABLE] = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.05f + 0.13f * 3.0f, 0.76f + 0.0375f, 0.125f, 0.0375f), glm::vec4(0.0f), "m_lootTablesWindowButton"));
    m_windowButtons[(unsigned int)Types::LOOT_TABLE]->setText("Loot Tables");
    m_windowButtons[(unsigned int)Types::LOOT_TABLE]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs &Arg)->bool{setLootTablesActive();}));


    m_windowButtons[(unsigned int)Types::STRUCTURE] = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.05f + 0.13f * 4.0f, 0.76f + 0.0375f, 0.125f, 0.0375f), glm::vec4(0.0f), "m_structuresWindowButton"));
    m_windowButtons[(unsigned int)Types::STRUCTURE]->setText("Structures");
    m_windowButtons[(unsigned int)Types::STRUCTURE]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs &Arg)->bool{setStructuresActive();}));
}

MainScreen::~MainScreen()
{
    //dtor
}

void MainScreen::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(.94f, .94f, 1.0f, 1.0f);

    m_gui.draw();
}

void MainScreen::update() {
    SDL_Event evnt;
    while (SDL_PollEvent(&evnt)) {
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
}

void MainScreen::saveXMLData(std::string filepath) {
    XMLModule::XMLData::write(filepath);
}

void MainScreen::setAllInactive() {
	for(unsigned int i = 0; i < m_numTypes; i++) {
		if(m_editWindows[i]) {
			m_editWindows[i]->setActive(false);
		}
	}
}

void MainScreen::setEditWindowActive(Types window) {
	setAllInactive();
    m_editWindows[(unsigned int)window]->setActive(true);
}

void MainScreen::addNewObject() {
    EditWindow* w = getActiveWindow();

    Object* obj = nullptr;
    unsigned int nextID = getNextID();

    if(w->m_name == "ItemEntities") {
        obj = new ItemEntity("NEW", nextID);
        m_itemEntitiesObjects.push_back(obj);
    } else if(w->m_name == "Projectiles") {
        obj = new Projectile("NEW", nextID);
        m_projectilesObjects.push_back(obj);
    } else if(w->m_name == "Particles") {
        obj = new Particle("NEW", nextID);
        m_particlesObjects.push_back(obj);
    } else if(w->m_name == "Blocks") {
        obj = new Block("NEW", nextID);
        m_blocksObjects.push_back(obj);
    } else if(w->m_name == "Items") {
        obj = new Item("NEW", nextID);
        m_itemsObjects.push_back(obj);
    } else if(w->m_name == "NPCs") {
        obj = new NPC("NEW", nextID);
        m_NPCsObjects.push_back(obj);
    } else if(w->m_name == "Biomes") {
        obj = new Biome("NEW", nextID);
        m_biomesObjects.push_back(obj);
    } else if(w->m_name == "Eras") {
        obj = new Era("NEW", nextID);
        m_erasObjects.push_back(obj);
    } else if(w->m_name == "Loot Drops") {
        obj = new LootDrop("NEW", nextID);
        m_lootDropsObjects.push_back(obj);
    } else if(w->m_name == "Loot Tables") {
        obj = new LootTable("NEW", nextID);
        m_lootTablesObjects.push_back(obj);
    } else if(w->m_name == "Structures") {
        obj = new Structure("NEW", nextID);
        m_structuresObjects.push_back(obj);
    }
    w->addObject(obj);
}

void MainScreen::removeObject() {
    EditWindow* w = getActiveWindow();

    std::vector<Object*>* objects;

    if(w->m_name == "ItemEntities") {
        objects = &m_itemEntitiesObjects;
    } else if(w->m_name == "Projectiles") {
        objects = &m_projectilesObjects;
    } else if(w->m_name == "Particles") {
        objects = &m_particlesObjects;
    } else if(w->m_name == "Blocks") {
        objects = &m_blocksObjects;
    } else if(w->m_name == "Items") {
        objects = &m_itemsObjects;
    } else if(w->m_name == "NPCs") {
        objects = &m_NPCsObjects;
    }

    if(w->m_selection >= 0) {
        w->m_itemListBox0->removeItem(w->m_itemListBox0->getItemFromIndex(w->m_selection));
        for(int i = w->m_selection; i < w->m_objects.size()-1; i++) {
            w->m_objects[i] = w->m_objects[i+1];
            (*objects)[i] = (*objects)[i+1];
        }
        w->m_objects.pop_back();
        objects->pop_back();
    }
}

int MainScreen::getNextID() {
    EditWindow* w = getActiveWindow();

    if(w) {
        unsigned int lowest = 0;

        for(int i = 0; i < w->m_objects.size(); i++) {
            if(w->m_objects[i]->m_id > lowest) {
                lowest = w->m_objects[i]->m_id;
            }
        }

        if(w->m_name == "ItemEntities" || w->m_name == "Projectiles" || w->m_name == "NPCs") {
            for(int i = 0; i < m_itemEntities.m_objects.size(); i++) {
                if(m_itemEntities.m_objects[i]->m_id > lowest) {
                    lowest = m_itemEntities.m_objects[i]->m_id;
                }
            }
            for(int i = 0; i < m_projectiles.m_objects.size(); i++) {
                if(m_projectiles.m_objects[i]->m_id > lowest) {
                    lowest = m_projectiles.m_objects[i]->m_id;
                }
            }
            for(int i = 0; i < m_NPCs.m_objects.size(); i++) {
                if(m_NPCs.m_objects[i]->m_id > lowest) {
                    lowest = m_NPCs.m_objects[i]->m_id;
                }
            }
        }

        return lowest + 1;
    }
    return 0;
}

EditWindow* MainScreen::getActiveWindow() {
    if(m_itemEntities.isActive()) {
        return &m_itemEntities;
    }
    if(m_projectiles.isActive()) {
        return &m_projectiles;
    }
    if(m_particles.isActive()) {
        return &m_particles;
    }
    if(m_blocks.isActive()) {
        return &m_blocks;
    }
    if(m_items.isActive()) {
        return &m_items;
    }
    if(m_NPCs.isActive()) {
        return &m_NPCs;
    }
    if(m_biomes.isActive()) {
        return &m_biomes;
    }
    if(m_eras.isActive()) {
        return &m_eras;
    }
    if(m_lootDrops.isActive()) {
        return &m_lootDrops;
    }
    if(m_lootTables.isActive()) {
        return &m_lootTables;
    }
    if(m_structures.isActive()) {
        return &m_structures;
    }
}
