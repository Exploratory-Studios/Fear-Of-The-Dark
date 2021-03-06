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
		~Animation() {
		}

		void init(unsigned int id);

		void draw(::GLEngine::SpriteBatch& sb,
				  GLEngine::ColourRGBA8	   colour,
				  glm::vec4&			   destRect,
				  float&				   depth,
				  float&				   angle,
				  glm::vec2&			   COR,
				  bool					   flipped);
		void draw(::GLEngine::SpriteBatch& sb,
				  GLEngine::ColourRGBA8	   colour,
				  glm::vec4&			   destRect,
				  float&				   depth,
				  glm::vec2				   direction);

		void drawNormal(::GLEngine::SpriteBatch& sb,
						glm::vec4&				 destRect,
						float&					 depth,
						float&					 angle,
						glm::vec2&				 COR,
						bool					 flipped);
		void drawNormal(::GLEngine::SpriteBatch& sb, glm::vec4& destRect, float& depth, glm::vec2 direction);

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
		unsigned int m_normalMapID;
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
		~SkeletalAnimation() {};

		void init(unsigned int id);

		void updateLimb(Limb* limb);
		void transitionLimb(Limb* limb);
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
		unsigned int getFrames() {
			return m_angles.size();
		}

		void setFrame(unsigned int frame) {
			if(m_angles.size() > 0)
				m_currentFrame = frame % (m_angles.size() / m_limbIndices.size());
		}
		bool isChanging() {
			return m_changing;
		}
		void setChanging(bool setting) {
			m_changing = setting;
		}

	  protected:
		int	 m_currentFrame = 0, m_lastFrame = 0;
		bool m_repeats	= false;
		bool m_changing = false;

		std::vector<unsigned int> m_limbIndices;

		// Uses a one dimensional vector. Pattern: Limb0 Angle0, Limb1 Angle0, Limb2 Angle0, Limb0 Angle1, Limb1 Angle1, Limb2 Angle1, etc.
		std::vector<float>	   m_angles;  // Relative to the centre of the limb.
		std::vector<glm::vec2> m_offsets; // Relative to the centre of the animation.
		std::vector<glm::vec2>
			m_centresOfRotation; // Relative to the bottom-left of the limb, in size units. (0.5, 0.5) is the centre of the limb.
	};

	class Limb {
		/// The limb will hold all data for limbs: position on the body, angle, texture.
	  public:
		Limb();
		Limb(Animation defaultAnimation,
			 unsigned int
				 index); // The idleAnimation is the animation that constantly runs. Most of the time, this is just a single-textured sprite which supplies the texture
		~Limb() {
		}

		void init(Animation defaultAnimation, unsigned int index);

		void activateSkeletalAnimation(SkeletalAnimation* anim);
		void changeSkeletalAnimation(
			SkeletalAnimation* anim); // blends last one with this one, so we get a smooth start.

		void setAnimation(Animation& anim); // Sets the idleAnimation
		void resetAnimation();				// Sets the idleAnimation to what it should be according to XML

		void		 tick();
		void		 update();
		virtual void draw(GLEngine::SpriteBatch& sb,
						  GLEngine::ColourRGBA8	 colour,
						  glm::vec4				 destRect,
						  float&				 depth,
						  bool					 flipped);
		virtual void drawNormal(GLEngine::SpriteBatch& sb, glm::vec4 destRect, float& depth, bool flipped);

		bool		 isAnimationActive();
		unsigned int getIndex();
		float		 getAngle();
		glm::vec2	 getOffset();
		glm::vec2	 getCentreOfRotation();

		void setAngle(float& angle);
		void setOffset(glm::vec2& offset);
		void setCentreOfRotation(glm::vec2& centre);

	  protected:
		SkeletalAnimation* m_activeAnimation = nullptr; // This is what actually moves the limb. This is changeable
		SkeletalAnimation* m_nextAnimation	 = nullptr; // For "blending" from an attack to a flinch, let's say.
		Animation		   m_idleAnimation;				// This is the "skin"
		Animation		   m_defaultAnimation;			// This is the defautl skin
		unsigned int
			m_index; // This is used to make sure that each arm/leg follows the correct skeletal animation's limb (right leg might be 0, left leg might be 1, etc.)

		float	  m_angle			 = 0.0f;
		glm::vec2 m_centreOfRotation = glm::vec2(0.0f);
		glm::vec2 m_offset			 = glm::vec2(0.0f);
	};

	class Body {
		// Basically just a wrapper class for a skeletal animation, a vector of limbs
	  public:
		Body() {
		}
		Body(std::vector<Limb> limbs) : m_limbs(limbs) {
		}
		~Body() {
			m_animation = nullptr;
			m_limbs.clear();
		};

		void update() {
			for(unsigned int i = 0; i < m_limbs.size(); i++) {
				m_limbs[i].update();
			}
		}

		void tick() {
			if(m_animation) {
				if(!m_animation->isChanging()) {
					m_animation->tick();
				}
			}
		}

		void draw(GLEngine::SpriteBatch& sb,
				  GLEngine::ColourRGBA8	 colour,
				  glm::vec4				 destRect,
				  float&				 depth,
				  bool					 flipped) {
			for(int i = m_limbs.size() - 1; i >= 0; i--) {
				float d = depth - i * 0.001f;
				m_limbs[i].draw(sb, colour, destRect, d, flipped);
			}
		}

		void drawNormal(GLEngine::SpriteBatch& sb, glm::vec4 destRect, float& depth, bool flipped) {
			for(int i = m_limbs.size() - 1; i >= 0; i--) {
				float d = depth - i * 0.001f;
				m_limbs[i].drawNormal(sb, destRect, d, flipped);
			}
		}

		void addLimb(Limb limb) {
			if(m_animation)
				limb.activateSkeletalAnimation(m_animation);

			m_limbs.push_back(limb);
		}

		Limb* getLimb(unsigned int index) {
			return index < m_limbs.size() ? &m_limbs[index] : nullptr;
		}

		void activateAnimation(SkeletalAnimation* skelly) {
			for(unsigned int i = 0; i < m_limbs.size(); i++) {
				m_limbs[i].activateSkeletalAnimation(skelly);
			}

			m_animation = skelly;
		}

		void changeAnimation(SkeletalAnimation* skelly) {
			for(unsigned int i = 0; i < m_limbs.size(); i++) {
				m_limbs[i].changeSkeletalAnimation(skelly);
			}

			m_animation = skelly;
			skelly->setChanging(true);
		}

		void resetAnimations() {
			for(unsigned int i = 0; i < m_limbs.size(); i++) {
				m_limbs[i].resetAnimation();
			}
		}

		SkeletalAnimation* getAnimation() {
			return m_animation;
		}

	  private:
		std::vector<Limb>  m_limbs; // Contain individual skins.
		SkeletalAnimation* m_animation = nullptr;
	};

} // namespace AnimationModule
