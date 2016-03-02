//
//  CairoKit.h
//  WallControl
//
//  Created by Eric Mika on 3/1/16.
//
//

#pragma once

#include "cinder/cairo/Cairo.h"

namespace kp {
namespace kit {
namespace cairo {

static void appendPolyline(ci::cairo::Context &context, const ci::PolyLine2f &polyline) {
	if (!polyline.getPoints().empty()) {
		context.moveTo(polyline.getPoints()[0]);

		for (size_t p = 1; p < polyline.getPoints().size(); ++p) {
			context.lineTo(polyline.getPoints()[p]);
		}

		if (polyline.isClosed()) {
			context.closePath();
		}
	}
}

} // namespace cairo
} // namespace kit
} // namespace kp