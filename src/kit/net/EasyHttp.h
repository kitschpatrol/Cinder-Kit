// EasyHTTP
//
// Simple asynchronous HTTP requests for Cinder.
// Depends on Cinder-Protocol: https://github.com/BanTheRewind/Cinder-Protocol
// And Cinder-ASIO: https://github.com/BanTheRewind/Cinder-ASIO

#pragma once

#include "EasyHttpSession.h"

namespace kit {
namespace net {

typedef std::shared_ptr<class EasyHttp> EasyHttpRef;

class EasyHttp : public std::enable_shared_from_this<EasyHttp> {
public:
	static EasyHttpRef create();

	// Most flexibility
	EasyHttpSessionRef request(HttpRequest request, uint16_t port,								 //
														 std::function<void(HttpResponse response)> success, //
														 std::function<void(std::string error)> failure = nullptr);

	// Convenience: Any verb on port 80
	EasyHttpSessionRef request(std::string url, std::string verb,									//
														 std::function<void(std::string response)> success, //
														 std::function<void(std::string error)> failure = nullptr);

	// More convenience: GET on port 80
	EasyHttpSessionRef request(std::string url,																		//
														 std::function<void(std::string response)> success, //
														 std::function<void(std::string error)> failure = nullptr);

	int getNumActiveSessions() const;

private:
	std::vector<EasyHttpSessionRef> mSessions;
	void addSession(EasyHttpSessionRef session);
	void removeSession(EasyHttpSessionRef session);
};

} // namespace net
} // namespace kit