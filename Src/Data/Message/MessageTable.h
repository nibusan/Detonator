#pragma once
#include <unordered_map>
#include <string>
#include "../../Common/StaticSingleton.h"

class MessageTable : public StaticSingleton<MessageTable> {
public:

private:
	std::unordered_map<int, std::wstring> messageTable_;

	MessageTable(void);
public:
	~MessageTable(void);
	THIS_CLASS_IS_STATIC_SINGLETON(MessageTable);

	/// <summary>
	/// ������
	/// </summary>
	void Init(void);

	/// @brief �w�肳�ꂽ���b�Z�[�W�̕������Ԃ�
	/// @param id ���b�Z�[�WID
	/// @return ���b�Z�[�W�̕�����
	[[nodiscard]] const std::wstring& GetMessage_(int id) const;
};