#include "../../Manager/SceneManager.h"
#include "ActorBase.h"
#include "../Common/Transform.h"
#include "AnimatorController.h"

AnimatorController::AnimatorController(std::shared_ptr<ActorBase> obj, const Transform& transform) : transform_(transform)
{
	// 親のポインタ
	parent_ = obj;

	// 遷移中かどうか
	isChanging_ = false;

	// アニメーションによる座標移動を無効化する際のオフセット
	invalidBrendPos_ = {};

	// アニメーションによる座標移動を有効化
	validAnimMove_ = false;

	// フレーム分割数
	splitFrameNum_ = 0;

	invalidFrameNo_ = 0;
}

AnimatorController::AnimatorController(std::shared_ptr<ActorBase> obj, const Transform& transform, std::vector<int> frameNo) : transform_(transform)
{
	// 親のポインタ
	parent_ = obj;

	// 遷移中かどうか
	isChanging_ = false;

	// アニメーションによる座標移動を無効化する際のオフセット
	invalidBrendPos_ = {};

	// アニメーションによる座標移動を有効化
	validAnimMove_ = false;

	// フレーム分割数
	splitFrameNum_ = frameNo.size();

	splitFrameNo_ = frameNo;

	isChangingFrame_ = std::vector(splitFrameNum_, false);

	invalidFrameNo_ = 0;
}

void AnimatorController::Update(void)
{
	// アニメーション遷移
	ChangingAnimation();

	for(int i = 0; i < splitFrameNum_;i++){
		ChangingAnimationFrame(i);
	}
	

	// アニメーション処理
	Animation();

	// アニメーションによる座標移動無効化
	InvalidAnimMove();

	CheckDettach();
}

void AnimatorController::SetAnimData(int animNo, int handle, VECTOR invalid, int frame , int nextAnimNo, bool endDetach)
{
	// アニメーションハンドルを格納
	ANIM_DATA data;
	data.animHandle = handle;
	data.animAttachNo_ = -1;
	data.animSpeed_ = 60.0f;
	data.animRate_ = 0.0f;
	data.animTime_ = 0.0f;
	data.animTotalTime_ = MV1GetAnimTotalTime(handle, 0);
	data.invalidPos_ = invalid;
	data.nextAnimNo_ = nextAnimNo;
	data.endDetach_ = endDetach;
	data.endCallAnimEnd_ = false;
	for (int i = 0; i < splitFrameNum_; i++){
		float rate = 0.0f;
		data.animRateFrame_.emplace_back(rate);

		bool isPriority = false;
		data.isPriorityFrame_.emplace_back(isPriority);
	}


	int i = -1;
	for (auto no : splitFrameNo_) {
		if (frame == -1) break;
		i++;
		if (no == frame) break;
	}
	data.applyFrameIndex_ = i;
	animData_.emplace(animNo, data);

	 
	//MV1SetAttachAnimBlendRateToFrame();
}

void AnimatorController::SetAnimTime(int animNo, float time)
{
	animData_.at(animNo).animTime_ = time;
}

void AnimatorController::SetAnimSpeed(int animNo, float speed)
{
	animData_.at(animNo).animSpeed_ = speed;
}

void AnimatorController::SetDetachSpeed(int animNo, float speed)
{
	animData_.at(animNo).detachSpeed_ = speed;
}

void AnimatorController::SetCallAnimEnd(int animNo, bool isCall)
{
	animData_.at(animNo).endCallAnimEnd_ = isCall;
}

void AnimatorController::SetNoticeTime(int animNo, float time)
{
	animData_.at(animNo).noticeTime_ = time;
}

void AnimatorController::SetStartTime(int animNo, float time)
{
	animData_.at(animNo).startTime_ = time;
}

void AnimatorController::SetIsStop(int animNo, bool isStop)
{
	animData_.at(animNo).isStop_ = isStop;
}

void AnimatorController::SetIsNotice(int animNo, bool isNotice)
{
	animData_.at(animNo).isNotice_ = isNotice;
}

