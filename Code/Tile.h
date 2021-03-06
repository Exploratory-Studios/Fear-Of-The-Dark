#pragma once

#include <Vertex.h>
#include <GLTexture.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <ParticleEngine2D.h>
#include <glm/glm.hpp>

#include "AudioManager.h"
#include "PresetValues.h"
#include "ScriptQueue.h"

#include "SaveDataTypes.h"

class World;

enum class TileIDs {
	AIR,
	DIRT,
	GRASS,
	SAND,
	STONE,
	BUSH,
	TORCH,
	TORCH_BRIGHT,
	TORCH_ANTI,
	FOLIAGE,
	WOOD,
	WATER,
	WOOD_POLE,
	SIGN_WOOD
};

class Tile;

Tile* createTile(unsigned int			 id,
				 glm::vec2				 pos,
				 int					 layer,
				 bool					 loadTex  = true,
				 SaveDataTypes::MetaData metaData = SaveDataTypes::MetaData());

class Tile {
  public:
	Tile();
	Tile(glm::vec2 pos, unsigned int layer, unsigned int id, SaveDataTypes::MetaData data, bool loadTex);
	Tile(glm::vec2 pos, unsigned int layer, TileIDs id, SaveDataTypes::MetaData data, bool loadTex);
	virtual ~Tile() {
	}

	virtual void initParticles(GLEngine::ParticleEngine2D* engine);

#ifdef DEV_CONTROLS
	std::string getPrintout() {
		std::string ret;
		ret = "Position: X=" + std::to_string(m_pos.x) + ", Y=" + std::to_string(m_pos.y) + "\n";
		ret += "ID=" + std::to_string(m_id) + ", Solid=" + std::to_string(m_solid) + ", " +
			   "LightLevel: Amb=" + std::to_string(m_ambientLight) + ", Sun=" + std::to_string(m_sunLight) +
			   ", Emit=" + std::to_string(m_emittedLight) + "\n";
		ret += "ExposedToSun: " + std::to_string(m_exposedToSun) + "\n";
		//ret += "Raw Temperature: " + std::to_string(m_temperature) + ", Real Temperature: " + std::to_string(getHeat(world)) + "\n";
		ret += "MetaData: " + m_metaData.getElements() + "\n";
		return ret;
	}
#endif

	virtual std::vector<ScriptingModule::Argument> generateLuaData() {
		std::vector<ScriptingModule::Argument> args = {
			{"blockX", std::to_string(m_pos.x)},
			{"blockY", std::to_string(m_pos.y)},
			{"blockLayer", std::to_string(m_layer)},
			{"blockID",
			 std::to_string(
				 m_id)} // TODO: Implement metadata. Each element to the metadata can be an element in this table.
		};

		m_metaData.getLuaArguments(args);

		return args;
	}

	glm::vec2 getPosition() const {
		return m_pos;
	}
	glm::vec2 getSize() const {
		return m_size;
	}
	GLuint getTextureID() const {
		return m_textureId;
	}
	GLuint getBumpMapID() const {
		return m_bumpMapId;
	}
	unsigned int getID() const {
		return m_id;
	}
	bool isSolid() const {
		return m_solid;
	}
	bool isNatural() const {
		return m_natural;
	}
	float	  getLight();
	glm::vec4 getSunlightCorners() {
		return m_cornerSunlight;
	}
	float getSunlight() {
		return m_sunLight;
	}
	float getAmbientLight() const {
		return m_ambientLight;
	}
	float getEmittedLight() const {
		return m_emittedLight;
	}
	float getSurroundingHeat();
	bool  isExposedToSunlight() const {
		 return m_exposedToSun;
	}
	bool isTransparent() const {
		return m_transparent;
	}
	float getHeat(); // Used when gameplay mechanics are in play (modifiers in use, not used when tiles are inheriting temperatures)
	float getRawHeat(); // Used whenever two tiles' temperatures are being compared, etc. (No modifiers excepting baseHeat)
	float getEmittedHeat() const {
		return m_emittedHeat;
	}
	bool doDraw() const {
		return m_draw;
	}
	unsigned int getLayer() const {
		return m_layer;
	}
	float getLightAtPoint(glm::vec2 posFromBL);

	virtual SaveDataTypes::TileData getSaveData();

	void setAmbientLight(float light) {
		m_ambientLight = light;
	}
	void addAmbientLight(float light) {
		m_ambientLight += light;
	}
	void setToDraw(bool draw) {
		m_draw = draw;
	}

	void setMetaData(SaveDataTypes::MetaData& md) {
		m_metaData = md;
	}
	SaveDataTypes::MetaData getMetaData() {
		return m_metaData;
	}
	void setSize(glm::vec2 s) {
		m_size = s;
	}

	void addCornerLight(float TL, float TR, float BR, float BL) {
		if(TL > 0)
			m_cornerLight.x += TL;
		if(TR > 0)
			m_cornerLight.y += TR;
		if(BR > 0)
			m_cornerLight.z += BR;
		if(BL > 0)
			m_cornerLight.w += BL;

		m_ambientLight = m_cornerLight.z; // Bottom right corner.
	}
	void subtractCornerLight(float TL, float TR, float BR, float BL) {
		if(TL > 0)
			m_cornerLight.x -= TL;
		if(TR > 0)
			m_cornerLight.y -= TR;
		if(BR > 0)
			m_cornerLight.z -= BR;
		if(BL > 0)
			m_cornerLight.w -= BL;

		m_ambientLight = m_cornerLight.z; // Bottom right corner.
	}

