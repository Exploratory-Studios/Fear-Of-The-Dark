#include "MainScreen.h"

#include "XMLData.h"

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

void EditWindow::addAttribute(Attribute* a) {
    CEGUI::ItemEntry* l = static_cast<CEGUI::ItemEntry*>(m_gui->createWidget(m_itemListBox1, "WindowsLook/ListboxItem", glm::vec4(0.0f, 0.0f, 1.0f, 0.1f), glm::vec4(0.0f), a->name + m_name + "AttributeEntry"));
    l->setSelectable(false);
    std::string sortT = std::string(m_itemListBox1->getItemCount(), 1);
    l->setText(sortT);

    CEGUI::DefaultWindow* label = static_cast<CEGUI::DefaultWindow*>(m_gui->createWidget(l, "WindowsLook/Label", glm::vec4(0.0f, 0.0f, 0.5f, 1.0f), glm::vec4(0.0f), a->name + m_name + "AttributeEntryLabel"));
    label->setText(a->name);

    if(a->type == ValueType::STRING) {
        CEGUI::Editbox* eb = static_cast<CEGUI::Editbox*>(m_gui->createWidget(l, "WindowsLook/Editbox", glm::vec4(0.5f, 0.0f, 0.5f, 1.0f), glm::vec4(0.0f), a->name + m_name + "AttributeEntryEBSTRING"));
        eb->setText(a->value);
        eb->subscribeEvent(CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs& args)->bool{
            a->value = (std::string)eb->getText().c_str();
        }));
    } else if(a->type == ValueType::BOOL) {
        CEGUI::ToggleButton* eb = static_cast<CEGUI::ToggleButton*>(m_gui->createWidget(l, "WindowsLook/Checkbox", glm::vec4(0.735f, 0.0f, 1.2f, 1.2f), glm::vec4(0.0f), a->name + m_name + "AttributeEntryEBBOOL"));
        bool b = a->value == "1";
        eb->setSelected(b);
        eb->subscribeEvent(CEGUI::ToggleButton::EventSelectStateChanged, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs& args)->bool{
            std::string val = std::to_string(eb->isSelected());
            a->value = val;
        }));
    } else if(a->type == ValueType::FLOAT) {
        CEGUI::Editbox* eb = static_cast<CEGUI::Editbox*>(m_gui->createWidget(l, "WindowsLook/Editbox", glm::vec4(0.5f, 0.0f, 0.5f, 1.0f), glm::vec4(0.0f), a->name + m_name + "AttributeEntryEBFLOAT"));
        eb->setText(a->value);
        eb->setValidationString("\\D{0}^[\\d]*([.][\\d]*)?$\\D{0}");
        eb->subscribeEvent(CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs& args)->bool{
            a->value = (std::string)eb->getText().c_str();
        }));
    } else if(a->type == ValueType::INT) {
        CEGUI::Editbox* eb = static_cast<CEGUI::Editbox*>(m_gui->createWidget(l, "WindowsLook/Editbox", glm::vec4(0.5f, 0.0f, 0.5f, 1.0f), glm::vec4(0.0f), a->name + m_name + "AttributeEntryEBINT"));
        eb->setText(a->value);
        eb->setValidationString("\\D{0}^[\\d]*$\\D{0}");
        eb->subscribeEvent(CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs& args)->bool{
            a->value = (std::string)eb->getText().c_str();
        }));
    } else if(a->type == ValueType::VECTOR_INT) {
        CEGUI::Editbox* eb = static_cast<CEGUI::Editbox*>(m_gui->createWidget(l, "WindowsLook/Editbox", glm::vec4(0.5f, 0.0f, 0.5f, 1.0f), glm::vec4(0.0f), a->name + m_name + "AttributeEntryEBVEC_INT"));
        eb->setText(a->value);
        eb->setValidationString("(((\\d)*)(,{1}(\\d)*)*)?$"); // One or 0 items, it must have a number, then 0 or more commas followed by a number.
        eb->subscribeEvent(CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs& args)->bool{
            a->value = (std::string)eb->getText().c_str();
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

    m_itemEntities.init("ItemEntities", m_gui);
     m_projectiles.init("Projectiles", m_gui);
       m_particles.init("Particles", m_gui);
          m_blocks.init("Blocks", m_gui);
           m_items.init("Items", m_gui);
            m_NPCs.init("NPCs", m_gui);
          m_biomes.init("Biomes", m_gui);
            m_eras.init("Eras", m_gui);
       m_lootDrops.init("Loot Drops", m_gui);
      m_lootTables.init("Loot Tables", m_gui);
      m_structures.init("Structures", m_gui);

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

    m_itemEntitiesWindowButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.05f, 0.76f, 0.125f, 0.0375f), glm::vec4(0.0f), "ItemEntitiesWindowButton"));
    m_itemEntitiesWindowButton->setText("ItemEntities");
    m_itemEntitiesWindowButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs &Arg)->bool{setItemEntitiesActive();}));


    m_projectilesWindowButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.05f + 0.13f, 0.76f, 0.125f, 0.0375f), glm::vec4(0.0f), "ProjectilesWindowButton"));
    m_projectilesWindowButton->setText("Projectiles");
    m_projectilesWindowButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs &Arg)->bool{setProjectilesActive();}));


    m_particlesWindowButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.05f + 0.13f * 2.0f, 0.76f, 0.125f, 0.0375f), glm::vec4(0.0f), "m_particlesWindowButton"));
    m_particlesWindowButton->setText("Particles");
    m_particlesWindowButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs &Arg)->bool{setParticlesActive();}));


    m_blocksWindowButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.05f + 0.13f * 3.0f, 0.76f, 0.125f, 0.0375f), glm::vec4(0.0f), "m_blocksWindowButton"));
    m_blocksWindowButton->setText("Blocks");
    m_blocksWindowButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs &Arg)->bool{setBlocksActive();}));


    m_itemsWindowButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.05f + 0.13f * 4.0f, 0.76f, 0.125f, 0.0375f), glm::vec4(0.0f), "m_itemsWindowButton"));
    m_itemsWindowButton->setText("Items");
    m_itemsWindowButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs &Arg)->bool{setItemsActive();}));


    m_NPCsWindowButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.05f + 0.13f * 5.0f, 0.76f, 0.125f, 0.0375f), glm::vec4(0.0f), "m_NPCsWindowButton"));
    m_NPCsWindowButton->setText("NPCs");
    m_NPCsWindowButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs &Arg)->bool{setNPCsActive();}));


    m_biomesWindowButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.05f, 0.76f + 0.0375f, 0.125f, 0.0375f), glm::vec4(0.0f), "m_BiomesWindowButton"));
    m_biomesWindowButton->setText("Biomes");
    m_biomesWindowButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs &Arg)->bool{setBiomesActive();}));


    m_erasWindowButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.05f + 0.13f, 0.76f + 0.0375f, 0.125f, 0.0375f), glm::vec4(0.0f), "m_erasWindowButton"));
    m_erasWindowButton->setText("Eras");
    m_erasWindowButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs &Arg)->bool{setErasActive();}));


    m_lootDropsWindowButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.05f + 0.13f * 2.0f, 0.76f + 0.0375f, 0.125f, 0.0375f), glm::vec4(0.0f), "m_lootDropsWindowButton"));
    m_lootDropsWindowButton->setText("Loot Drops");
    m_lootDropsWindowButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs &Arg)->bool{setLootDropsActive();}));


    m_lootTablesWindowButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.05f + 0.13f * 3.0f, 0.76f + 0.0375f, 0.125f, 0.0375f), glm::vec4(0.0f), "m_lootTablesWindowButton"));
    m_lootTablesWindowButton->setText("Loot Tables");
    m_lootTablesWindowButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs &Arg)->bool{setLootTablesActive();}));


    m_structuresWindowButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("WindowsLook/Button", glm::vec4(0.05f + 0.13f * 4.0f, 0.76f + 0.0375f, 0.125f, 0.0375f), glm::vec4(0.0f), "m_structuresWindowButton"));
    m_structuresWindowButton->setText("Structures");
    m_structuresWindowButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber([=](const CEGUI::EventArgs &Arg)->bool{setStructuresActive();}));
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
    std::string err = XMLData::init(filepath);

    if(err == "ERR") {
        std::cout << err << std::endl;
        return;
    }

    for(auto& i : XMLData::m_entityItemData) {
        Object* obj = new Object(i.second.name, i.first);

        obj->m_attributes.emplace_back("texture", ValueType::STRING, i.second.textureFilepath);
        obj->m_attributes.emplace_back("bumpMap", ValueType::STRING, i.second.bumpMapFilepath);
        obj->m_attributes.emplace_back("sizeX", ValueType::FLOAT, i.second.size.x);
        obj->m_attributes.emplace_back("sizeY", ValueType::FLOAT, i.second.size.y);
        obj->m_attributes.emplace_back("item", ValueType::INT, i.second.itemID);
        obj->m_attributes.emplace_back("updateScript", ValueType::STRING, i.second.updateScriptID);
        obj->m_attributes.emplace_back("tickScript", ValueType::STRING, i.second.tickScriptID);

        m_itemEntitiesObjects.push_back(obj);
        m_itemEntities.addObject(obj);
    }

    for(auto& i : XMLData::m_entityProjectileData) {
        Object* obj = new Object(i.second.name, i.first);

        obj->m_attributes.emplace_back("texture", ValueType::STRING, i.second.textureFilepath);
        obj->m_attributes.emplace_back("bumpMap", ValueType::STRING, i.second.bumpMapFilepath);
        obj->m_attributes.emplace_back("sizeX", ValueType::FLOAT, i.second.size.x);
        obj->m_attributes.emplace_back("sizeY", ValueType::FLOAT, i.second.size.y);
        obj->m_attributes.emplace_back("speed", ValueType::FLOAT, i.second.speed);
        obj->m_attributes.emplace_back("damage", ValueType::FLOAT, i.second.damage);
        obj->m_attributes.emplace_back("collides", ValueType::BOOL, i.second.collides);
        obj->m_attributes.emplace_back("updateScript", ValueType::STRING, i.second.updateScriptID);
        obj->m_attributes.emplace_back("tickScript", ValueType::STRING, i.second.tickScriptID);

        m_projectilesObjects.push_back(obj);
        m_projectiles.addObject(obj);
    }

    for(auto& i : XMLData::m_particleData) {
        Object* obj = new Object(i.second.name, i.first);

        obj->m_attributes.emplace_back("texture", ValueType::STRING, i.second.textureFilepath);
        obj->m_attributes.emplace_back("bumpMap", ValueType::STRING, i.second.bumpMapFilepath);
        obj->m_attributes.emplace_back("decayRate", ValueType::FLOAT, i.second.decayRate);
        obj->m_attributes.emplace_back("script", ValueType::STRING, i.second.scriptPath);

        m_particlesObjects.push_back(obj);
        m_particles.addObject(obj);
    }

    for(auto& i : XMLData::m_tileData) {
        Object* obj = new Object(i.second.name, i.first);

        obj->m_attributes.emplace_back("texture", ValueType::STRING, i.second.textureFilepath);
        obj->m_attributes.emplace_back("bumpMap", ValueType::STRING, i.second.bumpMapFilepath);
        obj->m_attributes.emplace_back("emittedLight", ValueType::FLOAT, i.second.emittedLight);
        obj->m_attributes.emplace_back("emittedHeat", ValueType::FLOAT, i.second.emittedHeat);
        obj->m_attributes.emplace_back("sizeX", ValueType::FLOAT, i.second.size.x);
        obj->m_attributes.emplace_back("sizeY", ValueType::FLOAT, i.second.size.y);
        obj->m_attributes.emplace_back("isSolid", ValueType::BOOL, i.second.solid);
        obj->m_attributes.emplace_back("isDrawn", ValueType::BOOL, i.second.drawn);
        obj->m_attributes.emplace_back("isNatural", ValueType::BOOL, i.second.natural);
        obj->m_attributes.emplace_back("isTransparent", ValueType::BOOL, i.second.transparent);
        obj->m_attributes.emplace_back("updateScript", ValueType::STRING, i.second.updateScriptID);
        obj->m_attributes.emplace_back("tickScript", ValueType::STRING, i.second.tickScriptID);
        obj->m_attributes.emplace_back("destructionScript", ValueType::STRING, i.second.destructionScriptID);
        obj->m_attributes.emplace_back("interactScript_walkedOn", ValueType::STRING, i.second.interactScriptID_walkedOn);
        obj->m_attributes.emplace_back("interactScript_used", ValueType::STRING, i.second.interactScriptID_used);

        m_blocksObjects.push_back(obj);
        m_blocks.addObject(obj);
    }

    for(auto& i : XMLData::m_entityNPCData) {
        Object* obj = new Object(i.second.name, i.first);

        obj->m_attributes.emplace_back("texture", ValueType::STRING, i.second.textureFilepath);
        obj->m_attributes.emplace_back("bumpMap", ValueType::STRING, i.second.bumpMapFilepath);
        obj->m_attributes.emplace_back("sizeX", ValueType::FLOAT, i.second.size.x);
        obj->m_attributes.emplace_back("sizeY", ValueType::FLOAT, i.second.size.y);
        obj->m_attributes.emplace_back("isDamagedByFalls", ValueType::BOOL, i.second.fallDamage);
        obj->m_attributes.emplace_back("isInvincible", ValueType::BOOL, i.second.canDie);
        obj->m_attributes.emplace_back("speed", ValueType::FLOAT, i.second.speed);
        obj->m_attributes.emplace_back("jumpHeight", ValueType::FLOAT, i.second.jumpHeight);
        obj->m_attributes.emplace_back("maxHealth", ValueType::FLOAT, i.second.maxHealth);
        obj->m_attributes.emplace_back("faction", ValueType::STRING, i.second.faction);
        obj->m_attributes.emplace_back("updateScript", ValueType::STRING, i.second.updateScriptID);
        obj->m_attributes.emplace_back("tickScript", ValueType::STRING, i.second.tickScriptID);
        obj->m_attributes.emplace_back("attackScript", ValueType::STRING, i.second.attackScriptID);

        m_NPCsObjects.push_back(obj);
        m_NPCs.addObject(obj);
    }

    for(auto& i : XMLData::m_itemData) {
        Object* obj = new Object(i.second.name, i.first);

        obj->m_attributes.emplace_back("texture", ValueType::STRING, i.second.textureFilepath);
        obj->m_attributes.emplace_back("weight", ValueType::FLOAT, i.second.weight);
        obj->m_attributes.emplace_back("useScript", ValueType::STRING, i.second.useScriptID);

        m_itemsObjects.push_back(obj);
        m_items.addObject(obj);
    }

    for(auto& i : XMLData::m_biomeData) {
        Object* obj = new Object(i.second.name, i.first);

        obj->m_attributes.emplace_back("backgroundTexture", ValueType::STRING, i.second.backgroundTexture);
        obj->m_attributes.emplace_back("baseHeight", ValueType::INT, i.second.baseHeight);
        obj->m_attributes.emplace_back("maxHeightDiff", ValueType::INT, i.second.maxHeightDiff);
        obj->m_attributes.emplace_back("maxTemperature", ValueType::FLOAT, i.second.maxTemperature);
        obj->m_attributes.emplace_back("baseTemperature", ValueType::FLOAT, i.second.baseTemperature);
        obj->m_attributes.emplace_back("flatness", ValueType::FLOAT, i.second.flatness);

        obj->m_attributes.emplace_back("entities/entityID", ValueType::VECTOR_INT, i.second.mobSpawnIds);
        obj->m_attributes.emplace_back("structures/structure", ValueType::VECTOR_INT, i.second.structureSpawnIds);
        obj->m_attributes.emplace_back("surfaceBlocks/blockID", ValueType::VECTOR_INT, i.second.surfaceBlockIds);
        obj->m_attributes.emplace_back("groundBlocks/blockID", ValueType::VECTOR_INT, i.second.undergroundBlockIds);

        m_biomesObjects.push_back(obj);
        m_biomes.addObject(obj);
    }

    for(auto& i : XMLData::m_eraData) {
        Object* obj = new Object(i.second.name, i.first);

        obj->m_attributes.emplace_back("biomes/biomeID", ValueType::VECTOR_INT, i.second.biomeIds);

        m_erasObjects.push_back(obj);
        m_eras.addObject(obj);
    }

    for(auto& i : XMLData::m_lootDropData) {
        Object* obj = new Object(i.second.name, i.first);

        obj->m_attributes.emplace_back("itemID", ValueType::INT, i.second.itemID);
        obj->m_attributes.emplace_back("minDrop", ValueType::INT, i.second.minDrop);
        obj->m_attributes.emplace_back("maxDrop", ValueType::INT, i.second.maxDrop);
        obj->m_attributes.emplace_back("chance", ValueType::FLOAT, i.second.chance);

        m_lootDropsObjects.push_back(obj);
        m_lootDrops.addObject(obj);
    }

    for(auto& i : XMLData::m_lootTableData) {
        Object* obj = new Object(i.second.name, i.first);

        obj->m_attributes.emplace_back("drops/dropID", ValueType::VECTOR_INT, i.second.drops);

        m_lootTablesObjects.push_back(obj);
        m_lootTables.addObject(obj);
    }

    for(auto& i : XMLData::m_structureData) {
        Object* obj = new Object(i.second.name, i.first);

        obj->m_attributes.emplace_back("structureFilepath", ValueType::STRING, i.second.structureFilepath);
        obj->m_attributes.emplace_back("biomes/biomeID", ValueType::VECTOR_INT, i.second.biomeIDs);
        obj->m_attributes.emplace_back("chance", ValueType::FLOAT, i.second.chance);
        obj->m_attributes.emplace_back("maxAmnt", ValueType::INT, i.second.maxAmnt);
        obj->m_attributes.emplace_back("minAmnt", ValueType::INT, i.second.minAmnt);

        m_structuresObjects.push_back(obj);
        m_structures.addObject(obj);
    }
}