void AnimatorController::ChangeAnimation(int animNo)
{
	if (animData_.at(animNo).applyFrameIndex_ != -1) {
		ChangeAnimationFrame(animNo, animData_.at(animNo).applyFrameIndex_);
		return;
	}

	// 現在優先されているアニメーションを探索
	int priNo = -1;
	for (auto data : animData_)
	{
		if (!data.second.isPriority_) continue;

		// 優先されているアニメーションのID
		priNo = data.first;
		break;
	}

	// 渡ってきたアニメーションを最優先にして再生させる
	// もし、すでに最優先なら処理を行わない
	if (priNo == animNo)
	{
		for (int i = 0; i < splitFrameNum_; i++) {
			ChangeAnimationFrame(animNo, i);
		}
		return;
	}
	
	// 最優先アニメーションに設定するブレンド率
	float animRate = 1.0f;

	// 何も再生されていない
	if (priNo == -1)
	{
		// 最優先アニメーション設定ーーーーーーーーーーーーーーーー
		// 最優先アニメーションをアタッチ
		auto& pri = animData_.at(animNo);
		pri.animAttachNo_ =
			MV1AttachAnim(transform_.modelId, 0, pri.animHandle);
		pri.animRate_ = animRate;
		pri.animTime_ = pri.startTime_;
		pri.isPriority_ = true;
		invalidBrendPos_ = pri.invalidPos_;
		for (int i = 0; i < splitFrameNum_; i++) {
			pri.animRateFrame_[i] = 1.0f;
			pri.isPriorityFrame_[i] = true;
		}
		//MV1SetAttachAnimBlendRate(transform_.modelId,
		//	pri.animAttachNo_, pri.animRate_);
		//MV1SetAttachAnimBlendRateToFrame(transform_.modelId,
		//	pri.animAttachNo_, 1, 0.0f);

		//auto& test = animData_.at(1);
		//test.animAttachNo_ = MV1AttachAnim(transform_.modelId, 0, animData_.at(1).animHandle);


		//MV1SetAttachAnimBlendRate(transform_.modelId,
		//	test.animAttachNo_, 0.0f);

		//MV1SetAttachAnimBlendRateToFrame(transform_.modelId,
		//	test.animAttachNo_,1 ,1.0f);
		//MV1SetAttachAnimBlendRateToFrame(transform_.modelId,
		//	pri.animAttachNo_, 60, 0.0f);
		//MV1SetAttachAnimBlendRateToFrame(transform_.modelId,
		//	pri.animAttachNo_, 0, 0.0f,0);

	}
	else
	{
		//// 優先されているアニメーションに次のアニメーションが設定
		//// されているかつ、まだ再生中の場合処理を行わない
		//if (animData_.at(priNo).nextAnimNo_ != -1 &&
		//	animData_.at(priNo).animTime_ < animData_.at(priNo).animTotalTime_) return;


		// 遷移フラグをON
		isChanging_ = true;

		// すでに他に優先されているものがある
		// それを優先させないように処理
		// 元最優先にさせるアニメーション
		auto& oldPri = animData_.at(priNo);
		oldPri.isPriority_ = false;

		// アニメーションブレンド率を計算
		for (auto data : animData_)
		{
			if (data.second.animAttachNo_ == -1)continue;

			// ブレンド率を引いていく
			animRate -= data.second.animRate_;
			// 0より小さくなることはないが念のため
			if (animRate < 0)animRate = 0;
		}

		// 最優先アニメーション設定ーーーーーーーーーーーーーーーー
		
		// 最優先ではないがすでにアタッチされている場合
		// 優先度のみ変更
		auto& pri = animData_.at(animNo);
		if (pri.animAttachNo_ == -1)
		{
			// 最優先アニメーションをアタッチ
			pri.animAttachNo_ =
				MV1AttachAnim(transform_.modelId, 0, pri.animHandle);
			pri.animRate_ = animRate;
			pri.animTime_ = pri.startTime_;
		}
		pri.isPriority_ = true;
	}

	for (int i = 0; i < splitFrameNum_; i++) {
		ChangeAnimationFrame(animNo, i);
	}
}

