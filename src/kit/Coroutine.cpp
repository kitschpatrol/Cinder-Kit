#include "Coroutine.h"
#include "cinder/CinderAssert.h"
#include "cinder/Log.h"
#include <algorithm>
#include <thread>

namespace kit {

// This is thrown when the outer thread stop():s the coroutine.
struct AbortException {};

static std::atomic<unsigned> s_cr_counter{0};

// ----------------------------------------------------------------------------

// Trick for allowing forward-declaration of std::thread.
class Coroutine::Thread : public std::thread {
public:
	using std::thread::thread;
};

// ----------------------------------------------------------------------------

Coroutine::Coroutine(const char *debug_name_base, std::function<void(InnerControl &ic)> fun) {
	_debug_name = std::string(debug_name_base) + " " + std::to_string(s_cr_counter++);
	CI_LOG_V(_debug_name << ": Coroutine starting");

	_mutex.lock();
	_inner = std::unique_ptr<InnerControl>(new InnerControl(*this));

	_thread = std::unique_ptr<Thread>(new Thread([this, fun] {
		CI_LOG_V(_debug_name << ": Coroutine thread starting up");

		_mutex.lock();
		CI_ASSERT(!_control_is_outer);

		try {
			fun(*_inner);
		} catch (AbortException &) {
			CI_LOG_V(_debug_name << ": AbortException caught");
		} catch (std::exception &e) {
			CI_LOG_E(_debug_name << ": Exception caught from Coroutine: " << e.what());
		} catch (...) {
			CI_LOG_E(_debug_name << ": Unknown exception caught from Coroutine");
		}
		_is_done = true;
		_control_is_outer = true;
		_mutex.unlock();
		_cond.notify_one();

		CI_LOG_V(_debug_name << ": Coroutine thread shutting down");
	}));

	CI_ASSERT(_inner);
}

Coroutine::~Coroutine() {
	stop();
	CI_ASSERT(!_thread);
	CI_LOG_V(_debug_name << ": Coroutine destroyed");
}

void Coroutine::stop() {
	if (_thread) {
		if (!_is_done) {
			CI_LOG_V("Aborting coroutine '" << _debug_name << "'...");
			_abort = true;
			while (!_is_done) {
				_inner->poll(0);
			}
		}
		_thread->join();
		CI_ASSERT(_is_done);
		_thread = nullptr;
	}
}

// Returns 'true' on done
bool Coroutine::poll(double dt) {
	CI_ASSERT(_inner);
	if (!_is_done) {
		_inner->poll(dt);
	}
	return _is_done;
}

// ----------------------------------------------------------------------------

void InnerControl::wait_sec(double s) {
	auto target_time = _time + s;
	wait_for([=]() { return target_time <= _time; });
}

void InnerControl::yield() {
	CI_ASSERT(_cr._control_is_outer.load() == false);
	_cr._control_is_outer = true;
	_cr._mutex.unlock();
	_cr._cond.notify_one();

	// Let the outer thread do its business. Wait for it to return to us:

	std::unique_lock<std::mutex> lock(_cr._mutex);
	_cr._cond.wait(lock, [=] { return !_cr._control_is_outer; });
	lock.release();
	CI_ASSERT(_cr._control_is_outer.load() == false);

	if (_cr._abort) {
		CI_LOG_V(_cr._debug_name << ": throwing AbortException");
		throw AbortException();
	}
}

// Called from Outer:
void InnerControl::poll(double dt) {
	CI_ASSERT(_cr._control_is_outer.load() == true);
	_cr._control_is_outer = false;
	_cr._mutex.unlock();
	_cr._cond.notify_one();

	// Let the inner thread do its business. Wait for it to return to us:

	std::unique_lock<std::mutex> lock(_cr._mutex);
	_cr._cond.wait(lock, [=] { return !!_cr._control_is_outer; });
	lock.release();
	CI_ASSERT(_cr._control_is_outer.load() == true);

	_time += dt;
}

// ----------------------------------------------------------------------------

void CoroutineSet::clear() {
	_list.clear();
}

std::shared_ptr<Coroutine> CoroutineSet::start(const char *debug_name, std::function<void(InnerControl &ic)> fun) {
	auto cr_ptr = std::make_shared<Coroutine>(debug_name, std::move(fun));
	_list.push_back(cr_ptr);
	return cr_ptr;
}

bool CoroutineSet::erase(std::shared_ptr<Coroutine> cr) {
	auto it = std::find(begin(_list), end(_list), cr);
	if (it != _list.end()) {
		_list.erase(it);
		return true;
	} else {
		return false;
	}
}

void CoroutineSet::poll(double dt) {
	for (auto it = begin(_list); it != end(_list);) {
		if ((*it)->poll(dt)) {
			it = _list.erase(it);
		} else {
			++it;
		}
	}
}

} // namespace kit
