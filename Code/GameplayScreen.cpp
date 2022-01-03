#include "GameplayScreen.h"

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

#include <SDL2/SDL_timer.h>

#ifdef DEV_CONTROLS
#include <iostream>
#include <regex>
#endif //DEV_CONTROLS

GameplayScreen::GameplayScreen(BARE2D::Window* window, WorldIOManager* WorldIOManager, BARE2D::InputManager* input) :
	m_window(window), m_WorldIOManager(WorldIOManager), m_inputManager(input)
{
}

GameplayScreen::~GameplayScreen()
{
}

int GameplayScreen::getNextScreenIndex() const
{
	return m_nextScreenIndex;
}

void GameplayScreen::initScreen()
{
}

void GameplayScreen::destroyScreen()
{
}

void GameplayScreen::onEntry()
{
	initUI();

	std::srand(std::time(NULL));

	m_dr = new BARE2D::DebugRenderer();
	m_dr->init();

	std::string textureFragShader = ASSETS_FOLDER_PATH + "Shaders/textureShader.frag";
	std::string textureVertShader = ASSETS_FOLDER_PATH + "Shaders/textureShader.vert";

	m_renderer = new BARE2D::BumpyRenderer(textureFragShader, textureVertShader);
	m_renderer->init();

	m_fontRenderer = new BARE2D::FontRenderer(textureFragShader, textureVertShader);
	m_fontRenderer->init();

	std::string mainFS = ASSETS_FOLDER_PATH + "Shaders/postProcesser.frag";
	std::string mainVS = ASSETS_FOLDER_PATH + "Shaders/postProcesser.vert";

	m_mainFBO = new BARE2D::FBORenderer(mainFS,
	                                    mainVS,
	                                    m_window->getWidth(),
	                                    m_window->getHeight(),
	                                    glm::vec2(m_window->getWidth(), m_window->getHeight()));
	m_mainFBO->init();

	m_normalFBO = new BARE2D::FBORenderer(textureFragShader,
	                                      textureVertShader,
	                                      m_window->getWidth(),
	                                      m_window->getHeight(),
	                                      glm::vec2(m_window->getWidth(), m_window->getHeight()));
	m_normalFBO->init();

	std::string skyFS = ASSETS_FOLDER_PATH + "Shaders/skyShader.frag";
	std::string skyVS = ASSETS_FOLDER_PATH + "Shaders/skyShader.vert";

	m_skyFBO = new BARE2D::FBORenderer(skyFS,
	                                   skyVS,
	                                   m_window->getWidth(),
	                                   m_window->getHeight(),
	                                   glm::vec2(m_window->getWidth(), m_window->getHeight()));
	m_skyFBO->init();

	std::string basicFS = ASSETS_FOLDER_PATH + "Shaders/superBasicShader.frag";
	std::string basicVS = ASSETS_FOLDER_PATH + "Shaders/superBasicShader.vert";

	m_sunlightFBO = new BARE2D::FBORenderer(basicFS,
	                                        basicVS,
	                                        m_window->getWidth(),
	                                        m_window->getHeight(),
	                                        glm::vec2(m_window->getWidth(), m_window->getHeight()));
	m_sunlightFBO->init();

	m_fluidFBO = new BARE2D::FBORenderer(basicFS,
	                                     basicVS,
	                                     m_window->getWidth(),
	                                     m_window->getHeight(),
	                                     glm::vec2(m_window->getWidth(), m_window->getHeight()));
	m_fluidFBO->init();

	// Link all the cameras together.
	std::shared_ptr<BARE2D::Camera2D> fboCamera = m_mainFBO->getCamera();
	m_normalFBO->setCamera(fboCamera);
	m_skyFBO->setCamera(fboCamera);
	m_sunlightFBO->setCamera(fboCamera);
	m_fluidFBO->setCamera(fboCamera);

	m_scale = INITIAL_ZOOM;
	Singletons::setGameCamera(m_renderer->getCamera().get());
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
		while(!found) {
			if(!Singletons::getWorld()->getTile(pos.x, pos.y, 0)->isSolid()) {
				found = true;
			}
			pos.y++;
		}

		EntityPlayer* p = new EntityPlayer(pos, 0);

		Singletons::getEntityManager()->setPlayer(p);
		Singletons::getEntityManager()->getPlayer()->initGUI();
	}

	m_console->init(m_questManager, this);

	m_dialogueManager = new DialogueModule::DialogueManager(m_questManager);

	tick();
}

