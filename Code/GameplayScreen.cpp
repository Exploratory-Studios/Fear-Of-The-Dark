#include "GameplayScreen.h"

#include "Entity.h"

#include <SDL2/SDL_timer.h>

#ifdef DEV_CONTROLS
#include <iostream>
#include <regex>
#endif //DEV_CONTROLS

GameplayScreen::GameplayScreen(GLEngine::Window* window, WorldIOManager* WorldIOManager) : m_window(window), m_WorldIOManager(WorldIOManager)
{

}

GameplayScreen::~GameplayScreen()
{

}

int GameplayScreen::getNextScreenIndex() const {
    return m_screenIndex;
}

int GameplayScreen::getPreviousScreenIndex() const {
    return m_screenIndex;
}

void GameplayScreen::build() {

}

void GameplayScreen::destroy() {

}

void GameplayScreen::onEntry() {

    initShaders();

    m_spriteBatch.init();
    m_spriteFont.init((ASSETS_FOLDER_PATH + "GUI/fonts/Amatic-Bold.ttf").c_str(), 96);

    m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
    m_camera.setPosition(m_camera.getPosition() + (glm::vec2(m_window->getScreenWidth() / 2, m_window->getScreenHeight() / 2)));

    m_uiCamera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
    m_uiCamera.setPosition(m_uiCamera.getPosition() + (glm::vec2(m_window->getScreenWidth() / 2, m_window->getScreenHeight() / 2)));

    m_dr.init();

    //m_WorldManager.init(m_WorldIOManager, &m_particle2d);
    m_scripter = new Scripter(this);
    m_audioManager.init();

    if(m_WorldIOManager->getWorld()) {
        std::vector<Entity*> entities;

        for(unsigned int j = 0; j < WORLD_SIZE; j++) {
            for(unsigned int i = 0; i < m_WorldIOManager->getWorld()->chunks[j]->getEntities().size(); i++) {
                entities.push_back(m_WorldIOManager->getWorld()->chunks[j]->getEntities()[i]);
            }
        }

        m_player = m_WorldIOManager->getWorld()->player;

        {
            int index = (m_player->getPosition().x / TILE_SIZE) / CHUNK_SIZE / WORLD_SIZE;
            m_player->setParentChunk(m_WorldIOManager->getWorld()->chunks[index%WORLD_SIZE]);
        }

        {
            for(unsigned int i = 0; i < entities.size(); i++) {
                int index = (entities[i]->getPosition().x / TILE_SIZE) / CHUNK_SIZE;
                entities[i]->setParentChunk(m_WorldIOManager->getWorld()->chunks[index]);
                m_WorldIOManager->getWorld()->chunks[index]->addEntity(entities[i]);
            }
        }
    }

    m_questManager = new QuestManager(ASSETS_FOLDER_PATH + "Questing/DialogueList.txt", ASSETS_FOLDER_PATH + "Questing/FlagList.txt", m_WorldIOManager->getScriptQueue());
    m_console = new Console();

    std::vector<Parameter> p;
    Parameter a;
    a.setPointer(m_questManager);
    p.push_back(a);

    m_WorldIOManager->getWorld()->chunks[0]->addEntity(createEntity((unsigned int)Categories::EntityIDs::MOB_NEUTRAL_QUESTGIVER_A, glm::vec2(10.0f * TILE_SIZE, (200.0f) * TILE_SIZE), nullptr, p));

    initUI();
    tick();
}

void GameplayScreen::onExit() {

}

