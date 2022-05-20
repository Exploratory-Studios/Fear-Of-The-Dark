#include "GameplayScreen.h"

#include <SDL2/SDL_timer.h>

#include <GLContextManager.hpp>

#include "GUI_InventoryReceiver.h"
#include "GUI_InventoryItem.h"
#include "GUI_InventoryItemRenderer.h"

#include "World.h"
#include "Tile.h"
#include "EntityNPC.h"
#include "EntityPlayer.h"
#include "FluidDomain.h"

#include "NPCInventory.h"

#include "Singletons.h"

#ifdef DEV_CONTROLS
#include <iostream>
#include <regex>
#endif //DEV_CONTROLS

GameplayScreen::GameplayScreen(BARE2D::Window* window, WorldIOManager* WorldIOManager, BARE2D::InputManager* input) :
	m_window(window), m_WorldIOManager(WorldIOManager), m_inputManager(input) {
}

GameplayScreen::~GameplayScreen() {
}

int GameplayScreen::getNextScreenIndex() const {
	return m_nextScreenIndex;
}

void GameplayScreen::initScreen() {
}

void GameplayScreen::destroyScreen() {
}

void GameplayScreen::onEntry() {
	initUI();

	std::srand(std::time(NULL));

	m_dr = new BARE2D::DebugRenderer();
	m_dr->init();

	std::string postFS	   = "Shaders/postProcesser.frag";
	std::string postVS	   = "Shaders/postProcesser.vert";
	std::string textureFS  = "Shaders/textureShader.frag";
	std::string textureVS  = "Shaders/textureShader.vert";
	std::string skyFS	   = "Shaders/skyShader.frag";
	std::string skyVS	   = "Shaders/skyShader.vert";
	std::string basicFS	   = "Shaders/superBasicShader.frag";
	std::string basicVS	   = "Shaders/superBasicShader.vert";
	std::string sunlightFS = "Shaders/sunlightShader.frag";
	std::string sunlightVS = "Shaders/sunlightShader.vert";
	std::string fluidFS	   = "Shaders/fluidShader.frag";
	std::string fluidVS	   = "Shaders/fluidShader.vert";

	m_fontRenderer = new BARE2D::FontRenderer(basicFS, basicVS);
	m_fontRenderer->init();

	m_mainFBO = new BARE2D::FBORenderer(
	    postFS,
	    postVS,
	    m_window->getWidth(),
	    m_window->getHeight(),
	    glm::vec2(m_window->getWidth(), m_window->getHeight()),
	    3); // The main FBO has 3 colour attachments - one for colour, one for normals, and one for sunlight
	m_mainFBO->init();

	m_basicRenderer = new BARE2D::BasicRenderer(basicFS, basicVS, m_window->getWidth(), m_window->getHeight());
	m_basicRenderer->init();

	m_worldRenderer = new BARE2D::BumpyRenderer(textureFS, textureVS, m_window->getWidth(), m_window->getHeight());
	m_worldRenderer->init();

	m_skyRenderer = new BARE2D::BasicRenderer(skyFS, skyVS, m_window->getWidth(), m_window->getHeight());
	m_skyRenderer->init();
	m_skyRenderer->getCamera()->setPosition(glm::vec2(-m_window->getWidth()/2.0f, -m_window->getHeight()/2.0f));

	m_sunlightRenderer = new BARE2D::BasicRenderer(sunlightFS, sunlightVS, m_window->getWidth(), m_window->getHeight());
	m_sunlightRenderer->init();

	m_fluidRenderer = new BARE2D::BasicRenderer(fluidFS, fluidVS, m_window->getWidth(), m_window->getHeight());
	m_fluidRenderer->init();

	// Link all the cameras together.
	std::shared_ptr<BARE2D::Camera2D> gameCamera = m_basicRenderer->getCamera();
	m_worldRenderer->setCamera(gameCamera);
	m_sunlightRenderer->setCamera(gameCamera);
	m_fluidRenderer->setCamera(gameCamera);

	m_scale = INITIAL_ZOOM;
	Singletons::setGameCamera(gameCamera.get());
	Singletons::getGameCamera()->setScale(m_scale, m_scale);
	Singletons::getGameCamera()->init(m_window->getWidth(), m_window->getHeight());
	Singletons::getGameCamera()->setPosition((glm::vec2(m_window->getWidth() / 2, m_window->getHeight() / 2)));
	Singletons::getGameCamera()->update();

	m_audio = new GameAudioManager();
	m_audio->init();
	/// TODO: Options set sound levels.

	m_console	   = new Console();
	m_questManager = new QuestModule::QuestManager();

	if(!Singletons::getEntityManager()->getPlayer()) {
		//Player p(glm::vec2(5.0f, 100.0f), true);
		bool	  found = false;
		glm::vec2 pos;
		pos.x = 5;
		pos.y = 0;
		while(!found) {
			if(!(Singletons::getWorld()->getTile(pos.x, pos.y, 0)->isSolid())) {
				found = true;
			}
			pos.y++;
			if(pos.y >= WORLD_HEIGHT - 5) {
				pos.x++;
				pos.y = 0;
			}
		}

		EntityPlayer* p = new EntityPlayer(pos, 0);

		Singletons::getEntityManager()->setPlayer(p);
		Singletons::getEntityManager()->getPlayer()->initGUI();
	}

	m_console->init(m_questManager, this);

	m_dialogueManager = new DialogueModule::DialogueManager(m_questManager);

	tick();
}

