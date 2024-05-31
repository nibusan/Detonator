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

// WinMain�֐�
//---------------------------------
int WINAPI WinMain(
	_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, 
	_In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{

	// Log.txt���o�͂��Ȃ�(main�ŏ���)���m�F����������true��
	SetOutApplicationLogValidFlag(false);

	// ���������[�N���o(�N�����ƕ�����Ƀ��������m�F���A�������������Ă��Ȃ����\�[�X������Βʒm)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//�Ȃ񂩂��Ă܂�
	SetHookWinProc(WndProc);

	// �C���X�^���X�̐���
	Application::CreateInstance();

	// �C���X�^���X�̎擾
	Application& instance = Application::GetInstance();

	if (instance.IsInitFail())
	{
		// ���������s
		return -1;
	}

	// ���s
	instance.Run();

	// ���
	instance.Destroy();

	return 0;

}