void AnimatorController::ChangeAnimationFrame(int animNo, int frameIndex)
{
	// インデックス探索
	int index = frameIndex;

	// 現在優先されているアニメーションを探索
	int priNo = -1;
	for (auto data : animData_)
	{
		if (!data.second.isPriorityFrame_[index]) continue;

		// 優先されているアニメーションのID
		priNo = data.first;
		break;
	}

	// 渡ってきたアニメーションを最優先にして再生させる
	// もし、すでに最優先なら処理を行わない
	if (priNo == animNo) return;

	// 最優先アニメーションに設定するブレンド率
	float animRate = 1.0f;

	// 何も再生されていない
	if (priNo == -1)
	{
		// 最優先アニメーション設定ーーーーーーーーーーーーーーーー
		// 最優先アニメーションをアタッチ
		auto& pri = animData_.at(animNo);
		pri.animAttachNo_ =
			MV1AttachAnim(transform_.modelId, 0, pri.animHandle);
		pri.animRateFrame_[index] = animRate;
		pri.animTime_ = pri.startTime_;
		pri.isPriorityFrame_[index] = true;
		invalidBrendPos_ = pri.invalidPos_;
		MV1SetAttachAnimBlendRate(transform_.modelId,
			pri.animAttachNo_, 0.0f);
		MV1SetAttachAnimBlendRateToFrame(transform_.modelId,
			pri.animAttachNo_, splitFrameNo_[index], pri.animRateFrame_[index]);
		//MV1SetAttachAnimBlendRateToFrame(transform_.modelId,
		//	pri.animAttachNo_,55 ,0.0f);
		//MV1SetAttachAnimBlendRateToFrame(transform_.modelId,
		//	pri.animAttachNo_, 60, 0.0f);
		//MV1SetAttachAnimBlendRateToFrame(transform_.modelId,
		//	pri.animAttachNo_, 0, 0.0f,0);

	}
	else
	{
		//// 優先されているアニメーションに次のアニメーションが設定
		//// されているかつ、まだ再生中の場合処理を行わない
		//if (animData_.at(priNo).nextAnimNo_ != -1 &&
		//	animData_.at(priNo).animTime_ < animData_.at(priNo).animTotalTime_) return;


		// 遷移フラグをON
		isChangingFrame_[index] = true;

		// すでに他に優先されているものがある
		// それを優先させないように処理
		// 元最優先にさせるアニメーション
		auto& oldPri = animData_.at(priNo);
		oldPri.isPriorityFrame_[index] = false;

		// アニメーションブレンド率を計算
		for (auto data : animData_)
		{
			if (data.second.animAttachNo_ == -1)continue;

			// ブレンド率を引いていく
			animRate -= data.second.animRateFrame_[index];
			// 0より小さくなることはないが念のため
			if (animRate < 0.0f)animRate = 0.0f;
		}

		// 最優先アニメーション設定ーーーーーーーーーーーーーーーー

		// 最優先ではないがすでにアタッチされている場合
		// 優先度のみ変更
		auto& pri = animData_.at(animNo);
		if (pri.animAttachNo_ == -1)
		{
			// 最優先アニメーションをアタッチ
			pri.animAttachNo_ =
				MV1AttachAnim(transform_.modelId, 0, pri.animHandle);
			pri.animRateFrame_[index] = animRate;
			pri.animTime_ = pri.startTime_;
			MV1SetAttachAnimBlendRate(transform_.modelId,
				pri.animAttachNo_, 0.0f);
			MV1SetAttachAnimBlendRateToFrame(transform_.modelId,
				pri.animAttachNo_, splitFrameNo_[index], pri.animRateFrame_[index]);
		}
		pri.isPriorityFrame_[index] = true;
	}
}

bool AnimatorController::IsAnimation(int animNo)
{
	// アニメーション
	auto& data = animData_.at(animNo);
	
	return (data.animAttachNo_ != -1 &&
			data.animRate_ != 0.0f);
}

float AnimatorController::GetAnimRate(int animNo)
{
	auto& data = animData_.at(animNo);
	return data.animRate_;
}

