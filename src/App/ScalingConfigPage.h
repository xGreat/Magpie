#pragma once

#include "ScalingConfigPage.g.h"
#include <winrt/Magpie.Runtime.h>


namespace winrt::Magpie::App::implementation {

struct ScalingConfigPage : ScalingConfigPageT<ScalingConfigPage> {
	ScalingConfigPage();

	void ComboBox_DropDownOpened(IInspectable const& sender, IInspectable const&);

	void CaptureModeComboBox_SelectionChanged(IInspectable const&, Controls::SelectionChangedEventArgs const&);

	void MultiMonitorUsageComboBox_SelectionChanged(IInspectable const&, Controls::SelectionChangedEventArgs const&);

	void Is3DGameModeToggleSwitch_Toggled(IInspectable const&, RoutedEventArgs const&);

	void ShowFPSToggleSwitch_Toggled(IInspectable const&, RoutedEventArgs const&);

	void VSyncToggleSwitch_Toggled(IInspectable const&, RoutedEventArgs const&);

	void TripleBufferingToggleSwitch_Toggled(IInspectable const&, RoutedEventArgs const&);

private:
	void _UpdateVSync();

	bool _initialized = false;
	Magpie::Runtime::MagSettings _magSettings{ nullptr };
};

}

namespace winrt::Magpie::App::factory_implementation {

struct ScalingConfigPage : ScalingConfigPageT<ScalingConfigPage, implementation::ScalingConfigPage> {
};

}
