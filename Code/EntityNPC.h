#pragma once

#include "Entity.h"

struct NavTile { // Pretty much the same thing as a navmesh
    NavTile() {}
    NavTile* parent = nullptr;
    std::vector<glm::vec3> nextNodes;
    glm::vec3 pos;
    float h = 0; // Heuristic (h(n))   ->   absolute distance from end goal.
    bool visited = false;
};

class Item;
class Inventory;
class Tile;

class EntityNPC : public Entity
{
    public:
        EntityNPC(glm::vec2 pos, unsigned int layer, unsigned int id, MetaData data, bool loadTex);
        EntityNPC(glm::vec2 pos, unsigned int layer, EntityIDs id, MetaData data, bool loadTex);
        virtual ~EntityNPC();

        void onTalk() {}
        void onTrade() {}

        virtual void collide(World* world, unsigned int entityIndex);
        virtual void onUpdate(World* world, float timeStep, unsigned int selfIndex);

        /// Getters
        Categories::Faction getFaction() const { return m_faction; }
        float getHealth() const { return m_health; }

        // Inventory
        void giveItem(Item* item);
        Inventory* getInventory();

        // Combat
        void die(World* world);
        void attack();

        // AI
        void setAITarget(World* world, unsigned int selfIndex); /// TODO: Make this work with certain quests, etc.

    protected:
        // Pathfinding
        void updateMovement(World* world);
        void pathfindToTarget(World* world, glm::vec3 target, bool goLeft);
        bool     fitsOnTile(World* world, Tile* t, bool needsFloor = false);
        NavTile* expandTile(World* world, glm::vec3 pos, int jumpHeight, glm::vec2 size, NavTile* parent, glm::vec3 target);
        void     calculateCost(World* world, NavTile* tile, glm::vec3 target); // Calculates a cost of a single path from an endnode
        void     addToFrontier(NavTile* tile, std::vector<NavTile*>& frontier); // Makes sure there are no copies, and handles them if there are.
        std::vector<glm::vec3> m_targets;
        unsigned int m_curTarget = 0;

        // Rendering
        virtual void animate(int& x, int& y, bool& flip, float time) override;

        // Fall damage
        float m_fallenDistance = 0.0f;
        bool m_takesFallDamage = true;

        // Dialogue/Trade
        unsigned int m_tradeTable = (unsigned int)-1;
        unsigned int m_dialogueID = (unsigned int)-1;
        Categories::Faction m_faction;

        // Loot
        //DropDatum m_lootTableStart = DropDatum(0, (ItemIDs)(0), 0, 0, -1.0f);
        unsigned int m_lootRolls = 0; // How many times the table is rolled on death
        /// TODO: Re-enable loot/drops
        // Combat
        int m_attackScript = -1;
        float m_maxHealth = 100.0f;
        float m_health = m_maxHealth;
        bool m_isDead = false;
        bool m_canDie = true;
        bool m_flying = false; // Can it fly?

        // Movement
        float m_runSpeed;
        float m_jumpHeight; // Maximum height = (m_jumpHeight^2)/(2*1.225/60)
                            // m_jumpHeight = sqrt(2*(1.225/60)*(maximum height))

        // Inventory
        Inventory* m_inventory = nullptr;
};
