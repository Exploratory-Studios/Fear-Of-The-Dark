#include "LoadScreen.h"

/// LoadScreen FUNCTIONS

LoadScreen::LoadScreen(GLEngine::Window* window, WorldIOManager* WorldIOManager) : m_window(window), m_worldIOManager(WorldIOManager)
{

}

LoadScreen::~LoadScreen()
{
    //dtor
}

int LoadScreen::getNextScreenIndex() const {
    return m_nextScreenIndex;
}

int LoadScreen::getPreviousScreenIndex() const {
    return SCREEN_INDEX_MAINMENU;
}

void LoadScreen::build() {

}

void LoadScreen::destroy() {

}

void LoadScreen::onEntry() {
    initShaders();

    m_spriteBatch.init();
    m_spriteFont.init((ASSETS_FOLDER_PATH + "GUI/fonts/QuietHorror.ttf").c_str(), 96);

    m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
    m_camera.setPosition(glm::vec2(m_window->getScreenWidth() / 2.0f, m_window->getScreenHeight() / 2.0f));

    m_uiCamera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
    m_uiCamera.setPosition(glm::vec2(m_window->getScreenWidth() / 2.0f, m_window->getScreenHeight() / 2.0f));

    initUI();
}

void LoadScreen::onExit() {

}

void LoadScreen::update() {
    checkInput();

    m_camera.update();
    m_uiCamera.update();

    m_gui.update();

    for(auto e : m_miniScreenEntries) e.checkScreen(m_miniScreen);

    m_time++;
}

void LoadScreen::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    m_textureProgram.use();

    GLint textureUniform = m_textureProgram.getUniformLocation("mySampler");
    glUniform1i(textureUniform, 0);
    glActiveTexture(GL_TEXTURE0);

    // Camera matrix
    glm::mat4 projectionMatrix = m_uiCamera.getCameraMatrix();
    GLint pUniform = m_textureProgram.getUniformLocation("P");
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

    m_spriteBatch.begin();

    float alpha = 0.0f;

    if(m_time <= 255.0f/2.0f) {
        alpha = m_time*2.0f;
    } else {
        alpha = 255.0f;
    }

    m_createButton->setAlpha(alpha / 255);
    m_loadButton->setAlpha(alpha / 255);

    m_gui.draw();

    m_spriteBatch.end();
    m_spriteBatch.renderBatch();

    m_textureProgram.unuse();
}