void AnimatorController::InvalidAnimMove(void)
{
	if (validAnimMove_) return;

	// 対象フレーム(今回は0版)のローカル行列を初期値にリセットする
	MV1ResetFrameUserLocalMatrix(transform_.modelId, invalidFrameNo_);

	// 対象フレームのローカル行列(大きさ、回転、位置)を取得する
	auto mat = MV1GetFrameLocalMatrix(transform_.modelId, invalidFrameNo_);
	auto scl = MGetSize(mat);			// 行列から大きさを取り出す
	auto rot = MGetRotElem(mat);		// 行列から回転を取り出す
	auto pos = MGetTranslateElem(mat);	// 行列から移動値を取り出す

	// 大きさ、回転、位置をローカル行列に戻す
	MATRIX mix = MGetIdent();
	mix = MMult(mix, MGetScale(scl));	// 大きさ
	mix = MMult(mix, rot);				// 回転
	mix = MMult(mix, MGetTranslate(invalidBrendPos_));

	// ここでローカル座標を行列に、そのまま戻さず、
	// 移動値をゼロにすることで、アニメーションによる移動を無効化している
	//mix = MMult(mix, pos);
	//mix = MMult(mix, MGetTranslate({ 0.0f, 0.0f, 0.0f }));
	// Y軸の変更は微調整なので気にしなくてよき

	// 合成した行列を対象フレームにセットし直して、
	// アニメーションの移動値を無効化
	MV1SetFrameUserLocalMatrix(transform_.modelId, invalidFrameNo_, mix);
}

void AnimatorController::Release(void)
{
	for (auto& data : animData_)
	{
		MV1DeleteModel(data.second.animHandle);
	}

	animData_.clear();
}

void AnimatorController::Reset(void)
{
	for (auto data : animData_)
	{
		AnimationDettach(data.first);
	}

	isChanging_ = false;
}

void AnimatorController::SetValidAnimMove(bool valid)
{
	validAnimMove_ = valid;
	if (valid) invalidBrendPos_ = {};
}

void AnimatorController::SetInvalidFrameNo(int no)
{
	invalidFrameNo_ = no;
}

void AnimatorController::DrawDebug(void)
{

	DrawFormatString(800, 60 + GetFontSize() * 1 + 100, 0xff00ff,
		"foward : %.2f", animData_.at(5).animRateFrame_[0]);
	DrawFormatString(800, 60 + GetFontSize() * 2 + 100, 0xff00ff,
		"right : %.2f", animData_.at(3).animRateFrame_[0]);
	DrawFormatString(800, 60 + GetFontSize() * 3 + 100, 0xff00ff,
		"gre : %.2f", animData_.at(9).animRateFrame_[0]);
}

void AnimatorController::AnimationDettach(int animNo)
{
	auto& data = animData_.at(animNo);

	if (data.animAttachNo_ == -1) return;

	MV1DetachAnim(transform_.modelId, data.animAttachNo_);

	// 値をリセット

	// アニメーション現在時間
	data.animTime_ = data.startTime_;

	// アニメーションアタッチNo(モデルとアニメーションの管理番号)
	data.animAttachNo_ = -1;

	// ブレンド用
	data.animRate_ = 0.0f;

	// 再生優先させるか
	data.isPriority_ = false;

	// 通知を送ったか
	data.isNotice_ = false;

	for (auto i = 0; i < splitFrameNum_; i++) {
		data.animRateFrame_[i] = 0.0f;
		data.isPriorityFrame_[i] = false;
	}
}

