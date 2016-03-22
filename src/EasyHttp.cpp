#include "EasyHttp.h"

#include "cinder/Log.h"
#include <algorithm>
#include <regex>

namespace kp {
namespace kit {

EasyHttpRef EasyHttp::create() {
	return EasyHttpRef(new EasyHttp())->shared_from_this();
}

EasyHttpSessionRef EasyHttp::request(HttpRequest request, uint16_t port,								 //
																		 std::function<void(HttpResponse response)> success, //
																		 std::function<void(std::string error)> failure) {
	EasyHttpSessionRef sessionRef = EasyHttpSession::create();

	// Keeps sessionref alive long enough to return
	addSession(sessionRef);

	// Avoid retain cycle
	std::weak_ptr<kp::kit::EasyHttpSession> weakSessionRef = sessionRef;

	sessionRef->request(request, port,
											[weakSessionRef, success, this](HttpResponse response) {
												// std::forward?
												success(response);
												removeSession(weakSessionRef.lock());
											},
											[weakSessionRef, failure, this](std::string error) {
												// std::forward?
												failure(error);
												removeSession(weakSessionRef.lock());
											});

	return sessionRef;
}

EasyHttpSessionRef EasyHttp::request(std::string url, std::string verb,									//
																		 std::function<void(std::string response)> success, //
																		 std::function<void(std::string error)> failure) {
	// split host and request
	// should use something more serious here
	// https://tools.ietf.org/html/rfc3986#appendix-B
	std::regex e("^(([^:\\/?#]+):)?(\\/\\/([^\\/?#]*))?([^?#]*)(\\\?([^#]*))?(#(.*))?");
	std::smatch sm; // same as std::match_results<string::const_iterator> sm;
	std::regex_match(url, sm, e);

	std::string host;
	if (std::string(sm[4]).size() > 0) {
		host = sm[4];
	} else {
		failure("Invalid URL");
	}

	std::string path;
	if (std::string(sm[5]).size() > 0) {
		path = sm[5];
	} else {
		path = "/";
	}

	// Query
	if (std::string(sm[7]).size() > 0) {
		path += "?" + std::string(sm[7]);
	}

	// Fragment
	// if (std::string(sm[9]).size() > 0) {
	//	path += sm[9];
	//}

	HttpRequest httpRequest = HttpRequest(verb, path, HttpVersion::HTTP_1_0);
	httpRequest.setHeader("Host", host);
	httpRequest.setHeader("Accept", "*/*");
	httpRequest.setHeader("Connection", "close");

	return request(httpRequest, 80,
								 [success](HttpResponse response) {														//
									 success(HttpResponse::bufferToString(response.getBody())); //
								 },																														//
								 failure);																										//
}

EasyHttpSessionRef EasyHttp::request(std::string url,																		//
																		 std::function<void(std::string response)> success, //
																		 std::function<void(std::string error)> failure) {
	return request(url, "GET", success, failure);
}

// Session list maintenance

void EasyHttp::addSession(EasyHttpSessionRef session) {
	CI_LOG_V("Adding session");
	mSessions.push_back(session);
}

void EasyHttp::removeSession(EasyHttpSessionRef session) {
	// Remove reference to the session. Usually this deletes the session.
	CI_LOG_V("Removing session");
	std::vector<EasyHttpSessionRef>::iterator position = std::find(mSessions.begin(), mSessions.end(), session);

	if (position != mSessions.end()) {
		mSessions.erase(position);
	} else {
		CI_LOG_E("Session not found for deletion!");
	}
}

int EasyHttp::getNumActiveSessions() const {
	return mSessions.size();
}

} // namespace kit
} // namespace kp