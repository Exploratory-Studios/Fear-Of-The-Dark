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
    delete m_scripter;
    delete m_questManager;
    delete m_console;
}

void GameplayScreen::onEntry() {

    initShaders();

    m_spriteBatch.init();
    m_spriteFont.init((ASSETS_FOLDER_PATH + "GUI/fonts/Amatic-Bold.ttf").c_str(), 96);

    m_scale = INITIAL_ZOOM;
    m_camera.setScale(m_scale);
    m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
    m_camera.setPosition(m_camera.getPosition() + (glm::vec2(m_window->getScreenWidth() / 2, m_window->getScreenHeight() / 2)));

    m_uiCamera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
    m_uiCamera.setPosition(m_uiCamera.getPosition() + (glm::vec2(m_window->getScreenWidth() / 2, m_window->getScreenHeight() / 2)));

    m_dr.init();

    //m_WorldManager.init(m_WorldIOManager, &m_particle2d);
    m_scripter = new Scripter(this);
    m_WorldIOManager->getAudioManager()->init();

    if(m_WorldIOManager->getWorld()) {
        for(unsigned int j = 0; j < WORLD_SIZE; j++) {
            m_WorldIOManager->getWorld()->chunks[j]->setAudioManager(m_WorldIOManager->getAudioManager());
        }

        if(!m_WorldIOManager->getWorld()->player) {
            m_player = reinterpret_cast<Player*>(createEntity((unsigned int)Categories::EntityIDs::MOB_PLAYER, glm::vec2(5.0f, 100.0f), m_WorldIOManager->getWorld()->chunks[(int)(5.0f) / CHUNK_SIZE], m_WorldIOManager->getAudioManager(), nullptr, &m_game->inputManager, m_WorldIOManager->getScriptQueue()));
            m_WorldIOManager->setPlayer(m_player);
        } else {
            m_player = m_WorldIOManager->getWorld()->player;
        }

        {
            int index = (m_player->getPosition().x) / CHUNK_SIZE / WORLD_SIZE;
            m_player->setParentChunk(&m_WorldIOManager->getWorld()->chunks[index%WORLD_SIZE]);
        }

        {
            for(unsigned int j = 0; j < WORLD_SIZE; j++) {
                std::vector<Entity*> entities;
                entities = m_WorldIOManager->getWorld()->chunks[j]->getEntities();
                for(unsigned int i = 0; i < entities.size(); i++) {
                    int index = (m_WorldIOManager->getWorld()->chunks[j]->getEntities()[i]->getPosition().x) / CHUNK_SIZE;
                    m_WorldIOManager->getWorld()->chunks[j]->getEntities()[i]->setParentChunk(m_WorldIOManager->getWorld()->chunks[index]);
                    m_WorldIOManager->getWorld()->chunks[index]->addEntity(m_WorldIOManager->getWorld()->chunks[j]->getEntities()[i]);
                }
            }
        }
    }

    m_questManager = new QuestManager(ASSETS_FOLDER_PATH + "Questing/DialogueList.txt", ASSETS_FOLDER_PATH + "Questing/FlagList.txt", ASSETS_FOLDER_PATH + "Questing/TradeList.txt", m_WorldIOManager->getScriptQueue());
    m_console = new Console();

    m_WorldIOManager->getWorld()->chunks[0]->addEntity(createEntity((unsigned int)Categories::EntityIDs::MOB_NEUTRAL_QUESTGIVER_A, glm::vec2(10.0f, (10.0f)), m_WorldIOManager->getWorld()->chunks[0], m_WorldIOManager->getAudioManager(), m_questManager));

    initUI();
    tick();

    if(m_player) {
        activateChunks();
    }
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

    if((int)m_frame % (int)(60 / TICK_RATE) == 0) { // m_frame is equal to current frame
        tick();
    }

    checkInput();

    if(m_gameState != GameState::PAUSE && m_currentState != GLEngine::ScreenState::EXIT_APPLICATION) {
        m_questManager->update(m_game->inputManager, m_player);
        m_scripter->update();

        // Set player caninteract

        if(m_player && !m_cutscenePause) {
            m_player->update(m_deltaTime, m_WorldIOManager->getWorld()->chunks);
            m_player->updateMouse(&m_camera);
            m_player->collide();
            m_player->setCanInteract(!m_questManager->isDialogueActive());
        }

        if(!m_cameraLocked) {
            if(std::abs((m_player->getPosition().x + m_player->getSize().x / 2.0f) - m_lastPlayerPos.x) >= (float)(CHUNK_SIZE) * (WORLD_SIZE/2)) {
                int sign = ((m_player->getPosition().x + m_player->getSize().x / 2.0f) - m_lastPlayerPos.x) / std::abs((m_player->getPosition().x + m_player->getSize().x / 2.0f) - m_lastPlayerPos.x);
                m_lastPlayerPos.x += (float)(WORLD_SIZE * CHUNK_SIZE) * sign;
                m_camera.setPosition(m_camera.getPosition() + glm::vec2((float)(WORLD_SIZE * CHUNK_SIZE) * sign, 0.0f));
            }
            m_lastPlayerPos = (m_lastPlayerPos + ((m_player->getPosition() + m_player->getSize() / glm::vec2(2.0f)) - m_lastPlayerPos) / glm::vec2(4.0f));
            m_camera.setPosition(m_lastPlayerPos); // If lastplayerpos is never updated, the camera is still 'locked' per say, but we can actually change the lastPlayerPos on purpose to get a smooth movement somewhere.
        } else {
            m_lastPlayerPos = (m_lastPlayerPos + (m_smoothMoveTarget - m_lastPlayerPos) * m_smoothMoveSpeed);
            m_camera.setPosition(m_lastPlayerPos);
        }

        for(unsigned int i = 0; i < m_activatedChunks.size(); i++) {
            int xOffset = std::abs(m_activatedChunks[i] + WORLD_SIZE) % WORLD_SIZE;
            m_WorldIOManager->getWorld()->chunks[xOffset]->update(m_time, m_deltaTime, m_WorldIOManager->getWorld()->chunks, m_player, !m_cutscenePause);
        }

        if(m_camera.getPosition().x > (float)(WORLD_SIZE * CHUNK_SIZE)) {
            m_camera.setPosition(m_camera.getPosition() - glm::vec2((float)(WORLD_SIZE * CHUNK_SIZE), 0.0f));
        } else if(m_camera.getPosition().x < 0.0000000f) {
            m_camera.setPosition(m_camera.getPosition() + glm::vec2((float)(WORLD_SIZE * CHUNK_SIZE), 0.0f));
        }

        if(m_scale > MIN_ZOOM && m_scale < MAX_ZOOM)
            m_camera.setScale(m_scale);

        if(m_player) {
            activateChunks();
        }

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

    //glClearColor(0.3f * dayLight, 0.4f * dayLight, 1.0f * dayLight, 1.0f);

    {
        m_skyTextureProgram.use();

        // Camera matrix
        glm::mat4 projectionMatrix = m_uiCamera.getCameraMatrix();
        GLint pUniform = m_skyTextureProgram.getUniformLocation("P");
        glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

        GLint sizeUniform = m_skyTextureProgram.getUniformLocation("screenSizeU");
        glUniform2f(sizeUniform, m_window->getScreenWidth(), m_window->getScreenHeight());

        GLint timeUniform = m_skyTextureProgram.getUniformLocation("time");
        glUniform1f(timeUniform, m_time);

        GLint lightUniform = m_skyTextureProgram.getUniformLocation("daylight");
        glUniform1f(lightUniform, dayLight);

        m_spriteBatch.begin(GLEngine::GlyphSortType::FRONT_TO_BACK);

        m_spriteBatch.draw(glm::vec4(0.0f, 0.0f, m_window->getScreenWidth(), m_window->getScreenHeight()), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 0, 1.0f, GLEngine::ColourRGBA8(0, 0, 0, 0));

        m_spriteBatch.end();
        m_spriteBatch.renderBatch();

        m_skyTextureProgram.unuse();

    }

    {
        m_textureProgram.use();

        // Camera matrix
        glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
        GLint pUniform = m_textureProgram.getUniformLocation("P");
        glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

        m_spriteBatch.begin(GLEngine::GlyphSortType::FRONT_TO_BACK); // lower numbers in back

        for(unsigned int i = 0; i < m_drawnChunks.size(); i++) {
            int xOffset = std::abs(m_drawnChunks[i] + WORLD_SIZE) % WORLD_SIZE;
            m_WorldIOManager->getWorld()->chunks[xOffset]->draw(m_spriteBatch, m_drawnChunks[i] - xOffset, m_time, m_camera);
            #ifdef DEBUG
            m_WorldIOManager->getWorld()->chunks[xOffset]->drawDebug(m_dr, m_drawnChunks[i] - xOffset);
            m_dr.end();
            #endif // DEBUG
        }

        m_player->draw(m_spriteBatch, m_time, 0);

        m_spriteBatch.end();
        m_spriteBatch.renderBatch();

        m_dr.render(projectionMatrix, 2);

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

    {
        m_vignetteTextureProgram.use();

        /*GLint textureUniform = m_vignetteTextureProgram.getUniformLocation("mySampler");
        glUniform1i(textureUniform, 0);
        glActiveTexture(GL_TEXTURE0);*/

        // Camera matrix
        glm::mat4 projectionMatrix = m_uiCamera.getCameraMatrix();
        GLint pUniform = m_vignetteTextureProgram.getUniformLocation("P");
        glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

        GLint sizeUniform = m_vignetteTextureProgram.getUniformLocation("screenSizeU");
        glUniform2f(sizeUniform, m_window->getScreenWidth(), m_window->getScreenHeight());

        GLint sanityUniform = m_vignetteTextureProgram.getUniformLocation("sanity");
        glUniform1f(sanityUniform, m_player->getSanity());

        GLint timeUniform = m_vignetteTextureProgram.getUniformLocation("time");
        glUniform1f(timeUniform, m_time);

        m_spriteBatch.begin(GLEngine::GlyphSortType::FRONT_TO_BACK);

        m_spriteBatch.draw(glm::vec4(0.0f, 0.0f, m_window->getScreenWidth(), m_window->getScreenHeight()), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 0, 0.0f, GLEngine::ColourRGBA8(0, 0, 0, 0));

        m_spriteBatch.end();
        m_spriteBatch.renderBatch();

        m_vignetteTextureProgram.unuse();
    }
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
                if(!m_questManager->isDialogueActive() && !m_questManager->isTradingActive()) updateScale();
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
        if(!m_selecting) {
            m_lastSelectedPosition = m_player->getSelectedBlock()->getPosition();
        } else {
            if(m_lastSelectedPosition.x > m_player->getSelectedBlock()->getPosition().x || m_lastSelectedPosition.y > m_player->getSelectedBlock()->getPosition().y) {
                logger->log("Tried to create a structure badly, please try again, first coord is lower left corner, second is upper right.");
            } else {

                std::string filepath = ASSETS_FOLDER_PATH + "/Structures/Test.bin";
                m_WorldIOManager->saveStructureToFile(filepath,
                                                      glm::vec4(m_lastSelectedPosition.x,
                                                      m_lastSelectedPosition.y,
                                                      m_player->getSelectedBlock()->getPosition().x - m_lastSelectedPosition.x,
                                                      m_player->getSelectedBlock()->getPosition().y - m_lastSelectedPosition.y)); // Long-ass line of code, or long ass-line of code? Only god knows, and there is no god.
            }
        }
        m_selecting = !m_selecting;
    }
    if(m_game->inputManager.isKeyPressed(SDLK_F3)) {
        std::string filepath = ASSETS_FOLDER_PATH + "/Structures/Test.bin";
        StructureData data = m_WorldIOManager->loadStructureFromFile(filepath);
        m_WorldIOManager->placeStructure(data, m_player->getSelectedBlock()->getPosition());
    }
    #endif // DEV_CONTROLS

    if(m_game->inputManager.isKeyPressed(SDLK_F4)) {
        //m_gameState = GameState::PAUSE;
        m_WorldIOManager->saveWorld("TestSave");
    } else if(m_game->inputManager.isKeyPressed(SDLK_F5)) {
        m_WorldIOManager->loadWorld("TestSave");

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

    m_vignetteTextureProgram.compileShaders(ASSETS_FOLDER_PATH + "Shaders/vignetteShader.vert", ASSETS_FOLDER_PATH + "Shaders/vignetteShader.frag");
    m_vignetteTextureProgram.addAttribute("vertexPosition");
    m_vignetteTextureProgram.addAttribute("vertexColour");
    m_vignetteTextureProgram.addAttribute("vertexUV");
    m_vignetteTextureProgram.linkShaders();

    m_skyTextureProgram.compileShaders(ASSETS_FOLDER_PATH + "Shaders/skyShader.vert", ASSETS_FOLDER_PATH + "Shaders/skyShader.frag");
    m_skyTextureProgram.addAttribute("vertexPosition");
    m_skyTextureProgram.addAttribute("vertexColour");
    m_skyTextureProgram.addAttribute("vertexUV");
    m_skyTextureProgram.linkShaders();
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
        m_fpsWidget = static_cast<CEGUI::DefaultWindow*>(m_gui.createWidget("FOTDSkin/Label", glm::vec4(0.05f, 0.05f, 0.9f, 0.9f), glm::vec4(0.0f), "FPS_STRING_WIDGET"));
        m_fpsWidget->setHorizontalAlignment(CEGUI::HorizontalAlignment::HA_LEFT);
        m_fpsWidget->setVerticalAlignment(CEGUI::VerticalAlignment::VA_TOP);
    }
    #endif //DEV_CONTROLS
}

void GameplayScreen::tick() {
    for(unsigned int i = 0; i < m_activatedChunks.size(); i++) {
        int xOffset = std::abs(m_activatedChunks[i] + WORLD_SIZE) % WORLD_SIZE;
        m_WorldIOManager->getWorld()->chunks[xOffset]->tick(m_WorldIOManager->getWorld()->time, m_player, m_WorldIOManager->getWorld()->worldEra, !m_cutscenePause);
    }

    if(!m_WorldIOManager->getAudioManager()->isMusicPlaying()) {
        int hour = ((int)m_time % DAY_LENGTH) / DAY_LENGTH + 0.5f;//(int)((m_time / TICK_RATE) + 12) % DAY_LENGTH;

        int randNum = std::rand() % 100;

        if(hour > 4.0f/24.0f && hour < 9.0f/24.0f) { // Morning (4am-9am)
            if(MORNING_MUSIC_LENGTH > 0)
                m_WorldIOManager->getAudioManager()->playMorningSong(randNum % MORNING_MUSIC_LENGTH);
        } else if(hour > 9.0f/24.0f && hour < 16.0f/24.0f) { // Day (9am-4pm)
            if(DAY_MUSIC_LENGTH > 0)
                m_WorldIOManager->getAudioManager()->playDaySong(randNum % DAY_MUSIC_LENGTH);
        } else if(hour > 16.0f/24.0f && hour < 21.0f/24.0f) { // Afternoon (4pm-9pm)
            if(AFTERNOON_MUSIC_LENGTH > 0)
                m_WorldIOManager->getAudioManager()->playAfternoonSong(randNum % AFTERNOON_MUSIC_LENGTH);
        } else if(hour > 21.0f/24.0f || hour < 4.0f/24.0f) { // Night (9pm-4am) (Overlap requires OR logic)
            if(hour >= 0.0f && hour <= 1.0f/24.0f) { // Nightmare hour (Midnight-1am)
                if(NIGHT_MUSIC_LENGTH > 0)
                    m_WorldIOManager->getAudioManager()->playNightmareSong(randNum % NIGHTMARE_MUSIC_LENGTH);
            } else {
                if(NIGHTMARE_MUSIC_LENGTH > 0)
                    m_WorldIOManager->getAudioManager()->playNightSong(randNum % NIGHT_MUSIC_LENGTH);
            }
        }
    }

    m_WorldIOManager->setWorldTime(m_WorldIOManager->getWorld()->time + 1);
}

void GameplayScreen::updateScale() {
    m_scale += (float)m_game->inputManager.getMouseScrollPosition() * m_scale / 10.0f;
    if(m_scale < MIN_ZOOM) {
        m_scale = MIN_ZOOM;
    } else if(m_scale > MAX_ZOOM) {
        m_scale = MAX_ZOOM;
    }
}

#include "ExtraFunctions.h"

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
    int chunkIndex = m_player->getChunkIndex();

    /*if(chunkIndex != m_lastActivated && chunkIndex >= 0) { // Make sure that we changed chunks
        m_activatedChunks.clear();

        const signed int each = std::ceil((VIEW_DIST - 1) / 2); // How many chunks on each side of the centre of the selection

        for(signed int i = -each; i <= each; i++) {
            m_activatedChunks.push_back(chunkIndex + i);

            int realIndex = (chunkIndex + i + WORLD_SIZE) % WORLD_SIZE;

            m_WorldIOManager->getWorld()->chunks[realIndex]->activateChunk();
        }

        m_lastActivated = chunkIndex;
    }*/

    bool m_playerChunkCovered = false;

    chunkIndex = (((int)m_camera.getPosition().x + WORLD_SIZE * CHUNK_SIZE) / CHUNK_SIZE) % WORLD_SIZE;

    m_drawnChunks.clear();
    m_activatedChunks.clear();
    for(signed int i = -MAX_VIEW_DIST; i <= MAX_VIEW_DIST; i++) {
        int x, y;
        x = m_WorldIOManager->getWorld()->chunks[chunkIndex]->tiles[0][0]->getPosition().x + (i * CHUNK_SIZE);
        y = m_WorldIOManager->getWorld()->chunks[chunkIndex]->tiles[0][0]->getPosition().y;

        if(m_camera.isBoxInView(glm::vec2(x, y), glm::vec2(CHUNK_SIZE, WORLD_HEIGHT))) {
            if(chunkIndex + i == m_player->getChunkIndex()) m_playerChunkCovered = true;
            m_drawnChunks.push_back(chunkIndex + i);
            m_activatedChunks.push_back(chunkIndex + i);
            int realIndex = (chunkIndex + i + WORLD_SIZE) % WORLD_SIZE;
            m_WorldIOManager->getWorld()->chunks[realIndex]->drawChunk();
            m_WorldIOManager->getWorld()->chunks[realIndex]->activateChunk();
        }
    }
    if(!m_playerChunkCovered) {
        int realIndex = (m_player->getChunkIndex() + WORLD_SIZE) % WORLD_SIZE;
        m_WorldIOManager->getWorld()->chunks[realIndex]->drawChunk();
        m_WorldIOManager->getWorld()->chunks[realIndex]->activateChunk();
    }
}
