#include "SaveDataTypes.h"

#include "Entity.h"
#include "EntityItem.h"
#include "EntityProjectile.h"
#include "EntityPlayer.h"

#include "Scripting/ScriptQueue.h"

#include "Inventory.h"

MetaData::MetaData(std::vector<MetaData_Aspect>& data) {
    for(unsigned int i = 0; i < data.size(); i++) {
        std::pair<std::string, std::string> p = std::make_pair(data[i].key, data[i].val);
        m_data.insert(p);
    }
}

bool MetaData::getElement(std::string& key, std::string& var) {
    auto i = m_data.find(key); // Try to access data. If it doesn't exist, returns m_data.end(). If successful, it returns an iterator to data.
    if(i == m_data.end()) {
        return false; // Didn't find it
    }
    var = i->second; // Found it, set var.
    return true;
}

void MetaData::setElement(std::string& key, std::string& val) {
    m_data[key] = val; // This operator ([]) either accesses an existing element, or creates and accesses a non-existent one.
    // No matter the circumstances, the end data will have the element with value `val`
}

void MetaData::getLuaArguments(std::vector<Argument>& args) {
    /// Adds all elements from this object to the vector `args`. Does not modify previously-existing objects in `args`.
    for(auto obj : m_data) {
        // Loop through every object.
        // Create an Argument object and populate it.
        Argument arg;
        arg.key = obj.first;
        arg.val = obj.second;
        args.push_back(arg);
    }
}

void MetaData::read(std::ifstream& file) {
    // Mirroring saving, we need the size of the read.
    unsigned int length;
    file.read(reinterpret_cast<char*>(&length), sizeof(unsigned int));

    for(unsigned int i = 0; i < length; i++) { // Now here's where it gets odd.
        // As we saved each string fairly raw, with only an extra '\0' to show us where each ended, we need to read character-by-character until we find it.
        std::string key;
        while(true) { // blah, blah, infinite loops are bad, blah... We'll just break out.
            char n;
            file.read(&n, sizeof(char)); // Read next character into n.
            if(n == '\0') { // If we've found the null-terminator, break out of the loop (don't add to the 'key' string)
                break;
            } else {
                key += n; // Add characters one-by-one into the key string.
            }
        }

        // Do it all again for the value!
        std::string val;
        while(true) {
            char n;
            file.read(&n, sizeof(char)); // Read next character into n.
            if(n == '\0') { // If we've found the null-terminator, break out of the loop (don't add to the 'key' string)
                break;
            } else {
                val += n; // Add characters one-by-one into the key string.
            }
        }

        // At this point, we've extracted both the key and value, now we just need to add them!
        setElement(key, val); // Boom! Done.
    }

    // m_data should be completely constructed now.
}

void MetaData::save(std::ofstream& file) {
    // BTW: This is a costly operation, traversing an unordered_map (compared to ordered)

    // First, we need to establish the length of data.
    unsigned int length = m_data.size();
    file.write(reinterpret_cast<char*>(&length), sizeof(unsigned int));

    // Then loop through all data, writing keys and values in sequence.
    for(auto i : m_data) { // use a fancy for-each loop. Never slower than a regular for loop :)
        file.write(i.first.c_str(), sizeof(char) * i.first.length()); // Write key
        file.write(i.second.c_str(), sizeof(char) * i.second.length()); // Write value
        // Each of these c-strings include a null-terminator ('\0'), so we can identify the ends of them each like that.
    }

    // That's all!
}

void TileData::save(std::ofstream& file) {
    file.write(reinterpret_cast<char*>(&pos.x), sizeof(float));
    file.write(reinterpret_cast<char*>(&pos.y), sizeof(float));
    file.write(reinterpret_cast<char*>(&layer), sizeof(float));
    file.write(reinterpret_cast<char*>(&id), sizeof(unsigned int));
    metaData.save(file);
}

