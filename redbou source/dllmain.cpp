#include "render.h"

VOID CreateConsole()
{
	AllocConsole();
	printf("test");
}

void HOOKInitalize()
{
	CreateConsole();
	HWND window = FindWindow(0, (L"Fortnite  "));

	IDXGISwapChain* swapChain = nullptr;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	auto                 featureLevel = D3D_FEATURE_LEVEL_11_0;

	DXGI_SWAP_CHAIN_DESC sd = { 0 };
	sd.BufferCount = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.OutputWindow = window;
	sd.SampleDesc.Count = 1;
	sd.Windowed = TRUE;

	if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, 0, &featureLevel, 1, D3D11_SDK_VERSION, &sd, &swapChain, &device, nullptr, &context))) {
		MessageBox(0, (L"dx11 failed hook"), (L"fatal error"), MB_ICONERROR);
		return;
	}

	auto table = *reinterpret_cast<PVOID**>(swapChain);
	auto present = table[8];
	auto resize = table[13];

	context->Release();
	device->Release();
	swapChain->Release();

	MH_Initialize();

	MH_CreateHook(present, present_hk, reinterpret_cast<PVOID*>(&presenth));
	MH_EnableHook(present);

	MH_CreateHook(resize, resize_hk, reinterpret_cast<PVOID*>(&resizeh));
	MH_EnableHook(resize);

	oriWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
}

void SigsInitalize()
{
	HOOKInitalize();
	Details.UWORLD = SDK::Utilities::Scanners::PatternScan(xorthis("48 8B 05 ? ? ? ? 4D 8B C2"));
	Details.UWORLD = RELATIVE_ADDR(Details.UWORLD, 7);

	if (!SDK::Utilities::valid_pointer(Details.UWORLD)) {
		SDK::Utilities::SpoofCall(MessageBoxA, (HWND)0, (LPCSTR)xorthis("UWORLD OUTDATED"), (LPCSTR)0, (UINT)0);
	}

	LineOfS = SDK::Utilities::Scanners::PatternScan(xorthis("E8 ? ? ? ? 48 8B 0D ? ? ? ? 33 D2 40 8A F8"));
	LineOfS = RELATIVE_ADDR(LineOfS, 5);
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		//HOOKInitalize();
		SigsInitalize();
	}

	return TRUE;
}