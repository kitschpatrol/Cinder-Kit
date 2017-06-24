#pragma once

//#include "cinder/Cinder.h"
//#include "cinder/Color.h"
//#include "cinder/Json.h"
#include "cinder/Rand.h"
//#include "cinder/Utilities.h"
#include "cinder/app/App.h"
//#include "cinder/app/RendererGl.h"
//#include "cinder/gl/gl.h"
//
//#include "boost/algorithm/string.hpp"
//#include "boost/uuid/uuid.hpp"
//#include "boost/uuid/uuid_generators.hpp"
//#include "boost/uuid/uuid_io.hpp"
//#include <chrono>
#include <complex>
//#include <ctime>		// struct std::tm
//#include <iomanip>	// std::get_time
//#include <iostream> // std::cin, std::cout
//#include <iterator>
//#include <map>
#include <random>
//#include <sstream>

#define KP_WATCH(VARIABLE_TO_WATCH) (CI_LOG_V(#VARIABLE_TO_WATCH << ": " << VARIABLE_TO_WATCH));
#define KP_LOG_FUNCTION (CI_LOG_V(__PRETTY_FUNCTION__ << " on thread " << std::this_thread::get_id()));

#define KP_WATCH_COUT(VARIABLE_TO_WATCH) (std::cout << #VARIABLE_TO_WATCH << ": " << VARIABLE_TO_WATCH << std::endl);
#define KP_LOG_FUNCTION_COUT (std::cout << __PRETTY_FUNCTION__ << " on thread " << std::this_thread::get_id() << std::endl);

// for machine name resolution
#ifdef CINDER_MSW
// ugh include winsock for asio error
#include <WinSock2.h>
#include <Windows.h>
#include <tchar.h>
#else
#include <unistd.h>
#endif

// namespace glm {
//// template <class T>
//// std::ostream &operator<<(std::ostream &out, const glm::tvec2<T> &vec) {
////	out << vec.x << " " << vec.y;
////	return out;
////}
//
// template <class T>
// std::istream &operator>>(std::istream &in, glm::tvec2<T> &vec) {
//	std::string s(std::istreambuf_iterator<char>(stream), {});
//
//	if (in.good() && in.ignore(256, ' ').good())
//		in >> vec.y;
//	return in;
//}
//}

namespace kit {

// Serializations

static float roundTo(float value, float roundStep) {
	int distance = static_cast<int>(fmod(value, roundStep));
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
	return ci::lmap(sinf(((static_cast<float>(ci::app::getElapsedSeconds()) + timeOffset) / frequencySeconds) * M_PI * 2), -1.0f, 1.0f, minAmplitude, maxAmplitude);
}

template <typename Iter, typename RandomGenerator>
static Iter randomElement(Iter start, Iter end, RandomGenerator &g) {
	std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
	std::advance(start, dis(g));
	return start;
}

template <typename Iter>
static Iter randomElement(Iter start, Iter end) {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	return randomElement(start, end, gen);
}

static ci::vec2 getRandomPoint(float xMax = 1.0f, float yMax = 1.0f) {
	return ci::vec2(ci::Rand::randFloat() * xMax, ci::Rand::randFloat() * yMax);
}

	/*
static boost::uuids::uuid getUUID() {
	return boost::uuids::random_generator()();
}

static std::string getUUIDString() {
	return boost::uuids::to_string(getUUID());
}
	 */

static ci::vec2 polarToCartesian(float radius, float theta) {
	std::complex<double> cartesian = std::polar(radius, theta);
	return ci::vec2(cartesian.real(), cartesian.imag());
}

static ci::vec2 cartesianToPolar(ci::vec2 cartesian) {
	return ci::vec2(glm::length(cartesian), std::atan2(cartesian.x, cartesian.y));
}

static std::string getMachineName(bool includeDomain = false) {
	char name[256];
	std::memset(name, 0, 256);

#ifdef CINDER_MSW
	TCHAR buffer[256] = TEXT("");
	DWORD dwSize = sizeof(buffer);
	COMPUTER_NAME_FORMAT nameFormat = includeDomain ? ComputerNamePhysicalDnsFullyQualified : ComputerNamePhysicalDnsHostname;
	if (GetComputerNameEx(nameFormat, buffer, &dwSize)) {
		for (int i = 0; i < dwSize; i++) {
			name[i] = buffer[i];
		}
	} else {
		strcpy(name, "Unknown_Host_Name");
	}
#else
	gethostname(name, 256);
#endif

	std::string nameString = std::string(name);

	if (includeDomain) {
		// skip domains after the dot
		std::size_t pos = nameString.find(".");

		if (pos != std::string::npos) {
			nameString = nameString.substr(0, pos);
		}
	}

	return nameString;
}

} // namespace kit
