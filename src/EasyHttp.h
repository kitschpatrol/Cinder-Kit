// EasyHttp.h
// SmartMuseumBasic
//
// Created by Eric Mika on 3/18/16.
//

#pragma once

#include "EasyHttpSession.h"
#include "cinder/Cinder.h"

namespace kp {
namespace kit {

typedef std::shared_ptr<class EasyHttp> EasyHttpRef;

class EasyHttp : public std::enable_shared_from_this<EasyHttp> {
public:
	static EasyHttpRef create();
	~EasyHttp();

	// GET on port 80
	EasyHttpSessionRef request(std::string url, std::function<void(std::string response)> success, std::function<void(std::string error)> failure = nullptr);

	// Any verb on port 80
	EasyHttpSessionRef request(std::string url, std::string verb, std::function<void(std::string response)> success,
														 std::function<void(std::string error)> failure = nullptr);

	// more flexible
	EasyHttpSessionRef request(HttpRequest request, uint16_t port, std::function<void(HttpResponse response)> success,
														 std::function<void(std::string error)> failure = nullptr);

	std::vector<EasyHttpSessionRef> mSessions;

protected:
	EasyHttp();

private:
	void addSession(EasyHttpSessionRef session);
	void removeSession(EasyHttpSessionRef session);
};

} // namespace kit
} // namespace kp