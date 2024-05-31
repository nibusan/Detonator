#pragma once
#include <chrono>
#include <memory>
#include <functional>
#include "../Common/Handle/Graphic/Graphic.h"
class SceneBase;
class Fader;
class CameraBase;
class ObjectBase;
class UIScreen;

class SceneManager
{

public:

	// �V�[���Ǘ��p�@
	enum class SCENE_ID
	{
		NONE,
		LOGO,		// ���S
		TITLE,		// �^�C�g��
		MENU,		// ���j���[
		sLOBBY,		// �V���O���v���C���r�[
		sGAME,		// �V���O���v���C�Q�[��
		sRESULT,	// �V���O���v���C���U���g
		mLOBBY,		// �}���`�v���C���r�[
		mGAME,		// �}���`�v���C�Q�[��
		mRESULT,	// �}���`�v���C���U���g
		MAX
	};

	// �C���X�^���X�̐���
	static void CreateInstance(void);

	// �C���X�^���X�̎擾
	static SceneManager& GetInstance(void);

	void Init(void);
	void Init3D(void);
	void Update(void);
	void Draw(void);

	// ���\�[�X�̔j��
	void Destroy(void);

	// ��ԑJ��
	void ChangeScene(SCENE_ID nextId);

	// �V�[��ID�̎擾
	SCENE_ID GetSceneID(void);
	SCENE_ID GetPreSceneID(void);

	// �f���^�^�C���̎擾
	double GetDeltaTime(void) const;

	// �����Ƀf���^�^�C�������������̂�Ԃ�
	float GetDeltaMult(float num);

	// �J�����̎擾
	std::shared_ptr<CameraBase> GetCamera(void) const;

	bool IsGameScene(void);

	void PlayBGM(void);
	void StopBGM(void);

	/// <summary>
	/// �f�o�b�O�p�̃e�L�X�g�t�@�C�����o�͂���
	/// </summary>
	/// <param name="name"> �o�͂���e�L�X�g�t�@�C���̖��O </param>
	/// <param name="data"> �o�͂���e�L�X�g </param>
	static void CreateDebugText(const std::string& name, const std::string& data);

	std::weak_ptr<UIScreen> GetUIScreen(void) const;
private:

	// �ÓI�C���X�^���X
	static SceneManager* instance_;

	SCENE_ID sceneId_;
	SCENE_ID waitSceneId_;
	SCENE_ID preSceneId_;

	// �t�F�[�h
	std::unique_ptr<Fader>fader_;

	// �e��V�[��
	std::unique_ptr<SceneBase> scene_;

	// �J����
	std::shared_ptr<CameraBase> camera_;

	// �V�[���J�ڒ�����
	bool isSceneChanging_;

	// �f���^�^�C��
	std::chrono::system_clock::time_point preTime_;
	double deltaTime_;

	// �}���`�v���C����
	bool isMultiPlayer_;

	// SceneManager�̕`��X�N���[��
	Unique_Graphic drawScreen_;

	// ���C�g�n���h��
	int lightHandle1_;
	int lightHandle2_;

	// UI�X�N���[��
	std::shared_ptr<UIScreen> uiScreen_;

	int sound_Music_;

	// �f�t�H���g�R���X�g���N�^��private�ɂ��āA
	// �O�����琶���ł��Ȃ��l�ɂ���
	SceneManager(void);
	// �R�s�[�R���X�g���N�^�����l
	SceneManager(const SceneManager&);
	// �f�X�g���N�^�����l
	~SceneManager(void) = default;

	// �f���^�^�C�������Z�b�g����
	void ResetDeltaTime(void);

	// �V�[���J��
	void DoChangeScene(SCENE_ID sceneId);

	// �t�F�[�h
	void Fade(void);

};