void GameplayScreen::update() {
    m_deltaTime = std::abs((60 / m_game->getFps()) + -1);
    m_deltaTime++;

    if(m_frame == 0.0f) {
        if(m_player) {
            activateChunks();
            tick();
        } else {
            logger->log("Could not initialize world (Full update when frames == 0). Some things may not be as expected.", true);
        }
    }

    if((int)m_frame % (int)(60 / m_tickRate) == 0) { // m_frame is equal to current frame
        tick();
    }

    checkInput();

    if(m_gameState != GameState::PAUSE) {
        m_questManager->update(m_game->inputManager);
        m_scripter->update();

        for(unsigned int i = 0; i < m_activatedChunks.size(); i++) {
            int xOffset = std::abs(m_activatedChunks[i] + WORLD_SIZE) % WORLD_SIZE;
            m_WorldIOManager->getWorld()->chunks[xOffset]->update(m_time, m_deltaTime, m_WorldIOManager->getWorld()->chunks);
        }

        // Set player caninteract

        if(m_player) {
            activateChunks();
            m_player->update(m_deltaTime, m_WorldIOManager->getWorld()->chunks);
            m_player->updateMouse(&m_camera);
            m_player->collide();
            m_player->setCanInteract(!m_questManager->isDialogueActive());
        }

        if((m_player->getPosition().x + m_player->getSize().x / 2.0f) - m_lastPlayerPos.x < -CHUNK_SIZE * TILE_SIZE) {
            m_lastPlayerPos.x -= WORLD_SIZE * CHUNK_SIZE * TILE_SIZE;
        } else if(m_player->getPosition().x + m_player->getSize().x / 2.0f - m_lastPlayerPos.x > CHUNK_SIZE * TILE_SIZE) {
            m_lastPlayerPos.x += WORLD_SIZE * CHUNK_SIZE * TILE_SIZE;
        }
        m_lastPlayerPos = (m_lastPlayerPos + ((m_player->getPosition() + m_player->getSize() / glm::vec2(2.0f)) - m_lastPlayerPos) / glm::vec2(4.0f));

        m_camera.setPosition(m_lastPlayerPos);

        m_camera.setScale(m_scale);

        m_camera.update();
        m_uiCamera.update();

        m_time++; /// Change the increment if time is slowed or quicker (potion effects?)

    }
    m_frame++;

    m_gui.update();
}
#include <stdio.h>
void GameplayScreen::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float dayLight = cos((float)m_WorldIOManager->getWorld()->time / (DAY_LENGTH / 6.28318f)) / 2.0f + 0.5f;

    glClearColor(0.3f * dayLight, 0.4f * dayLight, 1.0f * dayLight, 1.0f);

    {
        m_textureProgram.use();

        // Camera matrix
        glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
        GLint pUniform = m_textureProgram.getUniformLocation("P");
        glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

        m_spriteBatch.begin(GLEngine::GlyphSortType::FRONT_TO_BACK); // lower numbers in back

        for(unsigned int i = 0; i < m_activatedChunks.size(); i++) {
            int xOffset = std::abs(m_activatedChunks[i] + WORLD_SIZE) % WORLD_SIZE;
            m_WorldIOManager->getWorld()->chunks[xOffset]->draw(m_spriteBatch, m_activatedChunks[i] - xOffset, m_time);
        }

        m_player->draw(m_spriteBatch, m_time, 0);

        m_spriteBatch.end();
        m_spriteBatch.renderBatch();

        m_textureProgram.unuse();
    }

    {
        m_uiTextureProgram.use();

        GLint textureUniform = m_uiTextureProgram.getUniformLocation("mySampler");
        glUniform1i(textureUniform, 0);
        glActiveTexture(GL_TEXTURE0);

        // Camera matrix
        glm::mat4 projectionMatrix = m_uiCamera.getCameraMatrix();
        GLint pUniform = m_uiTextureProgram.getUniformLocation("P");
        glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

        m_questManager->draw();
        m_player->drawGUI(m_spriteBatch, m_spriteFont);

        m_gui.draw();

        m_dr.end();
        m_dr.render(projectionMatrix, 3);

        #ifdef DEV_CONTROLS
        if(m_debuggingInfo) {
            drawDebug();
        }
        #endif // DEV_CONTROLS

        m_uiTextureProgram.unuse();
    }

    /// TODO: Don't forget to have a background image for the world based on biome
}

/// GameplayScreen PRIVATE FUNCTIONS

void GameplayScreen::checkInput() {
    SDL_Event evnt;
    while (SDL_PollEvent(&evnt)) {
        if(!m_console->isShown()) m_game->onSDLEvent(evnt);
        m_gui.onSDLEvent(evnt);
        switch(evnt.type) {
            case SDL_QUIT:
                m_currentState = GLEngine::ScreenState::EXIT_APPLICATION;
                break;
            case SDL_MOUSEBUTTONDOWN:
                break;
            case SDL_MOUSEWHEEL:
                updateScale();
                break;
        }
    }

    #ifdef DEV_CONTROLS
    if(m_game->inputManager.isKeyPressed(SDLK_BACKSLASH)) {
        m_console->show();
    }
    if(m_game->inputManager.isKeyPressed(SDLK_F1)) {
        m_debuggingInfo = !m_debuggingInfo;
        m_fpsWidget->setVisible(m_debuggingInfo);
    }
    if(m_game->inputManager.isKeyPressed(SDLK_F2)) {
        std::string command = "createEntity 12 76 2 questManager";
        m_scripter->executeCommand(command);
    }
    #endif // DEV_CONTROLS

    if(m_game->inputManager.isKeyPressed(SDLK_ESCAPE)) {
        //m_gameState = GameState::PAUSE;
        m_WorldIOManager->saveWorld(*m_WorldIOManager->getWorld(), "TestSave", nullptr);
    } else if(m_game->inputManager.isKeyPressed(SDLK_F3)) {
        m_WorldIOManager->loadWorld("TestSave", nullptr);

    }

}

void GameplayScreen::initShaders() {
    m_textureProgram.compileShaders(ASSETS_FOLDER_PATH + "Shaders/textureShading.vert", ASSETS_FOLDER_PATH + "Shaders/textureShading.frag");
    m_textureProgram.addAttribute("vertexPosition");
    m_textureProgram.addAttribute("vertexColour");
    m_textureProgram.addAttribute("vertexUV");
    m_textureProgram.addAttribute("vertexLighting");
    m_textureProgram.linkShaders();

    m_uiTextureProgram.compileShaders(ASSETS_FOLDER_PATH + "Shaders/uiShader.vert", ASSETS_FOLDER_PATH + "Shaders/uiShader.frag");
    m_uiTextureProgram.addAttribute("vertexPosition");
    m_uiTextureProgram.addAttribute("vertexColour");
    m_uiTextureProgram.addAttribute("vertexUV");
    m_uiTextureProgram.linkShaders();
}

