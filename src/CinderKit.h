#pragma once

#include "cinder/Cinder.h"
#include "cinder/Color.h"
#include "cinder/Rand.h"
#include "cinder/Utilities.h"
#include "cinder/app/App.h"

#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_generators.hpp"
#include "boost/uuid/uuid_io.hpp"

#define KP_WATCH(VARIABLE_TO_WATCH) (CI_LOG_V(#VARIABLE_TO_WATCH << ": " << VARIABLE_TO_WATCH));
#define KP_LOG_FUNCTION (CI_LOG_V(__PRETTY_FUNCTION__ << " on thread " << std::this_thread::get_id));

namespace kp {
namespace kit {

static float roundTo(float value, float roundStep) {
	int distance = fmod(value, roundStep);
	// Round up
	if (distance > (roundStep * 0.5)) {
		return (value - distance) + roundStep;
	} else {
		return value - distance;
	}
}

static ci::vec2 roundTo(const ci::vec2 &point, float roundStep) {
	return ci::vec2(roundTo(point.x, roundStep), roundTo(point.y, roundStep));
}

static float getWave(float frequencySeconds = 1.0, float minAmplitude = 0.0, float maxAmplitude = 1.0, float timeOffset = 0.0) {
	ci::math<float>::sin(1.0);
	return ci::lmap(sinf(((ci::app::getElapsedSeconds() + timeOffset) / frequencySeconds) * M_PI * 2), -1.0f, 1.0f, minAmplitude, maxAmplitude);
}

static ci::Color getRandomColor() {
	return ci::Color(ci::CM_HSV, ci::Rand::randFloat(), 0.5f, 1.0f);
}

static boost::uuids::uuid getUUID() {
	return boost::uuids::random_generator()();
}

static std::string getUUIDString() {
	return boost::uuids::to_string(getUUID());
}

// From Reza Ali's Cinder UI
template <typename T>
std::string toString(T value, int precision, int width, char fill) {
	std::ostringstream out;
	out << std::fixed << std::setfill(fill) << std::setw(width) << std::setprecision(precision)
			<< ((std::abs<T>(value) < ((1.0 / pow(10.0, precision + 1)) * 5.0)) ? 0.0 : value);
	return out.str();
}

template <typename T>
std::string toString(T value, int precision, char fill) {
	std::ostringstream out;
	out << std::fixed << std::setfill(fill) << std::setprecision(precision) << ((std::abs<T>(value) < ((1.0 / pow(10.0, precision + 1)) * 5.0)) ? 0.0 : value);
	return out.str();
}

template <typename T>
std::string toString(T value, int precision) {
	std::ostringstream out;
	out << std::fixed << std::setfill('0') << std::setprecision(precision) << ((std::abs<T>(value) < ((1.0 / pow(10.0, precision + 1)) * 5.0)) ? 0.0 : value);
	return out.str();
}

static int codeStringPrecision = 4;
static int codeStringPadWith = 4; // does not work?

// Returns a string that represents the code that would have been used to generate the value
static std::string toCodeString(ci::vec2 value) {

	return "ci::vec2(" + kp::kit::toString(value.x, codeStringPrecision, codeStringPadWith, '0') + " ," +
				 kp::kit::toString(value.y, codeStringPrecision, codeStringPadWith, '0') + ")";
}

static std::string toCodeString(ci::Rectf value) {
	return "ci::Rectf(" +																																					//
				 kp::kit::toString(value.getX1(), codeStringPrecision, codeStringPadWith, '0') + ", " + //
				 kp::kit::toString(value.getY1(), codeStringPrecision, codeStringPadWith, '0') + ", " + //
				 kp::kit::toString(value.getX2(), codeStringPrecision, codeStringPadWith, '0') + ", " + //
				 kp::kit::toString(value.getY2(), codeStringPrecision, codeStringPadWith, '0') + ")";
}
}
} // namespace kp::kit
