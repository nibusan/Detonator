#pragma once
#include <functional>
#include <memory>
#include <vector>
#include "UIType.h"
#include "../Common/Vector2.h"
#include "../Common/Handle/Graphic/Graphic.h"
#include "../Common/Handle/PixelShader/PixelShader.h"
#include "../Common/Collision2D/Collision2D.h"
#include "../Common/Collision2D/Collision2D_Point.h"
#include "../Common/Collision2D/Collision2D_Rect.h"
#include "../Common/Collision2D/Collision2D_Circle.h"
#include "../Common/Collision2D/Collision2D_Polygon.h"

class UIBase : public std::enable_shared_from_this<UIBase> {
public:
	// UIのフェード状態
	enum class FADE_STATE {
		FADE_IN,
		FADE_OUT,
		NONE
	};

	// 指定された座標にUIのどこを原点として配置するかの種類
	enum class DRAW_POSITION_TYPE {
		UP_LEFT,		// 上段 左
		UP_CENTER,		// 上段 中心
		UP_RIGHT,		// 上段 右
		CENTER_LEFT,	// 中段 左
		CENTER_CENTER,	// 中段 中心
		CENTER_RIGHT,	// 中段 右
		DOWN_LEFT,		// 下段 左
		DOWN_CENTER,	// 下段 中心
		DOWN_RIGHT		// 下段 右
	};

protected:
	// フレームカウンタ
	int frameCNT_;

	// UIの種類
	UI::TYPE type_;

	// 座標(親UIが設定されている場合、親からの相対座標として扱うので注意)
	Vector2<float> pos_;

	// 絶対座標(親UIが存在する場合にのみ使用)
	Vector2<float> totalPos_;

	// スケール値
	float scale_;

	// 角度
	float angle_;

	// UI描画用スクリーン
	Unique_Graphic drawScreen_;

	// UI描画時に使用するピクセルシェーダー
	Weak_PixelShader usingPixelShader_;

	// 透明度(0.0 ~ 1.0)
	float alpha_;

	// クリックされたか
	bool isClick_;

	// クリックできるか
	bool isClickable_;

	// フェードの状態
	FADE_STATE fadeState_;

	// フェードの速度
	float fadeSpeed_;

	// UIが押されたときに実行するアクション
	std::function<void(void)> onClickFunction_;

	// どこを原点として描画するか
	DRAW_POSITION_TYPE drawPositionType_;

	// UIの当たり判定
	Shared_Collision2D collision_;

	// 子UI
	std::vector<std::shared_ptr<UIBase>> childs_;

	// 親UI
	std::weak_ptr<UIBase> parent_;

	// 非表示かどうか
	bool isActive_;

	// 強調表示されているか
	bool isHighlighted_;

	virtual void Init_UI(void) = 0;
	virtual bool Update_UI(void) = 0;
	virtual void Draw_UI(void) = 0;
	virtual void Release_UI(void) = 0;

	// 描画する座標を返す
	Vector2<float> GetDrawPos(const Vector2<float>& pos, const Vector2<float>& size, DRAW_POSITION_TYPE type) const;

	/// @brief 
	/// @param  
	virtual void HighlightedUpdate(void);
public:
	// コンストラクタ
	UIBase(void);

	// デストラクタ
	virtual ~UIBase(void) = default;

	void Init(void);
	bool Update(void);
	void Draw(void);
	void Release(void);

	/// @brief UI描画用のスクリーンを生成する
	/// @param size スクリーンのサイズ
	/// @note Initを呼び出した後にこの関数を呼んでください
	void CreateDrawScreen(Vector2<int> size);

	// UIの座標を返す
	[[nodiscard]] const Vector2<float>& GetPos(void) const;
	[[nodiscard]] Vector2<float> GetTotalPos(void) const;

	// UIの座標をセット
	void SetPos(const Vector2<float>& pos);
	void SetTotalPos(const Vector2<float>& pos);

	// UIのサイズを返す
	[[nodiscard]] const Vector2<int>& GetSize(void) const;

	// UIの種類を返す
	[[nodiscard]] UI::TYPE GetType(void) const;

	// UIのスケール値を返す
	[[nodiscard]] float GetScale(void) const;
	
	// UIのスケール値をセット
	void SetScale(float scale);

	// UIの角度を返す
	[[nodiscard]] float GetAngle(void) const;
	
	// UIの角度をセット
	void SetAngle(float rad);

	// UIの透明度を返す
	[[nodiscard]] float GetAlpha(void) const;
	
	// UIの透明度をセット
	void SetAlpha(float alpha);

	// クリックされたかを返す
	[[nodiscard]] bool IsClick(void) const;

	// クリックされたかをセット
	void SetIsClick(bool isClick);

	// このUIをクリックできるかを返す
	[[nodiscard]] bool IsClickable(void) const;

	// このUIをクリックできるかをセット
	void SetIsClickable(bool isClickable);

	// 使用するピクセルシェーダーをセット(定数を更新する場合は毎フレーム呼び出す必要あり)
	void SetUsePixelShader(Weak_PixelShader ps);

	// フェード状態をセット
	void SetFade(FADE_STATE state, float speed);

	// クリックされたときに呼び出される関数をセット
	void SetOnClickFunction(std::function<void(void)> func);

	// クリックされたときに呼び出される関数を呼ぶ
	void OnClick(void);

	// 描画時にどこを原点として描画するかを返す
	[[nodiscard]] DRAW_POSITION_TYPE GetDrawPositionType(void) const;
	
	// 描画時にどこを原点として描画するかをセット
	void SetDrawPositionType(DRAW_POSITION_TYPE type);

	// 当たり判定の情報を返す
	[[nodiscard]] Shared_Collision2D GetCollisionData(void) const;

	// 当たり判定の情報をセット
	void CreateCollisionData(DRAW_POSITION_TYPE posType, Shared_Collision2D collision);

	// 子UIを返す
	const std::vector<std::shared_ptr<UIBase>> GetChilds(void) const;

	// 子UIを追加
	void AddChild(std::shared_ptr<UIBase> child);

	// 親UIを返す
	const std::weak_ptr<UIBase> GetParent(void) const;

	// 親UIをセット
	void SetParent(std::weak_ptr<UIBase> parent);

	/// @brief アクティブ(表示状態)かどうかを返す
	/// @return フラグ
	bool IsActive(void) const;

	/// @brief アクティブ(表示状態)かどうかをセット
	/// @param flag フラグ
	void SetIsActive(bool flag);

	/// @brief このUIが強調表示されているかを返す
	/// @return フラグ
	bool IsHighlighted(void) const;

	/// @brief このUIが強調表示されているかをセット
	/// @param flag フラグ
	void SetIsHighlighted(bool flag);
};