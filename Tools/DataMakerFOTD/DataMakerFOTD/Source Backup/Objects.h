#pragma once

enum class ValueType {
    STRING,
    INT,
    FLOAT,
    BOOL,
    VECTOR_INT // Vector of integers
};

struct Attribute {
    Attribute(std::string nameP, ValueType typeP) : name(nameP), type(typeP) {}
    Attribute(std::string nameP, ValueType typeP, std::string& valueP) : name(nameP), type(typeP), value(valueP) {}
    Attribute(std::string nameP, ValueType typeP, float& valueP) : name(nameP), type(typeP), value(std::to_string(valueP)) {}
    Attribute(std::string nameP, ValueType typeP, int& valueP) : name(nameP), type(typeP), value(std::to_string(valueP)) {}
    Attribute(std::string nameP, ValueType typeP, unsigned int& valueP) : name(nameP), type(typeP), value(std::to_string(valueP)) {}
    Attribute(std::string nameP, ValueType typeP, bool& valueP) : name(nameP), type(typeP), value(std::to_string(valueP)) {}
    Attribute(std::string nameP, ValueType typeP, std::vector<unsigned int>& valueP) : name(nameP), type(typeP) {
        for(unsigned int i = 0; i < valueP.size(); i++) {
            if(i > 0) value += ",";
            value += std::to_string(valueP[i]);
        }
    }


    std::string name;
    std::string value;
    ValueType type;
};

class Object {
    public:
        Object(std::string name, unsigned int ID) : m_name(name), m_id(ID) { m_attributes.emplace_back("name", ValueType::STRING, m_name); }

        std::string m_name;
        unsigned int m_id;

        std::vector<Attribute> m_attributes;
        std::vector<Attribute*> getAttributes() { std::vector<Attribute*> ret; for(int i = 0; i < m_attributes.size(); i++) { ret.push_back(&m_attributes[i]); } return ret; }
};

// ItemEntity, Projectile, Particle, Block, Item, NPC, Biome, Era, LootDrop, LootTable, StructureData

class ItemEntity : public Object {
    public:
        ItemEntity(std::string name, unsigned int ID) : Object(name, ID) {
            m_attributes.emplace_back("texture", ValueType::STRING);
            m_attributes.emplace_back("bumpMap", ValueType::STRING);
            m_attributes.emplace_back("sizeX", ValueType::FLOAT);
            m_attributes.emplace_back("sizeY", ValueType::FLOAT);
            m_attributes.emplace_back("updateScript", ValueType::STRING);
            m_attributes.emplace_back("tickScript", ValueType::STRING);
            m_attributes.emplace_back("item", ValueType::INT);
        }


};

class Projectile : public Object {
    public:
        Projectile(std::string name, unsigned int ID) : Object(name, ID) {
            m_attributes.emplace_back("texture", ValueType::STRING);
            m_attributes.emplace_back("bumpMap", ValueType::STRING);
            m_attributes.emplace_back("sizeX", ValueType::FLOAT);
            m_attributes.emplace_back("sizeY", ValueType::FLOAT);
            m_attributes.emplace_back("updateScript", ValueType::STRING);
            m_attributes.emplace_back("tickScript", ValueType::STRING);
            m_attributes.emplace_back("collides", ValueType::BOOL);
            m_attributes.emplace_back("speed", ValueType::FLOAT);
            m_attributes.emplace_back("damage", ValueType::FLOAT);
        }
};

class Particle : public Object {
    public:
        Particle(std::string name, unsigned int ID) : Object(name, ID) {
            m_attributes.emplace_back("texture", ValueType::STRING);
            m_attributes.emplace_back("bumpMap", ValueType::STRING);
            m_attributes.emplace_back("script", ValueType::STRING);
        }
};

