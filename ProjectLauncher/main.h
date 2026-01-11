#pragma once

#include <Windows.h>
#include <thread>
#include <d3d9.h>

//our files
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"

#include "IconFontAwesome6.h"
#include "IconFontAwesome6_Brands.h"


extern ImFont* pFontAwesomeSolid, *pFontAwesomeReg, * pFontAwesomeBrands, * pFontArial;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter);

namespace myapp
{
	int WIDTH = 500;
	int HEIGHT = 500;

	bool active = true;

	HWND window = nullptr;
	WNDCLASSEXA windowClass = {};

	POINTS position = {};

	PDIRECT3D9 d3d = nullptr;
	LPDIRECT3DDEVICE9 dDevice = nullptr;
	D3DPRESENT_PARAMETERS pPresentParams = {};

	void CreateMyWindow(const char* winName, const char* className);
	void DestroyMyWindow(void);

	bool CreateDevice(void);
	void ResetDevice(void);
	void DestroyDevice(void);

	//IMGUI
	void Install_ImGui(void);
	void Uninstall_ImGui(void);

	void ImGuiBeginRender(void);
	void ImGuiEndRender(void);
	void ImGuiRender();
}