void TileData::read(std::ifstream& file) {
    file.read(reinterpret_cast<char*>(&pos.x), sizeof(float));
    file.read(reinterpret_cast<char*>(&pos.y), sizeof(float));
    file.read(reinterpret_cast<char*>(&layer), sizeof(float));
    file.read(reinterpret_cast<char*>(&id), sizeof(unsigned int));
    metaData.read(file);
}

ItemData::ItemData(Item i) : id(i.getID()), quantity(i.getQuantity()), metaData(i.getMetaData()) {

}

void ItemData::save(std::ofstream& file) {
    file.write(reinterpret_cast<char*>(&id), sizeof(unsigned int));
    file.write(reinterpret_cast<char*>(&quantity), sizeof(unsigned int));
    metaData.save(file);
}

void ItemData::read(std::ifstream& file) {
    file.read(reinterpret_cast<char*>(&id), sizeof(unsigned int));
    file.read(reinterpret_cast<char*>(&quantity), sizeof(unsigned int));
    metaData.read(file);
}

InventoryData::InventoryData(Inventory inventory) {
    for(unsigned int i = 0; i < inventory.getItems().size(); i++) {
        itemData.emplace_back(*(inventory.getItems()[i]));
    }
}

void InventoryData::save(std::ofstream& file) {
    for(unsigned int i = 0; i < itemData.size(); i++) {
        itemData[i].save(file);
    }
}

void InventoryData::read(std::ifstream& file) {
    for(unsigned int i = 0; i < itemData.size(); i++) {
        itemData[i].read(file);
    }
}

EntityData::EntityData(Entity e) : position(e.getPosition()),
                                   layer(e.getLayer()),
                                   velocity(e.getVelocity()),
                                   id(e.getID()),
                                   md(e.getMetaData()) {

}

void EntityData::save(std::ofstream& file) {
    file.write(reinterpret_cast<char*>(&position.x), sizeof(float));
    file.write(reinterpret_cast<char*>(&position.y), sizeof(float));
    file.write(reinterpret_cast<char*>(&layer), sizeof(unsigned int));
    file.write(reinterpret_cast<char*>(&velocity.x), sizeof(float));
    file.write(reinterpret_cast<char*>(&velocity.y), sizeof(float));
    file.write(reinterpret_cast<char*>(&id), sizeof(unsigned int));
    md.save(file);
}

void EntityData::read(std::ifstream& file) {
    file.read(reinterpret_cast<char*>(&position.x), sizeof(float));
    file.read(reinterpret_cast<char*>(&position.y), sizeof(float));
    file.read(reinterpret_cast<char*>(&layer), sizeof(unsigned int));
    file.read(reinterpret_cast<char*>(&velocity.x), sizeof(float));
    file.read(reinterpret_cast<char*>(&velocity.y), sizeof(float));
    file.read(reinterpret_cast<char*>(&id), sizeof(unsigned int));
    md.read(file);
}

EntityItemData::EntityItemData(EntityItem i) : EntityData(i) {

}

EntityNPCData::EntityNPCData(EntityNPC e) : inventory(*(e.getInventory())),
                                            health(e.getHealth()),
                                            EntityData(e) {

}

void EntityNPCData::save(std::ofstream& file) {
    file.write(reinterpret_cast<char*>(&position.x), sizeof(float));
    file.write(reinterpret_cast<char*>(&position.y), sizeof(float));
    file.write(reinterpret_cast<char*>(&layer), sizeof(unsigned int));
    file.write(reinterpret_cast<char*>(&velocity.x), sizeof(float));
    file.write(reinterpret_cast<char*>(&velocity.y), sizeof(float));
    file.write(reinterpret_cast<char*>(&id), sizeof(unsigned int));
    md.save(file);

    inventory.save(file);
    file.write(reinterpret_cast<char*>(&health), sizeof(float));
}

