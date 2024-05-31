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
	const std::function<void(void)>& GetOnClickFunction(int id) const;
	const std::function<void(std::weak_ptr<UIBase>)>& GetEndInputFunction(int id) const;
	const std::function<std::string(void)>& GetAddTextFunction(int id) const;
	const std::function<std::shared_ptr<UI_LogMessageArea::MESSAGE_DATA_BASE>(void)>& GetLogDataFunction(int id) const;
private:
	std::unordered_map<int, std::function<void(void)>> onClickFunctionList_;
	std::unordered_map<int, std::function<void(std::weak_ptr<UIBase>)>> endInputFunctionList_;
	std::unordered_map<int, std::function<std::string(void)>> getAddTextFunctionList_;
	std::unordered_map<int, std::function<std::shared_ptr<UI_LogMessageArea::MESSAGE_DATA_BASE>(void)>> getLogDataFunctionList_;

	UIFunctionList(void) = default;
	void RegisterOnClickFunction(void);
	void RegisterEndInputFunction(void);
	void RegisterGetAddTextFunction(void);
	void RegisterGetLogDataFunction(void);
};

