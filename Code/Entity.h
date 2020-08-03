#pragma once

#include <vector>
#include <GLTexture.h>
#include <SpriteBatch.h>
#include <DebugRenderer.h>

#include "ScriptQueue.h"
#include "SaveDataTypes.h"
#include "XMLDataTypes.h"

#include "Animation.h"

#include <SpriteBatch.h>

class Chunk;
namespace SaveDataTypes {
	class MetaData;
}
class World;

enum class EntityIDs {
	NPC_PLAYER,
	NPC_NEUTRAL_QUESTGIVER_A,
	NPC_NEUTRAL_COMPANIONCUBE,
	ITEM
};

class Entity {
		friend class Scripter;

	public:
		Entity(glm::vec2 pos, unsigned int layer, SaveDataTypes::MetaData data);
		Entity(SaveDataTypes::EntityData& saveData);
		virtual ~Entity();

		void update(float timeStep, unsigned int selfIndex);
		void tick();
		virtual void draw(GLEngine::SpriteBatch& sb, float time, int layerDifference, float xOffset);
		virtual void drawNormal(GLEngine::SpriteBatch& sb, float time, int layerDifference, float xOffset);
		void debugDraw(GLEngine::DebugRenderer& dr, float xOffset);
		void move(float timeStepVariable);

		virtual void collideWithTiles() {}
		virtual bool collideWithOther(Entity* other) {} // Returns false if the X distance makes collision impossible

		// Getters
		const glm::vec2& getPosition() const {
			return m_position;
		}
		const glm::vec2& getSize() const {
			return m_size;
		}
		glm::vec2 getVelocity() const {
			return m_velocity;
		}
		glm::vec4 getLightLevel() const {
			return m_cornerLight;
		}
		float getAverageLightLevel() const {
			return (m_cornerLight.x + m_cornerLight.y + m_cornerLight.z + m_cornerLight.w) / 4.0f;
		}
		void setPosition(glm::vec2 pos) {
			m_position = pos;
		}
		unsigned int getLayer() const {
			return m_layer;
		}
		std::string getUUID() const {
			return m_UUID;
		}
		unsigned int getID() const {
			return m_id;
		}
		XMLModule::EntityType getType() const  {
			return m_type;
		}
		SaveDataTypes::MetaData getMetaData() const {
			return m_metaData;
		}

		void setToDraw(bool draw) {
			m_draw = draw;
		}
		void setMetaData(SaveDataTypes::MetaData& m) {
			m_metaData = m;
		}

		void generateUUID();

		virtual std::vector<ScriptingModule::Argument> generateLuaValues() {
			std::vector<ScriptingModule::Argument> args = {
				{ "selfX", std::to_string(m_position.x) },
				{ "selfY", std::to_string(m_position.y) },
				{ "selfXVel", std::to_string(m_velocity.x) },
				{ "selfYVel", std::to_string(m_velocity.y) },
				{ "selfID", std::to_string(m_id) }
			};

			m_metaData.getLuaArguments(args);

			return args;
		}

		float getDepth() {
			/// Returns the depth for drawing.
			return 0.1f + (m_layer * (1.0f / (float)(WORLD_DEPTH)) * 0.9f);
		}

	protected:
		// Collision
		bool checkTilePosition(std::vector<glm::vec2>& collideTilePositions, float xPos, float yPos);
		virtual void collideWithTile(glm::vec2 tilePos, bool ground = false);

		void updateLightLevel();
		virtual void interact();

		void moveUpLayer();
		void moveDownLayer();

		void die();

		virtual void onUpdate(float timeStep, unsigned int selfIndex) {}
		virtual void onTick() {}
		virtual void onDraw(GLEngine::SpriteBatch& sb, float time, int layerDifference, float xOffset) {}

		virtual void animate(int& x, int& y, bool& flip, float time) {}


		// Internal
		// Rendering
		void loadTexture();

		bool m_controls[6]; // Up, down (crouching while on ground), left, right, backwards (layer++), forwards (layer--)

		bool m_exposedToSun = false;
		glm::vec4 m_cornerLight; // light values at each of the 4 corners. (clockwise, component 0 is at top left
		bool m_flippedTexture = false;

		glm::vec2 m_position = glm::vec2(0.0f);
		unsigned int m_layer = 0;
		glm::vec2 m_velocity = glm::vec2(0.0f);
		bool m_onGround = false;
		bool m_draw = true;

		SaveDataTypes::MetaData m_metaData;

		// XML Attributes
		unsigned int m_id;
		GLuint m_textureId = (GLuint) - 1;
		GLuint m_bumpMapId = (GLuint) - 1;

		std::string m_texturePath;
		std::string m_bumpMapPath;

		glm::vec2 m_size = glm::vec2(1.0f);

		int m_updateScriptId = -1;
		int m_tickScriptId = -1;

		bool m_gravity = true;

		XMLModule::EntityType m_type;

		// UUID
		std::string m_UUID = "NO_UUID"; // This is a universally unique identifier, and is generated by concatenating the current gametime, a few random numbers, and the position of this entity on creation
};

Entity* createEntity(glm::vec2 pos, unsigned int layer, unsigned int id, SaveDataTypes::MetaData data, bool loadTex);
Entity* createEntity(glm::vec2 pos, unsigned int layer, EntityIDs id, SaveDataTypes::MetaData data, bool loadTex);
