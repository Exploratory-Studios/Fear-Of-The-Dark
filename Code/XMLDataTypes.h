#pragma once

#include "SaveDataTypes.h"

#include <string>
#include <glm/glm.hpp>
#include <rapidxml/rapidxml.hpp>
#include <initializer_list>

#include <boost/variant.hpp>

namespace XMLModule {

    class GenericData;
    class TileData;

    enum class AttributeType {
        STRING,
        UNSIGNED_INT,
        INT,
        FLOAT,
        BOOL,
        FILEPATH_TEXTURE,
        FILEPATH_BUMPMAP,
        SCRIPT,
        STRING_FACTION,
        VECTOR_UNSIGNED_INT,
        VEC2
    };

    struct Attribute {
        Attribute() {}
        Attribute(std::string nameP, AttributeType typeP) {
            type = typeP;
            name = nameP;
            setDefault();
        }
        ~Attribute() {};

        void setDefault();

        AttributeType type;
        std::string name;
        boost::variant<std::string,
                       unsigned int,
                       int,
                       float,
                       bool,
                       std::vector<unsigned int>,
                       std::vector<int>,
                       glm::vec2> data; // Basically just an easier union.
    };

    class GenericData {
        /**
            Responsibilities:
             - Act as a base class, providing read and write functions
             - Hold a map of Attributes, using their name as the key.
             - Provide functions to initialize (for read/write) attributes (protected)
             - Provide a function to read map elements.
        **/
        public:
            GenericData(::std::vector<Attribute> attrs); // Children will use the constructor to list attributes in the map.
            ~GenericData() {}

            void init(::rapidxml::xml_node<>* node); // Inits all attribute values. (Read)
            void write(::rapidxml::xml_node<>* node); // Writes to XML document

            /*void getAttribute(::std::string name, std::string& dataPtr); // Just puts the data into the pointer
            void getAttribute(::std::string name, unsigned int& dataPtr); // Just puts the data into the pointer
            void getAttribute(::std::string name, int& dataPtr); // Just puts the data into the pointer
            void getAttribute(::std::string name, float& dataPtr); // Just puts the data into the pointer
            void getAttribute(::std::string name, bool& dataPtr); // Just puts the data into the pointer
            void getAttribute(::std::string name, std::vector<unsigned int>& dataPtr); // Just puts the data into the pointer
            void getAttribute(::std::string name, glm::vec2& dataPtr); // Just puts the data into the pointer*/

            template<typename T>
            void getAttribute(::std::string name, T& t);

            template<class T>
            T& getAttributeT(std::string index); // Templated, probably better version of above.

            SaveDataTypes::MetaData getMetaData() { return m_metadata; }

        protected:
            void addAttribute(std::string name, AttributeType type);

            std::unordered_map<std::string, Attribute> m_attributes;
            SaveDataTypes::MetaData m_metadata;
    };

    class TileData : public GenericData {
        public:
            TileData() : GenericData({ Attribute("texture", AttributeType::FILEPATH_TEXTURE),
                                       Attribute("bumpMap", AttributeType::FILEPATH_BUMPMAP),
                                       Attribute("emittedLight", AttributeType::FLOAT),
                                       Attribute("emittedHeat", AttributeType::FLOAT),
                                       Attribute("size", AttributeType::VEC2),
                                       Attribute("isSolid", AttributeType::BOOL),
                                       Attribute("isDrawn", AttributeType::BOOL),
                                       Attribute("isNatural", AttributeType::BOOL),
                                       Attribute("isTransparent", AttributeType::BOOL),
                                       Attribute("updateScript", AttributeType::SCRIPT),
                                       Attribute("tickScript", AttributeType::SCRIPT),
                                       Attribute("destructionScript", AttributeType::SCRIPT),
                                       Attribute("interactScript_walkedOn", AttributeType::SCRIPT),
                                       Attribute("interactScript_used", AttributeType::SCRIPT) }) {}

            virtual ~TileData() {}
    };

    class ParticleData : public GenericData {
        public:
            ParticleData() : GenericData({ Attribute("texture", AttributeType::FILEPATH_TEXTURE),
                                           Attribute("bumpMap", AttributeType::FILEPATH_BUMPMAP),
                                           Attribute("script", AttributeType::SCRIPT),
                                           Attribute("decayRate", AttributeType::FLOAT) }) {}
            virtual ~ParticleData() {}
    };

    class EntityNPCData : public GenericData {
        public:
            EntityNPCData() : GenericData({ Attribute("texture", AttributeType::FILEPATH_TEXTURE),
                                            Attribute("bumpMap", AttributeType::FILEPATH_BUMPMAP),
                                            Attribute("size", AttributeType::VEC2),
                                            Attribute("speed", AttributeType::FLOAT),
                                            Attribute("jumpHeight", AttributeType::FLOAT),
                                            Attribute("maxHealth", AttributeType::FLOAT),
                                            Attribute("faction", AttributeType::STRING_FACTION),
                                            Attribute("isDamagedByFalls", AttributeType::BOOL),
                                            Attribute("isInvincible", AttributeType::BOOL),
                                            Attribute("updateScript", AttributeType::SCRIPT),
                                            Attribute("tickScript", AttributeType::SCRIPT) }) {}

