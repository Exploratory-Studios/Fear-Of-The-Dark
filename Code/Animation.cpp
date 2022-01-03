#include "Animation.h"

#include "Entity.h"

#include <glm/glm.hpp>
#include <string>
#include <ResourceManager.hpp>
#include <iostream>

namespace AnimationModule {

	Animation::Animation() {
	}

	Animation::Animation(unsigned int id) {
		init(id);
	}

	void Animation::init(unsigned int id) {
		XMLModule::AnimationData d = getAnimationData(id);

		m_normalMapID = d.bumpmap.id;

		m_textureID	  = d.texture.id;
		float height  = d.texture.height;
		float y		  = d.y;
		m_frameWidth  = d.width;
		m_frameHeight = d.height;
		m_width		  = d.frames * m_frameWidth;

		float framesVert = height / m_frameHeight;

		m_uv.x = 0.0f;
		m_uv.y = (((framesVert - 1 - y) * m_frameHeight) / height);
		m_uv.z = (float)(m_frameWidth) / (float)(m_width);
		m_uv.w = (float)(m_frameHeight) / (float)(height);
		//m_uv.y = (float)(y * frameHeight) / (float)(height);
		//m_uv.z = (float)(m_frameWidth) / (float)(m_width);
		//m_uv.z = (float)(frameHeight) / (float)(height);
	}

	void Animation::draw(BARE2D::BumpyRenderer* renderer,
						 BARE2D::Colour			colour,
						 glm::vec4&				destRect,
						 float&					depth,
						 float&					angle,
						 glm::vec2&				COR,
						 bool					flipped) {
		glm::vec4 uv = m_uv;

		uv.x = (float)(m_currentFrame * m_frameWidth) / (float)(m_width);

		if(flipped) {
			uv.y += uv.w;
			uv.w *= -1;
		}

		renderer->draw(destRect, uv, m_textureID, m_normalMapID, depth, colour, angle, COR);
	}

	void Animation::draw(BARE2D::BumpyRenderer* renderer,
						 BARE2D::Colour			colour,
						 glm::vec4&				destRect,
						 float&					depth,
						 glm::vec2				direction) {
		m_uv.x = (float)(m_currentFrame * m_frameWidth) / (float)(m_width);

		const glm::vec2 right(1.0f, 0.0f);
		float			angle = acos(glm::dot(right, direction));
		if(direction.y < 0.0f)
			angle = -angle;

		renderer->draw(destRect, m_uv, m_textureID, m_normalMapID, depth, colour, angle);
	}

	void Animation::tick() {
		if(!isFinished()) {
			m_currentFrame++;
		} else if(m_loops) {
			m_currentFrame = 0;
		}
	}

	bool Animation::isFinished() {
		/// Returns true if this animation's m_currentFrame is the last frame.
		return m_currentFrame == ((m_width / m_frameWidth) - 1);
	}

	void Animation::restart() {
		/// Resets current frame to 0
		m_currentFrame = 0;
	}

	SkeletalAnimation::SkeletalAnimation() {
	}

	SkeletalAnimation::SkeletalAnimation(unsigned int id) {
		init(id);
	}

	void SkeletalAnimation::init(unsigned int id) {
		XMLModule::SkeletalAnimationData d = getSkeletalAnimationData(id);

		m_currentFrame = 0;

		m_angles			= d.angles;
		m_offsets			= d.offsets;
		m_centresOfRotation = d.centresOfRotation;
		m_limbIndices		= d.limbIndices;
		m_repeats			= d.repeats;
	}

