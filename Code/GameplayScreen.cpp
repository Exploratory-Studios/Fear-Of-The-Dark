#include "GameplayScreen.h"

#include "GUI_InventoryReceiver.h"
#include "GUI_InventoryItem.h"
#include "GUI_InventoryItemRenderer.h"

#include "ScripterMain.h"
#include "World.h"
#include "Tile.h"
#include "EntityNPC.h"
#include "EntityPlayer.h"

#include "NPCInventory.h"

#include "Factory.h"

#include <SDL2/SDL_timer.h>

#ifdef DEV_CONTROLS
#include <iostream>
#include <regex>
#endif //DEV_CONTROLS

GameplayScreen::GameplayScreen(GLEngine::Window* window, WorldIOManager* WorldIOManager, World* world, Options* options) : m_window(window), m_WorldIOManager(WorldIOManager), m_world(world), m_options(options) {

}

GameplayScreen::~GameplayScreen() {

}

int GameplayScreen::getNextScreenIndex() const {
	return m_nextScreenIndex;
}

int GameplayScreen::getPreviousScreenIndex() const {
	return m_screenIndex;
}

void GameplayScreen::build() {

}

void GameplayScreen::destroy() {

}

void GameplayScreen::onEntry() {

	initUI();

	m_hasBeenInited = true;

	std::srand(std::time(NULL));

	initShaders();

	m_spriteBatch.init();
	m_spriteFont.init((ASSETS_FOLDER_PATH + "GUI/fonts/Amatic-Bold.ttf").c_str(), 96);
	m_mainFBO.init(glm::vec4(0.0f, 0.0f, m_window->getScreenWidth(), m_window->getScreenHeight()));
	m_normalFBO.init(glm::vec4(0.0f, 0.0f, m_window->getScreenWidth(), m_window->getScreenHeight()));
	m_skyFBO.init(glm::vec4(0.0f, 0.0f, m_window->getScreenWidth(), m_window->getScreenHeight()));
	m_sunlightFBO.init(glm::vec4(0.0f, 0.0f, m_window->getScreenWidth(), m_window->getScreenHeight()));

	m_scale = INITIAL_ZOOM;
	m_camera.setScale(m_scale);
	m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
	m_camera.setPosition((glm::vec2(m_window->getScreenWidth() / 2, m_window->getScreenHeight() / 2)));

	m_uiCamera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
	m_uiCamera.setPosition((glm::vec2(m_window->getScreenWidth() / 2, m_window->getScreenHeight() / 2)));

	m_dr.init();

	m_audio = new AudioManager();
	m_audio->init();
	m_audio->setSoundsVolume(m_options->soundsVolume * m_options->masterVolume);
	m_audio->setMusicVolume(m_options->musicVolume * m_options->masterVolume);

	///m_WorldManager.init(m_WorldIOManager, &m_particle2d);
	m_scripter = new ScriptingModule::Scripter();

	m_console = new Console();
	m_questManager = new QuestModule::QuestManager(m_scripter);

	m_scripter->init(m_world, m_questManager, this, m_audio, &m_particle2d); /// TODO: Questmanager

	if(!Factory::getEntityManager()->getPlayer()) {
		//Player p(glm::vec2(5.0f, 100.0f), true);
		EntityPlayer p(glm::vec2(5, 100), 0, SaveDataTypes::MetaData(), true);

		Factory::getEntityManager()->setPlayer(p);

		Factory::getEntityManager()->getPlayer()->initGUI();
	}

	m_console->init(m_scripter, m_world, m_questManager, this);

	m_dialogueManager = new DialogueModule::DialogueManager(m_questManager);
	//m_dialogueManager->activateDialogue(0);

	m_camera.setPosition(Factory::getEntityManager()->getPlayer()->getPosition());

	tick();
}

void GameplayScreen::onExit() {

	delete m_scripter;
	delete m_questManager;
	delete m_dialogueManager;

	m_hasBeenInited = false;
	Factory::getGUI()->destroy();
	delete m_console;

	m_textureProgram.dispose();
	m_uiTextureProgram.dispose();
	m_vignetteTextureProgram.dispose();
	m_skyTextureProgram.dispose();

	m_spriteBatch.dispose();
	m_spriteFont.dispose();
	m_dr.dispose();

	m_gameState = GameState::PLAY;
}

