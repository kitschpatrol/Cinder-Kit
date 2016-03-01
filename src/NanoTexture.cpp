// NanoTexture.cpp
// RenderToTexture
//
// Created by Eric Mika on 2/28/16.
//

#include "cinder/Log.h"

#include "NanoTexture.h"

NanoTextureRef NanoTexture::create() {
	return NanoTexture::create(512, 512);
}

NanoTextureRef NanoTexture::create(float width, float height) {
	NanoTextureRef ref = std::shared_ptr<NanoTexture>(new NanoTexture());
	ref->setup(width, height);
	return ref;
}

NanoTexture::NanoTexture() {
	CI_LOG_V("NanoTexture Created");
}

NanoTexture::~NanoTexture() {
	CI_LOG_V("NanoTexture Destroyed");
}

void NanoTexture::setup(float width, float height) {
	// auto fboSize = mBounds.getSize() * getWindow()->getContentScale(); // TODO HIDPI

	// mip map...
	ci::gl::Texture::Format textureFormat;
	textureFormat.setMinFilter(GL_LINEAR_MIPMAP_NEAREST);
	textureFormat.setMagFilter(GL_LINEAR);
	textureFormat.enableMipmapping(true);

	ci::gl::Fbo::Format fboFormat;
	fboFormat.enableStencilBuffer();
	fboFormat.setColorTextureFormat(textureFormat);
	
	mFbo = ci::gl::Fbo::create(width, height, fboFormat);

	mCtx = std::make_shared<ci::nvg::Context>(ci::nvg::createContextGL()); // todo unique?
}

void NanoTexture::renderWithFunction(const std::function<void(ci::nvg::Context &, float, float)> &renderFunction) {
	auto &vg = *mCtx;

	ci::gl::ScopedFramebuffer fboScope(mFbo);
	ci::gl::viewport(mFbo->getSize());
	ci::gl::clear(ci::ColorAf::zero());
	ci::gl::clear(GL_STENCIL_BUFFER_BIT);

	vg.beginFrame(mFbo->getSize(), 1.0); // TODO HIDPI
	renderFunction(vg, mFbo->getWidth(), mFbo->getHeight());
	vg.endFrame();

	mTexture = mFbo->getColorTexture();
}

const ci::gl::TextureRef NanoTexture::getTexture() const {
	return mTexture;
}
