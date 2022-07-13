#pragma once
#include "pch.h"
#include "NewProfileDialog.g.h"
#include "CandidateWindow.g.h"


namespace winrt::Magpie::App::implementation {

struct CandidateWindow : CandidateWindowT<CandidateWindow> {
	CandidateWindow(uint64_t hWnd, uint32_t dpi, bool isLightTheme, CoreDispatcher const& dispatcher);

	CandidateWindow(Magpie::App::CandidateWindow const& other, int);

	event_token PropertyChanged(PropertyChangedEventHandler const& handler) {
		return _propertyChangedEvent.add(handler);
	}

	void PropertyChanged(event_token const& token) noexcept {
		_propertyChangedEvent.remove(token);
	}

	uint64_t HWnd() const noexcept {
		return _hWnd;
	}

	hstring Title() const noexcept {
		return _title;
	}

	IInspectable Icon() const noexcept {
		return _icon;
	}

	hstring DefaultProfileName() const noexcept {
		return _defaultProfileName;
	}

	void OnThemeChanged(bool isLightTheme);

	void OnDpiChanged(uint32_t newDpi);

private:
	void _Icon(IInspectable const& value) {
		_icon = value;
		_propertyChangedEvent(*this, PropertyChangedEventArgs(L"Icon"));
	}

	fire_and_forget _ResolveWindow(bool resolveIcon, bool resolveName);

	event<PropertyChangedEventHandler> _propertyChangedEvent;

	bool _isPackagedApp = false;
	uint64_t _hWnd = 0;
	hstring _title;
	IInspectable _icon{ nullptr };
	hstring _defaultProfileName;

	bool _isLightTheme = false;
	uint32_t _dpi = 96;
	CoreDispatcher _dispatcher{ nullptr };
};

struct NewProfileDialog : NewProfileDialogT<NewProfileDialog> {
    NewProfileDialog();

	void ComboBox_DropDownOpened(IInspectable const& sender, IInspectable const&);

	event_token PropertyChanged(PropertyChangedEventHandler const& handler) {
		return _propertyChangedEvent.add(handler);
	}

	void PropertyChanged(event_token const& token) noexcept {
		_propertyChangedEvent.remove(token);
	}

	IVector<Magpie::App::CandidateWindow> CandidateWindows() const noexcept {
		return _candidateWindows;
	}

	int32_t WindowIndex() const noexcept {
		return _windowIndex;
	}

	void WindowIndex(int32_t value) noexcept;

	IVector<IInspectable> Profiles() const noexcept {
		return _profiles;
	}

	int32_t ProfileIndex() const noexcept {
		return _profileIndex;
	}

	void ProfileIndex(int32_t value) {
		_profileIndex = value;
		_propertyChangedEvent(*this, PropertyChangedEventArgs(L"ProfileIndex"));
	}

	void RootScrollViewer_SizeChanged(IInspectable const&, IInspectable const&);

	void Loaded(IInspectable const&, RoutedEventArgs const&);

	void ActualThemeChanged(IInspectable const&, IInspectable const&);

private:
	IAsyncAction _DisplayInformation_DpiChanged(Windows::Graphics::Display::DisplayInformation const&, IInspectable const&);

	fire_and_forget _UpdateCandidateWindows();

	event<PropertyChangedEventHandler> _propertyChangedEvent;

	IVector<Magpie::App::CandidateWindow> _candidateWindows;
	int32_t _windowIndex = -1;

	IVector<IInspectable> _profiles;
	int32_t _profileIndex = 0;

	Windows::Graphics::Display::DisplayInformation _displayInfomation{ nullptr };
	Windows::Graphics::Display::DisplayInformation::DpiChanged_revoker _dpiChangedRevoker;
};

}

namespace winrt::Magpie::App::factory_implementation {

struct CandidateWindow : CandidateWindowT<CandidateWindow, implementation::CandidateWindow> {
};

struct NewProfileDialog : NewProfileDialogT<NewProfileDialog, implementation::NewProfileDialog> {
};

}
