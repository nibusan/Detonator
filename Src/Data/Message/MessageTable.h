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
	/// 初期化
	/// </summary>
	void Init(void);

	/// @brief 指定されたメッセージの文字列を返す
	/// @param id メッセージID
	/// @return メッセージの文字列
	[[nodiscard]] const std::wstring& GetMessage_(int id) const;
};