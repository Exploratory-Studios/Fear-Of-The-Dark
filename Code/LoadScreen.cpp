#include "LoadScreen.h"

/// LoadScreen FUNCTIONS

LoadScreen::LoadScreen(BARE2D::Window* window, WorldIOManager* WorldIOManager) :
	m_window(window), m_worldIOManager(WorldIOManager) {
}

LoadScreen::~LoadScreen() {
	//dtor
}

int LoadScreen::getNextScreenIndex() const {
	return m_nextScreenIndex;
}

void LoadScreen::initScreen() {
}

void LoadScreen::destroyScreen() {
}

void LoadScreen::onEntry() {
	std::string fragShader = "Shaders/textureShader.frag";
	std::string vertShader = "Shaders/textureShader.vert";

	m_renderer = new BARE2D::BasicRenderer(fragShader, vertShader);
	m_renderer->init();

	m_fontRenderer = new BARE2D::FontRenderer(fragShader, vertShader);
	m_fontRenderer->init();

	initUI();

	m_miniScreen = MiniScreen::MAIN;
	m_time		 = 0;
}

void LoadScreen::onExit() {
	for(auto e: m_miniScreenEntries)
		e.dispose();
	m_miniScreenEntries.clear();
	m_loadWorldNameListbox->resetList();
}

void LoadScreen::update(double dt) {
	for(auto e: m_miniScreenEntries)
		e.checkScreen(m_miniScreen);

	m_time++;
}

void LoadScreen::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	m_renderer->begin();

	float alpha = 0.0f;

	if(m_time <= 255.0f / 2.0f) {
		alpha = m_time * 2.0f;
	} else {
		alpha = 255.0f;
	}

	m_createButton->setAlpha(alpha / 255);
	m_loadButton->setAlpha(alpha / 255);

	BARE2D::BARECEGUI::getInstance()->draw();

	m_renderer->end();
	m_renderer->render();
}

