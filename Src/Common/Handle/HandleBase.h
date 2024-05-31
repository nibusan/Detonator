#pragma once

class HandleBase {
public:
	HandleBase(void);
	virtual ~HandleBase(void);

	/// <summary>
	/// �n���h��ID��Ԃ�
	/// </summary>
	/// <returns>�n���h��ID</returns>
	[[nodiscard]] int GetHandle(void) const;

	/// <summary>
	/// �n���h��ID���Z�b�g
	/// </summary>
	/// <param name="">�n���h��ID</param>
	void SetHandle(int handle);

	/// <summary>
	/// �f�X�g���N�^���Ă΂ꂽ�ۂɎ����I�Ƀn���h�����폜���邩��Ԃ�
	/// </summary>
	/// <returns> �폜���邩�̃t���O </returns>
	[[nodiscard]] bool IsAutoDeleteHandle(void) const;

	/// <summary>
	/// �f�X�g���N�^���Ă΂ꂽ�ۂɎ����I�Ƀn���h�����폜���邩���Z�b�g
	/// </summary>
	/// <param name="flag"> �폜���邩�̃t���O </param>
	void SetIsAutoDeleteHandle(bool flag);

protected:

	//�n���h��ID
	int handle_;

	//�f�X�g���N�^���Ă΂ꂽ�Ƃ��Ƀn���h����j�����邩���Ǘ�(true:�����j��)
	bool isAutoDeleteHandle_;
};
