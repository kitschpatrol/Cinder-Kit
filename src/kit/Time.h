#pragma once

namespace kit {

// http://en.cppreference.com/w/cpp/io/manip/get_time
static std::time_t stringToTime(const std::string timeString, const std::string format) {
	std::tm tm = {};
	std::stringstream ss(timeString.c_str());
	ss >> std::get_time(&tm, format.c_str());
	return std::mktime(&tm);
}

// http://en.cppreference.com/w/cpp/io/manip/put_time
static std::string timeToString(const std::time_t time, const std::string format) {
	std::tm *tm = std::localtime(&time);
	std::stringstream ss;
	ss << std::put_time(tm, format.c_str());
	return ss.str();
}

static std::string secondsToHMS(double seconds) {
	const int h = seconds / 60.0 / 60.0;
	const int m = static_cast<long>(seconds / 60.0) % 60;
	const int s = static_cast<long>(seconds) % 60;

	std::stringstream ss;
	ss << h << "h " << m << "m " << s << "s";
	return ss.str();
}

} // namespace kit