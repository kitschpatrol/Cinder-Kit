#pragma once

#include "cinder/gl/scoped.h"
#include "cinder/gl/draw.h"

namespace kit {
namespace graphics {

	static void drawCross(ci::vec2 position, float size, ci::ColorA color = ci::ColorA("magenta"), float lineWidth = 1.0) {
		ci::gl::ScopedColor crossColor(color);
		ci::gl::ScopedLineWidth crossLineWidth(lineWidth);
		ci::gl::drawLine(position - ci::vec2(size, 0), position + ci::vec2(size, 0));
		ci::gl::drawLine(position - ci::vec2(0, size), position + ci::vec2(0, size));
	}

} // namespace graphics
} // namespace kit