void GameplayScreen::update() {
	m_deltaTime = std::abs((FRAME_RATE / m_game->getFps()) + -1);
	m_deltaTime++;

	m_world->incrementTime();
	m_particle2d.update(1.0f);

	checkInput();

	if(m_world->getNextEra() != m_world->getEra()) {
		m_WorldIOManager->setWorldEra(m_world, m_world->getNextEra());
		/// TODO: Move to loading screen for this.
	}

	if(m_gameState != GameState::PAUSE && m_currentState != GLEngine::ScreenState::EXIT_APPLICATION) {
		m_scripter->update();
		m_questManager->update();

		// Set player caninteract

		if(Factory::getEntityManager()->getPlayer() && !m_cutscenePause) {
			Factory::getEntityManager()->getPlayer()->updateMouse(m_world, m_camera.convertScreenToWorld(m_game->inputManager.getMouseCoords()));
			Factory::getEntityManager()->getPlayer()->updateInput(&m_game->inputManager, m_world);
			/// TODO: Re-enable this -> m_world->getPlayer()->setCanInteract(!m_questManager->isDialogueActive());
		}

		m_world->updateTiles(getScreenBox() + glm::vec4(-10.0f, -10.0f, 20.0f, 20.0f));
		Factory::getEntityManager()->updateEntities(1.0f); /// TODO: Use timestep

		if(!m_cameraLocked) {
			EntityPlayer* player = Factory::getEntityManager()->getPlayer();

			if(std::abs((player->getPosition().x) - m_lastPlayerPos.x) >= (WORLD_SIZE / 2)) {
				int sign = ((player->getPosition().x + player->getSize().x / 2.0f) - m_lastPlayerPos.x) / std::abs((player->getPosition().x + player->getSize().x / 2.0f) - m_lastPlayerPos.x);
				m_lastPlayerPos.x += (float)(WORLD_SIZE) * sign;
				m_camera.setPosition(m_camera.getPosition() + glm::vec2((float)(WORLD_SIZE) * sign, 0.0f));
			}
			m_lastPlayerPos = (m_lastPlayerPos + ((player->getPosition() + player->getSize() / glm::vec2(2.0f)) - m_lastPlayerPos) / glm::vec2(4.0f));
			m_camera.setPosition(m_lastPlayerPos); // If lastplayerpos is never updated, the camera is still 'locked' per say, but we can actually change the lastPlayerPos on purpose to get a smooth movement somewhere.
		} else {
			m_lastPlayerPos = (m_lastPlayerPos + (m_smoothMoveTarget - m_lastPlayerPos) * m_smoothMoveSpeed);
			m_camera.setPosition(m_lastPlayerPos);
		}

		if((int)m_camera.getPosition().x > WORLD_SIZE) {
			m_camera.setPosition(m_camera.getPosition() - glm::vec2((float)(WORLD_SIZE), 0.0f));
		} else if((int)m_camera.getPosition().x < 0) {
			m_camera.setPosition(m_camera.getPosition() + glm::vec2((float)(WORLD_SIZE), 0.0f));
		}

		if(m_scale > MIN_ZOOM && m_scale < MAX_ZOOM)
			m_camera.setScale(m_scale);

		m_camera.update();
		m_uiCamera.update();

		m_time++; /// Change the increment if time is slowed or quicker (potion effects?)

		if((int)m_frame % (int)(60 / TICK_RATE) == 0) { // m_frame is equal to current frame
			tick();
		}

		m_frame++;
	}
	if(m_currentState != GLEngine::ScreenState::EXIT_APPLICATION) Factory::getGUI()->update();
}

