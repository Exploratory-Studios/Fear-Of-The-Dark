#pragma once

#include "XMLData.h"

#include <glm/glm.hpp>
#include <SpriteBatch.h>
#include <GLTexture.h>

namespace AnimationModule {

	class Animation;
	class SkeletalAnimation;
	class Limb;

	class Animation {
		public:
			Animation();
			Animation(unsigned int id);
			void init(unsigned int id);

			void draw(::GLEngine::SpriteBatch& sb, glm::vec4& destRect, float& depth, float& angle);
			void update();

			bool isFinished();
			void restart();

		private:
			unsigned int m_currentFrame = 0;

			unsigned int m_textureID;
			unsigned int m_width;
			unsigned int m_frameWidth;

			glm::vec4 m_uv;
	};

	class SkeletalAnimation {
			// Provides angles and relative positions for all limbs in a creature. Most of the time, this will just be a torso (and head), 2 arms, and 2 legs.
		public:
			SkeletalAnimation();
			SkeletalAnimation(unsigned int id);

			void init(unsigned int id);

			void updateLimb(Limb* limb);
			void update();

			bool isFinished();
			void restart();

			float getAngle(unsigned int index) {
				return m_angles[index];
			}
			glm::vec2 getOffset(unsigned int index) {
				return m_offsets[index];
			}

		private:
			int m_currentFrame = 0;
			bool m_repeats = false;

			unsigned int m_numLimbs = 5;

			// Uses a one dimensional vector. Pattern: Limb0 Angle0, Limb1 Angle0, Limb2 Angle0, Limb0 Angle1, Limb1 Angle1, Limb2 Angle1, etc.
			std::vector<float> m_angles; // Relative to the centre of the limb.
			std::vector<glm::vec2> m_offsets; // Relative to the centre of the animation.
	};

	class Limb {
			/// The limb will hold all data for limbs: position on the body, angle, texture.
		public:
			Limb();
			Limb(Animation idleAnimation, unsigned int index); // The idleAnimation is the animation that constantly runs. Most of the time, this is just a single-textured sprite which supplies the texture

			void init(Animation idleAnimation, unsigned int index);
			void activateSkeletalAnimation(SkeletalAnimation anim);

			void tick();
			void draw(GLEngine::SpriteBatch& sb, glm::vec4 destRect, float& depth);

			bool isAnimationActive();
			unsigned int getIndex();
			float getAngle();
			glm::vec2 getOffset();

			void setAngle(float& angle);
			void setOffset(glm::vec2& offset);

		private:
			SkeletalAnimation m_activeAnimation; // This is what actually moves the limb. This is changeable
			Animation m_idleAnimation; // This is the "skin"
			bool m_isAnimated = false; // Does it have an active skeletal animation?
			unsigned int m_index; // This is used to make sure that each arm/leg follows the correct skeletal animation's limb (right leg might be 0, left leg might be 1, etc.)

			float m_angle = 0.0f;
			glm::vec2 m_offset;
	};

}
