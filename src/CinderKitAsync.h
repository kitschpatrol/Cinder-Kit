//
//  Async.h
//  WorkbenchTablet
//
//  Created by Eric Mika on 3/18/16.
//
//

#pragma once

#include "cinder/app/App.h"

namespace kp {
namespace kit {

// some stuff from https://github.com/araid/Cinder-Async

static int sIdCount;
static std::map<int, std::shared_future<void>> sFutures;

static void callOnBackgroundThread(std::function<void()> func, std::function<void()> callback) {
	int futureId = sIdCount++;
	sFutures[futureId] = std::async(std::launch::async, [func, callback, futureId] {
		// std::cout << "Calling from thread " << std::this_thread::get_id() << std::endl;
		func();

		ci::app::App::get()->dispatchAsync([callback, futureId] { // called on main thread
			callback();
			sFutures.erase(futureId);
		});
	});
};

// don't capture references
static void callAfterDelay(double delaySeconds, std::function<void()> callback) {
	callOnBackgroundThread(
			[delaySeconds]() {																													//
				std::this_thread::sleep_for(std::chrono::duration<double>(delaySeconds)); //
			},
			callback);
};

} // namespace kit
} // namespace kp
