//
//  ofxPSBlend.h
//
//  Created by Akira Hayasaka on 5/14/12.
//  Copyright (c) 2012 ﾟ･:,｡ﾟ･:,｡★ﾟ･:,｡ﾟ･:,｡☆ﾟ･:,｡ﾟ･:,｡★ﾟ･:,｡ﾟ･:,｡☆. All rights reserved.
//
#pragma once

#include "cinder/Cinder.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/gl.h"

namespace kp {
namespace kit {
namespace gl {

class GlslProgRef;

enum class BlendMode {
	NORMAL = 0,
	MULTIPLY = 1,
	AVERAGE = 2,
	ADD = 3,
	SUBTRACT = 4,
	DIFFERENCE = 5,
	NEGATION = 6,
	EXCLUSION = 7,
	SCREEN = 8,
	OVERLAY = 9,
	SOFT_LIGHT = 10,
	HARD_LIGHT = 11,
	COLOR_DODGE = 12,
	COLOR_BURN = 13,
	LINEAR_LIGHT = 14,
	VIVID_LIGHT = 15,
	PIN_LIGHT = 16,
	HARD_MIX = 17,
	REFLECT = 18,
	GLOW = 19,
	PHOENIX = 20,
	HUE = 21,
	SATURATION = 22,
	COLOR = 23,
	LUMINOSITY = 24,
};

class PsBlend {

public:
	void drawWithBlend(ci::gl::Texture2dRef texture, BlendMode blendMode); // uses the current framebuffer
	void drawWithBlend(ci::gl::Texture2dRef sourceTexture, ci::gl::Texture2dRef destinationTexture, BlendMode blendMode);
	ci::gl::GlslProgRef getBlendShaderRef();

private:
	ci::gl::GlslProgRef mBlendShaderRef;
	static const std::string fragmentShaderGL2;
	static const std::string vertexShaderGL2;
	static const std::string fragmentShaderGL3;
	static const std::string vertexShaderGL3;
};

} // namespace gl
} // namespace kit
} // namespace kp