void AnimatorController::Animation(void)
{
	// アニメーション再生
	// 経過時間の取得
	float delta = SceneManager::GetInstance().GetDeltaTime();

	for (auto& data : animData_)
	{
		// アタッチされてないなら再生しない
		// アニメーション停止状態なら再生しない
		if (data.second.animAttachNo_ == -1 ||
			data.second.isStop_)continue;


		// アニメーション時間の進行
		data.second.animTime_ += (data.second.animSpeed_ * delta);
		if (data.second.animTime_ > data.second.animTotalTime_)
		{
			// 次に再生させるアニメーションがあるか否か
			if (data.second.nextAnimNo_ != -1)
			{


				if (data.second.endDetach_)
				{


					// アニメーション終了後処理
					if (data.second.endCallAnimEnd_) {
						parent_.lock()->AnimEnd(data.first);
					}
					



					// デタッチ
					AnimationDettach(data.first);
				}
			}
			else
			{
				// ループ再生
				data.second.animTime_ = 0.0f;

				if (data.second.endCallAnimEnd_)
				{
					parent_.lock()->AnimEnd(data.first);
				}
			}
		}

		// 通知
		if (data.second.noticeTime_ != 0.0f &&
			data.second.animTime_ >= data.second.noticeTime_ &&
			!data.second.isNotice_)
		{
			// 通知
			parent_.lock()->AnimNotice(data.first);
			data.second.isNotice_ = true;
		}


		if (data.second.animAttachNo_ == -1) continue;
		MV1SetAttachAnimTime(transform_.modelId, data.second.animAttachNo_, data.second.animTime_);
	}

	//auto data = animData_.at(1);
	//data.animTime_ += (data.animSpeed_ * delta);
	//if (data.animTime_ > data.animTotalTime_)
	//{
	//	data.animTime_ = 0.0f;
	//}
	//MV1SetAttachAnimTime(transform_.modelId, data.animAttachNo_, data.animTime_);


}

void AnimatorController::ChangingAnimation(void)
{

	if (!isChanging_) return;

	int i = 0;
	for (auto data : animData_){
		if (data.second.isPriority_)i++;
	}
	if (i > 1) {
		int n = 0;
	}
	// デルタタイム
	float delta = SceneManager::GetInstance().GetDeltaTime();

	float animRate = 1.0f;
	VECTOR blendPos = {};

	// 最優先アニメーションNo
	int priNo = -1;

	for (auto& data : animData_)
	{
		// 最優先アニメーションの探索も一緒に行っておく
		if (data.second.isPriority_)
		{
			priNo = data.first;
		}

		// アタッチされてないなら処理しない
		// また、最優先アニメーションなら処理を行わない
		if (data.second.animAttachNo_ == -1 ||
			data.second.isPriority_ ||
			data.second.applyFrameIndex_ != -1)continue;


		data.second.animRate_ -= data.second.detachSpeed_ * delta;
		if (data.second.animRate_ < 0.0f)
		{

			//if (data.second.endCallAnimEnd_)
			//{
			//	parent_.lock()->AnimEnd(data.first);
			//}


			data.second.animRate_ = 0.0f;

			// 遷移前アニメーションのブレンド率をセット
			MV1SetAttachAnimBlendRate(transform_.modelId,
				data.second.animAttachNo_, data.second.animRate_);
			//// ブレンド率がすべて0になったらデタッチさせる
			//bool detach = true;
			//for (auto rate : data.second.animRateFrame_) {
			//	if (rate != 0.0f){
			//		detach = false;
			//		break;
			//	}
			//}
			//if (detach)
			//{
			//	AnimationDettach(data.first);
			//}
		}
		else
		{
			// 遷移前アニメーションのブレンド率をセット
			MV1SetAttachAnimBlendRate(transform_.modelId,
				data.second.animAttachNo_, data.second.animRate_);

			// 座標移動無効化用座標をブレンド
			blendPos = VAdd(blendPos, VScale(data.second.invalidPos_, data.second.animRate_));

			// ブレンド率を引いていく
			animRate -= data.second.animRate_;
		}
	}

	// 念のため
	if (priNo == -1)return;


	// 最優先アニメーション設定ーーーーーーーーーーーーーーーーーーーーーーーーーー
	auto& pri = animData_.at(priNo);

	// 遷移後アニメーションのブレンド率をセット
	pri.animRate_ = animRate;
	MV1SetAttachAnimBlendRate(transform_.modelId,
		pri.animAttachNo_, pri.animRate_);

	// 座標移動無効化用座標をブレンド
	blendPos = VAdd(blendPos, VScale(pri.invalidPos_, pri.animRate_));

	// 座標移動無効化用座標をセット
	invalidBrendPos_ = blendPos;


	// もし、最優先アニメーションのブレンド率が
	// 1.0fになっていたら遷移フラグをOFF
	if (pri.animRate_ >= 1.0f)
	{
		isChanging_ = false;
	}
}