class Block : public Object {
    public:
        Block(std::string name, unsigned int ID) : Object(name, ID) {
            m_attributes.emplace_back("texture", ValueType::STRING);
            m_attributes.emplace_back("bumpMap", ValueType::STRING);
            m_attributes.emplace_back("emittedLight", ValueType::FLOAT);
            m_attributes.emplace_back("emittedHeat", ValueType::FLOAT);
            m_attributes.emplace_back("sizeX", ValueType::FLOAT);
            m_attributes.emplace_back("sizeY", ValueType::FLOAT);
            m_attributes.emplace_back("isSolid", ValueType::BOOL);
            m_attributes.emplace_back("isDrawn", ValueType::BOOL);
            m_attributes.emplace_back("isNatural", ValueType::BOOL);
            m_attributes.emplace_back("isTransparent", ValueType::BOOL);
            m_attributes.emplace_back("updateScript", ValueType::STRING);
            m_attributes.emplace_back("tickScript", ValueType::STRING);
            m_attributes.emplace_back("destructionScript", ValueType::STRING);

            m_attributes.emplace_back("interactScript_walkedOn", ValueType::STRING);
            m_attributes.emplace_back("interactScript_used", ValueType::STRING);
        }
};

class Item : public Object {
    public:
        Item(std::string name, unsigned int ID) : Object(name, ID) {
            m_attributes.emplace_back("texture", ValueType::STRING);
            m_attributes.emplace_back("weight", ValueType::FLOAT);
            m_attributes.emplace_back("useScript", ValueType::STRING);
        }
};

class NPC : public Object {
    public:
        NPC(std::string name, unsigned int ID) : Object(name, ID) {
            m_attributes.emplace_back("texture", ValueType::STRING);
            m_attributes.emplace_back("bumpMap", ValueType::STRING);
            m_attributes.emplace_back("sizeX", ValueType::FLOAT);
            m_attributes.emplace_back("sizeY", ValueType::FLOAT);
            m_attributes.emplace_back("updateScript", ValueType::STRING);
            m_attributes.emplace_back("tickScript", ValueType::STRING);
            m_attributes.emplace_back("isDamagedByFalls", ValueType::BOOL);
            m_attributes.emplace_back("isInvincible", ValueType::BOOL);
            m_attributes.emplace_back("speed", ValueType::FLOAT);
            m_attributes.emplace_back("jumpHeight", ValueType::FLOAT);
        }
};

class Biome : public Object {
    public:
        Biome(std::string name, unsigned int ID) : Object(name, ID) {
            m_attributes.emplace_back("backgroundTexture", ValueType::STRING);
            m_attributes.emplace_back("baseHeight", ValueType::INT);
            m_attributes.emplace_back("maxHeightDiff", ValueType::INT);
            m_attributes.emplace_back("maxTemperature", ValueType::FLOAT);
            m_attributes.emplace_back("baseTemperature", ValueType::FLOAT);
            m_attributes.emplace_back("flatness", ValueType::FLOAT);

            m_attributes.emplace_back("entities/entityID", ValueType::VECTOR_INT);
            m_attributes.emplace_back("structures/structure", ValueType::VECTOR_INT);
            m_attributes.emplace_back("surfaceBlocks/blockID", ValueType::VECTOR_INT);
            m_attributes.emplace_back("groundBlocks/blockID", ValueType::VECTOR_INT);
        }
};

class Era : public Object {
    public:
        Era(std::string name, unsigned int ID) : Object(name, ID) {
            m_attributes.emplace_back("biomes/biomeID", ValueType::VECTOR_INT);
        }
};

class LootDrop : public Object {
    public:
        LootDrop(std::string name, unsigned int ID) : Object(name, ID) {
            m_attributes.emplace_back("itemID", ValueType::INT);
            m_attributes.emplace_back("minDrop", ValueType::INT);
            m_attributes.emplace_back("maxDrop", ValueType::INT);
            m_attributes.emplace_back("chance", ValueType::FLOAT);
        }
};

class LootTable : public Object {
    public:
        LootTable(std::string name, unsigned int ID) : Object(name, ID) {
            m_attributes.emplace_back("drops/dropID", ValueType::VECTOR_INT);
        }
};

class Structure : public Object {
    public:
        Structure(std::string name, unsigned int ID) : Object(name, ID) {
            m_attributes.emplace_back("structureFilepath", ValueType::STRING);
            m_attributes.emplace_back("biomes/biomeID", ValueType::VECTOR_INT);
            m_attributes.emplace_back("chance", ValueType::FLOAT);
            m_attributes.emplace_back("maxAmnt", ValueType::INT);
            m_attributes.emplace_back("minAmnt", ValueType::INT);
        }
};