void MainScreen::saveXMLData(std::string filepath) {
    saveXMLFile(m_itemEntitiesObjects, "itemEntity", filepath + "/ItemEntities.xml");
    saveXMLFile(m_projectilesObjects,  "projectile", filepath + "/Projectiles.xml");
    saveXMLFile(m_particlesObjects,    "particle",   filepath + "/Particles.xml");
    saveXMLFile(m_blocksObjects,       "tile",       filepath + "/Blocks.xml");
    saveXMLFile(m_itemsObjects,        "item",       filepath + "/Items.xml");
    saveXMLFile(m_NPCsObjects,         "npc",        filepath + "/NPCs.xml");
    saveXMLFile(m_biomesObjects,       "biome",      filepath + "/Biomes.xml");
    saveXMLFile(m_erasObjects,         "era",        filepath + "/Eras.xml");
    saveXMLFile(m_lootDropsObjects,    "lootDrop",   filepath + "/Loot.xml");
    saveXMLFile(m_lootTablesObjects,   "lootTable",  filepath + "/Loot.xml", true); // Must append loot tables so as not to destroy the loot drops
    saveXMLFile(m_structuresObjects,   "structure",  filepath + "/Structures.xml");
}

void MainScreen::saveXMLFile(std::vector<Object*> objects, std::string itemTag, std::string filepath, bool append) {
    std::ofstream file;
    if(append) {
        file.open(filepath, std::ofstream::app);
    } else {
        file.open(filepath);
    }
    rapidxml::xml_document<> doc;
    for(int i = 0; i < objects.size(); i++) {
        saveXMLObject(objects[i], doc, itemTag);
    }
    file << doc;
    file.close();
}

