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
    m_spriteFont.init("../Assets/GUI/fonts/QuietHorror.ttf", 96);

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

    if(m_time <= 255.0f) {
        alpha = m_time;
    } else {
        alpha = 255.0f;
    }

    m_gui.draw();
    for(auto l : m_labels) l.draw(m_spriteFont, m_spriteBatch, GLEngine::ColourRGBA8(l.getColour().r,
                                                                                     l.getColour().g,
                                                                                     l.getColour().b,
                                                                                     alpha));

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
        m_gui.init("../Assets/GUI");
        m_gui.loadScheme("FOTDSkin.scheme");

        m_gui.setFont("QuietHorror-22");

        m_gui.setMouseCursor("FOTDSkin/MouseArrow");
        m_gui.showMouseCursor();
        SDL_ShowCursor(0);
    }

    {
        m_createButton = static_cast<CEGUI::PushButton*> (m_gui.createWidget("FOTDSkin/Button", glm::vec4(0.1f, 0.1f, 0.25f, 0.15f), glm::vec4(0.0f), "CreatePushbutton"));
        m_createButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LoadScreen::onCreateButtonClicked, this));
        m_labels.emplace_back("Create New", glm::vec2(0.8f, 0.7f), m_createButton, GLEngine::ColourRGBA8(255.0f, 255.0f, 255.0f, 255.0f), m_window);
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
        m_labels.emplace_back("Create", glm::vec2(0.8f, 0.5f), m_newWorldCreateNewButton, GLEngine::ColourRGBA8(255, 255, 255, 255), m_window);
        m_miniScreenEntries.emplace_back(m_newWorldCreateNewButton, MiniScreen::CREATE);

    }
}

void LoadScreen::initShaders() {
    m_textureProgram.compileShaders("../Assets/Shaders/textureShading.vert", "../Assets/Shaders/textureShading.frag");
    m_textureProgram.addAttribute("vertexPosition");
    m_textureProgram.addAttribute("vertexColour");
    m_textureProgram.addAttribute("vertexUV");
    m_textureProgram.linkShaders();
}

bool LoadScreen::onNewWorldCreateNewButtonClicked(const CEGUI::EventArgs& e) { /// TODO: clean post-this lines up

    std::string text = m_newWorldSeedEditbox->getText().c_str();
    unsigned int seed = 0;

    for(int i = 0; i < text.length(); i++) {
        seed += (char)text[i];
    }

    m_worldIOManager->createWorld(seed, m_newWorldNameEditbox->getText().c_str());

    m_currentState = GLEngine::ScreenState::CHANGE_NEXT;
    m_nextScreenIndex = SCREEN_INDEX_GAMEPLAY;

    return true;
}

bool LoadScreen::onCreateButtonClicked(const CEGUI::EventArgs& e) {
    m_miniScreen = MiniScreen::CREATE;
    return true;
}