void GameplayScreen::initUI() {
    {
        m_gui.init(ASSETS_FOLDER_PATH + "GUI");
        m_gui.loadScheme("FOTDSkin.scheme");

        m_gui.setFont("Amatic-26");

        m_gui.setMouseCursor("FOTDSkin/MouseArrow");
        m_gui.showMouseCursor();
        SDL_ShowCursor(0);
    }

    m_player->initGUI(&m_gui);
    m_questManager->initUI(&m_gui);
    m_console->init(m_gui, m_scripter);

    #ifdef DEV_CONTROLS
    {
        m_fpsWidget = static_cast<CEGUI::DefaultWindow*>(m_gui.createWidget("FOTDSkin/Label", glm::vec4(0.05f, 0.05f, 0.9f, 0.25f), glm::vec4(0.0f), "FPS_STRING_WIDGET"));
        m_fpsWidget->setHorizontalAlignment(CEGUI::HorizontalAlignment::HA_LEFT);
    }
    #endif //DEV_CONTROLS
}

void GameplayScreen::tick() {
    for(unsigned int i = 0; i < m_activatedChunks.size(); i++) {
        int xOffset = std::abs(m_activatedChunks[i] + WORLD_SIZE) % WORLD_SIZE;
        m_WorldIOManager->getWorld()->chunks[xOffset]->tick(m_WorldIOManager->getWorld()->time, m_player);
    }

    if(!m_audioManager.isMusicPlaying()) {
        ///m_audioManager.playMorningSong(0); TODO
    }

    m_WorldIOManager->setWorldTime(m_WorldIOManager->getWorld()->time + 1);
}

void GameplayScreen::updateScale() {
    //if(!isDialogueActive()) {
        m_scale += (float)m_game->inputManager.getMouseScrollPosition() * m_scale / 10.0f;
        if(m_scale < MIN_ZOOM) {
            m_scale = MIN_ZOOM;
        } else if(m_scale > MAX_ZOOM) {
            m_scale = MAX_ZOOM;
        }
   // }
}

#ifdef DEV_CONTROLS
void GameplayScreen::drawDebug() {
    std::string fps = "FPS: " + std::to_string((int)m_game->getFps()) + "\nMouse x,y: " + std::to_string(m_player->m_selectedBlock->getPosition().x) + "," + std::to_string(m_player->m_selectedBlock->getPosition().y);
    std::string placeString;
    switch((unsigned int)m_player->m_selectedBlock->getParentChunk()->getPlace()) {
        case (unsigned int)Categories::Places::ARCTIC: { placeString = "Arctic"; break; }
        case (unsigned int)Categories::Places::ASIA: { placeString = "Asia"; break; }
        case (unsigned int)Categories::Places::AUSTRALIA: { placeString = "Australia"; break; }
        case (unsigned int)Categories::Places::CANADA: { placeString = "Canada"; break; }
        case (unsigned int)Categories::Places::NORTH_AFRICA: { placeString = "North Africa"; break; }
        case (unsigned int)Categories::Places::RUSSIA: { placeString = "Russia"; break; }
        case (unsigned int)Categories::Places::SOUTH_AFRICA: { placeString = "South Africa"; break; }
        case (unsigned int)Categories::Places::USA: { placeString = "Excited States of America"; break; }
    }

    if(m_player->m_selectedBlock)
        fps += "\nSelected Block: Biome: " + placeString + ", " + m_player->m_selectedBlock->getPrintout();
    m_fpsWidget->setText(fps);
}
#endif //DEV_CONTROLS

void GameplayScreen::activateChunks() {
    const int chunkIndex = m_player->getChunkIndex();

    if(chunkIndex != m_lastActivated && chunkIndex >= 0) { // Make sure that we changed chunks
        m_activatedChunks.clear(); // I forgot I had this at my disposal :)

        const signed int each = std::ceil((VIEW_DIST - 1) / 2); // How many chunks on each side of the centre of the selection

        for(signed int i = -each; i <= each; i++) {
            //if(chunkIndex + i >= 0) {
                m_activatedChunks.push_back(chunkIndex + i);

                int realIndex = (chunkIndex + i + WORLD_SIZE) % WORLD_SIZE;

                m_WorldIOManager->getWorld()->chunks[realIndex]->activateChunk();
            //} else if(chunkIndex + i < 0) {
            //    m_activatedChunks.push_back(((chunkIndex + i) % WORLD_SIZE) + WORLD_SIZE);
            //}
        }

        m_lastActivated = chunkIndex;
        //std::cout << chunkIndex << std::endl;
    }
}
