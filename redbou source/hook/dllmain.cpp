#define _CRT_SECURE_NO_WARNINGS

#pragma region headers
#include <Windows.h>
#include <iostream>
#include <D3DX11.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_internal.h>

#include "scanner.h"
#pragma endregion headers

#pragma region defs
ID3D11RenderTargetView* rendertarget;
ID3D11DeviceContext* context;
ID3D11Device* device;
HRESULT(*present_original)(IDXGISwapChain* swapchain, UINT sync, UINT flags) = nullptr;
int X, Y;
HWND hwnd;
#pragma endregion defs

#pragma region hooked
HRESULT present_hooked(IDXGISwapChain* swapchain, UINT sync, UINT flags)
{
    if (!device)
    {
        ID3D11Texture2D* renderTarget = 0; 
        ID3D11Texture2D* backBuffer = 0;
        D3D11_TEXTURE2D_DESC backBufferDesc = { 0 };

        swapchain->GetDevice(__uuidof(device), (PVOID*)&device);
        device->GetImmediateContext(&context);

        swapchain->GetBuffer(0, __uuidof(renderTarget), (PVOID*)&renderTarget);
        device->CreateRenderTargetView(renderTarget, nullptr, &rendertarget);
        renderTarget->Release();

        swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (PVOID*)&backBuffer);
        backBuffer->GetDesc(&backBufferDesc);

        X = backBufferDesc.Width;
        Y = backBufferDesc.Height;

        backBuffer->Release();

        if (!hwnd)
        {
            //Change window name and class for the specific game, to check use spy++ its in the tool tab of visual studio :D
            //For loading a non steam game for example rogue company press add a game -> add a non-steam game
            hwnd = FindWindowW(L"UnrealWindow", L"Rogue Company  ");
            //if not found try to use foreground window?
            if (!hwnd)
                hwnd = GetForegroundWindow();
        }

        ImGui_ImplDX11_Init(hwnd, device, context);
        ImGui_ImplDX11_CreateDeviceObjects();
    }
    context->OMSetRenderTargets(1, &rendertarget, nullptr);

    ImGui_ImplDX11_NewFrame();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
    ImGui::Begin("##scene", nullptr, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar);
    ImGuiIO& IO = ImGui::GetIO();
    ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(IO.DisplaySize.x, IO.DisplaySize.y), ImGuiCond_Always);
    ImGuiWindow& window = *ImGui::GetCurrentWindow();



    window.DrawList->AddText(ImVec2(50, 100), ImGui::GetColorU32(ImVec4(1.f, 0.f, 0.f, 1.f)), "Steam Hook + Render POC");



    window.DrawList->PushClipRectFullScreen();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
    ImGui::Render();

    return present_original(swapchain, sync, flags);
}
#pragma endregion hooked

void hook(__int64 addr, __int64 func, __int64* orig)
{
    static uintptr_t hook_addr;
    if (!hook_addr)
        hook_addr = sigscan("GameOverlayRenderer64.dll", "48 ? ? ? ? 57 48 83 EC 30 33 C0");

    std::cout << "GameOverlayRenderer64.dll!Hook: 0x" << std::hex << hook_addr << std::endl;

    auto hook = ((__int64(__fastcall*)(__int64 addr, __int64 func, __int64* orig, __int64 smthng))(hook_addr));
    hook((__int64)addr, (__int64)func, orig, (__int64)1);
}

short GetAsyncKeyState(const int vKey)
    {
        static uintptr_t addrGetAsyncKeyState = NULL;

        if (!addrGetAsyncKeyState)
        {
        addrGetAsyncKeyState = sigscan("GameOverlayRenderer64.dll", "40 53 48 83 EC 20 80 3D ? ? ? ? ? 8B D9");
        }

        if (!addrGetAsyncKeyState)
            return false;

        using GetAsyncKeyState_t = short(__fastcall*)(int);
        auto fnGetAyncKeyState = (GetAsyncKeyState_t)addrGetAsyncKeyState;

        return fnGetAyncKeyState(vKey);
    }

void init()
{
    if (!GetModuleHandleA("GameOverlayRenderer64.dll"))
    {
        MessageBoxA(0, "SteamOverlayRenderer64.dll Not Loaded!", "Fatal Error", MB_ICONERROR);
        exit(0);
    }

    //please dont kill me for using this 🙏
    AllocConsole();
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);

    uintptr_t Steam_DXGI_PresentScene = sigscan("GameOverlayRenderer64.dll", "48 89 6C 24 18 48 89 74 24 20 41 56 48 83 EC 20 41 8B E8");

    std::cout << "GameOverlayRenderer64.dll!DXGIPresentSceneHooked: 0x" << std::hex << Steam_DXGI_PresentScene << std::endl;

    if (Steam_DXGI_PresentScene)
        hook(Steam_DXGI_PresentScene, (__int64)present_hooked, (__int64*)&present_original);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        init();

    return TRUE;
}