void GameplayScreen::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 0.0, 1.0, 1.0);

	{
		// Sky
		drawSkyToFBO(); // Readies the FBO.

		m_basicFBOTextureProgram.use(); // Begins to draw the FBO

		// Camera matrix
		glm::mat4 projectionMatrix = m_uiCamera.getCameraMatrix();
		GLint pUniform = m_basicFBOTextureProgram.getUniformLocation("P");
		glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);
		GLint textureUniform = m_basicFBOTextureProgram.getUniformLocation("textureSampler");
		glUniform1i(textureUniform, 0);

		m_skyFBO.draw();

		m_basicFBOTextureProgram.unuse();
	}

	{
		drawWorldToFBO();
		drawWorldNormalToFBO(); // Draws to normal FBO
		drawWorldSunlightToFBO(); // Draws to sunlight FBO.
		drawParticlesToFBO();

		// Draw main
		m_postProcessor.use();

		// Camera matrix
		glm::mat4 projectionMatrix = m_uiCamera.getCameraMatrix();
		GLuint pUniform = m_postProcessor.getUniformLocation("P");
		glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

		GLuint textureUniform = m_postProcessor.getUniformLocation("textureSampler");
		glUniform1i(textureUniform, 0);

		textureUniform = m_postProcessor.getUniformLocation("depthMap");
		glUniform1i(textureUniform, 1);

		// Sunlight
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_sunlightFBO.getTexture());
		textureUniform = m_postProcessor.getUniformLocation("sunlightMap");
		glUniform1i(textureUniform, 2);

		// Normal Map.
		//glActiveTexture(GL_TEXTURE3);
		//glBindTexture(GL_TEXTURE_2D, m_normalFBO.getTexture()); // Bind the texture
		//textureUniform = m_postProcessor.getUniformLocation("normalMap");
		//glUniform1i(textureUniform, 3);

		GLint playerDepthUniform = m_postProcessor.getUniformLocation("playerDepth");
		float playerDepth = 0.1f + (Factory::getEntityManager()->getPlayer()->getLayer() * (1.0f / (float)(WORLD_DEPTH)) * 0.9f);
		glUniform1f(playerDepthUniform, playerDepth);

		m_world->setLightsUniform(getScreenBox() + glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), &m_postProcessor, m_camera); // sets "lights" uniform of vec3s

		m_mainFBO.draw();
		m_postProcessor.unuse();

	}

	{
		// Particles
		//drawParticlesToFBO();

		/*m_basicFBOTextureProgram.use();

		// Camera matrix
		glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
		GLuint pUniform = m_basicFBOTextureProgram.getUniformLocation("P");
		glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

		GLuint textureUniform = m_basicFBOTextureProgram.getUniformLocation("textureSampler");
		glUniform1i(textureUniform, 0);

		m_particleFBO.draw();

		m_basicFBOTextureProgram.unuse();*/
	}

	drawGUIToScreen(); // These two actually do draw to the screen.
	drawPostToScreen();
}

void GameplayScreen::drawSkyToFBO() {

	float dayLight = cos((float)m_world->getTime() / (DAY_LENGTH / 6.28318f)) / 2.0f + 0.5f;

	glClearColor(0.3f * dayLight, 0.4f * dayLight, 1.0f * dayLight, 1.0f);

	m_skyFBO.begin();
	m_skyFBO.clear();

	// Sky
	m_skyTextureProgram.use();

	// Camera matrix
	glm::mat4 projectionMatrix = m_uiCamera.getCameraMatrix();
	GLint pUniform = m_skyTextureProgram.getUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	GLint textureUniform = m_textureProgram.getUniformLocation("textureSampler");
	glUniform1i(textureUniform, 0);

	GLint sizeUniform = m_skyTextureProgram.getUniformLocation("screenSizeU");
	glUniform2f(sizeUniform, m_window->getScreenWidth(), m_window->getScreenHeight());

	GLint lightUniform = m_skyTextureProgram.getUniformLocation("daylight");
	glUniform1f(lightUniform, dayLight);

	GLint playerDepthUniform = m_skyTextureProgram.getUniformLocation("playerXPos");
	float playerChunkX = std::fmod((Factory::getEntityManager()->getPlayer()->getPosition().x + (float)CHUNK_SIZE), (float)CHUNK_SIZE);
	glUniform1f(playerDepthUniform, playerChunkX / (float)(CHUNK_SIZE));

	GLint parallaxZoomUniform = m_skyTextureProgram.getUniformLocation("parallaxZoom");
	glUniform1f(parallaxZoomUniform, 0.95f);

	m_spriteBatch.begin();


	int playerX = (int)Factory::getEntityManager()->getPlayer()->getPosition().x;
	std::string backgroundPath = m_world->getBiome(playerX).backgroundTexture;

	GLuint backgroundID = GLEngine::ResourceManager::getTexture(backgroundPath).id;

	m_spriteBatch.draw(glm::vec4(0.0f, 0.0f, m_window->getScreenWidth(), m_window->getScreenHeight()), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), backgroundID, 1.0f, GLEngine::ColourRGBA8(255, 255, 255, 255));

	m_spriteBatch.end();
	m_spriteBatch.renderBatch();

	m_skyTextureProgram.unuse();

	m_skyFBO.end();
}

