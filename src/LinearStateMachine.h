// LinearStateMachine.h
// WorkbenchProjection
//
// Created by Eric Mika on 1/19/16.
//

#pragma once

#include "cinder/Cinder.h"
#include "cinder/Signals.h"

namespace kp {
namespace kit {

typedef std::shared_ptr<class LinearStateMachine> LinearStateMachineRef;

class LinearStateMachine : public std::enable_shared_from_this<LinearStateMachine> {
public:
	enum class State {
		TRANSITIONING_IN,
		STEADY_IN,
		TRANSITIONING_OUT,
		STEADY_OUT,
	};

	typedef std::pair<float, float> Range;
	typedef ci::signals::Signal<void(LinearStateMachine::State /*currentState*/, float /*stateProgress*/)> SignalStateProgress;
	typedef ci::signals::Signal<void(LinearStateMachine::State /*fromState*/, LinearStateMachine::State /*toState*/)> SignalStateChange;

	static LinearStateMachineRef create();
	~LinearStateMachine();

	float getLinearValue() const;
	void setLinearValue(float value, bool forceUpdate = false);

	// array of pairs defining the "in" and "out" point of an active segment of the linear range.
	// (4, 4) would transition in from 3->4, and out from 4->5
	const std::vector<Range> &getActiveRanges() const;
	void setActiveRanges(std::vector<Range> ranges);
	void setActiveRange(Range ranges); // convenience for setting single range

	void addActiveRange(Range range); // convenience for accumulating ranges, automatically merges with existing

	const State getPreviousState() const;
	const State getCurrentState() const;
	float getProgress() const;

	SignalStateProgress &getSignalStateProgress();
	SignalStateChange &getSignalStateChange();

	static std::string getStringFromState(LinearStateMachine::State state);

protected:
	LinearStateMachine();

private:
	float transitionInAmount = 1.0;
	float transitionOutAmount = 1.0;

	float mPreviousLinearValue = -1.0;
	float mLinearValue = -1.0;

	SignalStateProgress mSignalStateProgress;
	SignalStateChange mSignalStateChange;

	float mCurentStateProgress = -100.0;
	State mPreviousState = State::STEADY_OUT;
	State mCurrentState = State::STEADY_OUT;

	std::vector<Range> mActiveRanges; // TODO use a set for this, validate, etc.

	std::pair<LinearStateMachine::State /*state*/, float /*progress*/> calculateStateAndProgressForValue(float value);

	void setState(State state);				// emits if needed
	void setProgress(float progress); // emits if needed
};

} // namespace kit
} // namespace kp