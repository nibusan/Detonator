#pragma once
#include <unordered_map>
#include <functional>
#include "../Common/StaticSingleton.h"
#include "../UI/Parts/Game/LogMessageArea/UI_LogMessageArea.h"

class UIBase;
class UIFunctionList : public StaticSingleton<UIFunctionList> {
public:
	~UIFunctionList(void) = default;
	THIS_CLASS_IS_STATIC_SINGLETON(UIFunctionList);

	void Init(void);

	/// @brief �N���b�N���ɌĂяo�����R�[���o�b�N�֐���Ԃ�
	/// @param id ID
	/// @return �R�[���o�b�N�֐�
	const std::function<void(void)>& GetOnClickFunction(int id) const;

	/// @brief ��������͂ł���e�L�X�g�{�b�N�X�œ��͂��I���u�ԂɌĂяo�����R�[���o�b�N�֐���Ԃ�
	/// @param id ID
	/// @return �R�[���o�b�N�֐�
	const std::function<void(std::weak_ptr<UIBase>)>& GetEndInputFunction(int id) const;

	/// @brief ������ǉ����邱�Ƃ��ł���e�L�X�g�{�b�N�X�ɕ�����ǉ����邽�߂̃R�[���o�b�N�֐���Ԃ�
	/// @param id ID
	/// @return �R�[���o�b�N�֐�
	const std::function<std::string(void)>& GetAddTextFunction(int id) const;

	/// @brief ���O�f�[�^��Ԃ��R�[���o�b�N�֐���Ԃ�
	/// @param id ID
	/// @return �R�[���o�b�N�֐�
	const std::function<std::shared_ptr<UI_LogMessageArea::MESSAGE_DATA_BASE>(void)>& GetLogDataFunction(int id) const;
private:
	// �N���b�N���ɌĂяo�����R�[���o�b�N�֐����Ǘ�
	std::unordered_map<int, std::function<void(void)>> onClickFunctionList_;

	// ��������͂ł���e�L�X�g�{�b�N�X�œ��͂��I���u�ԂɌĂяo�����R�[���o�b�N�֐����Ǘ�
	std::unordered_map<int, std::function<void(std::weak_ptr<UIBase>)>> endInputFunctionList_;

	// ������ǉ����邱�Ƃ��ł���e�L�X�g�{�b�N�X�ɕ�����ǉ����邽�߂̃R�[���o�b�N�֐����Ǘ�
	std::unordered_map<int, std::function<std::string(void)>> getAddTextFunctionList_;

	// ���O�f�[�^��Ԃ��R�[���o�b�N�֐����Ǘ�
	std::unordered_map<int, std::function<std::shared_ptr<UI_LogMessageArea::MESSAGE_DATA_BASE>(void)>> getLogDataFunctionList_;

	UIFunctionList(void) = default;

	// �e�R�[���o�b�N�֐���o�^����֐�
	void RegisterOnClickFunction(void);
	void RegisterEndInputFunction(void);
	void RegisterGetAddTextFunction(void);
	void RegisterGetLogDataFunction(void);
};

