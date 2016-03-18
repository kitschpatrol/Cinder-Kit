// EasyHttpSession.cpp
// SmartMuseumBasic
//
// Created by Eric Mika on 3/7/16.
//

#include "EasyHttpSession.h"

#include "cinder/Cinder.h"
#include "cinder/Log.h"
#include "cinder/Utilities.h"
#include "cinder/app/App.h"

#include "HttpInterface.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace kp {
namespace kit {

// TODO fix retain cycles with
// pass into lambdas explicitly

EasyHttpSessionRef EasyHttpSession::create() {
	return EasyHttpSessionRef(new EasyHttpSession())->shared_from_this();
}

EasyHttpSession::EasyHttpSession() {
	CI_LOG_V("EasyHttpSession Created");
}

EasyHttpSession::~EasyHttpSession() {
	CI_LOG_V("EasyHttpSession Destroyed");
}

void EasyHttpSession::request(HttpRequest request, uint16_t port, std::function<void(HttpResponse response)> success,
															std::function<void(std::string error)> failure) {
	if (mSession && mSession->getSocket()->is_open()) {
		CI_LOG_E("Request in progress. Aborting. This should not be possible!");
		return;
	}

	verboseLog("Sending:");
	verboseLog(request.toString());

	mSuccess = false;
	mFailure = false;
	mHttpRequest = request;

	mSuccessCallback = success;
	mFailureCallback = failure;

	// Use cinder's ASIO instance
	mClient = TcpClient::create(App::get()->io_service());
	mClient->connectConnectEventHandler(&EasyHttpSession::onClientConnect, this);
	mClient->connectErrorEventHandler(&EasyHttpSession::onClientError, this);
	mClient->connectResolveEventHandler(&EasyHttpSession::onClientResolve, this);

	verboseLog("Connecting");

	CI_LOG_V("Connecting to : " << mHttpRequest.getHeader("Host"));

	mClient->connect(mHttpRequest.getHeader("Host"), port);
}

// ------------

void EasyHttpSession::onClientConnect(TcpSessionRef session) {
	mHttpResponse = HttpResponse();
	mSession = session;

	// TODO set timeout?
	// http://stackoverflow.com/questions/292997/can-you-set-so-rcvtimeo-and-so-sndtimeo-socket-options-in-boost-asio

	verboseLog("Connected");

	// - Request
	//	- response
	//	- error

	mSession->connectCloseEventHandler(&EasyHttpSession::onSessionClose, this);
	mSession->connectErrorEventHandler(&EasyHttpSession::onSessionError, this);
	mSession->connectReadCompleteEventHandler(&EasyHttpSession::onSessionReadComplete, this);
	mSession->connectReadEventHandler(&EasyHttpSession::onSessionRead, this);
	mSession->connectWriteEventHandler(&EasyHttpSession::onSessionWrite, this);
	mSession->write(mHttpRequest.toBuffer());
}

void EasyHttpSession::onClientResolve() {
	verboseLog("Endpoint resolved");
}

void EasyHttpSession::cancel() {
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

void EasyHttpSession::onSessionWrite(size_t bytesTransferred) {
	verboseLog(toString(bytesTransferred) + " bytes written");
	mSession->read();
}

void EasyHttpSession::onSessionRead(ci::BufferRef buffer) {
	verboseLog(toString(buffer->getSize()) + " bytes read");

	if (!mHttpResponse.hasHeader()) {
		mHttpResponse.parseHeader(HttpResponse::bufferToString(buffer));
		buffer = HttpResponse::removeHeader(buffer);
	}
	mHttpResponse.append(buffer);

	mSession->read();
}

void EasyHttpSession::onSessionReadComplete() {
	verboseLog("Response: \n" + toString(mHttpResponse));

	mSuccess = true;
	mSession->close();
}

void EasyHttpSession::onSessionClose() {
	verboseLog("Disconnected");
	conclude();
}

void EasyHttpSession::onClientError(string err, size_t bytesTransferred) {
	string text = "Client Error";
	if (!err.empty()) {
		text += ": " + err;
	}

	mFailure = true;
	conclude();
	CI_LOG_E(text);
}

void EasyHttpSession::onSessionError(string err, size_t bytesTransferred) {
	string text = "Session Error";
	if (!err.empty()) {
		text += ": " + err;
	}

	mFailure = true;
	conclude();
	CI_LOG_E(text);
}

void EasyHttpSession::conclude() {
	// TODo move this elsewhere?
	if (mSuccess && mSuccessCallback) {
		mSuccessCallback(mHttpResponse);
		// mSuccessCallback = nullptr;
	} else if (mFailure && mFailureCallback) {
		mFailureCallback("Something happened.");
		// mFailureCallback = nullptr;
	}
}

void EasyHttpSession::verboseLog(std::string message) {
	if (isAbsurdlyVerboseLoggingEnabled) {
		CI_LOG_V(message);
	}
}

} // namespace kit
} // namespace kp