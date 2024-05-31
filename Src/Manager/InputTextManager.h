#pragma once
// �Ȃ��Ă��ǂ����ǁA
// �L���ȃL�[���̓n���h�����擾�ł��Ȃ���������
class InputTextArea;

class InputTextManager
{
public:

	// �����I�ɃC���X�e���X�𐶐�����
	static void CreateInstance(void);

	// �ÓI�C���X�^���X�̎擾
	static InputTextManager& GetInstance(void);

	// �X�V
	static void Update(void);

	// �w�肳�ꂽ�L�[���̓n���h����L���ɂ���
	static void SetActive(InputTextArea* inputTextArea);

	// �L���ȃL�[���̓n���h�������f����
	static bool IsActiveHandle(InputTextArea* inputTextArea);

private:

	// �ÓI�C���X�^���X
	static InputTextManager* instance_;

	// �R���X�g���N�^
	InputTextManager(void);

	// �R�s�[�R���X�g���N�^
	InputTextManager(const InputTextManager& ins);

	// �f�X�g���N�^
	~InputTextManager(void);

	// �L���ȃL�[���̓n���h��������InputTextArea
	InputTextArea* inputTextArea_;

};

