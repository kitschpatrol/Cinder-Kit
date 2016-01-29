// LinearStateMachine.cpp
// WorkbenchProjection
//
// Created by Eric Mika on 1/19/16.
//

#include "LinearStateMachine.h"

#include "cinder/Cinder.h"
#include "cinder/Log.h"
#include "cinder/app/App.h"

namespace kp {
namespace kit {

LinearStateMachineRef LinearStateMachine::create() {
	return LinearStateMachineRef(new LinearStateMachine())->shared_from_this();
}

LinearStateMachine::LinearStateMachine() {
	// CI_LOG_V("LinearStateMachine Created");
}

LinearStateMachine::~LinearStateMachine() {
	// CI_LOG_V("LinearStateMachine Destroyed");
}

float LinearStateMachine::getLinearValue() const {
	return mLinearValue;
}

void LinearStateMachine::setLinearValue(float value) {
	if (mLinearValue != value) {
		mPreviousLinearValue = mLinearValue;
		mLinearValue = value;

		// Emit intermediate state transitions
		float delta = 0.1;
		float intermediaLinearValue = mPreviousLinearValue;
		if (mPreviousLinearValue < mLinearValue) {
			// walk right
			while (intermediaLinearValue < mLinearValue) {
				setState(calculateStateAndProgressForValue(intermediaLinearValue).first);
				intermediaLinearValue += delta;
			}
		} else {
			// walk left
			while (intermediaLinearValue > mLinearValue) {
				setState(calculateStateAndProgressForValue(intermediaLinearValue).first);
				intermediaLinearValue -= delta;
			}
		}

		// Calculate final state and progress

		std::pair<State, float> pair = calculateStateAndProgressForValue(mLinearValue);

		setState(pair.first);
		setProgress(pair.second);
	}
}

const std::vector<LinearStateMachine::Range> &LinearStateMachine::getActiveRanges() const {
	return mActiveRanges;
}

void LinearStateMachine::setActiveRanges(std::vector<Range> ranges) {
	if (mActiveRanges != ranges) {
		mActiveRanges = ranges;
		// todo refresh state?
	}
}

LinearStateMachine::SignalStateProgress &LinearStateMachine::getSignalStateProgress() {
	return mSignalStateProgress;
}

LinearStateMachine::SignalStateChange &LinearStateMachine::getSignalStateChange() {
	return mSignalStateChange;
}

const LinearStateMachine::State LinearStateMachine::getPreviousState() const {
	return mPreviousState;
}
const LinearStateMachine::State LinearStateMachine::getCurrentState() const {
	return mCurrentState;
}

float LinearStateMachine::getProgress() const {
	return mCurentStateProgress;
}

std::pair<LinearStateMachine::State, float> LinearStateMachine::calculateStateAndProgressForValue(float value) {
	// Assume nothing's around
	State state = State::STEADY_OUT;
	float progress = 1.0;

	for (const Range &range : mActiveRanges) {
		// Are we in it?
		const float leftEdge = range.first - transitionInAmount;
		const float rightEdge = range.second + transitionOutAmount;

		if (value > leftEdge && value < range.first) {
			// Transitioning in
			progress = ci::lmap(value, leftEdge, range.first, 0.0f, 1.0f);
			state = State::TRANSITIONING_IN;
			break;
		} else if (value >= range.first && value <= range.second) {
			// Steady in
			progress = ci::lmap(value, range.first, range.second, 0.0f, 1.0f);
			state = State::STEADY_IN;
			break;
		} else if (value > range.second && value < rightEdge) {
			// Transitioning Out, flip progress because it's convenient to treat transitioning in and out as the same thing most of the time
			progress = ci::lmap(value, range.second, rightEdge, 1.0f, 0.0f);
			state = State::TRANSITIONING_OUT;
			break;
		}
	}

	return std::make_pair(state, progress);
}

void LinearStateMachine::setProgress(float progress) {
	if (progress != mCurentStateProgress) {
		mCurentStateProgress = progress;
		mSignalStateProgress.emit(mCurrentState, mCurentStateProgress);
	}
}

void LinearStateMachine::setState(State state) {
	// check for changes
	// TODO fire intermediates as necessary?
	if (mCurrentState != state) {
		mPreviousState = mCurrentState;
		mCurrentState = state;

		// Cross zero
		if (mPreviousState == State::TRANSITIONING_OUT && mCurrentState == State::TRANSITIONING_IN) {
			mSignalStateChange.emit(State::STEADY_IN, State::TRANSITIONING_OUT);
			mPreviousState = State::STEADY_IN;
		} else if (mPreviousState == State::TRANSITIONING_IN && mCurrentState == State::TRANSITIONING_OUT) {
			mSignalStateChange.emit(State::STEADY_IN, State::TRANSITIONING_IN);
			mPreviousState = State::STEADY_IN;
		}

		mSignalStateChange.emit(mCurrentState, mPreviousState);
	}
}

std::string LinearStateMachine::getStringFromState(LinearStateMachine::State state) {
	switch (state) {
		case LinearStateMachine::State::TRANSITIONING_IN:
			return "TRANSITIONING_IN";
		case LinearStateMachine::State::TRANSITIONING_OUT:
			return "TRANSITIONING_OUT";
		case LinearStateMachine::State::STEADY_IN:
			return "STEADY_IN";
		case LinearStateMachine::State::STEADY_OUT:
			return "STEADY_OUT";
	}
}

} // namespace kit
} // namespace kp