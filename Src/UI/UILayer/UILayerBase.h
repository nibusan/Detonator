#pragma once
#include <memory>
#include <vector>
#include "../../Common/Handle/Graphic/Graphic.h"
#include "../../Data/UI/UILayerList.h"

class UIBase;
class UILayerBase {
public:
	UILayerBase(void);
	virtual ~UILayerBase(void) = default;

	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	/// @brief UIを追加
	/// @param ui UI
	void AddUI(std::shared_ptr<UIBase> ui);

	bool IsActive(void) const;
	void SetIsActive(bool flag);
protected:
	// UIを格納
	std::vector<std::shared_ptr<UIBase>> uis_;

	// アクティブ状態かどうか
	bool isActive_;

	// 描画用スクリーン
	Unique_Graphic drawScreen_;

	void UpdateUI(std::shared_ptr<UIBase> ui);
	void DrawUI(std::shared_ptr<UIBase> ui);
	void ReleaseUI(std::shared_ptr<UIBase> ui);
	virtual void SetDefaultUI(void) = 0;

	// UIの生成処理
	std::shared_ptr<UIBase> CreateUI(std::shared_ptr<UILayerList::DEFAULT_UI_DATA> uiData);
};