void MainScreen::saveXMLObject(Object* obj, rapidxml::xml_document<>& doc, std::string itemTag) {
    rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_type::node_element, itemTag.c_str());

    char* attr_id = doc.allocate_string(std::to_string(obj->m_id).c_str());
    char* attr_name = doc.allocate_string(obj->m_attributes[0].value.c_str());

    node->append_attribute(doc.allocate_attribute("id", attr_id));
    node->append_attribute(doc.allocate_attribute("name", attr_name));
    doc.append_node(node);

    for(int i = 1; i < obj->getAttributes().size(); i++) { // Ignores the name attribute, already set to parent node
        saveXMLAttribute(obj->getAttributes()[i], doc, node);
    }
}
#include <iostream>
void MainScreen::saveXMLAttribute(Attribute* a, rapidxml::xml_document<>& doc, rapidxml::xml_node<>* parent) {
    if(!a->value.empty()) {
        if(a->type != ValueType::VECTOR_INT) {
            char* node_name = doc.allocate_string(a->name.c_str());
            char* node_value = doc.allocate_string(a->value.c_str());

            rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_type::node_element, node_name, node_value);
            parent->append_node(node);
        } else {
            // First we must seperate the listName from the elementName (listName/elementName)
            std::string listName, elementName;
            unsigned int sepIndex = a->name.find('/'); // The index of the '/'
            listName = a->name.substr(0, sepIndex);
            elementName = a->name.substr(sepIndex+1, a->name.size()-(sepIndex+1));

            std::string val = a->value + ",";

            std::vector<std::string> elements;
            int index = val.find(",");
            while(index != std::string::npos) {
                elements.push_back(val.substr(0,index) + '\0');
                val = val.replace(0,index+1,"");
                index = val.find(",");
            }

            char* parent_node_name = doc.allocate_string(listName.c_str());
            rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_type::node_element, parent_node_name);

            for(unsigned int i = 0; i < elements.size(); i++) {
                char* node_name = doc.allocate_string(elementName.c_str());
                char* node_value = doc.allocate_string(elements[i].c_str());

                rapidxml::xml_node<>* n = doc.allocate_node(rapidxml::node_type::node_element, node_name, node_value);
                node->append_node(n);
            }

            parent->append_node(node);
        }
    }
}