	void SkeletalAnimation::updateLimb(Limb* limb) {
		// Interpolate betweem this keypoint and next point. One can assume that this is called every frame,
		// so a constant value (an integral from one angle/place to the next) can be added each frame.

		{
			bool found = false;
			for(unsigned int i = 0; i < m_limbIndices.size(); i++) {
				if(m_limbIndices[i] == limb->getIndex()) {
					found = true;
				}
			}
			if(!found)
				return;
		}

		if(m_currentFrame < m_angles.size() - 1) {
			unsigned int elementI = m_currentFrame * m_limbIndices.size() + limb->getIndex();
			unsigned int nextElementI =
				(((m_currentFrame + 1) * m_limbIndices.size()) % m_angles.size()) + limb->getIndex();

			if(m_lastFrame == m_currentFrame) { // Continue interpolation
				glm::vec2 diffPos	 = m_offsets[nextElementI] - m_offsets[elementI];
				float	  diffAngle	 = m_angles[nextElementI] - m_angles[elementI];
				glm::vec2 diffCentre = m_centresOfRotation[nextElementI] - m_centresOfRotation[elementI];

				glm::vec2 integralPos	 = diffPos / glm::vec2(FRAME_RATE / TICK_RATE);
				float	  integralAngle	 = diffAngle / (FRAME_RATE / TICK_RATE);
				glm::vec2 integralCentre = diffCentre / glm::vec2(FRAME_RATE / TICK_RATE);

				glm::vec2 newOffset = limb->getOffset() + integralPos;
				float	  newAngle	= limb->getAngle() + integralAngle;
				glm::vec2 newCentre = limb->getCentreOfRotation() + integralCentre;

				limb->setOffset(newOffset);
				limb->setAngle(newAngle);
				limb->setCentreOfRotation(newCentre);
			} else { // We're on a new frame. Make sure to set the angle, etc.
				limb->setOffset(m_offsets[elementI]);
				limb->setAngle(m_angles[elementI]);
				limb->setCentreOfRotation(m_centresOfRotation[elementI]);

				m_lastFrame = m_currentFrame;
			}
		}
	}

	void SkeletalAnimation::transitionLimb(Limb* limb) {
		// Move the limb from where it is to where the first frame of this animation is. This has to be done in some arbitrary amount of frames. This function is called every frame.

		// We can use m_currentFrame to count up to some arbitrary number. We can calculate how far we need to move each frame, using some arbitrary number of frames, and how far we need to go still
		const unsigned int framesPerTransition = 3;
		const unsigned int numFramesLeft	   = framesPerTransition - m_currentFrame;
		// If we divide the distance/angle from here to where we need to go by the frames left, we should have our amount to move.

		glm::vec2 diffPos	 = m_offsets[limb->getIndex()] - limb->getOffset();
		float	  diffAngle	 = m_angles[limb->getIndex()] - limb->getAngle();
		glm::vec2 diffCentre = m_centresOfRotation[limb->getIndex()] - limb->getCentreOfRotation();

		glm::vec2 integralPos	 = diffPos / glm::vec2(numFramesLeft);
		float	  integralAngle	 = diffAngle / numFramesLeft;
		glm::vec2 integralCentre = diffCentre / glm::vec2(numFramesLeft);

		glm::vec2 newOffset = limb->getOffset() + integralPos;
		float	  newAngle	= limb->getAngle() + integralAngle;
		glm::vec2 newCentre = limb->getCentreOfRotation() + integralCentre;

		limb->setOffset(newOffset);
		limb->setAngle(newAngle);
		limb->setCentreOfRotation(newCentre);

		if(glm::distance(limb->getOffset(), m_offsets[limb->getIndex()]) <= 0.01f) { // Check if we're done
			if(std::abs(limb->getAngle() - m_angles[limb->getIndex()]) <= 0.01f) {
				if(glm::distance(limb->getCentreOfRotation(), m_centresOfRotation[limb->getIndex()]) <= 0.01f) {
					// Everything is within an unnoticable distance, so we can continue with regular updating.
					limb->activateSkeletalAnimation(this);
					m_changing = false;
				}
			}
		}
	}

	void SkeletalAnimation::tick() {
		if(!isFinished()) {
			m_currentFrame = (m_currentFrame + 1) % (m_offsets.size() / m_limbIndices.size());
		}
	}

	bool SkeletalAnimation::isFinished() {
		if(m_repeats)
			return false;
		if(m_angles.size() == 0)
			return true;
		return (m_currentFrame + 1 >=
				(m_angles.size() / m_limbIndices.size())); // This only returns if it doesn't repeat
	}

	void SkeletalAnimation::restart() {
		m_currentFrame = 0;
	}

