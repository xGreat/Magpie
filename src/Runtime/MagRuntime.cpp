#include "pch.h"
#include "MagRuntime.h"
#if __has_include("MagRuntime.g.cpp")
#include "MagRuntime.g.cpp"
#endif
#include "MagApp.h"
#include "Logger.h"
#include <dispatcherqueue.h>


namespace winrt::Magpie::Runtime::implementation {

MagRuntime::~MagRuntime() {
	Stop();

	if (_magWindThread.joinable()) {
		_magWindThread.join();
	}
}

void MagRuntime::Run(uint64_t hwndSrc, MagSettings const& settings) {
	if (_running) {
		return;
	}

	_hwndSrc = hwndSrc;
	_running = true;
	_isRunningChangedEvent(*this, true);

	if (_magWindThread.joinable()) {
		_magWindThread.join();
	}

	_magWindThread = std::thread([=, this]() {
		winrt::init_apartment(winrt::apartment_type::multi_threaded);

		DispatcherQueueOptions options{};
		options.dwSize = sizeof(options);
		options.threadType = DQTYPE_THREAD_CURRENT;
		options.apartmentType = DQTAT_COM_NONE;

		HRESULT hr = CreateDispatcherQueueController(options, (ABI::Windows::System::IDispatcherQueueController**)put_abi(_dqc));
		if (FAILED(hr)) {
			_running = false;
			_isRunningChangedEvent(*this, false);
			return;
		}

		MagApp& app = MagApp::Get();
		app.Run((HWND)hwndSrc, settings);

		_running = false;
		_dqc = nullptr;
		_isRunningChangedEvent(*this, false);
	});
}

void MagRuntime::ToggleOverlay() {
	if (!_running || !_dqc) {
		return;
	}

	_dqc.DispatcherQueue().TryEnqueue([]() {
		MagApp::Get().ToggleOverlay();
	});
}

void MagRuntime::Stop() {
	if (!_running || !_dqc) {
		return;
	}

	_dqc.DispatcherQueue().TryEnqueue([]() {
		MagApp::Get().Stop();
	});

	if (_magWindThread.joinable()) {
		_magWindThread.join();
	}
}

// 调用者应处理线程同步
event_token MagRuntime::IsRunningChanged(EventHandler<bool> const& handler) {
	return _isRunningChangedEvent.add(handler);
}

void MagRuntime::IsRunningChanged(event_token const& token) noexcept {
	_isRunningChangedEvent.remove(token);
}

}