void GameplayScreen::onExit()
{
	delete m_questManager;
	delete m_dialogueManager;
	delete m_console;

	m_renderer->destroy();
	m_fontRenderer->destroy();
	m_dr->destroy();

	m_gameState = GameState::PLAY;

	Singletons::destroyGUI();
}

void GameplayScreen::update(double dt)
{
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
				m_lastPlayerPos.x += (float)(worldSize) * sign;
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

		if(m_scale > MIN_ZOOM && m_scale < MAX_ZOOM)
			Singletons::getGameCamera()->setScale(m_scale, m_scale);

		m_time++; /// Change the increment if time is slowed or quicker (potion effects?)

		if((int)m_frame % (int)(60 / TICK_RATE) == 0) { // m_frame is equal to current frame
			tick();
		}

		m_frame++;
	}
	if(m_screenState != BARE2D::ScreenState::EXIT_APPLICATION)
		Singletons::getGUI()->update();
}

void GameplayScreen::draw()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	{
		// Sky
		drawSkyToFBO(); // Readies the FBO.

		m_skyFBO->render();

		{
			m_renderer->begin();

			float sunPercentX, sunPercentY; // 0.45, 0.8f

			sunPercentX = 0.45f * std::cos(3.1415f * 2.0f * ((float)Singletons::getWorld()->getTime() / DAY_LENGTH) +
			                               3.1415f * 0.5f);
			sunPercentY = 0.8f * std::sin(3.1415f * 2.0f * ((float)Singletons::getWorld()->getTime() / DAY_LENGTH) +
			                              3.1415f * 0.5f);

			std::string sunPath = ASSETS_FOLDER_PATH + "/Textures/sun.png";

			GLuint sunID = BARE2D::ResourceManager::loadTexture(sunPath).id;

			m_renderer->draw(glm::vec4(m_window->getWidth() * (0.5f - sunPercentX) - 16.0f,
			                           (sunPercentY)*m_window->getHeight(),
			                           32.0f,
			                           32.0f),
			                 glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
			                 sunID,
			                 1.0f,
			                 BARE2D::Colour(255, 255, 255, 255));

			std::string moonPath = ASSETS_FOLDER_PATH + "/Textures/UNDEFINED.png";

			GLuint moonID = BARE2D::ResourceManager::loadTexture(moonPath).id;

			m_renderer->draw(glm::vec4(m_window->getWidth() * (0.5f + sunPercentX) - 16.0f,
			                           -(sunPercentY)*m_window->getHeight(),
			                           32.0f,
			                           32.0f),
			                 glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
			                 moonID,
			                 1.0f,
			                 BARE2D::Colour(0, 0, 255, 255));

			m_renderer->end();
			m_renderer->render();
		}
	}

	{
		drawWorldToFBO();
		drawWorldSunlightToFBO(); // Draws to sunlight FBO.
		drawParticlesToFBO();

		m_renderer->begin();

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

		float playerDepth =
		    0.1f + (Singletons::getEntityManager()->getPlayer()->getLayer() * (1.0f / (float)(WORLD_DEPTH)) * 0.9f);
		m_renderer->getShader()->setUniform("playerDepth", &playerDepth);

		Singletons::getWorld()->setLightsUniform(getScreenBox() + glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_renderer->getShader()); // sets "lights" uniform of vec3s

		m_mainFBO->render();

		m_renderer->end();
		m_renderer->render();
	}

	{
		drawFluidsToFBO();

		m_renderer->begin();

		m_renderer->getShader()->setUniform("time", &m_time);

		m_fluidFBO->render();

		m_renderer->end();
		m_renderer->render();
	}

	drawGUIToScreen(); // These two actually do draw to the screen.
	drawPostToScreen();
}

