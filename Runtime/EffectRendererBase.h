#pragma once
#include "pch.h"
#include "D2DContext.h"
#include "nlohmann/json.hpp"
#include "Env.h"



using EffectCreateFunc = HRESULT(
	ID2D1Factory1* d2dFactory,
	ID2D1DeviceContext* d2dDC,
	IWICImagingFactory2* wicImgFactory,
	const nlohmann::json& props,
	ComPtr<ID2D1Effect>& effect,
	std::pair<float, float>& scale
);


// ȡ���ڲ�ͬ�Ĳ���ʽ�����в�ͬ��������룬�����������ͨ�õĲ���
// �̳д�����Ҫʵ�� _PushAsOutputEffect��Apply
// ���ڹ��캯���е��� _Init
class EffectRendererBase {
public:
	EffectRendererBase() :
		_d2dDC(Env::$instance->GetD2DDC()),
		_d2dFactory(Env::$instance->GetD2DFactory())
	{
	}

	virtual ~EffectRendererBase() {}

	// ���ɸ��ƣ������ƶ�
	EffectRendererBase(const EffectRendererBase&) = delete;
	EffectRendererBase(EffectRendererBase&&) = delete;

	virtual ComPtr<ID2D1Image> Apply(IUnknown* inputImg) = 0;

protected:
	void _Init() {
		_ReadEffectsJson(Env::$instance->GetScaleModel());

		const RECT hostClient = Env::$instance->GetHostClient();
		const RECT srcClient = Env::$instance->GetSrcClient();

		float width = (srcClient.right - srcClient.left) * _scale.first;
		float height = (srcClient.bottom - srcClient.top) * _scale.second;
		float left = roundf((hostClient.right - hostClient.left - width) / 2);
		float top = roundf((hostClient.bottom - hostClient.top - height) / 2);
		Env::$instance->SetDestRect({left, top, left + width, top + height});
	}

	// �� effect ���ӵ� effect ����Ϊ���
	virtual void _PushAsOutputEffect(ComPtr<ID2D1Effect> effect) = 0;

private:
	void _ReadEffectsJson(const std::string_view& scaleModel) {
		const auto& models = nlohmann::json::parse(scaleModel);
		Debug::Assert(models.is_array(), L"json ��ʽ����");

		for (const auto &model : models) {
			Debug::Assert(model.is_object(), L"json ��ʽ����");

			const auto &moduleName = model.value("module", "");

			std::wstring moduleNameW;
			Debug::ThrowIfComFailed(
				Utils::UTF8ToUTF16(moduleName, moduleNameW),
				L"����ģ��������"
			);
			HMODULE dll = LoadLibrary((boost::wformat(L"effects\\%1%") % moduleNameW).str().c_str());
			Debug::ThrowIfWin32Failed(dll, (boost::wformat(L"����ģ��%1%����") % moduleNameW).str());
			
			auto createEffect = (EffectCreateFunc*)GetProcAddress(dll, "CreateEffect");
			Debug::ThrowIfWin32Failed(createEffect, L"�Ƿ���dll");

			ComPtr<ID2D1Effect> effect;
			std::pair<float, float> scale;
			Debug::ThrowIfComFailed(
				createEffect(_d2dFactory, _d2dDC, Env::$instance->GetWICImageFactory(), model, effect, scale),
				L"json��ʽ����"
			);

			// �滻 output effect
			_PushAsOutputEffect(effect);
			_UpdateScale(scale);
		}
	}
	
	void _UpdateScale(std::pair<float, float> scale) {
		if (scale.first == 0 || scale.second == 0) {
			SIZE hostSize = Utils::GetSize(Env::$instance->GetHostClient());
			SIZE srcSize = Utils::GetSize(Env::$instance->GetSrcClient());

			// ���ͼ�������Ļ
			float x = float(hostSize.cx) / srcSize.cx / _scale.first;
			float y = float(hostSize.cy) / srcSize.cy / _scale.second;

			scale.first = min(x, y);
			scale.second = scale.first;
		}

		_scale.first *= scale.first;
		_scale.second *= scale.second;
	}


private:
	// ���ͼ��ߴ�
	std::pair<float, float> _scale{ 1.0f,1.0f };

	ID2D1Factory1* _d2dFactory;
	ID2D1DeviceContext* _d2dDC;
};