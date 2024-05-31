#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <DxLib.h>
#include "Common/imgui/imgui.h"
#include "Common/imgui/backends/imgui_impl_dx11.h"
#include "Common/imgui/backends/imgui_impl_win32.h"
#include "Application.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

	return 0;
}

// WinMain関数
//---------------------------------
int WINAPI WinMain(
	_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, 
	_In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{

	// Log.txtを出力しない(mainで書く)→確認したい時はtrueに
	SetOutApplicationLogValidFlag(false);

	// メモリリーク検出(起動時と閉じた後にメモリを確認し、メモリ解放されていないリソースがあれば通知)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//なんかしてます
	SetHookWinProc(WndProc);

	// インスタンスの生成
	Application::CreateInstance();

	// インスタンスの取得
	Application& instance = Application::GetInstance();

	if (instance.IsInitFail())
	{
		// 初期化失敗
		return -1;
	}

	// 実行
	instance.Run();

	// 解放
	instance.Destroy();

	return 0;

}
