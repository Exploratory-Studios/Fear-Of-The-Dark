#include "OptionsMenuScreen.h"

#include <functional>

#include <BARECEGUI.hpp>

Slider::Slider(float* data, glm::vec4 destRect, std::string name, std::string shownName) : m_data(data) {
	init(destRect, name, shownName);
}

void Slider::init(glm::vec4& destRect, std::string& name, std::string& shownName) {
	m_slider = static_cast<CEGUI::Slider*>(
		BARE2D::BARECEGUI::getInstance()->createWidget("FOTDSkin/HorizontalSlider",
													   glm::vec4(destRect.x, destRect.y, destRect.z, destRect.w / 2.0f),
													   glm::vec4(0.0f),
													   nullptr,
													   name + "OptionsMenu"));
	m_slider->setCurrentValue(*m_data);
	std::function func = [=](const CEGUI::EventArgs& e) -> bool {
		*m_data = m_slider->getCurrentValue();
		return true;
	};
	m_slider->subscribeEvent(CEGUI::Slider::EventValueChanged, CEGUI::Event::Subscriber(func));

	m_label = static_cast<CEGUI::DefaultWindow*>(BARE2D::BARECEGUI::getInstance()->createWidget(
		"FOTDSkin/Label",
		glm::vec4(destRect.x, destRect.y + destRect.w / 2.0f, destRect.z, destRect.w / 2.0f),
		glm::vec4(0.0f),
		nullptr,
		name + "LabelOptionsMenu"));
	m_label->setText(shownName);
}

void Slider::setFont(std::string font) {
	m_label->setFont(font);
}

OptionsMenuScreen::OptionsMenuScreen(BARE2D::Window* window) : m_window(window) {
	//ctor
}

OptionsMenuScreen::~OptionsMenuScreen() {
	//dtor
}

int OptionsMenuScreen::getNextScreenIndex() const {
	return SCREEN_INDEX_OPTIONSMENU;
}

void OptionsMenuScreen::initScreen() {
}

void OptionsMenuScreen::destroyScreen() {
}

void OptionsMenuScreen::onEntry() {
	std::string vertexShader   = ASSETS_FOLDER_PATH + "Shaders/textureShader.vert";
	std::string fragmentShader = ASSETS_FOLDER_PATH + "Shaders/textureShader.frag";

	m_renderer = new BARE2D::BasicRenderer(fragmentShader, vertexShader);
	m_renderer->init();

	m_fontRenderer = new BARE2D::FontRenderer(fragmentShader, vertexShader);
	m_fontRenderer->init();

	initUI();
}

void OptionsMenuScreen::onExit() {
}

void OptionsMenuScreen::update(double dt) {
	m_time++;
}

void OptionsMenuScreen::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	float a = m_time;
	if(a > 255.0f)
		a = 255.0f;

	m_backButton->setAlpha(a);
}

void OptionsMenuScreen::initUI() {
	{
		std::string guiPath = ASSETS_FOLDER_PATH + "GUI";
		m_gui->init(guiPath, 1);
		m_gui->loadScheme("FOTDSkin.scheme");

		m_gui->setFont("QuietHorror-30");

		m_gui->setMouseCursor("FOTDSkin/MouseArrow");
		m_gui->setMouseCursorShown(true);
		SDL_ShowCursor(0);
	}

	m_backButton = static_cast<CEGUI::PushButton*>(m_gui->createWidget("FOTDSkin/Button",
																	   glm::vec4(0.1f, 0.8f, 0.30f, 0.1f),
																	   glm::vec4(0.0f),
																	   nullptr,
																	   "BackButtonOptionsMenu"));
	m_backButton->subscribeEvent(CEGUI::PushButton::EventClicked,
								 CEGUI::Event::Subscriber(&OptionsMenuScreen::EventBackButtonClicked, this));
	m_backButton->setText("[padding='l:0 t:15 r:0 b:0']Back");
	m_backButton->setFont("QuietHorror-42");

	m_masterVolumeSlider =
		new Slider(&(Options::masterVolume), glm::vec4(0.1f, 0.1f, 0.3f, 0.11f), "SliderMasterVolume", "Master Volume");
	m_soundsVolumeSlider = new Slider(&(Options::soundsVolume),
									  glm::vec4(0.1f, 0.22f, 0.3f, 0.11f),
									  "SliderSoundsVolume",
									  "Effects Volume");
	m_musicVolumeSlider =
		new Slider(&(Options::musicVolume), glm::vec4(0.1f, 0.34f, 0.3f, 0.11f), "SliderMusicVolume", "Music Volume");
}

bool OptionsMenuScreen::EventBackButtonClicked(const CEGUI::EventArgs& e) {
	m_screenState = BARE2D::ScreenState::CHANGE_PREV;
	Options::saveToFile("Options.bin");
	return true;
}
