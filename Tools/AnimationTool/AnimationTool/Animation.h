#pragma once

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

			void draw(::GLEngine::SpriteBatch& sb, glm::vec4& destRect, float& depth, float& angle);
			void update();

			void setToLoop(bool loops) {
				m_loops = loops;
			}

			unsigned int getFrameWidth() { return m_frameWidth; }
			unsigned int getFrameHeight() { return m_frameHeight; }

			bool isFinished();
			void restart();

		protected:
			unsigned int m_currentFrame = 0;

			unsigned int m_textureID;
			unsigned int m_width;
			unsigned int m_frameWidth;
			unsigned int m_frameHeight;

			bool m_loops = false;

			glm::vec4 m_uv;
	};

	class SkeletalAnimation {
			// Provides angles and relative positions for all limbs in a creature. Most of the time, this will just be a torso (and head), 2 arms, and 2 legs.
		public:
			SkeletalAnimation();

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

		protected:
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

			void activateSkeletalAnimation(SkeletalAnimation anim);

			void tick();
			void update();
			virtual void draw(GLEngine::SpriteBatch& sb, glm::vec4 destRect, float& depth);

			bool isAnimationActive();
			unsigned int getIndex();
			float getAngle();
			glm::vec2 getOffset();

			void setAngle(float& angle);
			void setOffset(glm::vec2& offset);

		protected:
			SkeletalAnimation m_activeAnimation; // This is what actually moves the limb. This is changeable
			Animation m_idleAnimation; // This is the "skin"
			bool m_isAnimated = false; // Does it have an active skeletal animation?
			unsigned int m_index; // This is used to make sure that each arm/leg follows the correct skeletal animation's limb (right leg might be 0, left leg might be 1, etc.)

			float m_angle = 0.0f;
			glm::vec2 m_offset = glm::vec2(0.0f);
	};

}
