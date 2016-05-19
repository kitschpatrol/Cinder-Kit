//
//  Async.h
//  WorkbenchTablet
//
//  Created by Eric Mika on 3/18/16.
//  some stuff from https://github.com/araid/Cinder-Async
//

#pragma once

#include "cinder/app/App.h"

namespace kit {

static int sIdCount;
static std::map<int, std::shared_future<void>> sFutures;

static void callOnBackgroundThread(std::function<void()> func, std::function<void()> callback) {
	int futureId = sIdCount++;
	sFutures[futureId] = async(std::launch::async, [func, callback, futureId] {
		// cout << "Calling from thread " << this_thread::get_id() << endl;
		func();

		ci::app::App::get()->dispatchAsync([callback, futureId] { // called on main thread
			callback();
			sFutures.erase(futureId);
		});
	});
};

// call "clean up" yourself. useful if you need to pass more stuff back to the main thread
static void callOnBackgroundThreadManualDispatch(std::function<void()> func) {
	int futureId = sIdCount++;

	sFutures[futureId] = async(std::launch::async, [func, futureId] {
		func();																					// needs to call back to the main thread on its own
		ci::app::App::get()->dispatchAsync([futureId] { // called on main thread
			sFutures.erase(futureId);
		});
	});
};

// don't capture references
static void callAfterDelay(double delaySeconds, std::function<void()> callback) {
	callOnBackgroundThread(
			[delaySeconds]() {
				// windows chokes on chrono::duration<double>(delaySeconds)
				std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long long>(delaySeconds * 1000.0f))); //
			},
			callback);
};

} // namespace kit