void GameplayScreen::drawWorldToFBO() {
	m_mainFBO.begin();

	m_mainFBO.clear();

	{
		// World: Tiles
		m_textureProgram.use();

		// Camera matrix
		glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
		GLint pUniform = m_textureProgram.getUniformLocation("P");
		glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

		GLint textureUniform = m_textureProgram.getUniformLocation("textureSampler");
		glUniform1i(textureUniform, 0);

		m_spriteBatch.begin(GLEngine::GlyphSortType::NONE);

		m_world->drawTiles(m_spriteBatch, m_spriteFont, m_dr, getScreenBox() + glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), &m_textureProgram); // handles spritebatch.begin and end
		Factory::getEntityManager()->drawEntities(m_spriteBatch, m_spriteFont, m_dr, getScreenBox() + glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

		m_spriteBatch.end();
		m_spriteBatch.renderBatch();

		m_textureProgram.unuse();
	}

	m_mainFBO.end();
}

void GameplayScreen::drawWorldNormalToFBO() {
	m_normalFBO.begin(); // Normal mapping begin
	m_normalFBO.clear();
	m_textureProgram.use();

	// Camera matrix
	glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
	GLint pUniform = m_textureProgram.getUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	GLint textureUniform = m_textureProgram.getUniformLocation("textureSampler");
	glUniform1i(textureUniform, 0);

	m_spriteBatch.begin(GLEngine::GlyphSortType::NONE);

	m_world->drawTilesNormal(m_spriteBatch, getScreenBox() + glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), &m_textureProgram);
	Factory::getEntityManager()->drawEntitiesNormal(m_spriteBatch, getScreenBox() + glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

	m_spriteBatch.end();
	m_spriteBatch.renderBatch();
	m_textureProgram.unuse();
	m_normalFBO.end(); // Normal mapping end.
}

void GameplayScreen::drawWorldSunlightToFBO() {
	m_sunlightFBO.begin(); // Normal mapping begin
	m_sunlightFBO.clear();
	m_sunlightProgram.use(); // Literally just writes the colour value.
	m_spriteBatch.begin();

	// Camera matrix
	glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
	GLint pUniform = m_sunlightProgram.getUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	m_world->drawSunlight(m_spriteBatch, getScreenBox() + glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)); // Should probably change screenBox to world coords. ///// NEVERMIND, It already does!

	m_spriteBatch.end();
	m_spriteBatch.renderBatch();
	m_sunlightProgram.unuse();
	m_sunlightFBO.end(); // Normal mapping end.
}

void GameplayScreen::drawParticlesToFBO() {
	m_mainFBO.begin();
	{
		// World: Particles
		m_textureProgram.use();

		// Camera matrix
		glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
		GLint pUniform = m_textureProgram.getUniformLocation("P");
		glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);
		GLint textureUniform = m_textureProgram.getUniformLocation("textureSampler");
		glUniform1i(textureUniform, 0);

		m_spriteBatch.begin();

		m_particle2d.draw(&m_spriteBatch, 0.0f);

		m_spriteBatch.end();
		m_spriteBatch.renderBatch();
		m_textureProgram.unuse();
	}
	m_mainFBO.end();
}

void GameplayScreen::drawGUIToScreen() {
	// GUI
	m_uiTextureProgram.use();

	GLint textureUniform = m_uiTextureProgram.getUniformLocation("textureSampler");
	glUniform1i(textureUniform, 0);
	glActiveTexture(GL_TEXTURE0);

	// Camera matrix
	glm::mat4 projectionMatrix = m_uiCamera.getCameraMatrix();
	GLint pUniform = m_uiTextureProgram.getUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	Factory::getEntityManager()->getPlayer()->drawGUI(m_spriteBatch, m_spriteFont, m_camera);

	Factory::getGUI()->draw();

	m_dr.end();
	m_dr.render(projectionMatrix, 3);

#ifdef DEV_CONTROLS
	if(m_debuggingInfo) {
		drawDebug();
	}
#endif // DEV_CONTROLS

	m_uiTextureProgram.unuse();
}