	void setNeedsSunCheck() {
		m_needsSunCheck = true;
	}
	void setToUpdate_heat() {
		m_updateHeat = true; // Sends the signal to update heat
	}
	void setSunLight(float light, bool left, bool right, bool top, bool bottom) {
		m_sunLight = light;

		if(top) {
			m_cornerSunlight.x = m_sunLight;
			m_cornerSunlight.y = m_sunLight;
		}
		if(right) {
			if(!top) {
				m_cornerSunlight.y = m_sunLight;
			}
			if(bottom) {
				m_cornerSunlight.z = m_sunLight;
			}
		}
		if(left) {
			if(!top) {
				m_cornerSunlight.x = m_sunLight;
			}
			if(bottom) {
				m_cornerSunlight.w = m_sunLight;
			}
		}
	}
	void resetSunlightCorners() {
		m_cornerSunlight = glm::vec4(0.0f);
	}

	void update(float time, bool updateLighting, float& sunlight);
	void specialUpdate(
		float time); // This is only called when the tile/surrounding tiles are changed. Calls updateScript
	void		 tick(float tickTime, float& sunlight);
	virtual void draw(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, int& xOffset, int& depthDifference);
	virtual void drawNormal(GLEngine::SpriteBatch& sb, int& xOffset, int& depthDifference);
	virtual void drawGUI(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, int& xOffset) {
	}

	void destroy();

	void onInteract_WalkedOn() {
		if(m_interactScriptID_walkedOn != -1)
			ScriptingModule::ScriptQueue::activateScript(m_interactScriptID_walkedOn, generateLuaData());
	}
	void onInteract_RightClicked() {
		if(m_interactScriptID_used != -1)
			ScriptingModule::ScriptQueue::activateScript(m_interactScriptID_used, generateLuaData());
	}
	// ... More interact functions

	void resetNeighboursLight();
	void setNeighboursLight();

	void calculateSunlight(
		float sunlight); // Determines and sets the sunlight corner values for this tile and surrounding tiles.

  protected:
	void setPosition(glm::vec2 pos) {
		m_pos = pos;
	}

	void handleMetaDataInit(SaveDataTypes::MetaData& data) {};

	virtual void onUpdate() {
	}
	virtual void onDraw(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, glm::vec4& pos, float& depth) {
	}
	void onTick() {
		if(m_tickScriptID != -1)
			ScriptingModule::ScriptQueue::activateScript(m_tickScriptID, generateLuaData());
	}
	void onDestruction() {
	}

	unsigned int m_updateScriptID			 = (unsigned int)-1;
	unsigned int m_tickScriptID				 = (unsigned int)-1;
	unsigned int m_interactScriptID			 = (unsigned int)-1;
	unsigned int m_destroyScriptID			 = (unsigned int)-1;
	unsigned int m_interactScriptID_walkedOn = (unsigned int)-1;
	unsigned int m_interactScriptID_used	 = (unsigned int)-1;

	void loadTexture();

	bool exposedToSun();

	glm::vec2	 m_pos	 = glm::vec2(69.420f, 69.420f);
	unsigned int m_layer = 0; // Higher numbers are farther behind
	glm::vec2	 m_size	 = glm::vec2(1, 1);

	std::string m_texturePath;
	std::string m_bumpMapPath;

	GLuint				  m_textureId	   = (GLuint)-1;
	GLuint				  m_bumpMapId	   = (GLuint)-1;
	GLEngine::ColourRGBA8 m_colour		   = GLEngine::ColourRGBA8(255.0f, 255.0f, 255.0f, 255.0f);
	float				  m_depthForRender = 0.0f;

	float m_ambientLight =
		0.0f; // Really should be inherited light or something, as this light is inherited from surrounding blocks
	float	  m_emittedLight = 0.0f;
	float	  m_sunLight;
	glm::vec4 m_cornerSunlight;
	glm::vec4 m_cornerLight; // Clockwise, 0 is top left

	bool m_exposedToSun	 = false;
	bool m_needsSunCheck = true; // Was there a block just placed in this column?

	float m_emittedHeat = 0.0f;

	float m_temperature		= 0.0f; // This should be affected by the biome, sunlight, and nearby heat sources.
	float m_lastTemperature = 0.0f;

	bool m_updateHeat = true;

	bool m_solid = true;  // Don't collide if true: Air, water, pillars, etc. -> This is the 'passable' aspect of blocks
	bool m_draw	 = false; // Don't draw if true: Air, etc.
	bool m_transparent = false; // 'Transmits' light?
	bool m_backdrop	   = false; // Does it draw a back wall?
	bool m_natural	   = false; // Is it a natural block? Does it get changed to stone when eras pass?
	bool m_setLighting = false; // Has this light source set the neighbours' light values??

	unsigned int m_id;

	SaveDataTypes::MetaData m_metaData;

  private:
	void init();
};