void GameplayScreen::drawSkyToFBO()
{
	float dayLight = cos((float)Singletons::getWorld()->getTime() / (DAY_LENGTH / 6.28318f)) / 2.0f + 0.5f;

	m_skyFBO->begin();

	glClearColor(0.3f * dayLight, 0.4f * dayLight, 1.0f * dayLight, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	{

		m_renderer->begin();

		glm::vec2 windowSizeUniform = glm::vec2(m_window->getWidth(), m_window->getHeight());
		m_renderer->getShader()->setUniform("screenSizeU", &windowSizeUniform);

		float dayLightUniform = dayLight * 0.5f + 0.5f;
		m_renderer->getShader()->setUniform("daylight", &dayLightUniform);

		float playerChunkX =
		    std::fmod((Singletons::getEntityManager()->getPlayer()->getPosition().x + (float)CHUNK_SIZE),
		              (float)CHUNK_SIZE);
		float playerXPosUniform = playerChunkX / (float)(CHUNK_SIZE);
		m_renderer->getShader()->setUniform("playerXPos", &playerXPosUniform);

		float parallaxZoomUniform = 0.95f;
		m_renderer->getShader()->setUniform("parallaxZoom", &parallaxZoomUniform);

		int			playerX = (int)Singletons::getEntityManager()->getPlayer()->getPosition().x;

		GLuint backgroundID = Singletons::getWorld()->getBiome(playerX).backgroundTexture.id;

		m_renderer->draw(glm::vec4(0.0f, 0.0f, m_window->getWidth(), m_window->getHeight()),
		                 glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
		                 backgroundID,
		                 1.0f,
		                 BARE2D::Colour(255, 255, 255, 255));

		m_renderer->end();
		m_renderer->render();
	}

	m_skyFBO->end();
}

void GameplayScreen::drawWorldToFBO()
{
	m_mainFBO->begin();

	glm::vec4 screenRect = getScreenBox();

	{
		{
			// World: Tiles

			m_renderer->begin();

			Singletons::getWorld()->drawTiles(m_renderer,
			                                  m_fontRenderer,
			                                  m_dr,
			                                  screenRect);
			Singletons::getEntityManager()->drawEntities(m_renderer, m_fontRenderer, m_dr, screenRect);

			m_renderer->end();
			m_renderer->render();
		}
	}

	m_mainFBO->end();
}

void GameplayScreen::drawWorldSunlightToFBO()
{
	m_sunlightFBO->begin(); // Normal mapping begin
	m_renderer->begin();

	Singletons::getWorld()->drawSunlight(m_renderer, getScreenBox());

	m_renderer->end();
	m_renderer->render();
	m_sunlightFBO->end(); // Normal mapping end.
}

void GameplayScreen::drawParticlesToFBO()
{
	m_mainFBO->begin();
	{
		// World: Particles
		m_renderer->begin();
		/// TODO: Particles
		//m_particle2d.draw(&m_renderer, 0.0f);

		m_renderer->end();
		m_renderer->render();
	}
	m_mainFBO->end();
}

void GameplayScreen::drawFluidsToFBO()
{
	glm::vec4 screenRect = getScreenBox();
	Singletons::getWorld()->updateFluidTextures(screenRect);

	m_fluidFBO->begin();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_renderer->begin();

	Singletons::getWorld()->drawFluids(m_renderer, screenRect);

	m_renderer->end();
	m_renderer->render();

	m_fluidFBO->end();
}

void GameplayScreen::drawGUIToScreen()
{
	{
		/// CEGUI Inventory GUI (Context 1)

		Singletons::getGUI()->setActiveContext(1);
		Singletons::getGUI()->draw();
		Singletons::getGUI()->setActiveContext(0);
	}

	{
		/// Inventory Overlays

		Singletons::getEntityManager()->getPlayer()->drawGUI(m_renderer, m_fontRenderer);
		glm::vec4 screenRect = getScreenBox();

		m_renderer->begin();

		Singletons::getWorld()->drawTilesGUI(m_renderer, m_fontRenderer, screenRect);

		if(Singletons::getEntityManager()->getPlayer()->isInventoryOpen())
			Singletons::getEntityManager()->getPlayer()->display(
			    m_renderer,
			    glm::vec2(0.2f, 0.36f) *
			    glm::vec2(Singletons::getGameCamera()->getScreenWidth(), Singletons::getGameCamera()->getScreenHeight()),
			    80.0f);

		m_renderer->end();
		m_renderer->render();
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

void GameplayScreen::drawPostToScreen()
{
	// Post
	m_renderer->begin();

	glm::vec2 screenSizeUniform = glm::vec2(m_window->getWidth(), m_window->getHeight());
	m_renderer->getShader()->setUniform("screenSizeU", &screenSizeUniform);

	float sanityUniform = Singletons::getEntityManager()->getPlayer()->getSanity();
	m_renderer->getShader()->setUniform("sanity", &sanityUniform);

	m_renderer->getShader()->setUniform("time", &m_time);

	m_renderer->draw(glm::vec4(0.0f, 0.0f, m_window->getWidth(), m_window->getHeight()),
	                 glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
	                 0,
	                 0.0f,
	                 BARE2D::Colour(0, 0, 0, 0));

	m_renderer->end();
	m_renderer->render();
}

/// GameplayScreen PRIVATE FUNCTIONS

void GameplayScreen::checkInput()
{
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
}

void GameplayScreen::initUI()
{
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

void GameplayScreen::tick()
{
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

void GameplayScreen::updateScale()
{
	m_scale += (float)m_inputManager->getMouseScrollwheelPosition() * m_scale / 20.0f;
	if(m_scale < MIN_ZOOM) {
		m_scale = MIN_ZOOM;
	} else if(m_scale > MAX_ZOOM) {
		m_scale = MAX_ZOOM;
	}
}

#include "ExtraFunctions.h"

#ifdef DEV_CONTROLS

void GameplayScreen::drawDebug()
{
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

void GameplayScreen::pauseGame()
{
	m_lastGameState = m_gameState;
	m_gameState		= GameState::PAUSE;
	for(unsigned int i = 0; i < m_pauseWidgets.size(); i++) {
		m_pauseWidgets[i]->show();
		m_pauseWidgets[i]->enable();
		m_pauseWidgets[i]->activate();
	}
}

void GameplayScreen::continueGame()
{
	m_gameState = m_lastGameState;
	for(unsigned int i = 0; i < m_pauseWidgets.size(); i++) {
		m_pauseWidgets[i]->hide();
		m_pauseWidgets[i]->disable();
		m_pauseWidgets[i]->deactivate();
	}
}

bool GameplayScreen::pause_resume_button_clicked(const CEGUI::EventArgs& e)
{
	continueGame();
	return true;
}

bool GameplayScreen::pause_save_button_clicked(const CEGUI::EventArgs& e)
{
	m_WorldIOManager->saveWorld();
	return true;
}

bool GameplayScreen::pause_quit_button_clicked(const CEGUI::EventArgs& e)
{
	pause_save_button_clicked(e);
	m_nextScreenIndex = SCREEN_INDEX_MAINMENU;
	m_screenState	  = BARE2D::ScreenState::CHANGE_NEXT;
	return true;
}

glm::vec4 GameplayScreen::getScreenBox()
{
	// Window coordinates
	glm::vec2 topLeft(0.0f, 0.0f);
	glm::vec2 bottomRight(m_window->getWidth(), m_window->getHeight());

	glm::vec2 gameplayCoordsTL = Singletons::getGameCamera()->getViewedPositionFromScreenPosition(topLeft);
	glm::vec2 gameplayCoordsBR = Singletons::getGameCamera()->getViewedPositionFromScreenPosition(bottomRight);

	glm::vec4 ret = glm::vec4(gameplayCoordsTL.x,
	                          gameplayCoordsBR.y,
	                          gameplayCoordsBR.x - gameplayCoordsTL.x,
	                          gameplayCoordsTL.y - gameplayCoordsBR.y) +
	                glm::vec4(-1.5f, -1.5f, 3.0f, 3.0f);

	if(std::abs(ret.x) > 10000 || std::abs(ret.y) > 1000) {
		BARE2D::Logger::getInstance()->log("Err");
	}

	return ret;
}
