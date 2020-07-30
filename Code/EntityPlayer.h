#pragma once

#include <GUI.h>
#include <Camera2D.h>
#include <SpriteFont.h>
#include <SpriteBatch.h>
#include <InputManager.h>

#include "EntityNPC.h"

class WorldIOManager;

class ArmourAttackWrapper {
		// Holds functions and objects for the Armour/Attack changing screen
	public:
		ArmourAttackWrapper(std::string& UUID, std::shared_ptr<NPCInventory> inventory);
		~ArmourAttackWrapper();

		void setToDraw(bool& setting);

		void draw(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, float x, float y); // This draws our armourGrid, attacksGrid, and inventory.
		void update();

	private:
		CEGUI::FrameWindow* m_window = nullptr;
		std::shared_ptr<InventoryBase> m_armourGrid; // These InventoryBase classes allow us to draw, disable resizing, etc.
		std::shared_ptr<InventoryBase> m_attacksGrid;
		std::shared_ptr<NPCInventory> m_inventory;
};

class EntityPlayer : public EntityNPC {
		friend class WorldIOManager;

	public:
		EntityPlayer(glm::vec2 pos, unsigned int layer, SaveDataTypes::MetaData data, bool loadTex);
		virtual ~EntityPlayer();

		void initGUI(); /// TODO: Move GUI to GameplayScreen

		virtual void onDraw(GLEngine::SpriteBatch& sb, float time, int layerDifference, float xOffset) override;
		void drawGUI(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf);

		void updateStats(World* world, float timeStep);
		void updateMouse(World* world, glm::vec2 mouseCoords);
		void updateInput(GLEngine::InputManager* input, World* world);

		/// Getters
		float getSanity() {
			return m_sanity;
		}
		float getThirst() {
			return m_thirst;
		}
		float getHunger() {
			return m_hunger;
		}
		float getExhaustion() {
			return m_exhaustion;
		}
		float getStamina() {
			return m_stamina;
		}
		float getTemperature() {
			return 1;
		}
		Entity* getSelectedEntity() {
			return m_selectedEntity;
		}
		Tile* getSelectedBlock() {
			return m_selectedBlock;
		}
		bool canInteract() const {
			return m_canInteract && !m_inventoryOpen && !m_bagOpen;
		}

		/// Setters
		void showInventory(bool open) {
			m_bagOpen = open;
		}
		void setSanity(float sanity) {
			m_sanity = sanity;    // I'm running low on this
		}
		void setCanInteract(bool setting) {
			m_canInteract = setting;
		}

		SaveDataTypes::EntityPlayerData getPlayerSaveData();

	protected:
		// GUI
		bool m_bagOpen = false; // "Quick inventory"
		bool m_inventoryOpen = false; // Armour and attacks
		bool m_questListOpen = false;
		bool m_skillTreeOpen = false;

		ArmourAttackWrapper* m_armourAttackWrapper = nullptr;

		GLEngine::InputManager* m_input = nullptr; /// TODO: What the hell, past Davis??? Move this shit outta here!

		CEGUI::PopupMenu* m_statusBoxFrame = nullptr;
		CEGUI::PopupMenu* m_buffBoxFrame = nullptr;

		CEGUI::ProgressBar* m_sanityBar = nullptr;
		CEGUI::ProgressBar* m_healthBar = nullptr;
		CEGUI::ProgressBar* m_thirstBar = nullptr;
		CEGUI::ProgressBar* m_hungerBar = nullptr;
		CEGUI::ProgressBar* m_exhaustionBar = nullptr;
		CEGUI::ProgressBar* m_staminaBar = nullptr;

		CEGUI::DefaultWindow* m_statusBoxLabel = nullptr;


		// Selection
		Tile* m_selectedBlock = nullptr;
		Entity* m_selectedEntity = nullptr;
		int m_selectedHotbox = 0;


		// Internal
		bool m_debuggingInfo = false; // FPS, selectedBlock, etc.
		bool m_canInteract = true; // Sets if the player can't interact with objects, people, etc. Used when player is in cutscenes, talking, etc.
		// ItemBlock* m_favouriteBlocks; // For building hotbar


		// Stats
		float m_sanity = 1.0f; // Fear level will be displayed as a heartbeat sound (fear is how fast your sanity drops)
		float m_health = 1.0f;
		float m_thirst = 1.0f;
		float m_hunger = 1.0f;
		float m_exhaustion = 1.0f;
		float m_stamina = 1.0f;
};