            virtual ~EntityNPCData() {}
    };

    class EntityProjectileData : public GenericData {
        public:
            EntityProjectileData() : GenericData({  Attribute("texture", AttributeType::FILEPATH_TEXTURE),
                                                    Attribute("bumpMap", AttributeType::FILEPATH_BUMPMAP),
                                                    Attribute("size", AttributeType::VEC2),
                                                    Attribute("speed", AttributeType::FLOAT),
                                                    Attribute("collides", AttributeType::BOOL),
                                                    Attribute("damage", AttributeType::FLOAT),
                                                    Attribute("updateScript", AttributeType::SCRIPT),
                                                    Attribute("tickScript", AttributeType::SCRIPT) }) {}

            virtual ~EntityProjectileData() {}
    };

    class EntityItemData : public GenericData {
        public:
            EntityItemData() : GenericData({    Attribute("texture", AttributeType::FILEPATH_TEXTURE),
                                                Attribute("bumpMap", AttributeType::FILEPATH_BUMPMAP),
                                                Attribute("size", AttributeType::VEC2),
                                                Attribute("item", AttributeType::UNSIGNED_INT),
                                                Attribute("updateScript", AttributeType::SCRIPT),
                                                Attribute("tickScript", AttributeType::SCRIPT) }) {}
    };

    class ItemData : public GenericData {
        public:
            ItemData() : GenericData({  Attribute("texture", AttributeType::FILEPATH_TEXTURE),
                                        Attribute("weight", AttributeType::FLOAT),
                                        Attribute("useScript", AttributeType::SCRIPT) }) {}
    };

    class BiomeData : public GenericData {
        public:
            BiomeData() : GenericData({ Attribute("backgroundTexture", AttributeType::FILEPATH_TEXTURE),
                                        Attribute("baseHeight", AttributeType::UNSIGNED_INT),
                                        Attribute("maxHeightDiff", AttributeType::UNSIGNED_INT),
                                        Attribute("maxTemperature", AttributeType::FLOAT),
                                        Attribute("baseTemperature", AttributeType::FLOAT),
                                        Attribute("flatness", AttributeType::FLOAT),
                                        Attribute("entities/entityID", AttributeType::VECTOR_UNSIGNED_INT) }) {}

        /// TODO:
        //std::vector<unsigned int> structureSpawnIds = {}; // All structures (defined by Structure objects)
        //std::vector<unsigned int> surfaceBlockIds = {}; // All blocks that make up the first few of the surface
        //std::vector<unsigned int> undergroundBlockIds = {}; // All blocks that make up the rest of the ground

    };

    class EraData : public GenericData {
        public:
            EraData() : GenericData({  Attribute("biomes/biomeID", AttributeType::VECTOR_UNSIGNED_INT) }) {}
    };

    class LootDropData : public GenericData {
        public:
            LootDropData() : GenericData({  Attribute("itemID", AttributeType::UNSIGNED_INT),
                                            Attribute("minDrop", AttributeType::UNSIGNED_INT),
                                            Attribute("maxDrop", AttributeType::UNSIGNED_INT),
                                            Attribute("chance", AttributeType::FLOAT) }) {}
    };

    class LootTableData : public GenericData {
        public:
            LootTableData() : GenericData({ Attribute("drops/dropID", AttributeType::VECTOR_UNSIGNED_INT) }) {}
    };

    class StructureData : public GenericData {
        public:
            StructureData() : GenericData({ Attribute("structureID", AttributeType::UNSIGNED_INT),
                                            Attribute("biomeID", AttributeType::UNSIGNED_INT),
                                            Attribute("chance", AttributeType::FLOAT),
                                            Attribute("maxAmnt", AttributeType::UNSIGNED_INT),
                                            Attribute("minAmnt", AttributeType::UNSIGNED_INT) }) {}
    };

    class QuestData : public GenericData {
        public:
            QuestData() : GenericData({ Attribute("objectives/objectiveID", AttributeType::VECTOR_UNSIGNED_INT),
                                        Attribute("completionScript", AttributeType::SCRIPT) }) {}
    };

    class QuestObjectiveData : public GenericData {
        public:
            QuestObjectiveData() : GenericData({ Attribute("text", AttributeType::STRING),
                                                 Attribute("confirmationScript", AttributeType::SCRIPT) }) {}
    };
}

#include "XMLDataTypes.tcc" // Just for templated functions.