	Limb::Limb() {
	}

	Limb::Limb(Animation defaultAnimation, unsigned int index) {
		// The idleAnimation is the animation that constantly runs. Most of the time, this is just a single-textured sprite which supplies the texture
		init(defaultAnimation, index);
	}

	void Limb::init(Animation defaultAnimation, unsigned int index) {
		m_defaultAnimation = defaultAnimation;
		m_idleAnimation	   = defaultAnimation;
		m_index			   = index;
		m_centreOfRotation = glm::vec2(0.5f);
	}

	void Limb::activateSkeletalAnimation(SkeletalAnimation* anim) {
		// Sets current m_activeAnimation and sets m_animationTime to 0.0f
		if(!anim->affectsLimb(m_index)) {
			return;
		}

		m_activeAnimation = anim;
		m_nextAnimation	  = nullptr; // Make sure that this animation is the one we're actually using now.

		m_offset		   = anim->getOffset(m_index);
		m_angle			   = anim->getAngle(m_index);
		m_centreOfRotation = anim->getCentreOfRotation(m_index);
	}

	void Limb::changeSkeletalAnimation(SkeletalAnimation* anim) {
		// Sets current m_activeAnimation and sets m_animationTime to 0.0f
		if(!anim->affectsLimb(m_index)) {
			return;
		}

		m_nextAnimation = anim;
		m_nextAnimation->setFrame(0); // So transitionLimb works properly.
	}

	void Limb::setAnimation(Animation& anim) {
		m_idleAnimation = anim;
	}

	void Limb::resetAnimation() {
		m_idleAnimation = m_defaultAnimation;
	}

	void Limb::tick() {
		// Sets the idle animation to the next state. If active, continues the skeletal animation too. If the skeletal animation finishes, set m_animationTime to -1.0f;
		m_idleAnimation.tick();
	}

	void Limb::update() {
		if(m_nextAnimation) {
			m_nextAnimation->transitionLimb(this);
		} else if(isAnimationActive()) {
			m_activeAnimation->updateLimb(this);
		}
	}

	void Limb::draw(BARE2D::BumpyRenderer* renderer,
					BARE2D::Colour		   colour,
					glm::vec4			   destRect,
					float&				   depth,
					bool				   flipped) {
		// Transform based on centre of rotation
		const glm::vec2 centre =
			flipped ? glm::vec2(1.0f - m_centreOfRotation.x, m_centreOfRotation.y) : m_centreOfRotation;
		const glm::vec2 offset = flipped ? glm::vec2(-m_offset.x, m_offset.y) : m_offset;
		float			angle  = flipped ? M_PI - m_angle : m_angle;

		float xDist = (offset.x + destRect.z / 2.0f) - centre.x * destRect.z;
		float yDist = (offset.y + destRect.w / 2.0f) - centre.y * destRect.w;

		if(flipped)
			yDist *= -1;

		destRect.x += xDist * std::cos(angle) - yDist * std::sin(angle) + centre.x * destRect.z;
		destRect.y += xDist * std::sin(angle) + yDist * std::cos(angle) + centre.y * destRect.w;

		destRect.x -= destRect.z / 2.0f;
		destRect.y -= destRect.w / 2.0f;

		glm::vec2 c(0.5f, 0.5f);

		m_idleAnimation.draw(renderer, colour, destRect, depth, angle, c, flipped);
	}

	bool Limb::isAnimationActive() {
		if(m_activeAnimation) {
			return !m_activeAnimation->isFinished();
		}
		return false;
	};

	unsigned int Limb::getIndex() {
		return m_index;
	}

	float Limb::getAngle() {
		return m_angle;
	}

	glm::vec2 Limb::getOffset() {
		return m_offset;
	}

	glm::vec2 Limb::getCentreOfRotation() {
		return m_centreOfRotation;
	}

	void Limb::setAngle(float& angle) {
		m_angle = angle;
	}

	void Limb::setOffset(glm::vec2& offset) {
		m_offset = offset;
	}

	void Limb::setCentreOfRotation(glm::vec2& centre) {
		m_centreOfRotation = centre;
	}

} // namespace AnimationModule