void GameplayScreen::drawPostToScreen() {
	// Post
	m_vignetteTextureProgram.use();

	// Camera matrix
	glm::mat4 projectionMatrix = m_uiCamera.getCameraMatrix();
	GLint pUniform = m_vignetteTextureProgram.getUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	GLint sizeUniform = m_vignetteTextureProgram.getUniformLocation("screenSizeU");
	glUniform2f(sizeUniform, m_window->getScreenWidth(), m_window->getScreenHeight());

	GLint sanityUniform = m_vignetteTextureProgram.getUniformLocation("sanity");
	glUniform1f(sanityUniform, Factory::getEntityManager()->getPlayer()->getSanity());

	GLint timeUniform = m_vignetteTextureProgram.getUniformLocation("time");
	glUniform1f(timeUniform, m_time);

	m_spriteBatch.begin();

	m_spriteBatch.draw(glm::vec4(0.0f, 0.0f, m_window->getScreenWidth(), m_window->getScreenHeight()), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 0, 0.0f, GLEngine::ColourRGBA8(0, 0, 0, 0));

	m_spriteBatch.end();
	//m_spriteBatch.renderBatch();

	m_vignetteTextureProgram.unuse();
}


/// GameplayScreen PRIVATE FUNCTIONS

void GameplayScreen::checkInput() {
	SDL_Event evnt;
	while(SDL_PollEvent(&evnt)) {
		if(!m_console->isShown()) m_game->onSDLEvent(evnt);
		Factory::getGUI()->onSDLEvent(evnt);
		switch(evnt.type) {
			case SDL_QUIT:
				m_currentState = GLEngine::ScreenState::EXIT_APPLICATION;
				break;
			case SDL_MOUSEBUTTONDOWN:
				break;
			case SDL_MOUSEWHEEL:
				/*if(!m_questManager->isDialogueActive() && !m_questManager->isTradingActive()) */
				updateScale();
				break;
		}
	}

	if(m_game->inputManager.isKeyPressed(SDLK_ESCAPE)) {
		if(m_gameState == GameState::PLAY) {
			pauseGame();
		} else if(m_gameState == GameState::PAUSE) {
			continueGame();
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
#endif // DEV_CONTROLS

	if(m_game->inputManager.isKeyPressed(SDLK_F4)) {
		//m_gameState = GameState::PAUSE;
		m_WorldIOManager->saveWorld(m_world);
	} else if(m_game->inputManager.isKeyPressed(SDLK_F5)) {
		m_WorldIOManager->loadWorld(m_world->getName(), m_world);
	}

}

void GameplayScreen::initShaders() {

	m_textureProgram.compileShaders(ASSETS_FOLDER_PATH + "Shaders/textureShader.vert", ASSETS_FOLDER_PATH + "Shaders/textureShader.frag");
	m_textureProgram.addAttribute("vertexPosition");
	m_textureProgram.addAttribute("vertexColour");
	m_textureProgram.addAttribute("vertexUV");
	m_textureProgram.linkShaders();

	glUniform1i(m_textureProgram.getUniformLocation("textureSampler"), 0); // set bump-map and regular texture samplers

	m_uiTextureProgram.compileShaders(ASSETS_FOLDER_PATH + "Shaders/textureShader.vert", ASSETS_FOLDER_PATH + "Shaders/textureShader.frag");
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

	m_basicFBOTextureProgram.compileShaders(ASSETS_FOLDER_PATH + "Shaders/superBasicShader.vert", ASSETS_FOLDER_PATH + "Shaders/superBasicShader.frag");
	m_basicFBOTextureProgram.addAttribute("vertexPosition");
	m_basicFBOTextureProgram.addAttribute("vertexColour");
	m_basicFBOTextureProgram.addAttribute("vertexUV");
	m_basicFBOTextureProgram.linkShaders();

	m_postProcessor.compileShaders(ASSETS_FOLDER_PATH + "Shaders/postProcesser.vert", ASSETS_FOLDER_PATH + "Shaders/postProcesser.frag");
	m_postProcessor.addAttribute("vertexPosition");
	m_postProcessor.addAttribute("vertexColour");
	m_postProcessor.addAttribute("vertexUV");
	m_postProcessor.linkShaders();

	m_sunlightProgram.compileShaders(ASSETS_FOLDER_PATH + "Shaders/colourShader.vert", ASSETS_FOLDER_PATH + "Shaders/colourShader.frag");
	m_sunlightProgram.addAttribute("vertexPosition");
	m_sunlightProgram.addAttribute("vertexColour");
	m_sunlightProgram.addAttribute("vertexUV");
	m_sunlightProgram.linkShaders();
}

void GameplayScreen::initUI() {
	{
		Factory::getGUI()->init(ASSETS_FOLDER_PATH + "GUI");
		Factory::getGUI()->loadScheme("FOTDSkin.scheme");

		Factory::getGUI()->setFont("Amatic-26");

		Factory::getGUI()->setMouseCursor("FOTDSkin/MouseArrow");
		Factory::getGUI()->showMouseCursor();
		SDL_ShowCursor(0);
	}

	// Register custom objects
	{
		CEGUI::WindowFactoryManager::addFactory<CEGUI::TplWindowFactory<CEGUI::GUI_InventoryReceiver>>();
		CEGUI::WindowFactoryManager::addFactory<CEGUI::TplWindowFactory<CEGUI::GUI_InventoryItem>>();
		CEGUI::WindowRendererManager::addFactory<CEGUI::TplWindowRendererFactory<CEGUI::GUI_InventoryItemRenderer>>();
	}

	{
		// Pause screen
		CEGUI::PushButton* resumeButton = static_cast<CEGUI::PushButton*>(Factory::getGUI()->createWidget("FOTDSkin/Button", glm::vec4(0.3f, 0.3f, 0.4f, 0.1f), glm::vec4(0.0f), "PAUSE_RESUME_BUTTON"));
		resumeButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameplayScreen::pause_resume_button_clicked, this));
		resumeButton->setText("Resume Game");
		resumeButton->disable();
		resumeButton->hide();
		m_pauseWidgets.push_back(static_cast<CEGUI::Window*>(resumeButton));

		CEGUI::PushButton* saveButton = static_cast<CEGUI::PushButton*>(Factory::getGUI()->createWidget("FOTDSkin/Button", glm::vec4(0.3f, 0.45f, 0.4f, 0.1f), glm::vec4(0.0f), "PAUSE_SAVE_BUTTON"));
		saveButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameplayScreen::pause_save_button_clicked, this));
		saveButton->setText("Save Game");
		saveButton->disable();
		saveButton->hide();
		m_pauseWidgets.push_back(static_cast<CEGUI::Window*>(saveButton));

		CEGUI::PushButton* quitButton = static_cast<CEGUI::PushButton*>(Factory::getGUI()->createWidget("FOTDSkin/Button", glm::vec4(0.3f, 0.6f, 0.4f, 0.1f), glm::vec4(0.0f), "PAUSE_QUIT_BUTTON"));
		quitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameplayScreen::pause_quit_button_clicked, this));
		quitButton->setText("Save & Quit Game");
		quitButton->disable();
		quitButton->hide();
		m_pauseWidgets.push_back(static_cast<CEGUI::Window*>(quitButton));
	}

