#pragma once

#include <map>
#include <functional>
#include <vector>

template <typename MSG, typename ... ARGS>
class Observable
{
protected:
	std::map<MSG, std::vector<std::function<void(ARGS...)>>> _callbacks;
public:
	void Observe(MSG msg, std::function<void(ARGS...)> callback) {
		//_callbacks[msg] = callback;
		_callbacks[msg].push_back(callback);
	}

	void Invoke(MSG msg, ARGS... args) {
		if (_callbacks.find(msg) != _callbacks.end()) {
			//_callbacks[msg](std::forward<ARGS>(args)...);
			for (auto f : _callbacks[msg]) {
				f(std::forward<ARGS>(args)...);
			}
		}
	}
};