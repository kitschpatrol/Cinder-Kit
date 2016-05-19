#pragma once

#include "cinder/Rand.h"

namespace kit {

static ci::Color getRandomColor(float saturation = 1.0f, float brightness = 1.0f) {
	return ci::Color(ci::CM_HSV, ci::Rand::randFloat(), saturation, brightness);
}

template <typename T>
static float perceptualColorDistance(ci::ColorT<T> a, ci::ColorT<T> b) {
	return glm::distance(a.get(ci::CM_HSV), b.get(ci::CM_HSV));
}

template <typename T>
static uint32_t colorToInt(ci::ColorT<T> color) {
	ci::Color8u c(color); // cast to int color
	return (c.r << 16) + (c.g << 8) + c.b;
}

// RGBA... hmm
template <typename T>
static uint32_t colorToInt(ci::ColorAT<T> color) {
	ci::ColorA8u c(color); // cast to int color
	return (c.r << 24) < (c.g << 16) + (c.b << 8) + c.a;
}

} // namespace kit