void LoadScreen::initUI() {
	BARE2D::BARECEGUI* gui = BARE2D::BARECEGUI::getInstance();

	{
		std::string guiPath = ASSETS_FOLDER_PATH + "GUI";
		gui->init(guiPath, 1);
		gui->loadScheme("FOTDSkin.scheme");

		gui->setFont("QuietHorror-30");

		gui->setMouseCursor("FOTDSkin/MouseArrow");
		gui->setMouseCursorShown(true);
		SDL_ShowCursor(0);
	}

	{
		m_createButton = static_cast<CEGUI::PushButton*>(gui->createWidget("FOTDSkin/Button",
																		   glm::vec4(0.1f, 0.3f, 0.35f, 0.35f),
																		   glm::vec4(0.0f),
																		   nullptr,
																		   "CreatePushbutton"));
		m_createButton->subscribeEvent(CEGUI::PushButton::EventClicked,
									   CEGUI::Event::Subscriber(&LoadScreen::onCreateButtonClicked, this));
		m_createButton->setText("[padding='l:0 t:15 r:0 b:0']New World");
		m_createButton->setFont("QuietHorror-42");
		m_miniScreenEntries.emplace_back(m_createButton, MiniScreen::MAIN);

		CEGUI::PushButton* temp = static_cast<CEGUI::PushButton*>(gui->createWidget("FOTDSkin/Label",
																					glm::vec4(0.1f, 0.05f, 0.8f, 0.2f),
																					glm::vec4(0.0f),
																					nullptr,
																					"PlayMainLabel"));
		temp->setText("[padding='l:0 t:30 r:0 b:0']Play");
		temp->setFont("QuietHorror-118");
		m_miniScreenEntries.emplace_back(temp, MiniScreen::MAIN);
	}

	{
		m_newWorldNameEditbox = static_cast<CEGUI::Editbox*>(gui->createWidget("FOTDSkin/Editbox",
																			   glm::vec4(0.1f, 0.1f, 0.25f, 0.1f),
																			   glm::vec4(0.0f),
																			   nullptr,
																			   "WorldNameEditbox"));
		m_newWorldNameEditbox->setText("Name");
		m_miniScreenEntries.emplace_back(m_newWorldNameEditbox, MiniScreen::CREATE);

		m_newWorldSeedEditbox = static_cast<CEGUI::Editbox*>(gui->createWidget("FOTDSkin/Editbox",
																			   glm::vec4(0.1f, 0.25f, 0.25f, 0.1f),
																			   glm::vec4(0.0f),
																			   nullptr,
																			   "SeedEditbox"));
		m_newWorldSeedEditbox->setText("Seed");
		m_miniScreenEntries.emplace_back(m_newWorldSeedEditbox, MiniScreen::CREATE);

		m_newWorldCreateNewButton =
			static_cast<CEGUI::PushButton*>(gui->createWidget("FOTDSkin/Button",
															  glm::vec4(0.35f, 0.7f, 0.3f, 0.15f),
															  glm::vec4(0.0f),
															  nullptr,
															  "CreateNewWorldPushbutton"));
		m_newWorldCreateNewButton->subscribeEvent(
			CEGUI::PushButton::EventClicked,
			CEGUI::Event::Subscriber(&LoadScreen::onNewWorldCreateNewButtonClicked, this));
		m_newWorldCreateNewButton->setText("[padding='l:0 t:15 r:0 b:0']Create World");
		m_newWorldCreateNewButton->setFont("QuietHorror-42");
		m_miniScreenEntries.emplace_back(m_newWorldCreateNewButton, MiniScreen::CREATE);

		m_newWorldFlatCheckbox = static_cast<CEGUI::ToggleButton*>(gui->createWidget("FOTDSkin/Checkbox",
																					 glm::vec4(0.1f, 0.4f, 0.12f, 0.1f),
																					 glm::vec4(0.0f),
																					 nullptr,
																					 "WorldFlatToggleButton"));
		m_newWorldFlatCheckbox->setText("[padding='l:0 t:10 r:0 b:0']Flat World");
		m_miniScreenEntries.emplace_back(m_newWorldFlatCheckbox, MiniScreen::CREATE);

		m_newWorldSizeLabel = static_cast<CEGUI::DefaultWindow*>(gui->createWidget("FOTDSkin/Label",
																				   glm::vec4(0.6f, 0.05f, 0.3f, 0.05f),
																				   glm::vec4(0.0f),
																				   nullptr,
																				   "WorldSizeLabel"));
		m_miniScreenEntries.emplace_back(m_newWorldSizeLabel, MiniScreen::CREATE);

		m_newWorldSizeSlider = static_cast<CEGUI::Slider*>(gui->createWidget("FOTDSkin/HorizontalSlider",
																			 glm::vec4(0.6f, 0.1f, 0.3f, 0.1f),
																			 glm::vec4(0),
																			 nullptr,
																			 "WorldSizeSlider"));
		m_newWorldSizeSlider->subscribeEvent(CEGUI::Slider::EventValueChanged,
											 CEGUI::Event::Subscriber(&LoadScreen::onNewWorldSizeValueChanged, this));
		m_newWorldSizeSlider->subscribeEvent(CEGUI::Slider::EventThumbTrackEnded,
											 CEGUI::Event::Subscriber(&LoadScreen::onNewWorldSizeSliderReleased, this));
		m_newWorldSizeSlider->setMaxValue(19.0f);
		m_newWorldSizeSlider->setClickStep(1.0f);
		m_newWorldSizeSlider->setCurrentValue(1.0f);
		m_miniScreenEntries.emplace_back(m_newWorldSizeSlider, MiniScreen::CREATE);
	}

	{
		m_loadButton = static_cast<CEGUI::PushButton*>(gui->createWidget("FOTDSkin/Button",
																		 glm::vec4(0.55f, 0.3f, 0.35f, 0.35f),
																		 glm::vec4(0.0f),
																		 nullptr,
																		 "LoadPushButton"));
		m_loadButton->subscribeEvent(CEGUI::PushButton::EventClicked,
									 CEGUI::Event::Subscriber(&LoadScreen::onLoadButtonClicked, this));
		m_loadButton->setText("[padding='l:0 t:15 r:0 b:0']Load World");
		m_loadButton->setFont("QuietHorror-42");
		m_miniScreenEntries.emplace_back(m_loadButton, MiniScreen::MAIN);
	}

	{
		m_loadWorldNameListbox = static_cast<CEGUI::Listbox*>(gui->createWidget("FOTDSkin/Listbox",
																				glm::vec4(0.6f, 0.1f, 0.3f, 0.8f),
																				glm::vec4(0.0f),
																				nullptr,
																				"LoadWorldNameListbox"));
		m_loadWorldNameListbox->setFont("Amatic-38");
		m_miniScreenEntries.emplace_back(m_loadWorldNameListbox, MiniScreen::LOAD);

		m_loadWorldLoadButton = static_cast<CEGUI::PushButton*>(gui->createWidget("FOTDSkin/Button",
																				  glm::vec4(0.1f, 0.3f, 0.25f, 0.15f),
																				  glm::vec4(0.0f),
																				  nullptr,
																				  "LoadWorldLoadPushButton"));
		m_loadWorldLoadButton->subscribeEvent(
			CEGUI::PushButton::EventClicked,
			CEGUI::Event::Subscriber(&LoadScreen::onLoadWorldLoadButtonClicked, this));
		m_loadWorldLoadButton->setText("[padding='l:0 t:15 r:0 b:0']Load World");
		m_loadWorldLoadButton->setFont("QuietHorror-42");
		m_miniScreenEntries.emplace_back(m_loadWorldLoadButton, MiniScreen::LOAD);
	}

	m_backButton = static_cast<CEGUI::PushButton*>(gui->createWidget("FOTDSkin/Button",
																	 glm::vec4(0.1f, 0.8f, 0.2f, 0.1f),
																	 glm::vec4(0.0f),
																	 nullptr,
																	 "BackPushButton"));
	m_backButton->subscribeEvent(CEGUI::PushButton::EventClicked,
								 CEGUI::Event::Subscriber(&LoadScreen::onBackButtonClicked, this));
	m_backButton->setText("[padding='l:0 t:15 r:0 b:0']Back");
	m_backButton->setFont("QuietHorror-42");
}

