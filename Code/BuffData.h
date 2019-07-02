#pragma once

#include <string>

enum class BuffTypes {
    INCREASE,
    DECREASE,
    HOLD,
    SET_ZERO,
    CUSTOM
};
class Buff { // Abstract type, make other child classes to actually use Buffs
    public:
        Buff(BuffTypes type, std::string name, GLuint textureId) : m_type(type), m_name(name), m_textureId(textureId) { }
        virtual void update() = 0;

        const BuffTypes getType() const { return m_type; }
        const std::string getName() const { return m_name; }
        const GLuint getTextureId() const { return m_textureId; }
    protected:
        BuffTypes m_type;
        std::string m_name;
        GLuint m_textureId;
};

class Buff_Increase_Sanity_StarLight : public Buff { // Naming is as follows: Buff_[Type]_[What it effects]_[Creative Name]
    public:
        Buff_Increase_Sanity_StarLight(float* sanity) : Buff(BuffTypes::INCREASE, "Starlight", 0), m_sanity(sanity) { }
        virtual void update() override {
            *m_sanity += 0.01f;
        }
    protected:
        float* m_sanity = nullptr;
};
