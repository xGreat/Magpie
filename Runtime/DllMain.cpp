﻿// DllMain.cpp : Runtime.dll 的入口点。
//


#include "pch.h"
#include "MagWindow.h"
#include "Env.h"

HINSTANCE hInst = NULL;


// DLL 入口
BOOL APIENTRY DllMain(
    HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        hInst = hModule;
        break;
    case DLL_PROCESS_DETACH:
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}


API_DECLSPEC void WINAPI RunMagWindow(
    void reportStatus(int status, const wchar_t* errorMsg),
    const char* scaleModel,
    int captureMode,
    bool showFPS,
    bool lowLatencyMode,
    bool noVSync,
    bool noDisturb
) {
    reportStatus(1, nullptr);

    Debug::ThrowIfComFailed(
        CoInitializeEx(NULL, COINIT_MULTITHREADED),
        L"初始化 COM 出错"
    );

    try {
        HWND hwndSrc = GetForegroundWindow();
        Debug::ThrowIfWin32Failed(
            hwndSrc,
            L"GetForegroundWindow 返回 NULL"
        );

        Env::CreateInstance(hInst, hwndSrc, scaleModel, captureMode, showFPS, lowLatencyMode, noVSync, noDisturb);
        MagWindow::CreateInstance();
    } catch(const magpie_exception& e) {
        reportStatus(0, e.what().c_str());
        return;
    } catch (...) {
        Debug::WriteErrorMessage(L"创建全屏窗口发生未知错误");
        reportStatus(0, L"未知错误");
        return;
    }
    
    reportStatus(2, nullptr);

    // 主消息循环
    MagWindow::$instance->RunMsgLoop();

    reportStatus(0, nullptr);
}
