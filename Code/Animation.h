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

			void draw(::GLEngine::SpriteBatch& sb, GLEngine::ColourRGBA8 colour, glm::vec4& destRect, float& depth, float& angle, glm::vec2& COR);
			void tick();

			void setToLoop(bool loops) {
				m_loops = loops;
			}

			unsigned int getFrameWidth() {
				return m_frameWidth;
			}
			unsigned int getFrameHeight() {
				return m_frameHeight;
			}

			void setFrame(unsigned int frame) {
				m_currentFrame = frame % (m_width / m_frameWidth);
			}

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
			SkeletalAnimation(unsigned int id);

			void init(unsigned int id);

			void updateLimb(Limb* limb);
			void tick();

			bool isFinished();
			void restart();

			bool affectsLimb(unsigned int index) {
				for(unsigned int i = 0; i < m_limbIndices.size(); i++) {
					if(m_limbIndices[i] == index) {
						return true;
					}
				}
				return false;
			}

			void setToLoop(bool loops) {
				m_repeats = loops;
			}

			float getAngle(unsigned int index) {
				return m_angles[index];
			}
			glm::vec2 getOffset(unsigned int index) {
				return m_offsets[index];
			}
			glm::vec2 getCentreOfRotation(unsigned int index) {
				return m_centresOfRotation[index];
			}

			void setFrame(unsigned int frame) {
				if(m_angles.size() > 0) m_currentFrame = frame % (m_angles.size() / m_limbIndices.size());
			}

		protected:
			int m_currentFrame = 0, m_lastFrame = 0;
			bool m_repeats = false;

			std::vector<unsigned int> m_limbIndices;

			// Uses a one dimensional vector. Pattern: Limb0 Angle0, Limb1 Angle0, Limb2 Angle0, Limb0 Angle1, Limb1 Angle1, Limb2 Angle1, etc.
			std::vector<float> m_angles; // Relative to the centre of the limb.
			std::vector<glm::vec2> m_offsets; // Relative to the centre of the animation.
			std::vector<glm::vec2> m_centresOfRotation; // Relative to the bottom-left of the limb, in size units. (0.5, 0.5) is the centre of the limb.
	};

	class Limb {
			/// The limb will hold all data for limbs: position on the body, angle, texture.
		public:
			Limb();
			Limb(Animation idleAnimation, unsigned int index); // The idleAnimation is the animation that constantly runs. Most of the time, this is just a single-textured sprite which supplies the texture

			void init(Animation idleAnimation, unsigned int index);
			void activateSkeletalAnimation(SkeletalAnimation anim);

			void tick();
			void update();
			virtual void draw(GLEngine::SpriteBatch& sb, GLEngine::ColourRGBA8 colour, glm::vec4 destRect, float& depth);

			bool isAnimationActive();
			unsigned int getIndex();
			float getAngle();
			glm::vec2 getOffset();
			glm::vec2 getCentreOfRotation();

			void setAngle(float& angle);
			void setOffset(glm::vec2& offset);
			void setCentreOfRotation(glm::vec2& centre);

		protected:
			SkeletalAnimation m_activeAnimation; // This is what actually moves the limb. This is changeable
			Animation m_idleAnimation; // This is the "skin"
			bool m_isAnimated = false; // Does it have an active skeletal animation?
			unsigned int m_index; // This is used to make sure that each arm/leg follows the correct skeletal animation's limb (right leg might be 0, left leg might be 1, etc.)

			float m_angle = 0.0f;
			glm::vec2 m_centreOfRotation = glm::vec2(0.0f);
			glm::vec2 m_offset = glm::vec2(0.0f);
	};

}
