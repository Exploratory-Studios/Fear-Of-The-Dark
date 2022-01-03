#include "ScreenIndices.h"
#include "PresetValues.h"

#include <Screen.hpp>
#include <Window.hpp>
#include <InputManager.hpp>

#include <Camera2D.hpp>
#include <BARECEGUI.hpp>
#include <BasicRenderer.hpp>
#include <FontRenderer.hpp>
#include <Vertex.hpp>
#include <ResourceManager.hpp>

#define SPLASHSCREEN_STUDIO		0
#define SPLASHSCREEN_OPENGL_SDL 1
#define SPLASHSCREEN_CEGUI		2
#define SPLASHSCREEN_TITLE		3
#define SPLASHSCREENS_NUM		4

enum class SplashScreenState { CHANGE_NEXT, NONE };

class Text {
  public:
	Text(glm::vec2		pos,
		 float			startTime,
		 float			endTime,
		 float			fadeTime,
		 BARE2D::Colour colour,
		 std::string	text,
		 glm::vec2		textScale,
		 int			splashScreen) :
		m_pos(pos),
		m_startTime(startTime * 60.0f), m_endTime(endTime * 60.0f), m_fadeTime(fadeTime * 60.0f),
		m_colour(BARE2D::Colour(colour.r, colour.g, colour.b, 0.0f)), m_text(text), m_textScale(textScale),
		m_splashScreen(splashScreen) {
	}

	void draw(BARE2D::FontRenderer* fontRenderer, BARE2D::BasicRenderer* renderer, float time, int activeSplashScreen);
	void update(float time);

  private:
	glm::vec2	   m_pos;
	float		   m_startTime;
	float		   m_endTime;
	float		   m_fadeTime;
	BARE2D::Colour m_colour;
	std::string	   m_text;
	glm::vec2	   m_textScale;
	int			   m_splashScreen;
};

class Image {
  public:
	Image(glm::vec2		 pos,
		  float			 startTime,
		  float			 endTime,
		  float			 fadeTime,
		  BARE2D::Colour colour,
		  std::string	 filepath,
		  glm::vec2		 size,
		  int			 splashScreen) :
		m_pos(pos),
		m_startTime(startTime * 60.0f), m_endTime(endTime * 60.0f), m_fadeTime(fadeTime * 60.0f),
		m_colour(BARE2D::Colour(colour.r, colour.g, colour.b, 0.0f)), m_filepath(filepath), m_size(size),
		m_splashScreen(splashScreen) {
	}

	void draw(BARE2D::BasicRenderer* renderer, float time, int activeSplashScreen);
	void update(float time);

  private:
	glm::vec2	   m_pos;
	float		   m_startTime;
	float		   m_endTime;
	float		   m_fadeTime;
	BARE2D::Colour m_colour;
	std::string	   m_filepath;
	glm::vec2	   m_size;
	int			   m_splashScreen;
};

class StartupScreen : public BARE2D::Screen {
  public:
	StartupScreen(BARE2D::Window* window, BARE2D::InputManager* input) : m_window(window), m_input(input) {
	}
	virtual ~StartupScreen() {
	}

	virtual int getNextScreenIndex() const override {
		return SCREEN_INDEX_MAINMENU;
	}
	virtual void initScreen() override;
	virtual void destroyScreen() override;
	virtual void onEntry() override;
	virtual void onExit() override;
	virtual void update(double dt) override;
	virtual void draw() override;

  private:
	void initShaders();
	void initUI();
	void initAnimations();

	void checkInput();

	std::vector<Text>  m_textAnimations;
	std::vector<Image> m_imageAnimations;

	float m_time = 0.0f;

	SplashScreenState m_splashScreenState = SplashScreenState::NONE;
	int				  m_splashScreen	  = SPLASHSCREEN_STUDIO;

	BARE2D::Window*		  m_window = nullptr;
	BARE2D::InputManager* m_input  = nullptr;

	BARE2D::BasicRenderer* m_renderer	  = nullptr;
	BARE2D::FontRenderer*  m_fontRenderer = nullptr;
};
