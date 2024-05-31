#pragma once

namespace UI {
	enum class TYPE {
		IMAGE = 0,	// �摜
		TEXT,		// �e�L�X�g
		BUTTON,		// �{�^��
		SLIDER,		// �X���C�_�[
		WINDOW,		// �E�B���h�E
		INPUT_TEXT_AREA,	// �e�L�X�g���͗p�{�b�N�X
		COLOR_BOX,	// �P�F�œh��Ԃ��ꂽ�{�b�N�X
		MODEL_PREVIEW,	// 3D���f����`�悷��g
		CURSOR,		// �J�[�\��
		MESSAGE_BOX,	// ���b�Z�[�W�{�b�N�X
		WEAPON_INFO,	// ����̏��
		LOG_MESSAGE_AREA,	// ���O���b�Z�[�W��\������G���A
		RESULT_TEXT,	// ���U���g��ʂŃX�R�A�Ȃǂ�\������p�̃e�L�X�g
		GAME_TIME,	// �Q�[���̎c�莞��
		CHANGEABLE_TEXT,	// �e�L�X�g�i�ύX�\�j
		MAX
	};
}