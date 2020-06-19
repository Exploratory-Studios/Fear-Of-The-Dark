#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include <memory>
#include <fstream>

#include "PresetValues.h"


class lua_State;
class Entity;
class EntityNPC;
class EntityProjectile;
class EntityItem;
class EntityPlayer;
class Inventory;
class Item;

namespace ScriptingModule { class Argument; }

namespace SaveDataTypes {

    struct MetaData_Aspect {
        MetaData_Aspect(std::string nameP, int valP) : key(nameP), val(std::to_string(valP)) {}
        MetaData_Aspect(std::string nameP, float valP) : key(nameP), val(std::to_string(valP)) {}
        MetaData_Aspect(std::string nameP, std::string valP) : key(nameP), val(valP) {}

        std::string key;
        std::string val;
    };

    class MetaData {
        public:
            MetaData() {} // Simply initializes empty data fields
            MetaData(std::vector<MetaData_Aspect>& data); // Constructs a metadata object with the given data.

            // Attempts to retrieve an element's value with key key, and place its value into the variable var. Returns true if successful, false otherwise.
            bool getElement(std::string& key, std::string& var);
            // Sets an element's value using the given key and value.
            void setElement(std::string& key, std::string& val);

            std::string getElements(); // Returns a string of all elements. used for debugging

            void getLuaArguments(std::vector<::ScriptingModule::Argument>& args);

            void readFromLuaTable(lua_State* state, int tableIndex); // Use the "::" to reference the global namespace

            MetaData& operator+=(MetaData& other) {
                /// Adds all elements from the other to this.
                for(auto obj : other.m_data) {
                    std::string key, val;

                    key = obj.first;
                    val = obj.second;

                    setElement(key, val);
                }

                return *this;
            }

            // Truncates data from the given file into this object
            void read(std::ifstream& file);
            // Concatenates data from this object to the given file
            void save(std::ofstream& file);

        private:
            void init(std::vector<MetaData_Aspect>& data);

            std::unordered_map<std::string, std::string> m_data; // unordered maps are better for non-data traversal situations. This is one of those.
    };

    class TileData {
        public:
            void save(std::ofstream& file);
            void read(std::ifstream& file);

            glm::vec2 pos;
            float layer;
            unsigned int id;
            MetaData metaData;
    };

    struct ItemData {
        ItemData() {}
        ItemData(Item i);

        void read(std::ifstream& file);
        void save(std::ofstream& file);

        unsigned int id;
        unsigned int quantity;
        MetaData metaData;
    };

    struct InventoryData {
        InventoryData() {}
        InventoryData(Inventory inv);

        void read(std::ifstream& file);
        void save(std::ofstream& file);

        std::vector<ItemData> itemData;
    };

    struct EntityData {
        virtual void read(std::ifstream& file);
        virtual void save(std::ofstream& file);

        glm::vec2 position;
        unsigned int layer;
        glm::vec2 velocity;
        unsigned int id;
        MetaData md;

    protected:
        EntityData() {}
        EntityData(Entity e);

    };

    struct EntityItemData : public EntityData {
        EntityItemData() : EntityData() {}
        EntityItemData(EntityItem i);
    };

    struct EntityNPCData : public EntityData {
        EntityNPCData() : EntityData() {}
        EntityNPCData(EntityNPC e);

        virtual void read(std::ifstream& file);
        virtual void save(std::ofstream& file);

        InventoryData inventory;
        float health;
    };

    struct EntityProjectileData : public EntityData {
        EntityProjectileData() : EntityData() {}
        EntityProjectileData(EntityProjectile p);
    };

    struct EntityPlayerData : public EntityNPCData {
        EntityPlayerData() : EntityNPCData() {}
        EntityPlayerData(EntityPlayer p);

        virtual void read(std::ifstream& file);
        virtual void save(std::ofstream& file);

        float sanity;
        float thirst;
        float hunger;
        float exhaustion;
        float stamina;
    };

    class ChunkData {
    public:
        void save(std::ofstream& file);
        void read(std::ifstream& file);

        TileData tiles[WORLD_HEIGHT][CHUNK_SIZE][WORLD_DEPTH] {};
        unsigned int biomeID;
    };

    class WorldData {
        void save(std::ofstream& file);
        void read(std::ifstream& file);

        ChunkData chunks[WORLD_SIZE / CHUNK_SIZE] {};
        std::vector<EntityData> entities;
    };

}