#ifdef DEV_CONTROLS
	{
		m_fpsWidget = static_cast<CEGUI::DefaultWindow*>(Factory::getGUI()->createWidget("FOTDSkin/Label", glm::vec4(0.05f, 0.05f, 0.9f, 0.9f), glm::vec4(0.0f), "FPS_STRING_WIDGET"));
		m_fpsWidget->setHorizontalAlignment(CEGUI::HorizontalAlignment::HA_LEFT);
		m_fpsWidget->setVerticalAlignment(CEGUI::VerticalAlignment::VA_TOP);
	}
#endif //DEV_CONTROLS
}

void GameplayScreen::tick() {
	// Happens not every second, nor each frame, but somewhere in between.
	m_world->tickTiles(getScreenBox() + glm::vec4(-20.0f, -20.0f, 40.0f, 40.0f));
	Factory::getEntityManager()->tickEntities();

	if(!m_audio->isMusicPlaying()) {
		float hour = (float)((int)m_time % DAY_LENGTH) / (float)DAY_LENGTH + 0.5f;//(int)((m_time / TICK_RATE) + 12) % DAY_LENGTH;

		int randNum = std::rand() % 100;

		if(hour > 4.0f / 24.0f && hour < 9.0f / 24.0f) { // Morning (4am-9am)
			if(MORNING_MUSIC_LENGTH > 0)
				m_audio->playMorningSong(randNum % MORNING_MUSIC_LENGTH);
		} else if(hour > 9.0f / 24.0f && hour < 16.0f / 24.0f) { // Day (9am-4pm)
			if(DAY_MUSIC_LENGTH > 0)
				m_audio->playDaySong(randNum % DAY_MUSIC_LENGTH);
		} else if(hour > 16.0f / 24.0f && hour < 21.0f / 24.0f) { // Afternoon (4pm-9pm)
			if(AFTERNOON_MUSIC_LENGTH > 0)
				m_audio->playAfternoonSong(randNum % AFTERNOON_MUSIC_LENGTH);
		} else if(hour > 21.0f / 24.0f || hour < 4.0f / 24.0f) { // Night (9pm-4am) (Overlap requires OR logic)
			if(hour >= 0.0f && hour <= 1.0f / 24.0f) { // Nightmare hour (Midnight-1am)
				if(NIGHT_MUSIC_LENGTH > 0)
					m_audio->playNightmareSong(randNum % NIGHTMARE_MUSIC_LENGTH);
			} else {
				if(NIGHTMARE_MUSIC_LENGTH > 0)
					m_audio->playNightSong(randNum % NIGHT_MUSIC_LENGTH);
			}
		}
	}
}

