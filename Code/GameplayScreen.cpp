#include "GameplayScreen.h"

#include "Entity.h"

GameplayScreen::GameplayScreen(GLEngine::Window* window, WorldManager* worldManager) : m_window(window), m_worldManager(worldManager)
{

}

GameplayScreen::~GameplayScreen()
{

}

int GameplayScreen::getNextScreenIndex() const {

}

int GameplayScreen::getPreviousScreenIndex() const {

}

void GameplayScreen::build() {

}

void GameplayScreen::destroy() {

}

void GameplayScreen::onEntry() {
    initShaders();

    m_spriteBatch.init();
    m_spriteFont.init("../Assets/GUI/fonts/QuietHorror.ttf", 96);

    m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
    m_camera.setPosition(m_camera.getPosition() + (glm::vec2(m_window->getScreenWidth() / 2, m_window->getScreenHeight() / 2)));
    m_uiCamera.init(m_window->getScreenWidth(), m_window->getScreenHeight());

    initUI();

    testEnt.init(glm::vec2(60 * TILE_SIZE, 50 * TILE_SIZE), Categories::Entity_Type::MOB, 0);
}

void GameplayScreen::onExit() {

}

void GameplayScreen::update() {
    checkInput();

    m_camera.update();
    m_uiCamera.update();

    m_gui.update();

    Tile tiles[WORLD_HEIGHT][CHUNK_SIZE] = m_worldManager->getWorld()->chunks[0].tiles; /// I hate myself

    if(m_gameState != GameState::PAUSE) {
        for(int i = 0; i < WORLD_SIZE; i++) {
            m_worldManager->getWorld()->chunks[i].update();
        }
    }

    std::vector<Entity> entities;

    testEnt.collide(entities, tiles);
    testEnt.update();

    m_time++; /// Change the increment if time is slowed or quicker (potion effects?)
}

void GameplayScreen::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.8f, 1.0f);

    m_textureProgram.use();

    GLint textureUniform = m_textureProgram.getUniformLocation("mySampler");
    glUniform1i(textureUniform, 0);
    glActiveTexture(GL_TEXTURE0);

    // Camera matrix
    glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
    GLint pUniform = m_textureProgram.getUniformLocation("P");
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

    m_spriteBatch.begin();

    if(m_gameState != GameState::PAUSE) {
        for(int i = 0; i < WORLD_SIZE; i++) {
            m_worldManager->getWorld()->chunks[i].draw(m_spriteBatch);
        }
    }

    testEnt.draw(m_spriteBatch);

    m_spriteBatch.end();
    m_spriteBatch.renderBatch();

    m_textureProgram.unuse();

    /// TODO: Don't forget to have a background image for the world based on biome
}

/// GameplayScreen PRIVATE FUNCTIONS

void GameplayScreen::checkInput() {
    SDL_Event evnt;
    while (SDL_PollEvent(&evnt)) {
        m_game->onSDLEvent(evnt);
        m_gui.onSDLEvent(evnt);
        switch(evnt.type) {
            case SDL_QUIT:
                m_currentState = GLEngine::ScreenState::EXIT_APPLICATION;
                break;
            case SDL_MOUSEBUTTONDOWN:

                break;
        }
    }
}

void GameplayScreen::initShaders() {
    m_textureProgram.compileShaders("../Assets/Shaders/textureShading.vert", "../Assets/Shaders/textureShading.frag");
    m_textureProgram.addAttribute("vertexPosition");
    m_textureProgram.addAttribute("vertexColour");
    m_textureProgram.addAttribute("vertexUV");
    m_textureProgram.linkShaders();
}

void GameplayScreen::initUI() {
    {
        m_gui.init("../Assets/GUI");
        m_gui.loadScheme("FOTDSkin.scheme");
        m_gui.setFont("QuietHorror-22");

        m_gui.setMouseCursor("FOTDSkin/MouseArrow");
        m_gui.showMouseCursor();
        SDL_ShowCursor(0);
    }
}