void GameplayScreen::onExit() {
	delete m_questManager;
	delete m_dialogueManager;
	delete m_console;

	m_fontRenderer->destroy();
	m_dr->destroy();

	m_gameState = GameState::PLAY;

	Singletons::destroyGUI();
}

void GameplayScreen::update(double dt) {
	Singletons::getWorld()->incrementTime();
	m_particle2d.update(dt);

	checkInput();

	if(Singletons::getWorld()->getNextEra() != Singletons::getWorld()->getEra()) {
		m_WorldIOManager->setWorldEra(Singletons::getWorld()->getNextEra());
		/// TODO: Move to loading screen for this.
	}

	if(m_gameState != GameState::PAUSE && m_screenState != BARE2D::ScreenState::EXIT_APPLICATION) {
		m_questManager->update();

		// Set player caninteract

		if(Singletons::getEntityManager()->getPlayer() && !m_cutscenePause) {
			Singletons::getEntityManager()->getPlayer()->updateMouse(
			    Singletons::getGameCamera()->getViewedPositionFromScreenPosition(m_inputManager->getMousePosition()));
			Singletons::getEntityManager()->getPlayer()->updateInput(m_inputManager);
			/// TODO: Re-enable this -> m_world->getPlayer()->setCanInteract(!m_questManager->isDialogueActive());
		}

		glm::vec4 screenRect = getScreenBox();
		Singletons::getWorld()->updateTiles(screenRect);
		glm::vec4 regScreenRect = getScreenBox();
		Singletons::getWorld()->updateFluids(1.0f / 600.0f * dt, regScreenRect);
		Singletons::getEntityManager()->updateEntities(dt); /// TODO: Use timestep

		unsigned int worldSize = Singletons::getWorld()->getSize();

		if(!m_cameraLocked) {
			EntityPlayer* player = Singletons::getEntityManager()->getPlayer();

			if(std::abs((player->getPosition().x) - m_lastPlayerPos.x) >= (worldSize / 2)) {
				int sign = ((player->getPosition().x + player->getSize().x / 2.0f) - m_lastPlayerPos.x) /
				           std::abs((player->getPosition().x + player->getSize().x / 2.0f) - m_lastPlayerPos.x);
				m_lastPlayerPos.x += (float)(worldSize)*sign;
				Singletons::getGameCamera()->setPosition(Singletons::getGameCamera()->getPosition() +
				        glm::vec2((float)(worldSize)*sign, 0.0f));
			}
			m_lastPlayerPos =
			    (m_lastPlayerPos +
			     ((player->getPosition() + player->getSize() / glm::vec2(2.0f)) - m_lastPlayerPos) / glm::vec2(4.0f));
			Singletons::getGameCamera()->setPosition(
			    m_lastPlayerPos); // If lastplayerpos is never updated, the camera is still 'locked' per say, but we can actually change the lastPlayerPos on purpose to get a smooth movement somewhere.
		} else {
			m_lastPlayerPos = (m_lastPlayerPos + (m_smoothMoveTarget - m_lastPlayerPos) * m_smoothMoveSpeed);
			Singletons::getGameCamera()->setPosition(m_lastPlayerPos);
		}

		if((int)Singletons::getGameCamera()->getPosition().x > worldSize) {
			Singletons::getGameCamera()->setPosition(Singletons::getGameCamera()->getPosition() -
			        glm::vec2((float)(worldSize), 0.0f));
		} else if((int)Singletons::getGameCamera()->getPosition().x < 0) {
			Singletons::getGameCamera()->setPosition(Singletons::getGameCamera()->getPosition() +
			        glm::vec2((float)(worldSize), 0.0f));
		}

		m_time++; /// Change the increment if time is slowed or quicker (potion effects?)

		if((int)m_frame % (int)(60 / TICK_RATE) == 0) { // m_frame is equal to current frame
			tick();
		}

		m_frame++;
	}
	if(m_screenState != BARE2D::ScreenState::EXIT_APPLICATION)
		Singletons::getGUI()->update();
}

