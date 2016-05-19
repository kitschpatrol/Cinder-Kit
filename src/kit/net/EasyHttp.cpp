#include "EasyHttp.h"

#include "cinder/Log.h"
#include <algorithm>
#include <regex>

using namespace std;

namespace kit {
namespace net {

EasyHttpRef EasyHttp::create() {
	return EasyHttpRef(new EasyHttp())->shared_from_this();
}

EasyHttpSessionRef EasyHttp::request(HttpRequest request, uint16_t port,						//
																		 function<void(HttpResponse response)> success, //
																		 function<void(string error)> failure) {
	EasyHttpSessionRef sessionRef = EasyHttpSession::create();

	// Keeps sessionref alive long enough to return
	addSession(sessionRef);

	// Avoid retain cycle
	weak_ptr<kit::net::EasyHttpSession> weakSessionRef = sessionRef;

	if (request.getBody() != nullptr) {
		CI_LOG_I("Request to " << request.getUri() << " with body:\n" << HttpRequest::bufferToString(request.getBody()) << "\n");
	} else {
		CI_LOG_I("Request to " << request.getUri());
	}

	sessionRef->request(request, port,
											[weakSessionRef, success, this](HttpResponse response) {
												// forward?
												success(response);
												removeSession(weakSessionRef.lock());
											},
											[weakSessionRef, failure, this](string error) {
												// forward?
												failure(error);
												removeSession(weakSessionRef.lock());
											});

	return sessionRef;
}

EasyHttpSessionRef EasyHttp::request(string url, string verb,									//
																		 function<void(string response)> success, //
																		 function<void(string error)> failure) {
	// split host and request
	// should use something more serious here
	// https://tools.ietf.org/html/rfc3986#appendix-B
	regex e("^(([^:\\/?#]+):)?(\\/\\/([^\\/?#]*))?([^?#]*)(\\\?([^#]*))?(#(.*))?");
	smatch sm; // same as match_results<string::const_iterator> sm;
	regex_match(url, sm, e);

	string host;
	if (string(sm[4]).size() > 0) {
		host = sm[4];
	} else {
		failure("Invalid URL");
	}

	string path;
	if (string(sm[5]).size() > 0) {
		path = sm[5];
	} else {
		path = "/";
	}

	// Query
	if (string(sm[7]).size() > 0) {
		path += "?" + string(sm[7]);
	}

	// Fragment
	// if (string(sm[9]).size() > 0) {
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

EasyHttpSessionRef EasyHttp::request(string url,															//
																		 function<void(string response)> success, //
																		 function<void(string error)> failure) {
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
	vector<EasyHttpSessionRef>::iterator position = find(mSessions.begin(), mSessions.end(), session);

	if (position != mSessions.end()) {
		mSessions.erase(position);
	} else {
		CI_LOG_E("Session not found for deletion!");
	}
}

int EasyHttp::getNumActiveSessions() const {
	return mSessions.size();
}

} // namespace net
} // namespace kit
