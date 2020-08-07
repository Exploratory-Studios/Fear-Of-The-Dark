#include "TileLiquid.h"

TileLiquid::TileLiquid(glm::vec2 pos, unsigned int layer, unsigned int id, SaveDataTypes::MetaData data, bool loadTex) : Tile(pos, layer, id, data, loadTex) {

}

TileLiquid::~TileLiquid() {

}

void TileLiquid::draw(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, int& xOffset, int& depthDifference) {
	if(m_draw) {
		if(m_textureId == (GLuint) - 1) {
			loadTexture();
		}

		glm::vec4 pos = glm::vec4(m_pos.x + xOffset, m_pos.y, m_size.x, m_size.y * m_level);
		sb.draw(pos,
		        glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
		        m_textureId,
		        m_depthForRender,
		        m_colour);

		onDraw(sb, sf, pos, m_depthForRender);
	}
}

void TileLiquid::drawNormal(GLEngine::SpriteBatch& sb, int& xOffset, int& depthDifference) {
	if(m_draw) {
		if(m_bumpMapId == (GLuint) - 1) {
			loadTexture();
		}

		glm::vec4 pos = glm::vec4(m_pos.x + xOffset, m_pos.y, m_size.x, m_size.y * m_level);
		sb.draw(pos,
		        glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
		        m_bumpMapId,
		        m_depthForRender,
		        m_colour);

		//onDraw(sb, sf, pos, depth);
	}
}

void TileLiquid::onUpdate() {
	// Check surrounding tiles and average?
}
