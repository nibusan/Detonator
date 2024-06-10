#pragma once
#include <memory>
#include <vector>
#include "../UILayer/UILayerBase.h"

class UIScreen {
public:
	UIScreen(void) = default;
	~UIScreen(void) = default;

	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	/// @brief UILayerをプッシュする
	/// @param uiLayer 対象のUILayer
	void PushUILayer(std::unique_ptr<UILayerBase> uiLayer);

	/// @brief UILayerをポップする
	void PopUILayer(void);

	/// @brief UILayerの数を返す 
	/// @return UILayerの数
	int GetUILayerCount(void) const;

	/// @brief 先頭のUILayerをアクティブにするかをセットする
	/// @param flag アクティブにするか
	void SetIsActive_FrontUILayer(bool flag);

	/// @brief UILayerを削除する 
	void DeleteUILayer(void);
private:
	// 管理するUILayer(キューのように扱ってます)
	std::vector<std::unique_ptr<UILayerBase>> uiLayers_;

	// 削除するUILayerのID
	std::vector<int> deleteUILayerID_;

	// 削除する回数
	int deleteCount_;
};