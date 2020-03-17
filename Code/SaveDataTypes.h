#pragma once

struct MetaData_Aspect {
    MetaData_Aspect(std::string nameP, int valP) : name(nameP), val(std::to_string(valP)) {}
    MetaData_Aspect(std::string nameP, float valP) : name(nameP), val(std::to_string(valP)) {}
    MetaData_Aspect(std::string nameP, std::string valP) : name(nameP), val(valP) {}

    std::string name;
    std::string val;
};

class MetaData {
    public:
        MetaData(std::initializer_list<MetaData_Aspect> data) {
            m_data = "";
            if(data.size() > 0) {
                for(auto& d : data) {
                    m_data += d.name + ":" + d.val + ",";
                }
                if(m_data.size() > 0) m_data.pop_back();
            }
        }
        MetaData(std::string data) {
            m_data = data;
        }
        MetaData() {
            m_data = "";
        }

        void addAspect(std::string aspectName, float val) {
            if(m_data.find(aspectName) == std::string::npos) {
                m_data += "," + aspectName + ":" + std::to_string(val);
            } else {
                m_data.replace(m_data.find(aspectName) + aspectName.size()+1, m_data.find(',', m_data.find(aspectName)), std::to_string(val));
            }
        }
        void addAspect(std::string aspectName, int val) {
            if(m_data.find(aspectName) == std::string::npos) {
                m_data += "," + aspectName + ":" + std::to_string(val);
            } else {
                m_data.replace(m_data.find(aspectName) + aspectName.size()+1, m_data.find(',', m_data.find(aspectName)), std::to_string(val));
            }
        }
        void addAspect(std::string aspectName, std::string val) {
            if(m_data.find(aspectName) == std::string::npos) {
                m_data += "," + aspectName + ":" + val;
            } else {
                m_data.replace(m_data.find(aspectName) + aspectName.size()+1, m_data.find(',', m_data.find(aspectName)), val);
            }
        }

        void save(std::ofstream& file) {
            unsigned int len = m_data.size();
            file.write(reinterpret_cast<char*>(&len), sizeof(unsigned int));
            if(len > 0) file.write(m_data.c_str(), m_data.size());
        }

        void read(std::ifstream& file) {
            unsigned int len;
            file.read(reinterpret_cast<char*>(&len), sizeof(unsigned int));
            char* data = new char();
            if(len > 0) {
                file.read(&data[0], len);
                m_data = std::string(data);
            } else {
                m_data = "";
            }
        }

        bool getAspect(std::string name, int& variable) {
            // Finds an aspect's value based on a given name, using string::find();, gives it to the variable
            if(m_data.find(name) != std::string::npos) {
                std::string ret = "";
                std::string str = m_data.substr(m_data.find(name), std::string::npos);
                str = str.substr(str.find(':')+1, std::string::npos); // Now we have only the value, and everything after
                for(auto& s : str) {
                    if(s == ',') {
                        break;
                    }
                    ret += s;
                }
                variable = std::stoi(ret);
                return true;
            } else {
                return false;
            }
        }
        bool getAspect(std::string name, float& variable) {
            // Finds an aspect's value based on a given name, using string::find();, gives it to the variable
            if(m_data.find(name) != std::string::npos) {
                std::string ret = "";
                std::string str = m_data.substr(m_data.find(name), std::string::npos);
                str = str.substr(str.find(':')+1, std::string::npos); // Now we have only the value, and everything after
                for(auto& s : str) {
                    if(s == ',') {
                        break;
                    }
                    ret += s;
                }
                variable = std::stof(ret);
                return true;
            } else {
                return false;
            }
        }
        bool getAspect(std::string name, std::string& variable) {
            // Finds an aspect's value based on a given name, using string::find();, gives it to the variable
            if(m_data.find(name) != std::string::npos) {
                std::string ret = "";
                std::string str = m_data.substr(m_data.find(name), std::string::npos);
                str = str.substr(str.find(':')+1, std::string::npos); // Now we have only the value, and everything after
                for(auto& s : str) {
                    if(s == ',') {
                        break;
                    }
                    ret += s;
                }
                variable = ret;
                return true;
            } else {
                return false;
            }
        }
    private:
        std::string m_data = "";
};

class TileData {
    public:
        void save(std::ofstream& file) {
            file.write(reinterpret_cast<char*>(&pos.x), sizeof(float));
            file.write(reinterpret_cast<char*>(&pos.y), sizeof(float));
            file.write(reinterpret_cast<char*>(&layer), sizeof(float));
            file.write(reinterpret_cast<char*>(&id), sizeof(unsigned int));
            metaData->save(file);
        }

        void read(std::ifstream& file) {
            file.read(reinterpret_cast<char*>(&pos.x), sizeof(float));
            file.read(reinterpret_cast<char*>(&pos.y), sizeof(float));
            file.read(reinterpret_cast<char*>(&layer), sizeof(float));
            file.read(reinterpret_cast<char*>(&id), sizeof(unsigned int));
            metaData->read(file);
        }

        glm::vec2 pos;
        float layer;
        unsigned int id;
        MetaData* metaData = new MetaData();
};

struct ItemData {
    unsigned int id;
    unsigned int quantity;
    MetaData* metaData = new MetaData();
};

struct PlayerInventoryData {
    unsigned int items; // number of items to load
    std::vector<ItemData> itemData;
    float absMaxWeight;
};

struct PlayerData {
    bool canInteract;
    float m_sanity, m_health, m_thirst, m_hunger, m_exhaustion, m_stamina;
    glm::vec2 position;
    unsigned int favouriteItemIndices[10];
    PlayerInventoryData inventory;
};

class ChunkData {
    public:
        void save(std::ofstream& file) {
            for(int y = 0; y < WORLD_HEIGHT; y++) {
                for(int x = 0; x < CHUNK_SIZE; x++) {
                    for(int k = 0; k < WORLD_DEPTH; k++) {
                        tiles[y][x][k].save(file);
                    }
                }
            }
            file.write(reinterpret_cast<char*>(&(place)), sizeof(unsigned int));
        }
        void read(std::ifstream& file) {
            for(int y = 0; y < WORLD_HEIGHT; y++) {
                for(int x = 0; x < CHUNK_SIZE; x++) {
                    for(int k = 0; k < WORLD_DEPTH; k++) {
                        tiles[y][x][k].read(file);
                    }
                }
            }
            file.read(reinterpret_cast<char*>(&(place)), sizeof(unsigned int));
        }
        //std::vector<EntityData> entities;
        /// TODO: create entities the same way blocks and items are created (Blocks.h & Items.h)
        TileData tiles[WORLD_HEIGHT][CHUNK_SIZE][WORLD_DEPTH] {};
        Categories::Places place;
};

struct StructureData {
    unsigned int id;
    unsigned int width;
    std::vector<TileData> tiles; // really long, but not 2d. Use width variable to convert to 2d: pos=(int)(x / width) + x % width
};