void EntityNPCData::read(std::ifstream& file) {
    file.read(reinterpret_cast<char*>(&position.x), sizeof(float));
    file.read(reinterpret_cast<char*>(&position.y), sizeof(float));
    file.read(reinterpret_cast<char*>(&layer), sizeof(unsigned int));
    file.read(reinterpret_cast<char*>(&velocity.x), sizeof(float));
    file.read(reinterpret_cast<char*>(&velocity.y), sizeof(float));
    file.read(reinterpret_cast<char*>(&id), sizeof(unsigned int));
    md.read(file);

    inventory.read(file);
    file.read(reinterpret_cast<char*>(&health), sizeof(float));
}

EntityProjectileData::EntityProjectileData(EntityProjectile p) : EntityData(p) {

}

EntityPlayerData::EntityPlayerData(EntityPlayer p) : sanity(p.getSanity()),
                                                     thirst(p.getThirst()),
                                                     hunger(p.getHunger()),
                                                     exhaustion(p.getExhaustion()),
                                                     stamina(p.getStamina()),
                                                     EntityNPCData(p) {

}

void EntityPlayerData::save(std::ofstream& file) {
    file.write(reinterpret_cast<char*>(&position.x), sizeof(float));
    file.write(reinterpret_cast<char*>(&position.y), sizeof(float));
    file.write(reinterpret_cast<char*>(&layer), sizeof(unsigned int));
    file.write(reinterpret_cast<char*>(&velocity.x), sizeof(float));
    file.write(reinterpret_cast<char*>(&velocity.y), sizeof(float));
    file.write(reinterpret_cast<char*>(&id), sizeof(unsigned int));
    md.save(file);

    inventory.save(file);
    file.write(reinterpret_cast<char*>(&health), sizeof(float));

    file.write(reinterpret_cast<char*>(&sanity), sizeof(float));
    file.write(reinterpret_cast<char*>(&thirst), sizeof(float));
    file.write(reinterpret_cast<char*>(&hunger), sizeof(float));
    file.write(reinterpret_cast<char*>(&exhaustion), sizeof(float));
    file.write(reinterpret_cast<char*>(&stamina), sizeof(float));
}

void EntityPlayerData::read(std::ifstream& file) {
    file.read(reinterpret_cast<char*>(&position.x), sizeof(float));
    file.read(reinterpret_cast<char*>(&position.y), sizeof(float));
    file.read(reinterpret_cast<char*>(&layer), sizeof(unsigned int));
    file.read(reinterpret_cast<char*>(&velocity.x), sizeof(float));
    file.read(reinterpret_cast<char*>(&velocity.y), sizeof(float));
    file.read(reinterpret_cast<char*>(&id), sizeof(unsigned int));
    md.read(file);

    inventory.read(file);
    file.read(reinterpret_cast<char*>(&health), sizeof(float));

    file.read(reinterpret_cast<char*>(&sanity), sizeof(float));
    file.read(reinterpret_cast<char*>(&thirst), sizeof(float));
    file.read(reinterpret_cast<char*>(&hunger), sizeof(float));
    file.read(reinterpret_cast<char*>(&exhaustion), sizeof(float));
    file.read(reinterpret_cast<char*>(&stamina), sizeof(float));
}

void ChunkData::save(std::ofstream& file) {
    for(int y = 0; y < WORLD_HEIGHT; y++) {
        for(int x = 0; x < CHUNK_SIZE; x++) {
            for(int k = 0; k < WORLD_DEPTH; k++) {
                tiles[y][x][k].save(file);
            }
        }
    }
    file.write(reinterpret_cast<char*>(&(place)), sizeof(unsigned int));
}

void ChunkData::read(std::ifstream& file) {
    for(int y = 0; y < WORLD_HEIGHT; y++) {
        for(int x = 0; x < CHUNK_SIZE; x++) {
            for(int k = 0; k < WORLD_DEPTH; k++) {
                tiles[y][x][k].read(file);
            }
        }
    }
    file.read(reinterpret_cast<char*>(&(place)), sizeof(unsigned int));
}






