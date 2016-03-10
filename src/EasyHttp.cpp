// EasyHttp.cpp
// SmartMuseumBasic
//
// Created by Eric Mika on 3/7/16.
//

#include "EasyHttp.h"

#include "cinder/Log.h"
#include "cinder/Utilities.h"

#include "HttpInterface.h"
#include <regex>

using namespace ci;
using namespace ci::app;
using namespace std;

namespace kp {
namespace kit {

EasyHttpRef EasyHttp::create() {
	return EasyHttpRef(new EasyHttp())->shared_from_this();
}

EasyHttp::EasyHttp() {
	verboseLog("EasyHttp Created");
}

EasyHttp::~EasyHttp() {
	verboseLog("EasyHttp Destroyed");
}

void EasyHttp::request(HttpRequest request, uint16_t port, std::function<void(HttpResponse response)> success, std::function<void(std::string error)> failure) {
	if (mSession && mSession->getSocket()->is_open()) {
		verboseLog("Request in progress.");
		return;
	}

	mSuccess = false;
	mFailure = false;
	mHttpRequest = request;

	mSuccessCallback = success;
	mFailureCallback = failure;

	// Use cinder's ASIO instance
	mClient = TcpClient::create(App::get()->io_service());
	mClient->connectConnectEventHandler(&EasyHttp::onClientConnect, this);
	mClient->connectErrorEventHandler(&EasyHttp::onClientError, this);
	mClient->connectResolveEventHandler(&EasyHttp::onClientResolve, this);

	verboseLog("Connecting");

	CI_LOG_V("Connecting to : " << mHttpRequest.getHeader("Host"));

	mClient->connect(mHttpRequest.getHeader("Host"), port);
}

void EasyHttp::request(std::string url, std::function<void(std::string response)> success, std::function<void(std::string error)> failure) {

	// split host and request
	// should use something more serious here
	// https://tools.ietf.org/html/rfc3986#appendix-B
	std::regex e("^(([^:\\/?#]+):)?(\\/\\/([^\\/?#]*))?([^?#]*)(\\\?([^#]*))?(#(.*))?");
	std::smatch sm; // same as std::match_results<string::const_iterator> sm;
	std::regex_match(url, sm, e);

	string host;
	if (string(sm[4]).size() > 0) {
		host = sm[4];
	} else {
		CI_LOG_E("Invalud URL");
	}

	string path;
	if (string(sm[5]).size() > 0) {
		path = sm[5];
	} else {
		path = "/";
	}

	HttpRequest httpRequest = HttpRequest("GET", path, HttpVersion::HTTP_1_0);
	httpRequest.setHeader("Host", host);
	httpRequest.setHeader("Accept", "*/*");
	httpRequest.setHeader("Connection", "close");

	// have to do this for lifetime...?
	mBasicSuccessCallback = success;
	request(httpRequest, 80,
					[&](HttpResponse response) {
						// Wrap the callback
						mBasicSuccessCallback(HttpResponse::bufferToString(mHttpResponse.getBody()));
						mBasicSuccessCallback = nullptr;
					},
					failure);
}

// ------------

void EasyHttp::onClientConnect(TcpSessionRef session) {
	mHttpResponse = HttpResponse();
	mSession = session;

	// TODO set timeout?
	// http://stackoverflow.com/questions/292997/can-you-set-so-rcvtimeo-and-so-sndtimeo-socket-options-in-boost-asio

	verboseLog("Connected");

	// - Request
	//	- response
	//	- error

	mSession->connectCloseEventHandler(&EasyHttp::onSessionClose, this);
	mSession->connectErrorEventHandler(&EasyHttp::onSessionError, this);
	mSession->connectReadCompleteEventHandler(&EasyHttp::onSessionReadComplete, this);
	mSession->connectReadEventHandler(&EasyHttp::onSessionRead, this);
	mSession->connectWriteEventHandler(&EasyHttp::onSessionWrite, this);
	mSession->write(mHttpRequest.toBuffer());
}

void EasyHttp::onClientResolve() {
	verboseLog("Endpoint resolved");
}

void EasyHttp::cancel() {
	CI_LOG_E("Cancellation not yet implemented");
	//	TODO best approach...
	//	if (mSession && mSession->getSocket()->is_open()) {
	//		mSession->close();
	//		verboseLog("Canceling by closing session");
	//	} else {
	//		mClient->getResolver()->cancel(); // hmm
	//		verboseLog("Canceling by cancelling resolver");
	//	}
}

void EasyHttp::onSessionWrite(size_t bytesTransferred) {
	verboseLog(toString(bytesTransferred) + " bytes written");
	mSession->read();
}

void EasyHttp::onSessionRead(ci::BufferRef buffer) {
	verboseLog(toString(buffer->getSize()) + " bytes read");

	if (!mHttpResponse.hasHeader()) {
		mHttpResponse.parseHeader(HttpResponse::bufferToString(buffer));
		buffer = HttpResponse::removeHeader(buffer);
	}
	mHttpResponse.append(buffer);

	mSession->read();
}

void EasyHttp::onSessionReadComplete() {
	verboseLog("Response: \n" + toString(mHttpResponse));

	mSuccess = true;
	mSession->close();
}

void EasyHttp::onSessionClose() {
	verboseLog("Disconnected");
	conclude();
}

void EasyHttp::onClientError(string err, size_t bytesTransferred) {
	string text = "Client Error";
	if (!err.empty()) {
		text += ": " + err;
	}

	mFailure = true;
	conclude();
	CI_LOG_E(text);
}

void EasyHttp::onSessionError(string err, size_t bytesTransferred) {
	string text = "Session Error";
	if (!err.empty()) {
		text += ": " + err;
	}

	mFailure = true;
	conclude();
	CI_LOG_E(text);
}

void EasyHttp::conclude() {
	// TODo move this elsewhere?
	if (mSuccess && mSuccessCallback) {
		mSuccessCallback(mHttpResponse);
	} else if (mFailure && mFailureCallback) {
		mFailureCallback("Something happened.");
	}
	mSuccessCallback = nullptr;
	mFailureCallback = nullptr;
}

void EasyHttp::verboseLog(std::string message) {
	if (isAbsurdlyVerboseLoggingEnabled) {
		CI_LOG_V(message);
	}
}

} // namespace kit
} // namespace kp