void GameplayScreen::updateScale() {
	m_scale += (float)m_game->inputManager.getMouseScrollPosition() * m_scale / 20.0f;
	if(m_scale < MIN_ZOOM) {
		m_scale = MIN_ZOOM;
	} else if(m_scale > MAX_ZOOM) {
		m_scale = MAX_ZOOM;
	}
}

#include "ExtraFunctions.h"

#ifdef DEV_CONTROLS

void GameplayScreen::drawDebug() {
	if(Factory::getEntityManager()->getPlayer()->getSelectedBlock()) {
		EntityPlayer* p = Factory::getEntityManager()->getPlayer();

		int blockX, blockY;
		blockX = p->getSelectedBlock()->getPosition().x;
		blockY = p->getSelectedBlock()->getPosition().y;

		std::string biomeString = m_world->getBiome(p->getSelectedBlock()->getPosition().x).name;

		std::string display = "FPS: " + std::to_string((int)m_game->getFps());
		display += "\nMouse x,y: " + std::to_string(blockX) + "," + std::to_string(blockY);
		display += "\nSelected Block: Biome: " + biomeString + ", " + p->getSelectedBlock()->getPrintout(m_world);

		m_fpsWidget->setText(display);
	}
}

#endif //DEV_CONTROLS

void GameplayScreen::pauseGame() {
	m_gameState = GameState::PAUSE;
	for(unsigned int i = 0; i < m_pauseWidgets.size(); i++) {
		m_pauseWidgets[i]->show();
		m_pauseWidgets[i]->enable();
		m_pauseWidgets[i]->activate();
	}
}

void GameplayScreen::continueGame() {
	m_gameState = GameState::PLAY;
	for(unsigned int i = 0; i < m_pauseWidgets.size(); i++) {
		m_pauseWidgets[i]->hide();
		m_pauseWidgets[i]->disable();
		m_pauseWidgets[i]->deactivate();
	}
}

bool GameplayScreen::pause_resume_button_clicked(const CEGUI::EventArgs& e) {
	continueGame();
	return true;
}

bool GameplayScreen::pause_save_button_clicked(const CEGUI::EventArgs& e) {
	m_WorldIOManager->saveWorld(m_world);
	return true;
}

bool GameplayScreen::pause_quit_button_clicked(const CEGUI::EventArgs& e) {
	pause_save_button_clicked(e);
	m_nextScreenIndex = SCREEN_INDEX_MAINMENU;
	m_currentState = GLEngine::ScreenState::CHANGE_NEXT;
	return true;
}

glm::vec4 GameplayScreen::getScreenBox() {
	// Window coordinates
	glm::vec2 topLeft(0.0f, 0.0f);
	glm::vec2 bottomRight(m_window->getScreenWidth(), m_window->getScreenHeight());

	glm::vec2 gameplayCoordsTL = m_camera.convertScreenToWorld(topLeft);
	glm::vec2 gameplayCoordsBR = m_camera.convertScreenToWorld(bottomRight);

	return glm::vec4(gameplayCoordsTL.x, gameplayCoordsBR.y, gameplayCoordsBR.x - gameplayCoordsTL.x, gameplayCoordsTL.y - gameplayCoordsBR.y) + glm::vec4(-1.5f, -1.5f, 1.0f, 1.0f);
}
