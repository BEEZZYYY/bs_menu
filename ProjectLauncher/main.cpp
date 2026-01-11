#include "main.h"

ImFont* pFontAwesomeSolid = nullptr, *pFontAwesomeReg = nullptr, * pFontAwesomeBrands = nullptr, * pFontArial = nullptr;

LRESULT CALLBACK WindowProcess(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
		return true;

	switch (message)
	{
		case WM_SIZE: 
		{
			if (myapp::dDevice && wideParameter != SIZE_MINIMIZED)
			{
				myapp::pPresentParams.BackBufferWidth = LOWORD(longParameter);
				myapp::pPresentParams.BackBufferHeight = HIWORD(wideParameter);
				myapp::ResetDevice();
			}
		}return 0;

		case WM_SYSCOMMAND: {
			if ((wideParameter & 0xfff0) == SC_KEYMENU)
				return 0;
		}break;

		case WM_DESTROY: {
			PostQuitMessage(0);
		}return 0;

		case WM_LBUTTONDOWN: {
			myapp::position = MAKEPOINTS(longParameter);
		}return 0;


		case WM_MOUSEMOVE:
		{
			if (wideParameter == MK_LBUTTON)
			{
				const auto points = MAKEPOINTS(longParameter);
				auto rect = RECT{};
				GetWindowRect(myapp::window, &rect);
				rect.left += points.x - myapp::position.x;
				rect.top += points.y - myapp::position.y;

				if (myapp::position.x >= 0 &&
					myapp::position.x <= myapp::pPresentParams.BackBufferWidth &&
					myapp::position.y >= 0 &&
					myapp::position.y <= myapp::pPresentParams.BackBufferHeight) //if you want to move only by the top put 30 or less
				{
					SetWindowPos(myapp::window, HWND_TOPMOST, rect.left, rect.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER);
				}
			}
		}return 0;

		default:
			return DefWindowProcA(window, message, wideParameter, longParameter);

	}
	return 0;
}

void myapp::CreateMyWindow(const char* winName, const char* className)
{
	windowClass.cbSize = sizeof(WNDCLASSEXA);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = WindowProcess;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hIcon = 0;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = 0;
	windowClass.lpszMenuName = 0;
	windowClass.lpszClassName = className;
	windowClass.hIconSm = 0;

	RegisterClassExA(&windowClass);

	window = CreateWindowA(className, winName, WS_POPUP, 100, 100, WIDTH, HEIGHT, 0, 0, windowClass.hInstance, 0);

	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);
}

void myapp::DestroyMyWindow(void)
{
	DestroyWindow(window);
	::UnregisterClassA(windowClass.lpszClassName, windowClass.hInstance);
}

bool myapp::CreateDevice(void)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d)
		return false;

	ZeroMemory(&pPresentParams, sizeof(pPresentParams));

	pPresentParams.Windowed = TRUE;
	pPresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pPresentParams.BackBufferFormat = D3DFMT_UNKNOWN;
	pPresentParams.EnableAutoDepthStencil = TRUE;
	pPresentParams.AutoDepthStencilFormat = D3DFMT_D16;
	pPresentParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pPresentParams, &dDevice) < 0)
		return false;

	return true;
}

void myapp::ResetDevice(void)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = dDevice->Reset(&pPresentParams);

	if (result == D3DERR_INVALIDCALL)
		IM_ASSERT(0);

	ImGui_ImplDX9_CreateDeviceObjects();
}

void myapp::DestroyDevice(void)
{
	if (dDevice)
	{
		dDevice->Release();
		dDevice = nullptr;
	}

	if (d3d)
	{
		d3d->Release();
		d3d = nullptr;
	}
}

//IMGUI
void myapp::Install_ImGui(void)
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui::StyleColorsDark();

	//main font
	pFontArial = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\arial.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
	IM_ASSERT(pFontArial != NULL);

	static const ImWchar icons_rangesA[] = { ICON_MIN_FA, ICON_MAX_FA, 0 }; //regular
	static const ImWchar icons_rangesB[] = { ICON_MIN_FAB, ICON_MAX_FAB, 0 }; //brands

	ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;

	pFontAwesomeReg = io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAR, 16.0f, &icons_config, icons_rangesA);
	IM_ASSERT(pFontAwesomeReg != NULL);

	pFontAwesomeSolid = io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, 16.0f, &icons_config, icons_rangesA);
	IM_ASSERT(pFontAwesomeSolid != NULL);

	pFontAwesomeBrands = io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAB, 16.0f, &icons_config, icons_rangesB);
	IM_ASSERT(pFontAwesomeBrands != NULL);

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(dDevice);

}
void myapp::Uninstall_ImGui(void)
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void myapp::ImGuiBeginRender(void)
{
	MSG msg;

	while (PeekMessage(&msg, 0,0,0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void myapp::ImGuiEndRender(void)
{
	ImGui::EndFrame();

	dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	if (dDevice->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		dDevice->EndScene();
	}

	//Handle loss of the D3D9 device
	const auto resl = dDevice->Present(0, 0, 0, 0);
	if (resl == D3DERR_DEVICELOST && dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		ResetDevice();

}

void myapp::ImGuiRender()
{
	static ImGuiWindowFlags winFlags = {
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse };

	ImGui::SetNextWindowPos({ (float)0, (float)0 });
	ImGui::SetNextWindowSize({ (float)WIDTH, (float)HEIGHT });
	if (ImGui::Begin("Test Window", &active, winFlags))
	{
		ImGui::PushFont(pFontAwesomeReg);

		ImGui::Button(ICON_FA_ANCHOR_LOCK " Test Button");

		ImGui::PopFont();

		ImGui::End();
	}
}


//create thread for render
int __stdcall wWinMain(HINSTANCE instance, HINSTANCE lastInstance, PWSTR args, int cmdShow)
{
	myapp::CreateMyWindow("TestWindow", "TestWindowClass");
	myapp::CreateDevice();
	myapp::Install_ImGui();

	while (myapp::active)
	{
		myapp::ImGuiBeginRender();
		myapp::ImGuiRender();
		myapp::ImGuiEndRender();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	myapp::Uninstall_ImGui();
	myapp::DestroyDevice();
	myapp::DestroyMyWindow();

	return EXIT_SUCCESS;
}