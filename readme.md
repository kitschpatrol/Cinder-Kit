<!-- title({ prefix: "Cinder-" }) -->

# Cinder-Kit

<!-- /title -->

<!-- badges -->

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/license/mit)

<!-- /badges -->

<!-- short-description -->

**CinderBlock for small utilities covering graphics, input, HTTP, asynchronous tasks, JSON, strings, colors, time, curves, and other odds and ends.**

<!-- /short-description -->

## Overview

A grab bag of helpers for [Cinder](https://libcinder.org). Usually a mess.

Similar in spirit but not in scope to [KPKit](https://github.com/kitschpatrol/KPKit).

The [CinderBlock](https://libcinder.org/docs/guides/cinder-blocks/index.html) includes math and logging shortcuts, background tasks, coroutines, JSON serialization for vectors, string and time formatting, color helpers, spline curves, a state machine driven by a linear value, mouse-to-touch simulation, NanoVG and Cairo drawing helpers, and a simple asynchronous HTTP client.

Pronounced "syndicate". 😬

## Getting started

### Dependencies

- [Cinder](https://libcinder.org) on macOS or Windows.
- [Cinder-Asio](https://github.com/BanTheRewind/Cinder-Asio) and [Cinder-Protocol](https://github.com/kitschpatrol/Cinder-Protocol) for HTTP helpers.
- [Cinder-NanoVG](https://github.com/notlion/Cinder-NanoVG) and Cinder's bundled Cairo block for graphics helpers.

The Cinder-Protocol link points to the kitschpatrol fork, which adds the `cinderblock.xml` that TinderBox needs. `String.h` uses Boost's string algorithms from the Boost headers that Cinder bundled at the time.

Most of the block was written in early 2016, and its code was last changed in June 2017, when Cinder's master branch was at 0.9.2dev. It was used on macOS and Windows, but the Xcode and Visual Studio versions weren't recorded. It hasn't been tested with later Cinder releases.

### Installation

From your Cinder `blocks` directory:

```sh
git clone https://github.com/kitschpatrol/Cinder-Kit.git
```

Then add **Kit** to a project with [TinderBox](https://libcinder.org/docs/guides/tinderbox/index.html).

TinderBox also needs the blocks that Kit requires. Cairo ships with Cinder; clone the others into `blocks` as well:

```sh
git clone https://github.com/BanTheRewind/Cinder-Asio.git
git clone https://github.com/kitschpatrol/Cinder-Protocol.git
git clone --recursive https://github.com/notlion/Cinder-NanoVG.git
```

To skip TinderBox, add the block's `src` directory to your project's include paths and compile the `.cpp` files for the modules you use.

## Usage

Include the headers you need from [`src/kit`](src/kit). Helpers live in the `kit` namespace, with app, graphics, and networking utilities in `kit::app`, `kit::graphics`, and `kit::net`.

```cpp
#include "kit/Json.h"

auto json = kit::toJson(ci::vec2(10.0f, 20.0f));
auto point = kit::fromJson(json);
```

`callOnBackgroundThread()`, `callAfterDelay()`, and the HTTP client deliver their callbacks on the main thread:

```cpp
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Log.h"

#include "kit/Async.h"
#include "kit/net/EasyHttp.h"

class MyApp : public ci::app::App {
public:
	void setup() override {
		kit::callOnBackgroundThread([] { /* Slow work */ }, [] { CI_LOG_I("Back on the main thread"); });
		kit::callAfterDelay(2.0, [] { CI_LOG_I("Two seconds later"); });

		mHttp = kit::net::EasyHttp::create();
		mHttp->request(
				"http://example.com/",
				[](std::string response) { CI_LOG_I(response); },
				[](std::string error) { CI_LOG_E(error); });
	}

private:
	kit::net::EasyHttpRef mHttp; // Must outlive its requests
};

CINDER_APP(MyApp, ci::app::RendererGl)
```

What's in each header:

| Header                       | Contents                                                                                                                                                                          |
| ---------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `kit/Kit.h`                  | `KP_WATCH` and `KP_LOG_FUNCTION` logging macros, `roundTo()`, `getWave()`, `randomElement()`, `getRandomPoint()`, `polarToCartesian()` / `cartesianToPolar()`, `getMachineName()` |
| `kit/Async.h`                | `callOnBackgroundThread()`, `callOnBackgroundThreadManualDispatch()`, `callAfterDelay()`                                                                                          |
| `kit/Color.h`                | `getRandomColor()`, `perceptualColorDistance()`, `colorToInt()`                                                                                                                   |
| `kit/Coroutine.h`            | `Coroutine`, `InnerControl`, and `CoroutineSet`: coroutines backed by threads                                                                                                     |
| `kit/Curve.h`                | `curveThrough()` for building a `ci::BSpline` through points, `SplineArcLengthParameterizer` for constant-speed travel along a spline                                             |
| `kit/Json.h`                 | `toJson()` / `fromJson()` for `ci::vec2` and `std::pair`                                                                                                                          |
| `kit/LinearStateMachine.h`   | `LinearStateMachine`: turns a linear value into transition states and progress, with signals                                                                                      |
| `kit/String.h`               | `startsWith()`, `pluralize()`, `toString()` with precision and padding, `toCodeString()` for `ci::vec2` and `ci::Rectf`                                                           |
| `kit/Time.h`                 | `stringToTime()`, `timeToString()`, `secondsToHMS()`                                                                                                                              |
| `kit/app/MouseToTouch.h`     | `MouseToTouch`: emits touch events from left-button mouse input                                                                                                                   |
| `kit/graphics/Cairo.h`       | `appendPolyline()` for a `ci::cairo::Context`                                                                                                                                     |
| `kit/graphics/Gl.h`          | `drawCross()`                                                                                                                                                                     |
| `kit/graphics/NanoTexture.h` | `NanoTexture`: renders NanoVG drawing into a texture                                                                                                                              |
| `kit/graphics/PsBlend.h`     | `BlendMode` and `PsBlend` for Photoshop-style blend modes (unfinished)                                                                                                            |
| `kit/net/EasyHttp.h`         | `EasyHttp`: asynchronous HTTP requests through Cinder-Protocol and Cinder-Asio                                                                                                    |

## Background

### Implementation notes

`callOnBackgroundThread()` runs its function with `std::async`, then hands the callback to the main thread with `App::dispatchAsync()`. `callAfterDelay()` sleeps on a background thread and then runs its callback on the main thread, so the callback shouldn't capture references to locals that may be gone by then.

`EasyHttp` connects through Cinder's own Asio `io_service`, so its callbacks run on the main thread. Each request gets an `EasyHttpSession`, which `EasyHttp` holds until the request finishes.

`Coroutine` runs each coroutine on its own thread, but only one of the inner and outer threads runs at a time. Call `poll(dt)` regularly; the time deltas drive `InnerControl::wait_sec()`.

`LinearStateMachine` checks a linear value against a list of active ranges. A range of `(4, 4)` transitions in from 3 to 4 and out from 4 to 5. Moving the value emits state change and progress signals, including the intermediate states it passes through.

`MouseToTouch` turns left-button mouse down, drag, and up events into touch events on the current window. The mouse-up handler doesn't check the button, because on Windows `isLeftDown()` is false on mouse up.

Some modules are adapted from other people's code:

- `Curve` is [David Wicks](https://github.com/sansumbrella)' curve code.
- `Coroutine` is from Emil Ernerfeldt's [emilib](https://github.com/emilk/emilib), described in his post [Coroutines for games](https://www.ilikebigbits.com/blog/2016/3/20/coroutines-for-games/).
- `PsBlend` is based on Akira Hayasaka's [ofxPSBlend](https://github.com/Akira-Hayasaka/ofxPSBlend) for openFrameworks.
- Parts of `Async.h` come from [Cinder-Async](https://github.com/araid/Cinder-Async).
- The `toString()` helpers in `String.h` are from Reza Ali's Cinder UI.

### Known issues

- `PsBlend` is unfinished. Both `drawWithBlend()` overloads are empty, and `getBlendShaderRef()` loads `shaders/passthrough.vert` and `shaders/difference.frag` from the app's assets, which aren't included, instead of using the embedded shader strings. `PsBlend.cpp` also includes the app's `Resources.h`.
- The URL-based `EasyHttp::request()` overloads always send HTTP/1.0 to port 80, so `https://` URLs and URLs with a port don't work.
- The `EasyHttp` failure callback defaults to `nullptr` but is called without a check, so a failed request without one throws `std::bad_function_call`. Connection and session errors always pass "Something bad happened." to it, without the underlying error. An invalid URL calls the failure callback and then still sends the request.
- `EasyHttpSession::cancel()` isn't implemented, and requests have no timeout.
- The `Async.h` helpers keep their state in `static` variables in the header, so each source file that includes it gets its own copy. Call them from the main thread.
- `roundTo()` truncates the remainder to an integer, so it only rounds correctly when the value and the step are whole numbers.
- `KP_LOG_FUNCTION` and `KP_LOG_FUNCTION_COUT` use `__PRETTY_FUNCTION__`, which Visual Studio doesn't support.
- `getUUID()` and `getUUIDString()` are commented out in `Kit.h`, because Boost's UUID library isn't in the Boost headers bundled with Cinder.
- `NanoTexture` doesn't account for high-density displays.
- On Windows, `getMachineName()` passes `GetComputerNameEx()` the buffer size in bytes instead of `TCHAR`s and copies each `TCHAR` into a `char`, so in Unicode builds long names can overrun the buffers and non-ASCII characters come out wrong.
- Cinder's current master branch (0.9.4dev) no longer bundles Boost, and it moved the deprecated `ci::JsonTree` to `cinder/JsonTree.h` (`cinder/Json.h` is now the nlohmann-based `ci::Json`), so `String.h` and `Json.h` need changes to build against it.

## Maintainers

[kitschpatrol](https://github.com/kitschpatrol)

<!-- contributing -->

## Contributing

[Issues](https://github.com/kitschpatrol/Cinder-Kit/issues) are welcome and appreciated.

Please open an issue to discuss changes before submitting a pull request. Unsolicited PRs (especially AI-generated ones) are unlikely to be merged.

This repository uses [MDAT](https://github.com/kitschpatrol/mdat) for readme placeholder expansion.

<!-- /contributing -->

<!-- license -->

## License

[MIT](license.txt) © Eric Mika

<!-- /license -->

The MIT license covers Eric Mika's code in [`src/kit`](src/kit). `Curve.h` and `Curve.cpp` are Copyright (c) 2013 David Wicks, under the BSD 2-Clause terms in their headers. The emilib coroutine code is in the public domain. `PsBlend.h` keeps Akira Hayasaka's ofxPSBlend copyright notice, and ofxPSBlend doesn't state a license.