void GameplayScreen::draw() {
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	{
		// Sky
		//drawSunMoon();
	}

	drawSky();

	/*m_mainFBO->begin();

	glm::vec2 screenSizeUniform = glm::vec2(m_window->getWidth(), m_window->getHeight());
	m_mainFBO->getShader()->setUniform("screenSizeU", &screenSizeUniform);

	float sanityUniform = Singletons::getEntityManager()->getPlayer()->getSanity();
	m_mainFBO->getShader()->setUniform("sanity", &sanityUniform);

	m_mainFBO->getShader()->setUniform("time", &m_time);

	float playerDepth =
	    0.1f + (Singletons::getEntityManager()->getPlayer()->getLayer() * (1.0f / (float)(WORLD_DEPTH)) * 0.9f);
	m_mainFBO->getShader()->setUniform("playerDepth", &playerDepth);

	{
		/// FLOATING POINT ERROR IN DRAWWORLD
		//drawWorld();
		//drawWorldSunlight();
		drawParticles();

		/// TODO: Instead of having a bunch of different FBOs, have the master FBO have 3 colour attachments & a depth one.
		/// One for regular colouring, one for the normal mapping, and one for sunlight.
		/// Each shader can specify which attachment it is writing to - through clever use of 'out vec4 colour{0,1,2,3,4,5,6,7,8}'
		// Depth is already automatically the last attachment (at GL_TEXTURE1 for this FBO with one attachment)
		// Sunlight
		//BARE2D::GLContextManager::getContext()->setActiveTexture(GL_TEXTURE2);
		//BARE2D::GLContextManager::getContext()->bindTexture(GL_TEXTURE_2D, m_sunlightFBO->getTexture());

		// Normal Map.
		//BARE2D::GLContextManager::getContext()->setActiveTexture(GL_TEXTURE3);
		//BARE2D::GLContextManager::getContext()->bindTexture(GL_TEXTURE_2D, m_normalFBO->getTexture());

		//Singletons::getWorld()->setLightsUniform(getScreenBox() + glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
		//										 m_mainFBO->getShader()); // sets "lights" uniform of vec3s
		/// TODO: put other calls in here to draw to main FBO.
	}

	//drawFluids();

	m_mainFBO->end();
	m_mainFBO->render();

	//drawGUI();*/
}

