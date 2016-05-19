//
//  TouchSimulator.hpp
//  CinderTabletTest
//
//  Created by Eric Mika on 12/7/15.
//
//

#pragma once

#include "cinder/App/App.h"
#include "cinder/Cinder.h"

namespace kit {
namespace app {

class MouseToTouch {

public:
	MouseToTouch();
	~MouseToTouch();

	void setIsEnabled(bool value);
	bool getIsEnabled();

protected:
	bool mIsEnabled;
	ci::vec2 mLastMousePosition;

	ci::signals::Connection mMouseDownConnection;
	ci::signals::Connection mMouseDragConnection;
	ci::signals::Connection mMouseUpConnection;
	void onMouseDownEvent(ci::app::MouseEvent event);
	void onMouseDragEvent(ci::app::MouseEvent event);
	void onMouseUpEvent(ci::app::MouseEvent event);
};

} // namespace app
} // namespace kit