void AnimatorController::ChangingAnimationFrame(int index)
{
	if (!isChangingFrame_[index]) return;

	int i = 0;
	for (auto data : animData_) {
		if (data.second.isPriorityFrame_[index])i++;
	}
	if (i > 1) {
		int n = 0;
	}

	// デルタタイム
	float delta = SceneManager::GetInstance().GetDeltaTime();

	float animRate = 1.0f;
	//VECTOR blendPos = {};

	// 最優先アニメーションNo
	int priNo = -1;

	for (auto& data : animData_)
	{
		// 最優先アニメーションの探索も一緒に行っておく
		if (data.second.isPriorityFrame_[index])
		{
			priNo = data.first;
		}

		// アタッチされてないなら処理しない
		// また、最優先アニメーションなら処理を行わない
		if (data.second.animAttachNo_ == -1 ||
			data.second.isPriorityFrame_[index])continue;

		//if(data.second.applyFrameIndex_ != splitFrameNo_[index] &&
		//	data.second.applyFrameIndex_ != -1)continue;

		data.second.animRateFrame_[index] -= data.second.detachSpeed_ * delta;
		if (data.second.animRateFrame_[index] < 0.0f)
		{

			//if (data.second.endCallAnimEnd_)
			//{
			//	parent_.lock()->AnimEnd(data.first);
			//}


			data.second.animRateFrame_[index] = 0.0f;


			// 遷移前アニメーションのブレンド率をセット
			MV1SetAttachAnimBlendRateToFrame(transform_.modelId,
				data.second.animAttachNo_, splitFrameNo_[index], data.second.animRateFrame_[index]);
		}
		else
		{
			// 遷移前アニメーションのブレンド率をセット
			MV1SetAttachAnimBlendRateToFrame(transform_.modelId,
				data.second.animAttachNo_, splitFrameNo_[index], data.second.animRateFrame_[index]);

			// 座標移動無効化用座標をブレンド
			//blendPos = VAdd(blendPos, VScale(data.second.invalidPos_, data.second.animRateFrame_[index]));

			// ブレンド率を引いていく
			animRate -= data.second.animRateFrame_[index];
		}
	}



	// 念のため
	if (priNo == -1)return;


	// 最優先アニメーション設定ーーーーーーーーーーーーーーーーーーーーーーーーーー
	auto& pri = animData_.at(priNo);

	// 遷移後アニメーションのブレンド率をセット
	pri.animRateFrame_[index] = animRate;
	MV1SetAttachAnimBlendRateToFrame(transform_.modelId,
		pri.animAttachNo_, splitFrameNo_[index], pri.animRateFrame_[index]);

	// 座標移動無効化用座標をブレンド
	//blendPos = VAdd(blendPos, VScale(pri.invalidPos_, pri.animRateFrame_[index]));

	// 座標移動無効化用座標をセット
	//invalidBrendPos_ = blendPos;


	// もし、最優先アニメーションのブレンド率が
	// 1.0fになっていたら遷移フラグをOFF
	if (pri.animRateFrame_[index] >= 1.0f)
	{
		//isChangingFrame_[index] = false;
	}
}

void AnimatorController::CheckDettach(void)
{
	for (auto& data : animData_)
	{
		if (data.second.animAttachNo_ == -1) continue;

		// ブレンド率がすべて0になったらデタッチさせる
		bool detach = (data.second.animRate_ == 0.0f);
		for (auto rate : data.second.animRateFrame_) {
			if (rate != 0) {
				detach = false;
				break;
			}
		}
		if (detach)
		{
			//if (data.second.endCallAnimEnd_)
			//{
			//	parent_.lock()->AnimEnd(data.first);
			//}

			AnimationDettach(data.first);
		}
	}

}