void GameplayScreen::drawSky() {
	float dayLight = cos((float)Singletons::getWorld()->getTime() / (DAY_LENGTH / 6.28318f)) / 2.0f + 0.5f;

	{
		m_skyRenderer->begin();

		glClearColor(0.3f * dayLight, 0.4f * dayLight, 1.0f * dayLight, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::vec2 windowSizeUniform = glm::vec2(m_window->getWidth(), m_window->getHeight());
		m_skyRenderer->getShader()->setUniform("screenSizeU", &windowSizeUniform);

		float dayLightUniform = dayLight * 0.5f + 0.5f;
		m_skyRenderer->getShader()->setUniform("daylight", &dayLightUniform);

		float playerChunkX =
		    std::fmod((Singletons::getEntityManager()->getPlayer()->getPosition().x + (float)CHUNK_SIZE),
		              (float)CHUNK_SIZE);

		float playerXPosUniform = playerChunkX / (float)(CHUNK_SIZE);
		m_skyRenderer->getShader()->setUniform("playerXPos", &playerXPosUniform);

		float parallaxZoomUniform = 0.95f;
		m_skyRenderer->getShader()->setUniform("parallaxZoom", &parallaxZoomUniform);

		int playerX = (int)Singletons::getEntityManager()->getPlayer()->getPosition().x;

		GLuint backgroundID = Singletons::getWorld()->getBiome(playerX).backgroundTexture.id;

		m_skyRenderer->draw(glm::vec4(0.0, 0.0, windowSizeUniform.x, windowSizeUniform.y),
		                    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
		                    backgroundID,
		                    1.0f,
		                    BARE2D::Colour(255, 255, 255, 255));

		m_skyRenderer->end();
		m_skyRenderer->render();
	}
}

void GameplayScreen::drawSunMoon() {
	m_basicRenderer->begin();

	float sunPercentX, sunPercentY; // 0.45, 0.8f

	sunPercentX =
	    0.45f * std::cos(3.1415f * 2.0f * ((float)Singletons::getWorld()->getTime() / DAY_LENGTH) + 3.1415f * 0.5f);
	sunPercentY =
	    0.8f * std::sin(3.1415f * 2.0f * ((float)Singletons::getWorld()->getTime() / DAY_LENGTH) + 3.1415f * 0.5f);

	std::string sunPath = "sun.png";

	GLuint sunID = BARE2D::ResourceManager::loadTexture(sunPath).id;

	m_basicRenderer->draw(glm::vec4(m_window->getWidth() * (0.5f - sunPercentX) - 16.0f,
	                                (sunPercentY)*m_window->getHeight(),
	                                32.0f,
	                                32.0f),
	                      glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
	                      sunID,
	                      1.0f,
	                      BARE2D::Colour(255, 255, 255, 255));

	std::string moonPath = "UNDEFINED.png";

	GLuint moonID = BARE2D::ResourceManager::loadTexture(moonPath).id;

	m_basicRenderer->draw(glm::vec4(m_window->getWidth() * (0.5f + sunPercentX) - 16.0f,
	                                -(sunPercentY)*m_window->getHeight(),
	                                32.0f,
	                                32.0f),
	                      glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
	                      moonID,
	                      1.0f,
	                      BARE2D::Colour(0, 0, 255, 255));

	m_basicRenderer->end();
	m_basicRenderer->render();
}

void GameplayScreen::drawWorld() {
	glm::vec4 screenRect = getScreenBox();

	{
		{
			// World: Tiles

			m_worldRenderer->begin();

			Singletons::getWorld()->drawTiles(m_worldRenderer, m_fontRenderer, m_dr, screenRect);
			Singletons::getEntityManager()->drawEntities(m_worldRenderer, m_fontRenderer, m_dr, screenRect);

			m_worldRenderer->end();
			m_worldRenderer->render();
		}
	}
}

void GameplayScreen::drawWorldSunlight() {
	m_sunlightRenderer->begin();

	Singletons::getWorld()->drawSunlight(m_sunlightRenderer, getScreenBox());

	m_sunlightRenderer->end();
	m_sunlightRenderer->render();
}

void GameplayScreen::drawParticles() {
	{
		// World: Particles
		/// TODO: Particles
		//m_particle2d.draw(&m_renderer, 0.0f);
	}
}

void GameplayScreen::drawFluids() {
	glm::vec4 screenRect = getScreenBox();
	Singletons::getWorld()->updateFluidTextures(screenRect);

	m_fluidRenderer->begin();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Singletons::getWorld()->drawFluids(m_fluidRenderer, screenRect);

	m_fluidRenderer->end();
	m_fluidRenderer->render();
}

void GameplayScreen::drawGUI() {
	{
		/// CEGUI Inventory GUI (Context 1)

		Singletons::getGUI()->setActiveContext(1);
		Singletons::getGUI()->draw();
		Singletons::getGUI()->setActiveContext(0);
	}

	{
		/// Inventory Overlays

		Singletons::getEntityManager()->getPlayer()->drawGUI(m_basicRenderer, m_fontRenderer);
		glm::vec4 screenRect = getScreenBox();

		Singletons::getWorld()->drawTilesGUI(m_basicRenderer, m_fontRenderer, screenRect);

		m_worldRenderer->begin();

		if(Singletons::getEntityManager()->getPlayer()->isInventoryOpen())
			Singletons::getEntityManager()->getPlayer()->display(
			    m_worldRenderer,
			    glm::vec2(0.2f, 0.36f) * glm::vec2(Singletons::getGameCamera()->getScreenWidth(),
			                                       Singletons::getGameCamera()->getScreenHeight()),
			    80.0f);

		m_worldRenderer->end();
		m_worldRenderer->render();
	}

	{
		// General GUI (Context 0)

		m_dr->begin();

		Singletons::getGUI()->draw();

#ifdef DEV_CONTROLS
		if(m_debuggingInfo) {
			drawDebug();
		}
#endif // DEV_CONTROLS

		m_dr->end();
		m_dr->render();
	}
}

/// GameplayScreen PRIVATE FUNCTIONS

void GameplayScreen::checkInput() {
	if(m_inputManager->isKeyPressed(SDLK_ESCAPE)) {
		if(m_gameState == GameState::PAUSE) {
			continueGame();
		} else {
			pauseGame();
		}
	}

#ifdef DEV_CONTROLS
	if(m_inputManager->isKeyPressed(SDLK_BACKSLASH)) {
		m_console->show();
	}
	if(m_inputManager->isKeyPressed(SDLK_F1)) {
		m_debuggingInfo = !m_debuggingInfo;
		m_fpsWidget->setVisible(m_debuggingInfo);
	}

	if(m_inputManager->isKeyPressed(SDLK_g)) {
		m_debugBool = !m_debugBool;
	}
#endif // DEV_CONTROLS

	if(m_inputManager->isKeyPressed(SDLK_F4)) {
		//m_gameState = GameState::PAUSE;
		m_WorldIOManager->saveWorld();
	} else if(m_inputManager->isKeyPressed(SDLK_F5)) {
		m_WorldIOManager->loadWorld(Singletons::getWorld()->getName());
	}

	updateScale();
}

void GameplayScreen::initUI() {
	{
		std::string guiPath = ASSETS_FOLDER_PATH + "GUI";
		Singletons::getGUI()->init(guiPath, 2);
		/**
		 * .................. GUI CONTEXTS ...................
		 *
		 * #1.....................Basic, overtop of everything
		 *
		 * #2....................Inventory Systems,
		 * ................underneath world->drawTilesUI calls
		 *
		 * . . . Continue, every index behind the next
		 *
		 *
		**/

		Singletons::getGUI()->loadScheme("FOTDSkin.scheme");

		Singletons::getGUI()->setFont("Amatic-26");

		Singletons::getGUI()->setMouseCursor("FOTDSkin/MouseArrow");
		Singletons::getGUI()->setMouseCursorShown(true);
		SDL_ShowCursor(0);
	}

	// Register custom objects
	{
		// Only register if it's not already registered
		CEGUI::WindowFactoryManager* singleton = CEGUI::Singleton<CEGUI::WindowFactoryManager>::getSingletonPtr();
		if(!singleton->isFactoryPresent((CEGUI::TplWindowFactory<CEGUI::GUI_InventoryReceiver>()).getTypeName())) {
			CEGUI::WindowFactoryManager::addFactory<CEGUI::TplWindowFactory<CEGUI::GUI_InventoryReceiver>>();
			CEGUI::WindowFactoryManager::addFactory<CEGUI::TplWindowFactory<CEGUI::GUI_InventoryItem>>();
			CEGUI::WindowRendererManager::addFactory<CEGUI::TplWindowRendererFactory<CEGUI::GUI_InventoryItemRenderer>>();
		}
	}

	{
		// Pause screen
		CEGUI::PushButton* resumeButton =
		    static_cast<CEGUI::PushButton*>(Singletons::getGUI()->createWidget("FOTDSkin/Button",
		                                    glm::vec4(0.3f, 0.3f, 0.4f, 0.1f),
		                                    glm::vec4(0.0f),
		                                    nullptr,
		                                    "PAUSE_RESUME_BUTTON"));
		resumeButton->subscribeEvent(CEGUI::PushButton::EventClicked,
		                             CEGUI::Event::Subscriber(&GameplayScreen::pause_resume_button_clicked, this));
		resumeButton->setText("Resume Game");
		resumeButton->disable();
		resumeButton->hide();
		m_pauseWidgets.push_back(static_cast<CEGUI::Window*>(resumeButton));

		CEGUI::PushButton* saveButton =
		    static_cast<CEGUI::PushButton*>(Singletons::getGUI()->createWidget("FOTDSkin/Button",
		                                    glm::vec4(0.3f, 0.45f, 0.4f, 0.1f),
		                                    glm::vec4(0.0f),
		                                    nullptr,
		                                    "PAUSE_SAVE_BUTTON"));
		saveButton->subscribeEvent(CEGUI::PushButton::EventClicked,
		                           CEGUI::Event::Subscriber(&GameplayScreen::pause_save_button_clicked, this));
		saveButton->setText("Save Game");
		saveButton->disable();
		saveButton->hide();
		m_pauseWidgets.push_back(static_cast<CEGUI::Window*>(saveButton));

		CEGUI::PushButton* quitButton =
		    static_cast<CEGUI::PushButton*>(Singletons::getGUI()->createWidget("FOTDSkin/Button",
		                                    glm::vec4(0.3f, 0.6f, 0.4f, 0.1f),
		                                    glm::vec4(0.0f),
		                                    nullptr,
		                                    "PAUSE_QUIT_BUTTON"));
		quitButton->subscribeEvent(CEGUI::PushButton::EventClicked,
		                           CEGUI::Event::Subscriber(&GameplayScreen::pause_quit_button_clicked, this));
		quitButton->setText("Save & Quit Game");
		quitButton->disable();
		quitButton->hide();
		m_pauseWidgets.push_back(static_cast<CEGUI::Window*>(quitButton));
	}

#ifdef DEV_CONTROLS
	{
		m_fpsWidget =
		    static_cast<CEGUI::DefaultWindow*>(Singletons::getGUI()->createWidget("FOTDSkin/Label",
		                                       glm::vec4(0.05f, 0.05f, 0.9f, 0.9f),
		                                       glm::vec4(0.0f),
		                                       nullptr,
		                                       "FPS_STRING_WIDGET"));
		m_fpsWidget->setHorizontalAlignment(CEGUI::HorizontalAlignment::HA_LEFT);
		m_fpsWidget->setVerticalAlignment(CEGUI::VerticalAlignment::VA_TOP);
	}
#endif //DEV_CONTROLS
}

void GameplayScreen::tick() {
	// Happens not every second, nor each frame, but somewhere in between.
	Singletons::getWorld()->tickTiles(getScreenBox() + glm::vec4(-20.0f, -20.0f, 40.0f, 40.0f));
	Singletons::getEntityManager()->tickEntities();

	if(!BARE2D::AudioManager::getInstance()->isMusicPlaying()) {
		float hour = (float)((int)m_time % DAY_LENGTH) / (float)DAY_LENGTH +
		             0.5f; //(int)((m_time / TICK_RATE) + 12) % DAY_LENGTH;

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
			if(hour >= 0.0f && hour <= 1.0f / 24.0f) {			 // Nightmare hour (Midnight-1am)
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
	float delta = (float)m_inputManager->getMouseScrollwheelPosition() * m_scale / 20.0f;
	if(delta != 0.0f) {
		m_scale += delta;
		if(m_scale < MIN_ZOOM) {
			m_scale = MIN_ZOOM;
		} else if(m_scale > MAX_ZOOM) {
			m_scale = MAX_ZOOM;
		}
		Singletons::getGameCamera()->setScale(m_scale, m_scale);
	}
}

#include "ExtraFunctions.h"

#ifdef DEV_CONTROLS

void GameplayScreen::drawDebug() {
	//if(Singletons::getEntityManager()->getPlayer()->getSelectedBlock()) {
	EntityPlayer* p = Singletons::getEntityManager()->getPlayer();

	/*int blockX, blockY;
	blockX = p->getSelectedBlock()->getPosition().x;
	blockY = p->getSelectedBlock()->getPosition().y;

	std::string biomeString = Singletons::getWorld()->getBiome(p->getSelectedBlock()->getPosition().x).name;*/

	/// TODO: FPS readout
	std::string display = "";
	if(p->getSelectedBlock())
		display += "\nMouse x,y: " + std::to_string(p->getSelectedBlock()->getPosition().x) + "," +
		           std::to_string(p->getSelectedBlock()->getPosition().y);
	//display += "\nSelected Block: Biome: " + biomeString + ", " + p->getSelectedBlock()->getPrintout();

	m_fpsWidget->setText(display);
	//}
}

#endif //DEV_CONTROLS

void GameplayScreen::pauseGame() {
	m_lastGameState = m_gameState;
	m_gameState		= GameState::PAUSE;
	for(unsigned int i = 0; i < m_pauseWidgets.size(); i++) {
		m_pauseWidgets[i]->show();
		m_pauseWidgets[i]->enable();
		m_pauseWidgets[i]->activate();
	}
}

void GameplayScreen::continueGame() {
	m_gameState = m_lastGameState;
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
	m_WorldIOManager->saveWorld();
	return true;
}

bool GameplayScreen::pause_quit_button_clicked(const CEGUI::EventArgs& e) {
	pause_save_button_clicked(e);
	m_nextScreenIndex = SCREEN_INDEX_MAINMENU;
	m_screenState	  = BARE2D::ScreenState::CHANGE_NEXT;
	return true;
}

glm::vec4 GameplayScreen::getScreenBox() {
	// Window coordinates
	glm::vec2 topLeft(0.0f, m_window->getHeight());
	glm::vec2 bottomRight(m_window->getWidth(), 0.0f);//m_window->getWidth(), m_window->getHeight());

	glm::vec2 gameplayCoordsTL = Singletons::getGameCamera()->getViewedPositionFromScreenPosition(topLeft);
	glm::vec2 gameplayCoordsBR = Singletons::getGameCamera()->getViewedPositionFromScreenPosition(bottomRight);

	glm::vec4 ret = glm::vec4(gameplayCoordsTL.x,
	                          gameplayCoordsBR.y,
	                          gameplayCoordsBR.x - gameplayCoordsTL.x,
	                          gameplayCoordsTL.y - gameplayCoordsBR.y) +
	                glm::vec4(-1.5f, -1.5f, 3.0f, 3.0f);

	//if(std::abs(ret.x) > 10000 || std::abs(ret.y) > 10000) {
	//Singletons::getGameCamera()->getViewedPositionFromScreenPosition(bottomRight);
	BARE2D::Logger::getInstance()->log("S");
	BARE2D::Logger::getInstance()->log(std::to_string(ret.x));
	BARE2D::Logger::getInstance()->log(std::to_string(ret.y));
	BARE2D::Logger::getInstance()->log(std::to_string(ret.z));
	BARE2D::Logger::getInstance()->log(std::to_string(ret.w));
	//}

	return ret;
}
