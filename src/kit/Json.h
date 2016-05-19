#pragma once

namespace kit {

// there must be a better place / way

// Pairs... todo figure out how to overload this template without renaming
template <typename T, typename U>
static ci::JsonTree toJson(std::string key, std::pair<T, U> value) {
	ci::JsonTree object = ci::JsonTree::makeObject(key);
	object.addChild(ci::JsonTree("first", value.first));
	object.addChild(ci::JsonTree("second", value.second));
	return object;
}

// hmm have to specify return type manually
//		std::pair<float, float> test = kp::kit::fromJsonPair<float, float>(json);
// http://stackoverflow.com/questions/26081814/can-c-templates-infer-return-type
template <typename T, typename U>
static std::pair<T, U> fromJson(const ci::JsonTree &json, bool flag = false) {
	return std::make_pair(json["first"].getValue<T>(), json["second"].getValue<U>());
}

// Vectors
static ci::JsonTree toJson(ci::vec2 value) {
	ci::JsonTree object = ci::JsonTree::makeObj ect();
	object.addChild(ci::JsonTree("x", value.x));
	object.addChild(ci::JsonTree("y", value.y));
	return object;
}

static ci::JsonTree toJson(std::string key, ci::vec2 value) {
	ci::JsonTree object = ci::JsonTree::makeObject(key);
	object.addChild(ci::JsonTree("x", value.x));
	object.addChild(ci::JsonTree("y", value.y));
	return object;
}

static ci::vec2 fromJson(const ci::JsonTree &json) {
	return ci::vec2(json["x"].getValue<float>(), json["y"].getValue<float>());
}

} // namespace kit