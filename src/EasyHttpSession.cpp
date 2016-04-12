#include "EasyHttpSession.h"

#include "HttpInterface.h"
#include "cinder/Cinder.h"
#include "cinder/Log.h"
#include "cinder/Utilities.h"
#include "cinder/app/App.h"

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

void EasyHttpSession::request(HttpRequest request, uint16_t port,									//
															std::function<void(HttpResponse response)> success, //
															std::function<void(std::string error)> failure) {
	if (mSession && mSession->getSocket()->is_open()) {
		CI_LOG_E("Request in progress. Aborting. This should not be possible!");
		return;
	}


	mStartTime = getElapsedSeconds();

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
		CI_LOG_V("Request to " << mHttpRequest.getUri() << " succeeded in " << (getElapsedSeconds() - mStartTime) << " seconds.");
		mSuccessCallback(mHttpResponse);
	} else if (mFailure && mFailureCallback) {
		CI_LOG_V("Request to " << mHttpRequest.getUri() << " failed in " << (getElapsedSeconds() - mStartTime) << " seconds.");
		mFailureCallback("Something bad happened.");
	}
}

void EasyHttpSession::verboseLog(std::string message) {
	if (isAbsurdlyVerboseLoggingEnabled) {
		CI_LOG_V(message);
	}
}

} // namespace kit
} // namespace kp