void MainScreen::setItemEntitiesActive() {
    m_itemEntities.setActive(true);
    m_projectiles.setActive(false);
    m_particles.setActive(false);
    m_blocks.setActive(false);
    m_items.setActive(false);
    m_NPCs.setActive(false);
    m_biomes.setActive(false);
    m_eras.setActive(false);
    m_lootDrops.setActive(false);
    m_lootTables.setActive(false);
    m_structures.setActive(false);
}

void MainScreen::setProjectilesActive() {
    m_itemEntities.setActive(false);
    m_projectiles.setActive(true);
    m_particles.setActive(false);
    m_blocks.setActive(false);
    m_items.setActive(false);
    m_NPCs.setActive(false);
    m_biomes.setActive(false);
    m_eras.setActive(false);
    m_lootDrops.setActive(false);
    m_lootTables.setActive(false);
    m_structures.setActive(false);
}

void MainScreen::setParticlesActive() {
    m_itemEntities.setActive(false);
    m_projectiles.setActive(false);
    m_particles.setActive(true);
    m_blocks.setActive(false);
    m_items.setActive(false);
    m_NPCs.setActive(false);
    m_biomes.setActive(false);
    m_eras.setActive(false);
    m_lootDrops.setActive(false);
    m_lootTables.setActive(false);
    m_structures.setActive(false);
}

