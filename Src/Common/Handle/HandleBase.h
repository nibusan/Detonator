#pragma once

class HandleBase {
public:
	HandleBase(void);
	virtual ~HandleBase(void);

	/// <summary>
	/// ハンドルIDを返す
	/// </summary>
	/// <returns>ハンドルID</returns>
	[[nodiscard]] int GetHandle(void) const;

	/// <summary>
	/// ハンドルIDをセット
	/// </summary>
	/// <param name="">ハンドルID</param>
	void SetHandle(int handle);

	/// <summary>
	/// デストラクタが呼ばれた際に自動的にハンドルを削除するかを返す
	/// </summary>
	/// <returns> 削除するかのフラグ </returns>
	[[nodiscard]] bool IsAutoDeleteHandle(void) const;

	/// <summary>
	/// デストラクタが呼ばれた際に自動的にハンドルを削除するかをセット
	/// </summary>
	/// <param name="flag"> 削除するかのフラグ </param>
	void SetIsAutoDeleteHandle(bool flag);

protected:

	//ハンドルID
	int handle_;

	//デストラクタが呼ばれたときにハンドルを破棄するかを管理(true:自動破棄)
	bool isAutoDeleteHandle_;
};
