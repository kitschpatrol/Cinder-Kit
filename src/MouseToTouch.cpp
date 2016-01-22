//
//  TouchSimulator.cpp
//  CinderTabletTest
//
//  Created by Eric Mika on 12/7/15.
//
//

#include "MouseToTouch.h"
#include "cinder/Log.h"

namespace kp {
namespace kit {
namespace app {

MouseToTouch::MouseToTouch() {
	mIsEnabled = false;
}

MouseToTouch::~MouseToTouch() {
	setIsEnabled(false);
}

void MouseToTouch::setIsEnabled(bool value) {
	if (value && !mIsEnabled) {
		// enable
		CI_LOG_V("Started Mapping Mouse To Touch");
		mMouseDownConnection = ci::app::getWindow()->getSignalMouseDown().connect(std::bind(&MouseToTouch::onMouseDownEvent, this, std::placeholders::_1));
		mMouseDragConnection = ci::app::getWindow()->getSignalMouseDrag().connect(std::bind(&MouseToTouch::onMouseDragEvent, this, std::placeholders::_1));
		mMouseUpConnection = ci::app::getWindow()->getSignalMouseUp().connect(std::bind(&MouseToTouch::onMouseUpEvent, this, std::placeholders::_1));
	} else if (!value && mIsEnabled) {
		CI_LOG_V("Stopped Mapping Mouse To Touch");
		mMouseDownConnection.disable();
		mMouseDragConnection.disable();
		mMouseUpConnection.disable();
	}
	mIsEnabled = value;
}

void MouseToTouch::onMouseDownEvent(ci::app::MouseEvent event) {
	if (event.isLeftDown()) {
		mLastMousePosition = event.getPos();

		ci::app::TouchEvent::Touch touch = ci::app::TouchEvent::Touch(event.getPos(), mLastMousePosition, -1, ci::app::getElapsedSeconds(), nullptr);
		std::vector<ci::app::TouchEvent::Touch> touches;
		touches.push_back(touch);

		ci::app::TouchEvent touchEvent = ci::app::TouchEvent(ci::app::WindowRef(), touches);
		ci::app::getWindow()->emitTouchesBegan(&touchEvent);
	}
}

void MouseToTouch::onMouseDragEvent(ci::app::MouseEvent event) {
	if (event.isLeftDown()) {
		ci::app::TouchEvent::Touch touch = ci::app::TouchEvent::Touch(event.getPos(), mLastMousePosition, -1, ci::app::getElapsedSeconds(), nullptr);
		std::vector<ci::app::TouchEvent::Touch> touches;
		touches.push_back(touch);

		ci::app::TouchEvent touchEvent = ci::app::TouchEvent(ci::app::WindowRef(), touches);
		ci::app::getWindow()->emitTouchesMoved(&touchEvent);

		mLastMousePosition = event.getPos();
	}
}

void MouseToTouch::onMouseUpEvent(ci::app::MouseEvent event) {
	if (event.isLeftDown()) {
		ci::app::TouchEvent::Touch touch = ci::app::TouchEvent::Touch(event.getPos(), mLastMousePosition, -1, ci::app::getElapsedSeconds(), nullptr);
		std::vector<ci::app::TouchEvent::Touch> touches;
		touches.push_back(touch);

		ci::app::TouchEvent touchEvent = ci::app::TouchEvent(ci::app::WindowRef(), touches);
		ci::app::getWindow()->emitTouchesEnded(&touchEvent);
	}
}

} // namespace app
} // namespace kit
} // namespace kp