#include <DxLib.h>
#include "Data/Audio/AudioList.h"
#include "Manager/AudioManager.h"
#include "Manager/ResourceManager.h"
#include "Manager/InputManager.h"
#include "Manager/SceneManager.h"
#include "Manager/EffectManager.h"
#include "Manager/CollisionManager.h"
#include "Manager/GameManager.h"
#include "Net/NetManager.h"
#include "Application.h" 

Application* Application::instance_ = nullptr;

const std::string Application::PATH_IMAGE = "Data/Image/";
const std::string Application::PATH_MODEL = "Data/Model/";
const std::string Application::PATH_EFFECT = "Data/Effect/";

void Application::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new Application();
	}
	instance_->Init();
}

Application& Application::GetInstance(void)
{
	return *instance_;
}

void Application::Init(void)
{

	// �A�v���P�[�V�����̏����ݒ�
	SetWindowText("DETONATION");

	// �E�B���h�E�T�C�Y
	int screenSize_X = SCREEN_SIZE_X;
	int screenSize_Y = SCREEN_SIZE_Y;

	// �F���[�h
	int bitColor = 32;

	// ���z�t���X�N���[���ɂ���
	SetGraphMode(screenSize_X, screenSize_Y, 32);
	ChangeWindowMode(true);
	GetDefaultState(&screenSize_X, &screenSize_Y, &bitColor);
	SetWindowSize(screenSize_X, screenSize_Y);

	// �E�B���h�E����t�H�[�J�X���O�ꂽ�������������s����
	//SetAlwaysRunFlag(true);

	// XAudio2���g�p����
	SetEnableXAudioFlag(true);

	// �P���[�g���ɑ�������l��ݒ肷��(3D�̃T�E���h�Ŏg�p)
	Set3DSoundOneMetre(100.0f);

	isShutdown_ = false;

	// DxLib�̏�����
	SetUseDirect3DVersion(DX_DIRECT3D_11);
	isInitFail_ = false;
	if (DxLib_Init() == -1)
	{
		isInitFail_ = true;
		return;
	}

	Effekseer_Init(8000);
	SetChangeScreenModeGraphicsSystemResetFlag(false);
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();

	// �L�[���䏉����
	SetUseDirectInputFlag(true);
	InputManager::CreateInstance();

	// �Փ˔���Ǘ�������
	CollisionManager::CreateInstance();

	// ���\�[�X�Ǘ�������
	ResourceManager::GetInstance().Init();

	// �I�[�f�B�I���X�g������
	AudioList::GetInstance().Init();

	// �I�[�f�B�I�Ǘ�������
	AudioManager::GetInstance().Init();

	// �V�[���Ǘ�������
	SceneManager::CreateInstance();
	
	// �G�t�F�N�g�Ǘ�������
	EffectManager::CreateInstance();

	// �l�b�g���[�N�Ǘ�������
	NetManager::CreateInstance();

	// �Q�[���Ǘ�������
	GameManager::CreateInstance();
}

void Application::Run(void)
{

	auto& inputManager = InputManager::GetInstance();
	auto& sceneManager = SceneManager::GetInstance();
	auto& netManager = NetManager::GetInstance();

	// �Q�[�����[�v
	while (ProcessMessage() == 0 && !isShutdown_)
	{
		inputManager.Update();
		// �l�b�g���[�N�Ǘ��X�V(�t���[���̍ŏ�)
		netManager.Update();

		inputManager.NetUpdate();

		
		sceneManager.Update();
		
		sceneManager.Draw();

		// �l�b�g���[�N�Ǘ��X�V(�t���[���̍Ō�)
		netManager.UpdateEndOfFrame();

		ScreenFlip();

	}

}

void Application::Destroy(void)
{
	// �ȑO�̐݌v���ƃ��������[�N���N�����Ă������߁A
	// (�ÓI�C���X�^���X��������Ă����f�X�g���N�^���Ăяo����Ȃ�)
	// �����I�ɃC���X�^���X��j������
	InputManager::GetInstance().Destroy();
	SceneManager::GetInstance().Destroy();
	EffectManager::GetInstance().Destroy();
	CollisionManager::GetInstance().Destroy();
	GameManager::GetInstance().Destroy();
	NetManager::GetInstance().Destroy();
	
	Effkseer_End();

	// DxLib�I��
	if (DxLib_End() == -1)
	{
		isReleaseFail_ = true;
	}

	delete instance_;

}

bool Application::IsInitFail(void) const
{
	return isInitFail_;
}

bool Application::IsReleaseFail(void) const
{
	return isReleaseFail_;
}

void Application::Shutdown(void) {
	isShutdown_ = true;
}

Application::Application(void)
{
	isInitFail_ = false;
	isReleaseFail_ = false;
}
