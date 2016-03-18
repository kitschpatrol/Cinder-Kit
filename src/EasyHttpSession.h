// EasyHttpSession.h
// SmartMuseumBasic
//
// Created by Eric Mika on 3/7/16.
//

#pragma once

// For use with
// https://github.com/BanTheRewind/Cinder-Protocol
// and
// https://github.com/BanTheRewind/Cinder-Asio

// Example:
// EasyHttpSessionRef EasyHttpSession = EasyHttpSession::create();
// EasyHttpSession->request("http://example.com",
//									 [&](std::string successResponse) { CI_LOG_V("Success!" << successResponse); },
//                   [&](std::string failureResponse) { CI_LOG_V("Fail!" << failureResponse); }
//                  );

#include "cinder/Cinder.h"

#include "CinderAsio.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "TcpClient.h"

namespace kp {
namespace kit {

typedef std::shared_ptr<class EasyHttpSession> EasyHttpSessionRef;

class EasyHttpSession : public std::enable_shared_from_this<EasyHttpSession> {
	friend class EasyHttp;

public:
	static EasyHttpSessionRef create();
	~EasyHttpSession();

	void request(HttpRequest request, uint16_t port, std::function<void(HttpResponse response)> success,
							 std::function<void(std::string error)> failure = nullptr);
	void cancel();

	bool isAbsurdlyVerboseLoggingEnabled = false;

protected:
	EasyHttpSession();

private:
	HttpRequest mHttpRequest;
	HttpResponse mHttpResponse;

	// precautions
	bool mSuccess;
	bool mFailure;

	std::function<void(HttpResponse response)> mSuccessCallback;
	std::function<void(std::string error)> mFailureCallback;
	std::function<void(std::string response)> mBasicSuccessCallback;

	TcpClientRef mClient;
	TcpSessionRef mSession;

	void onClientConnect(TcpSessionRef session);
	void onClientError(std::string err, std::size_t bytesTransferred);
	void onClientResolve();

	void onSessionWrite(std::size_t bytesTransferred);
	void onSessionRead(ci::BufferRef buffer);
	void onSessionReadComplete();

	void onSessionClose();
	void onSessionError(std::string err, std::size_t bytesTransferred);

	void conclude();

	void verboseLog(std::string message);
};

} // namespace kit
} // namespace kp