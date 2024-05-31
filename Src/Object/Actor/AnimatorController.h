#pragma once
#include <memory>
#include <unordered_map>

class Transform;
class ActorBase;

class AnimatorController
{
public:

	AnimatorController(std::shared_ptr<ActorBase> obj, const Transform& transform);
	AnimatorController(std::shared_ptr<ActorBase> obj, const Transform& transform, std::vector<int> frameNo);
	~AnimatorController(void) = default;

	//// アニメーションを適用させる部位
	//enum class BODY
	//{
	//	UP,
	//	DOWN,
	//	FULL
	//};

	struct ANIM_DATA
	{
		// 変わらないもの------------------------------
		// アニメーションハンドル
		int animHandle = -1;

		// アニメーションスピード
		float animSpeed_ = 60.0f;

		// アニメーション総再生時間
		float animTotalTime_ = 0.0f;

		// 座標移動無効化用
		VECTOR invalidPos_ = {};

		// 再生終了後に再生させるアニメーション
		int nextAnimNo_ = -1;

		// 再生終了後、即座にデタッチさせる
		bool endDetach_ = false;

		// 再生終了後、親オブジェクトのAnimEndを呼ぶ
		bool endCallAnimEnd_ = false;

		// ブレンド率減少速度
		float detachSpeed_ = 2.0f;

		// この再生時間になったら親に通知を送る(0秒目は送らない)
		float noticeTime_ = 0.0f;

		// アニメーション開始時間
		float startTime_ = 0.0f;

		// 適用させるフレーム
		int applyFrameIndex_ = -1;

		// 変わる可能性のあるもの----------------------
		// アニメーション現在時間
		float animTime_ = 0.0f;

		// アニメーションアタッチNo(モデルとアニメーションの管理番号)
		int animAttachNo_ = -1;

		// ブレンド用
		float animRate_ = 0.0f;

		// 再生優先させるか
		bool isPriority_ = false;

		// アニメーションをモデルフレームごとに分割する場合使う
		std::vector<float>animRateFrame_ = {};
		std::vector<bool>isPriorityFrame_ = {};

		// 通知を送ったか
		bool isNotice_ = false;

		// アニメーション再生をストップ
		bool isStop_ = false;
	};


	void Update(void);

	void SetAnimData(int animNo, int handle,  VECTOR invalid = {}, int frame = -1, int nextAnimNo = -1, bool endDetach = false);
	
	void SetAnimTime(int animNo, float time);

	void SetAnimSpeed(int animNo, float speed);

	void SetDetachSpeed(int animNo, float speed);

	void SetCallAnimEnd(int animNo, bool isCall);

	void SetNoticeTime(int animNo, float time);

	void SetStartTime(int animNo, float time);

	void SetIsStop(int animNo, bool isStop);

	void SetIsNotice(int animNo, bool isNotice);

	void ChangeAnimation(int animNo);
	void ChangeAnimationFrame(int animNo , int frameIndex);

	// 指定のアニメーションが再生中かどうか
	bool IsAnimation(int animNo);

	float GetAnimRate(int animNo);

	void Release(void);

	void Reset(void);

	void SetValidAnimMove(bool valid);

	void SetInvalidFrameNo(int no);

	void DrawDebug(void);

private:

	// 遷移時間
	static constexpr float ANIM_CHANGE_TIME = 0.5f;

	// 親のポインタ
	std::weak_ptr<ActorBase> parent_;

	const Transform& transform_;

	// フレーム分割数
	int splitFrameNum_;

	// 
	std::vector<int> splitFrameNo_;

	// アニメーションデータ
	std::unordered_map<int, ANIM_DATA> animData_;

	// 遷移中かどうか
	bool isChanging_;
	std::vector<bool>isChangingFrame_;

	// アニメーションによる座標移動を無効化する際のオフセット
	VECTOR invalidBrendPos_;
	
	int invalidFrameNo_;

	// アニメーションによる座標移動を有効化
	bool validAnimMove_;

	// アニメーションデタッチ
	void AnimationDettach(int animNo);

	// アニメーション
	void Animation(void);

	// アニメーション遷移
	void ChangingAnimation(void);
	void ChangingAnimationFrame(int index);

	void CheckDettach(void);

	// アニメーションによる座標移動無効化
	void InvalidAnimMove(void);

};

