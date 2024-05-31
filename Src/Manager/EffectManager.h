#pragma once
#include <filesystem>
#include <array>
#include <unordered_map>
#include <utility>
#include <DxLib.h>
#include "../Common/Handle/Effect/EffectBase.h"
#include "../Common/Handle/Effect/2D/Effect2D.h"
#include "../Common/Handle/Effect/3D/Effect3D.h"
#include "../Manager/GameManager.h"

class EffectManager
{

public:
	
	// 種類
	enum class EFFECT_TYPE
	{
		BUBBLE,
		MAGMA,
		UTSUHO,
		SMOKE,
		DESTROY,
		BULLETSMOKE,
		BULLET,
		DAMAGE,
		BARRIER,
		BARRIERLIGHT,
		BULLETBOOST,
		EXPLOSION,
		MAX
	};

	// エフェクトのデータ
	struct EFFECT
	{
		EFFECT_TYPE type;	// 種類
		VECTOR pos;			// 座標
		VECTOR scl;			// スケール
		VECTOR rot;			// 角度
		float speed;		// アニメーションの速度
	};


	// インスタンスの生成
	static void CreateInstance(void);

	// インスタンスの取得
	static EffectManager& GetInstance(void);

	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	// リソースの破棄
	void Destroy(void);
	
	/// @brief エフェクトを追加する
	/// @param effect エフェクトのデータ
	/// @return	エフェクトのハンドルID
	int AddEffect(EFFECT& effect);

	/// @brief 指定されたエフェクトを削除
	/// @param id AddEffectで返ってきたハンドルID
	void DeleteEffect(int id);

	/// @brief 指定されたエフェクトのデータを返す
	/// @param id AddEffectで返ってきたハンドルID
	/// @return エフェクトのweak_ptr
	const std::weak_ptr<EffectBase> GetData(int id) const;

private:
	// 2D or 3D
	enum class DIMENSION_TYPE
	{
		_2D,
		_3D
	};

	// エフェクトのデータ
	struct EFFECT_DATA
	{
		std::filesystem::path path;
		DIMENSION_TYPE dimensionType;
		std::shared_ptr<EffectBase> resource;
	};

	// 静的インスタンス
	static EffectManager* instance_;

	// エフェクトリスト
	std::unordered_map<int, EFFECT_DATA> effectList_;

	// 再生されてるエフェクト
	std::unordered_map<int, std::shared_ptr<EffectBase>> effects_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	EffectManager(void);

	// コピーコンストラクタも同様
	EffectManager(const EffectManager&);

	// デストラクタも同様
	~EffectManager(void) = default;

	// エフェクトリストの読み込み
	void LoadEffectList(void);

	// エフェクトリストから指定されたエフェクトのリソースを返す
	const EFFECT_DATA& GetEffectResource(EFFECT_TYPE type);

	int GetPlay2DEffectHandle(const EFFECT_DATA& resourceEffect) const;
	int GetPlay3DEffectHandle(const EFFECT_DATA& resourceEffect) const;
};