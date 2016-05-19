#pragma once

#include "boost/algorithm/string.hpp"

namespace kit {

static bool startsWith(const std::string &haystack, const std::string &needle) {
	return boost::algorithm::starts_with(haystack, needle);
}

template <typename T>
static std::string pluralize(T count, std::string singular, std::string plural) {
	return (count == 1) ? singular : plural;
}

// From Reza Ali's Cinder UI... issue son windows?
template <typename T>
std::string toString(T value, int precision, int width, char fill) {
	std::ostringstream out;
	out << std::fixed << std::setfill(fill) << std::setw(width) << std::setprecision(precision) << ((std::abs<T>(value) < ((1.0 / pow(10.0, precision + 1)) * 5.0)) ? 0.0 : value);
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

// Returns a string that represents the code that would have been used to generate the value
static int codeStringPrecision = 4;
static int codeStringPadWith = 4; // does not work?

static std::string toCodeString(ci::vec2 value) {

	return "ci::vec2(" + toString(value.x, codeStringPrecision, codeStringPadWith, '0') + " ," + toString(value.y, codeStringPrecision, codeStringPadWith, '0') + ")";
}

static std::string toCodeString(ci::Rectf value) {
	return "ci::Rectf(" +																																 //
				 toString(value.getX1(), codeStringPrecision, codeStringPadWith, '0') + ", " + //
				 toString(value.getY1(), codeStringPrecision, codeStringPadWith, '0') + ", " + //
				 toString(value.getX2(), codeStringPrecision, codeStringPadWith, '0') + ", " + //
				 toString(value.getY2(), codeStringPrecision, codeStringPadWith, '0') + ")";
}

} // namespace kit