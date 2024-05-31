#pragma once
#include <array>
#include <map>
#include <string>

class ItemList
{

public:

	//アイテムのカテゴリ
	enum ITEM_CATEGORY 
	{
		GUN = 0,	//銃
		BULLET,		//弾丸
		BOMB,		//ボム
		MAX
	};

	//アイテムのデータ
	struct ITEM_DATA 
	{
		ITEM_CATEGORY category;	//カテゴリID
		int id;	//アイテムID
		std::wstring name;	//アイテムの名前
		int maxStackNum;	//スタックできる最大数
	};

	// 明示的にインスタンスを生成する
	static void CreateInstance(void);

	// 静的インスタンスの取得
	static ItemList& GetInstance(void);

	// 初期化処理
	void Init(void);

	/// <summary>
	/// 指定されたアイテムのデータを返す
	/// </summary>
	/// <param name="category"> カテゴリID </param>
	/// <param name="id"> アイテムID </param>
	/// <returns> アイテムのデータ </returns>
	[[nodiscard]]
	const ITEM_DATA& GetItemData(ITEM_CATEGORY category, int id) const;

private:
	// 静的インスタンス
	static ItemList* instance_;

	// アイテムリスト
	std::array<std::map<int, ITEM_DATA>, static_cast<int>(ITEM_CATEGORY::MAX)> itemList_;

	// コンストラクタ
	ItemList(void);

	// コピーコンストラクタ
	ItemList(const ItemList& ins);

	// デストラクタ
	~ItemList(void);
};

