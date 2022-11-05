#include "pch.h"
#include "AboutViewModel.h"
#if __has_include("AboutViewModel.g.cpp")
#include "AboutViewModel.g.cpp"
#endif
#include "Version.h"
#include "UpdateService.h"


namespace winrt::Magpie::UI::implementation {

hstring AboutViewModel::Version() const noexcept {
	return hstring(L"v"s + MAGPIE_VERSION_W);
}

fire_and_forget AboutViewModel::CheckForUpdate() {
	co_await UpdateService::Get().CheckForUpdateAsync();
}

}