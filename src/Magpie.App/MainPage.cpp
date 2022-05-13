#include "pch.h"
#include <winrt/Windows.System.Profile.h>
#include "MainPage.h"
#if __has_include("MainPage.g.cpp")
#include "MainPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;
using namespace Windows::System::Profile;
using namespace Windows::UI::ViewManagement;


UINT GetOSBuild() {
	const winrt::hstring& deviceFamilyVersion = AnalyticsInfo::VersionInfo().DeviceFamilyVersion();
	uint64_t version = std::stoull(deviceFamilyVersion.c_str());
	return (version & 0x00000000FFFF0000L) >> 16;
}

namespace winrt::Magpie::App::implementation
{
    MainPage::MainPage()
    {
        InitializeComponent();
    }

    void MainPage::ThemeRadioButton_Checked(IInspectable const& sender, RoutedEventArgs const&) {
        if (sender == LightThemeRadioButton()) {
			_theme = 0;
        } else if (sender == DarkThemeRadioButton()) {
			_theme = 1;
        } else {
			_theme = 2;
        }

		_UpdateHostTheme();
    }

	void MainPage::HostWnd(uint64_t value) {
		_hostWnd = value;
		_UpdateHostTheme();
	}

	void MainPage::_UpdateHostTheme() {
		constexpr const DWORD DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1 = 19;
		constexpr const DWORD DWMWA_MICA_EFFECT = 1029;

		HWND hwndHost = (HWND)_hostWnd;
		if (!hwndHost) {
			return;
		}

		BOOL isDarkMode = FALSE;
		if (_theme == 0) {
			isDarkMode = FALSE;
		} else if(_theme == 1) {
			isDarkMode = TRUE;
		} else {
			isDarkMode = _uiSettings.GetColorValue(UIColorType::Background).R < 128;
		}

		if (_theme == 2) {
			if (!_colorChangedToken) {
				_colorChangedToken = _uiSettings.ColorValuesChanged(
					[this](UISettings const& sender, IInspectable const& args) {
						_UpdateHostTheme();
					}
				);
			}
		} else {
			if (_colorChangedToken) {
				_uiSettings.ColorValuesChanged(_colorChangedToken);
				_colorChangedToken = {};
			}
		}

		auto osBuild = GetOSBuild();
		
		// 使标题栏适应黑暗模式
		// build 18985 之前 DWMWA_USE_IMMERSIVE_DARK_MODE 的值不同
		// https://github.com/MicrosoftDocs/sdk-api/pull/966/files
		
		HRESULT hr = DwmSetWindowAttribute(
			hwndHost,
			osBuild < 18985 ? DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1 : DWMWA_USE_IMMERSIVE_DARK_MODE,
			&isDarkMode,
			sizeof(isDarkMode)
		);
		if (FAILED(hr)) {
			OutputDebugString(L"fail\n");
		}

		// 在 Win11 中应用 Mica
		if (osBuild >= 22000) {
			BOOL mica = TRUE;
			DwmSetWindowAttribute(hwndHost, DWMWA_MICA_EFFECT, &mica, sizeof(mica));
		}

		// 确保更改已应用
		SetWindowPos(hwndHost, NULL, 0, 0, 0, 0,
			SWP_DRAWFRAME | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);

		//RequestedTheme(isDarkMode ? ElementTheme::Dark : ElementTheme::Light);
	}
}