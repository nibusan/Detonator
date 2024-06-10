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

	/// @brief クリック時に呼び出されるコールバック関数を返す
	/// @param id ID
	/// @return コールバック関数
	const std::function<void(void)>& GetOnClickFunction(int id) const;

	/// @brief 文字を入力できるテキストボックスで入力が終わる瞬間に呼び出されるコールバック関数を返す
	/// @param id ID
	/// @return コールバック関数
	const std::function<void(std::weak_ptr<UIBase>)>& GetEndInputFunction(int id) const;

	/// @brief 文字を追加することができるテキストボックスに文字を追加するためのコールバック関数を返す
	/// @param id ID
	/// @return コールバック関数
	const std::function<std::string(void)>& GetAddTextFunction(int id) const;

	/// @brief ログデータを返すコールバック関数を返す
	/// @param id ID
	/// @return コールバック関数
	const std::function<std::shared_ptr<UI_LogMessageArea::MESSAGE_DATA_BASE>(void)>& GetLogDataFunction(int id) const;
private:
	// クリック時に呼び出されるコールバック関数を管理
	std::unordered_map<int, std::function<void(void)>> onClickFunctionList_;

	// 文字を入力できるテキストボックスで入力が終わる瞬間に呼び出されるコールバック関数を管理
	std::unordered_map<int, std::function<void(std::weak_ptr<UIBase>)>> endInputFunctionList_;

	// 文字を追加することができるテキストボックスに文字を追加するためのコールバック関数を管理
	std::unordered_map<int, std::function<std::string(void)>> getAddTextFunctionList_;

	// ログデータを返すコールバック関数を管理
	std::unordered_map<int, std::function<std::shared_ptr<UI_LogMessageArea::MESSAGE_DATA_BASE>(void)>> getLogDataFunctionList_;

	UIFunctionList(void) = default;

	// 各コールバック関数を登録する関数
	void RegisterOnClickFunction(void);
	void RegisterEndInputFunction(void);
	void RegisterGetAddTextFunction(void);
	void RegisterGetLogDataFunction(void);
};