void LoadScreen::checkInput() {
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

void LoadScreen::initUI() {
    {
        m_gui.init(ASSETS_FOLDER_PATH + "GUI");
        m_gui.loadScheme("FOTDSkin.scheme");

        m_gui.setFont("QuietHorror-30");

        m_gui.setMouseCursor("FOTDSkin/MouseArrow");
        m_gui.showMouseCursor();
        SDL_ShowCursor(0);
    }

    {
        m_createButton = static_cast<CEGUI::PushButton*> (m_gui.createWidget("FOTDSkin/Button", glm::vec4(0.1f, 0.1f, 0.25f, 0.15f), glm::vec4(0.0f), "CreatePushbutton"));
        m_createButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LoadScreen::onCreateButtonClicked, this));
        m_createButton->setText("[padding='l:0 t:15 r:0 b:0']New World");
        m_createButton->setFont("QuietHorror-42");
        m_miniScreenEntries.emplace_back(m_createButton, MiniScreen::MAIN);
    }

    {
        m_newWorldNameEditbox = static_cast<CEGUI::Editbox*> (m_gui.createWidget("FOTDSkin/Editbox", glm::vec4(0.1f, 0.1f, 0.25f, 0.1f), glm::vec4(0.0f), "WorldNameEditbox"));
        m_newWorldNameEditbox->setText("Name");
        m_miniScreenEntries.emplace_back(m_newWorldNameEditbox, MiniScreen::CREATE);

        m_newWorldSeedEditbox = static_cast<CEGUI::Editbox*> (m_gui.createWidget("FOTDSkin/Editbox", glm::vec4(0.1f, 0.25f, 0.25f, 0.1f), glm::vec4(0.0f), "SeedEditbox"));
        m_newWorldSeedEditbox->setText("Seed");
        m_miniScreenEntries.emplace_back(m_newWorldSeedEditbox, MiniScreen::CREATE);

        m_newWorldCreateNewButton = static_cast<CEGUI::PushButton*> (m_gui.createWidget("FOTDSkin/Button", glm::vec4(0.35f, 0.7f, 0.3f, 0.15f), glm::vec4(0.0f), "CreateNewWorldPushbutton"));
        m_newWorldCreateNewButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LoadScreen::onNewWorldCreateNewButtonClicked, this));
        m_newWorldCreateNewButton->setText("[padding='l:0 t:15 r:0 b:0']Create World");
        m_newWorldCreateNewButton->setFont("QuietHorror-42");
        m_miniScreenEntries.emplace_back(m_newWorldCreateNewButton, MiniScreen::CREATE);

        m_newWorldFlatCheckbox = static_cast<CEGUI::ToggleButton*> (m_gui.createWidget("FOTDSkin/RadioButton", glm::vec4(0.1f, 0.4f, 0.12f, 0.1f), glm::vec4(0.0f), "WorldFlatToggleButton"));
        m_newWorldFlatCheckbox->setText("[padding='l:0 t:10 r:0 b:0']Flat World");
        m_miniScreenEntries.emplace_back(m_newWorldFlatCheckbox, MiniScreen::CREATE);
    }

    {
        m_loadButton = static_cast<CEGUI::PushButton*> (m_gui.createWidget("FOTDSkin/Button", glm::vec4(0.1f, 0.3f, 0.25f, 0.15f), glm::vec4(0.0f), "LoadPushButton"));
        m_loadButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LoadScreen::onLoadButtonClicked, this));
        m_loadButton->setText("[padding='l:0 t:15 r:0 b:0']Load World");
        m_loadButton->setFont("QuietHorror-42");
        m_miniScreenEntries.emplace_back(m_loadButton, MiniScreen::MAIN);
    }

    {
        m_loadWorldNameEditbox = static_cast<CEGUI::Editbox*>(m_gui.createWidget("FOTDSkin/Editbox", glm::vec4(0.1f, 0.1, 0.25f, 0.15f), glm::vec4(0.0f), "LoadWorldNameEditbox"));
        m_loadWorldNameEditbox->setText("Name");
        m_miniScreenEntries.emplace_back(m_loadWorldNameEditbox, MiniScreen::LOAD);

        m_loadWorldLoadButton = static_cast<CEGUI::PushButton*>(m_gui.createWidget("FOTDSkin/Button", glm::vec4(0.1f, 0.3f, 0.25f, 0.15f), glm::vec4(0.0f), "LoadWorldLoadPushButton"));
        m_loadWorldLoadButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LoadScreen::onLoadWorldLoadButtonClicked, this));
        m_loadWorldLoadButton->setText("[padding='l:0 t:15 r:0 b:0']Load World");
        m_loadWorldLoadButton->setFont("QuietHorror-42");
        m_miniScreenEntries.emplace_back(m_loadWorldLoadButton, MiniScreen::LOAD);
    }
}

void LoadScreen::initShaders() {
    m_textureProgram.compileShaders(ASSETS_FOLDER_PATH + "Shaders/textureShading.vert", ASSETS_FOLDER_PATH + "Shaders/textureShading.frag");
    m_textureProgram.addAttribute("vertexPosition");
    m_textureProgram.addAttribute("vertexColour");
    m_textureProgram.addAttribute("vertexUV");
    m_textureProgram.linkShaders();
}

bool LoadScreen::onNewWorldCreateNewButtonClicked(const CEGUI::EventArgs& e) { /// TODO: clean post-this lines up

    std::string text = m_newWorldSeedEditbox->getText().c_str();
    unsigned int seed = 0;

    for(unsigned int i = 0; i < text.length(); i++) {
        seed += (char)text[i];
    }

    m_worldIOManager->createWorld(seed, m_newWorldNameEditbox->getText().c_str(), m_newWorldFlatCheckbox->isSelected());

    //m_worldIOManager->createWorld(seed, m_newWorldNameEditbox->getText().c_str(), m_newWorldFlatCheckbox->isSelected(), nullptr);

    m_currentState = GLEngine::ScreenState::CHANGE_NEXT;
    m_nextScreenIndex = SCREEN_INDEX_WAIT;

    return true;
}

bool LoadScreen::onLoadWorldLoadButtonClicked(const CEGUI::EventArgs& e) { /// TODO: clean post-this lines up

    std::string text = m_loadWorldNameEditbox->getText().c_str();

    m_worldIOManager->loadWorld(text);

    m_currentState = GLEngine::ScreenState::CHANGE_NEXT;
    m_nextScreenIndex = SCREEN_INDEX_WAIT;

    return true;
}

bool LoadScreen::onCreateButtonClicked(const CEGUI::EventArgs& e) {
    m_miniScreen = MiniScreen::CREATE;
    return true;
}

bool LoadScreen::onLoadButtonClicked(const CEGUI::EventArgs& e) {
    m_miniScreen = MiniScreen::LOAD;
    return true;
}
