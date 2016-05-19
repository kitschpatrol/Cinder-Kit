// NanoTexture.h
// RenderToTexture
//
// Created by Eric Mika on 2/28/16.
// Simplifies drawing with NanoVG into a texture, useful for replacing cairo etc.
//
// Example usage:
// member
// NanoTextureRef nanoTexture;
//
// // member
// NanoTextureRef nanoTexture = NanoTexture::create();
//
// // setup
// nanoTexture = NanoTexture::create();
// nanoTexture->renderWithFunction([&](ci::nvg::Context &vg, float width, float height) {
// 	// Draw rect
// 	vg.beginPath();
// 	vg.rect(10.0, 10.0, 90.0, 90.0);
// 	vg.strokeColor(ci::ColorAf("red"));
// 	vg.strokeWidth(10.0);
//	vg.stroke();
// });
//
// // in draw
// gl::draw(nanoTexture->getTexture());
//

#pragma once

#include "ci_nanovg_gl.hpp"
//#include "cinder/Cinder.h"
#include "cinder/gl/gl.h"

namespace kit {
namespace graphics {

typedef std::shared_ptr<class NanoTexture> NanoTextureRef;

class NanoTexture : public std::enable_shared_from_this<NanoTexture> {
public:
	static NanoTextureRef create();
	static NanoTextureRef create(float width, float height);

	virtual ~NanoTexture();

	const ci::gl::TextureRef getTexture() const;

	// spares subclass for trivial drawing
	void renderWithFunction(																																														//
			const std::function<void(ci::nvg::Context & /*context*/, float /*width */, float /* height */)> &renderFunction //
			);																																																							//

protected:
	NanoTexture();
	void setup(float width, float height);

private:
	ci::gl::TextureRef mTexture;
	ci::gl::FboRef mFbo;
	std::shared_ptr<ci::nvg::Context> mCtx;
};

} // namespace graphics
} // namespace kit
