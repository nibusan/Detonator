#pragma once
#include <filesystem>
#include <string>
#include <DxLib.h>
#include "../HandleBase.h"

class Sound : public HandleBase {
public:
	/// @brief 2D�̉���3D�̉���
	enum class DIMENSION_TYPE {
		_2D = 0,
		_3D
	};

	Sound(void);

	/// @brief �R���X�g���N�^
	/// @param path �t�@�C���ւ̃p�X
	/// @param dimensionType  2D or 3D�̂ǂ��炩
	Sound(const std::filesystem::path& path, DIMENSION_TYPE dimensionType);

	/// @brief �R���X�g���N�^
	/// @param handle �n���h��
	/// @param dimensionType 2D or 3D�̂ǂ��炩
	Sound(int handle, DIMENSION_TYPE dimensionType);

	~Sound(void) override;

	/// @brief �T�E���h�̒�����Ԃ�
	/// @return �T�E���h�̒���
	[[nodiscard]] LONGLONG GetTotalTime(void) const;

	/// @brief �����Đ�(2D)
	/// @param isLoop ���[�v���邩
	void Play(bool isLoop);

	/// @brief �����Đ�����Ԃ�(2D)
	/// @return �Đ������ǂ���
	bool IsPlaying(void);

	/// @brief �����~�߂�(2D)
	void Stop(void);

	/// @brief �����Đ�(3D)
	/// @param listnerPos �����l�̍��W
	/// @param listnerDir �����l�������Ă����
	/// @param emitterPos ����炷���W
	void Play3D(VECTOR listnerPos, VECTOR listnerDir, VECTOR emitterPos);
private:
	// �g�[�^���̍Đ�����
	LONGLONG totalTime_;

	// 2D��3D��
	DIMENSION_TYPE dimensionType_;
};

using Unique_Sound = std::unique_ptr<Sound>;
using Shared_Sound = std::shared_ptr<Sound>;
using Weak_Sound = std::weak_ptr<Sound>;