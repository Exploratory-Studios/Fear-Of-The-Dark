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
        Buff(BuffTypes type, std::string name, GLuint textureId, float lingerTime = 2.0f) : m_type(type), m_name(name), m_textureId(textureId), m_lingerTime(lingerTime) { }
        virtual void update() { if(m_lingerTime <= 0.0f) m_active = false; if(m_active) { m_lingerTime--; customUpdate(); } }
        virtual void customUpdate() = 0;

        const BuffTypes getType() const { return m_type; }
        const std::string getName() const { return m_name; }
        const GLuint getTextureId() const { return m_textureId; }

        const bool isActive() const { return m_lingerTime > 0.0f; }

        bool operator==(const Buff& lhs) {
            return lhs.getName() == getName();
        }

    protected:
        float m_lingerTime = 0.0f;
        BuffTypes m_type;
        std::string m_name; // This is used as an identifier, so that we don't get multiple of the same buff
        GLuint m_textureId;
        bool m_active = true;
};

class Buff_Increase_Sanity_StarLight : public Buff { // Naming is as follows: Buff_[Type]_[What it effects]_[Creative Name]
    public:
        Buff_Increase_Sanity_StarLight(float* sanity) : Buff(BuffTypes::INCREASE, "Starlight", 0), m_sanity(sanity) { }
        virtual void customUpdate() override {
            *m_sanity += 1.0f / 60.0f / 600.0f; // If you spend 10 minutes under star light, you should return to full sanity
        }
    protected:
        float* m_sanity = nullptr;
};

class Buff_Decrease_Sanity_LowLight : public Buff { // Naming is as follows: Buff_[Type]_[What it effects]_[Creative Name]
    public:
        Buff_Decrease_Sanity_LowLight(float* sanity) : Buff(BuffTypes::DECREASE, "LowLight", 0), m_sanity(sanity) { }
        virtual void customUpdate() override {
            *m_sanity -= 1.0f / 60.0f / 300.0f; // If you spend 5 minutes in low light, you should lose 1 full bar of sanity
        }
    protected:
        float* m_sanity = nullptr;
};