void MainScreen::setBlocksActive() {
    m_itemEntities.setActive(false);
    m_projectiles.setActive(false);
    m_particles.setActive(false);
    m_blocks.setActive(true);
    m_items.setActive(false);
    m_NPCs.setActive(false);
    m_biomes.setActive(false);
    m_eras.setActive(false);
    m_lootDrops.setActive(false);
    m_lootTables.setActive(false);
    m_structures.setActive(false);
}

void MainScreen::setItemsActive() {
    m_itemEntities.setActive(false);
    m_projectiles.setActive(false);
    m_particles.setActive(false);
    m_blocks.setActive(false);
    m_items.setActive(true);
    m_NPCs.setActive(false);
    m_biomes.setActive(false);
    m_eras.setActive(false);
    m_lootDrops.setActive(false);
    m_lootTables.setActive(false);
    m_structures.setActive(false);
}

void MainScreen::setNPCsActive() {
    m_itemEntities.setActive(false);
    m_projectiles.setActive(false);
    m_particles.setActive(false);
    m_blocks.setActive(false);
    m_items.setActive(false);
    m_NPCs.setActive(true);
    m_biomes.setActive(false);
    m_eras.setActive(false);
    m_lootDrops.setActive(false);
    m_lootTables.setActive(false);
    m_structures.setActive(false);
}

