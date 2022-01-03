#include "TileLiquid.h"

TileLiquid::TileLiquid(glm::vec2 pos, unsigned int layer, unsigned int id) :
	Tile(pos, layer, id) {
}

TileLiquid::~TileLiquid() {
}

void TileLiquid::draw(BARE2D::BumpyRenderer* renderer,
					  BARE2D::FontRenderer*	 fontRenderer,
					  int&					 xOffset,
					  int&					 depthDifference) {
	if(m_draw) {
		glm::vec4 pos = glm::vec4(m_pos.x + xOffset, m_pos.y, m_size.x, m_size.y * m_level);
		renderer->draw(pos, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_texture.id, m_bumpmap.id, m_depthForRender, m_colour);

		onDraw(renderer, fontRenderer, pos, m_depthForRender);
	}
}

void TileLiquid::onUpdate() {
	// Check surrounding tiles and average?
}
