// EasyHttpSession
//
// One instance of an HTTP request, managed by EasyHttp.

#pragma once

#include "CinderAsio.h"

#include "cinder/Cinder.h"

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

	void cancel(); // not implemented
	bool isAbsurdlyVerboseLoggingEnabled = false;


	~EasyHttpSession();

private:
	// Only EasyHttp can create EasyHttpSessions via friend association
	void request(HttpRequest request, uint16_t port,								 //
							 std::function<void(HttpResponse response)> success, //
							 std::function<void(std::string error)> failure = nullptr);

	HttpRequest mHttpRequest;
	HttpResponse mHttpResponse;

	// need something better than these flags...
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