void MainScreen::setBiomesActive() {
    m_itemEntities.setActive(false);
    m_projectiles.setActive(false);
    m_particles.setActive(false);
    m_blocks.setActive(false);
    m_items.setActive(false);
    m_NPCs.setActive(false);
    m_biomes.setActive(true);
    m_eras.setActive(false);
    m_lootDrops.setActive(false);
    m_lootTables.setActive(false);
    m_structures.setActive(false);
}

void MainScreen::setErasActive() {
    m_itemEntities.setActive(false);
    m_projectiles.setActive(false);
    m_particles.setActive(false);
    m_blocks.setActive(false);
    m_items.setActive(false);
    m_NPCs.setActive(false);
    m_biomes.setActive(false);
    m_eras.setActive(true);
    m_lootDrops.setActive(false);
    m_lootTables.setActive(false);
    m_structures.setActive(false);
}

void MainScreen::setLootDropsActive() {
    m_itemEntities.setActive(false);
    m_projectiles.setActive(false);
    m_particles.setActive(false);
    m_blocks.setActive(false);
    m_items.setActive(false);
    m_NPCs.setActive(false);
    m_biomes.setActive(false);
    m_eras.setActive(false);
    m_lootDrops.setActive(true);
    m_lootTables.setActive(false);
    m_structures.setActive(false);
}

void MainScreen::setLootTablesActive() {
    m_itemEntities.setActive(false);
    m_projectiles.setActive(false);
    m_particles.setActive(false);
    m_blocks.setActive(false);
    m_items.setActive(false);
    m_NPCs.setActive(false);
    m_biomes.setActive(false);
    m_eras.setActive(false);
    m_lootDrops.setActive(false);
    m_lootTables.setActive(true);
    m_structures.setActive(false);
}

void MainScreen::setStructuresActive() {
    m_itemEntities.setActive(false);
    m_projectiles.setActive(false);
    m_particles.setActive(false);
    m_blocks.setActive(false);
    m_items.setActive(false);
    m_NPCs.setActive(false);
    m_biomes.setActive(false);
    m_eras.setActive(false);
    m_lootDrops.setActive(false);
    m_lootTables.setActive(false);
    m_structures.setActive(true);
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