bool LoadScreen::onNewWorldCreateNewButtonClicked(const CEGUI::EventArgs& e) { /// TODO: clean post-this lines up

	std::string	 text = m_newWorldSeedEditbox->getText().c_str();
	unsigned int seed = 0;

	for(unsigned int i = 0; i < text.length(); i++) {
		seed += (char)text[i];
	}

	m_worldIOManager->createWorld(seed,
								  m_newWorldNameEditbox->getText().c_str(),
								  m_newWorldFlatCheckbox->isSelected(),
								  (m_newWorldSizeSlider->getCurrentValue() + 1) * 32);

	//m_worldIOManager->createWorld(seed, m_newWorldNameEditbox->getText().c_str(), m_newWorldFlatCheckbox->isSelected(), nullptr);

	m_screenState	  = BARE2D::ScreenState::CHANGE_NEXT;
	m_nextScreenIndex = SCREEN_INDEX_WAIT;

	return true;
}

bool LoadScreen::onNewWorldSizeValueChanged(const CEGUI::EventArgs& e) {
	float val = m_newWorldSizeSlider->getCurrentValue();
	if(std::abs(val - std::floor(val)) < std::abs(val - std::ceil(val))) {
		val = std::floor(val);
	} else {
		val = std::ceil(val);
	}
	m_newWorldSizeLabel->setText("Chunks: " + std::to_string((int)val));
	return true;
}

bool LoadScreen::onNewWorldSizeSliderReleased(const CEGUI::EventArgs& e) {
	float val = m_newWorldSizeSlider->getCurrentValue();
	if(std::abs(val - std::floor(val)) < std::abs(val - std::ceil(val))) {
		val = std::floor(val);
	} else {
		val = std::ceil(val);
	}
	m_newWorldSizeSlider->setCurrentValue((int)val);
	m_newWorldSizeSlider->invalidate();

	return true;
}

bool LoadScreen::onLoadWorldLoadButtonClicked(const CEGUI::EventArgs& e) { /// TODO: clean post-this lines up

	std::string text = m_loadWorldNameListbox->getFirstSelectedItem()->getText().c_str();

	m_worldIOManager->loadWorld(text);

	m_screenState	  = BARE2D::ScreenState::CHANGE_NEXT;
	m_nextScreenIndex = SCREEN_INDEX_WAIT;

	return true;
}

bool LoadScreen::onCreateButtonClicked(const CEGUI::EventArgs& e) {
	m_miniScreen = MiniScreen::CREATE;
	return true;
}

bool LoadScreen::onLoadButtonClicked(const CEGUI::EventArgs& e) {
	m_miniScreen = MiniScreen::LOAD;
	getDirectoryEntries();
	return true;
}

bool LoadScreen::onBackButtonClicked(const CEGUI::EventArgs& e) {
	if(m_miniScreen == MiniScreen::MAIN) {
		m_nextScreenIndex = SCREEN_INDEX_MAINMENU;
		m_screenState	  = BARE2D::ScreenState::CHANGE_NEXT;
	} else if(m_miniScreen == MiniScreen::CREATE) {
		m_miniScreen = MiniScreen::MAIN;
		for(auto& m: m_miniScreenEntries) {
			m.checkScreen(m_miniScreen);
		}
	} else if(m_miniScreen == MiniScreen::SETTINGS) {
		m_miniScreen = MiniScreen::CREATE;
		for(auto& m: m_miniScreenEntries) {
			m.checkScreen(m_miniScreen);
		}
	} else if(m_miniScreen == MiniScreen::LOAD) {
		m_miniScreen = MiniScreen::MAIN;
		for(auto& m: m_miniScreenEntries) {
			m.checkScreen(m_miniScreen);
		}
	}
	return true;
}

void LoadScreen::getDirectoryEntries() {
	m_loadWorldNameListbox->resetList();

	fs::path				 path(SAVES_PATH);
	std::vector<std::string> entries;
	if(fs::exists(path) && fs::is_directory(path)) {
		for(fs::directory_iterator it(path); it != fs::directory_iterator(); it++) {
			auto filename = static_cast<std::string>(it->path().filename().string());
			if(fs::is_regular_file(it->status())) {
				if(filename.rfind(".bin") != std::string::npos) {
					if(filename[0] != '.')
						entries.push_back(filename.erase(filename.rfind(".bin"), 4));
				}
			}
		}
	} else {
		BARE2D::Logger::getInstance()->log("Bad saves path: " + path.string());
	}

	for(unsigned int i = 0; i < entries.size(); i++) {
		CEGUI::ListboxTextItem* item = new CEGUI::ListboxTextItem(entries[i], i);
		item->setSelectionColours(CEGUI::Colour(0.2f, 0.2f, 0.2f));
		item->setSelectionBrushImage("FOTDSkin/GenericBrush");

		item->setFont("Amatic-38");

		m_loadWorldNameListbox->addItem(item);
	}
}
