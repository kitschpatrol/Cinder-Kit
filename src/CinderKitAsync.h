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

using namespace std;
using namespace ci;
using namespace ci::app;

// some stuff from https://github.com/araid/Cinder-Async

static int sIdCount;
static map<int, shared_future<void>> sFutures;

static void callOnBackgroundThread(function<void()> func, function<void()> callback) {
	int futureId = sIdCount++;
	sFutures[futureId] = async(launch::async, [func, callback, futureId] {
		// cout << "Calling from thread " << this_thread::get_id() << endl;
		func();

		App::get()->dispatchAsync([callback, futureId] { // called on main thread
			callback();
			sFutures.erase(futureId);
		});
	});
};

// don't capture references
static void callAfterDelay(double delaySeconds, function<void()> callback) {
	callOnBackgroundThread(
			[delaySeconds]() {
				// windows chokes on chrono::duration<double>(delaySeconds)
				this_thread::sleep_for(chrono::milliseconds(static_cast<long long>(delaySeconds * 1000.0f))); //
			},
			callback);
};

} // namespace kit
} // namespace kp
