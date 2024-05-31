#include "../../Manager/SceneManager.h"
#include "ActorBase.h"
#include "../Common/Transform.h"
#include "AnimatorController.h"

AnimatorController::AnimatorController(std::shared_ptr<ActorBase> obj, const Transform& transform) : transform_(transform)
{
	// �e�̃|�C���^
	parent_ = obj;

	// �J�ڒ����ǂ���
	isChanging_ = false;

	// �A�j���[�V�����ɂ����W�ړ��𖳌�������ۂ̃I�t�Z�b�g
	invalidBrendPos_ = {};

	// �A�j���[�V�����ɂ����W�ړ���L����
	validAnimMove_ = false;

	// �t���[��������
	splitFrameNum_ = 0;

	invalidFrameNo_ = 0;
}

AnimatorController::AnimatorController(std::shared_ptr<ActorBase> obj, const Transform& transform, std::vector<int> frameNo) : transform_(transform)
{
	// �e�̃|�C���^
	parent_ = obj;

	// �J�ڒ����ǂ���
	isChanging_ = false;

	// �A�j���[�V�����ɂ����W�ړ��𖳌�������ۂ̃I�t�Z�b�g
	invalidBrendPos_ = {};

	// �A�j���[�V�����ɂ����W�ړ���L����
	validAnimMove_ = false;

	// �t���[��������
	splitFrameNum_ = frameNo.size();

	splitFrameNo_ = frameNo;

	isChangingFrame_ = std::vector(splitFrameNum_, false);

	invalidFrameNo_ = 0;
}

void AnimatorController::Update(void)
{
	// �A�j���[�V�����J��
	ChangingAnimation();

	for(int i = 0; i < splitFrameNum_;i++){
		ChangingAnimationFrame(i);
	}
	

	// �A�j���[�V��������
	Animation();

	// �A�j���[�V�����ɂ����W�ړ�������
	InvalidAnimMove();

	CheckDettach();
}

void AnimatorController::SetAnimData(int animNo, int handle, VECTOR invalid, int frame , int nextAnimNo, bool endDetach)
{
	// �A�j���[�V�����n���h�����i�[
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

	// ���ݗD�悳��Ă���A�j���[�V������T��
	int priNo = -1;
	for (auto data : animData_)
	{
		if (!data.second.isPriority_) continue;

		// �D�悳��Ă���A�j���[�V������ID
		priNo = data.first;
		break;
	}

	// �n���Ă����A�j���[�V�������ŗD��ɂ��čĐ�������
	// �����A���łɍŗD��Ȃ珈�����s��Ȃ�
	if (priNo == animNo)
	{
		for (int i = 0; i < splitFrameNum_; i++) {
			ChangeAnimationFrame(animNo, i);
		}
		return;
	}
	
	// �ŗD��A�j���[�V�����ɐݒ肷��u�����h��
	float animRate = 1.0f;

	// �����Đ�����Ă��Ȃ�
	if (priNo == -1)
	{
		// �ŗD��A�j���[�V�����ݒ�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
		// �ŗD��A�j���[�V�������A�^�b�`
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
		//// �D�悳��Ă���A�j���[�V�����Ɏ��̃A�j���[�V�������ݒ�
		//// ����Ă��邩�A�܂��Đ����̏ꍇ�������s��Ȃ�
		//if (animData_.at(priNo).nextAnimNo_ != -1 &&
		//	animData_.at(priNo).animTime_ < animData_.at(priNo).animTotalTime_) return;


		// �J�ڃt���O��ON
		isChanging_ = true;

		// ���łɑ��ɗD�悳��Ă�����̂�����
		// �����D�悳���Ȃ��悤�ɏ���
		// ���ŗD��ɂ�����A�j���[�V����
		auto& oldPri = animData_.at(priNo);
		oldPri.isPriority_ = false;

		// �A�j���[�V�����u�����h�����v�Z
		for (auto data : animData_)
		{
			if (data.second.animAttachNo_ == -1)continue;

			// �u�����h���������Ă���
			animRate -= data.second.animRate_;
			// 0��菬�����Ȃ邱�Ƃ͂Ȃ����O�̂���
			if (animRate < 0)animRate = 0;
		}

		// �ŗD��A�j���[�V�����ݒ�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
		
		// �ŗD��ł͂Ȃ������łɃA�^�b�`����Ă���ꍇ
		// �D��x�̂ݕύX
		auto& pri = animData_.at(animNo);
		if (pri.animAttachNo_ == -1)
		{
			// �ŗD��A�j���[�V�������A�^�b�`
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
	// �C���f�b�N�X�T��
	int index = frameIndex;

	// ���ݗD�悳��Ă���A�j���[�V������T��
	int priNo = -1;
	for (auto data : animData_)
	{
		if (!data.second.isPriorityFrame_[index]) continue;

		// �D�悳��Ă���A�j���[�V������ID
		priNo = data.first;
		break;
	}

	// �n���Ă����A�j���[�V�������ŗD��ɂ��čĐ�������
	// �����A���łɍŗD��Ȃ珈�����s��Ȃ�
	if (priNo == animNo) return;

	// �ŗD��A�j���[�V�����ɐݒ肷��u�����h��
	float animRate = 1.0f;

	// �����Đ�����Ă��Ȃ�
	if (priNo == -1)
	{
		// �ŗD��A�j���[�V�����ݒ�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
		// �ŗD��A�j���[�V�������A�^�b�`
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
		//// �D�悳��Ă���A�j���[�V�����Ɏ��̃A�j���[�V�������ݒ�
		//// ����Ă��邩�A�܂��Đ����̏ꍇ�������s��Ȃ�
		//if (animData_.at(priNo).nextAnimNo_ != -1 &&
		//	animData_.at(priNo).animTime_ < animData_.at(priNo).animTotalTime_) return;


		// �J�ڃt���O��ON
		isChangingFrame_[index] = true;

		// ���łɑ��ɗD�悳��Ă�����̂�����
		// �����D�悳���Ȃ��悤�ɏ���
		// ���ŗD��ɂ�����A�j���[�V����
		auto& oldPri = animData_.at(priNo);
		oldPri.isPriorityFrame_[index] = false;

		// �A�j���[�V�����u�����h�����v�Z
		for (auto data : animData_)
		{
			if (data.second.animAttachNo_ == -1)continue;

			// �u�����h���������Ă���
			animRate -= data.second.animRateFrame_[index];
			// 0��菬�����Ȃ邱�Ƃ͂Ȃ����O�̂���
			if (animRate < 0.0f)animRate = 0.0f;
		}

		// �ŗD��A�j���[�V�����ݒ�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[

		// �ŗD��ł͂Ȃ������łɃA�^�b�`����Ă���ꍇ
		// �D��x�̂ݕύX
		auto& pri = animData_.at(animNo);
		if (pri.animAttachNo_ == -1)
		{
			// �ŗD��A�j���[�V�������A�^�b�`
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
	// �A�j���[�V����
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

	// �Ώۃt���[��(�����0��)�̃��[�J���s��������l�Ƀ��Z�b�g����
	MV1ResetFrameUserLocalMatrix(transform_.modelId, invalidFrameNo_);

	// �Ώۃt���[���̃��[�J���s��(�傫���A��]�A�ʒu)���擾����
	auto mat = MV1GetFrameLocalMatrix(transform_.modelId, invalidFrameNo_);
	auto scl = MGetSize(mat);			// �s�񂩂�傫�������o��
	auto rot = MGetRotElem(mat);		// �s�񂩂��]�����o��
	auto pos = MGetTranslateElem(mat);	// �s�񂩂�ړ��l�����o��

	// �傫���A��]�A�ʒu�����[�J���s��ɖ߂�
	MATRIX mix = MGetIdent();
	mix = MMult(mix, MGetScale(scl));	// �傫��
	mix = MMult(mix, rot);				// ��]
	mix = MMult(mix, MGetTranslate(invalidBrendPos_));

	// �����Ń��[�J�����W���s��ɁA���̂܂ܖ߂����A
	// �ړ��l���[���ɂ��邱�ƂŁA�A�j���[�V�����ɂ��ړ��𖳌������Ă���
	//mix = MMult(mix, pos);
	//mix = MMult(mix, MGetTranslate({ 0.0f, 0.0f, 0.0f }));
	// Y���̕ύX�͔������Ȃ̂ŋC�ɂ��Ȃ��Ă悫

	// ���������s���Ώۃt���[���ɃZ�b�g�������āA
	// �A�j���[�V�����̈ړ��l�𖳌���
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

	// �l�����Z�b�g

	// �A�j���[�V�������ݎ���
	data.animTime_ = data.startTime_;

	// �A�j���[�V�����A�^�b�`No(���f���ƃA�j���[�V�����̊Ǘ��ԍ�)
	data.animAttachNo_ = -1;

	// �u�����h�p
	data.animRate_ = 0.0f;

	// �Đ��D�悳���邩
	data.isPriority_ = false;

	// �ʒm�𑗂�����
	data.isNotice_ = false;

	for (auto i = 0; i < splitFrameNum_; i++) {
		data.animRateFrame_[i] = 0.0f;
		data.isPriorityFrame_[i] = false;
	}
}

void AnimatorController::Animation(void)
{
	// �A�j���[�V�����Đ�
	// �o�ߎ��Ԃ̎擾
	float delta = SceneManager::GetInstance().GetDeltaTime();

	for (auto& data : animData_)
	{
		// �A�^�b�`����ĂȂ��Ȃ�Đ����Ȃ�
		// �A�j���[�V������~��ԂȂ�Đ����Ȃ�
		if (data.second.animAttachNo_ == -1 ||
			data.second.isStop_)continue;


		// �A�j���[�V�������Ԃ̐i�s
		data.second.animTime_ += (data.second.animSpeed_ * delta);
		if (data.second.animTime_ > data.second.animTotalTime_)
		{
			// ���ɍĐ�������A�j���[�V���������邩�ۂ�
			if (data.second.nextAnimNo_ != -1)
			{


				if (data.second.endDetach_)
				{


					// �A�j���[�V�����I���㏈��
					if (data.second.endCallAnimEnd_) {
						parent_.lock()->AnimEnd(data.first);
					}
					



					// �f�^�b�`
					AnimationDettach(data.first);
				}
			}
			else
			{
				// ���[�v�Đ�
				data.second.animTime_ = 0.0f;

				if (data.second.endCallAnimEnd_)
				{
					parent_.lock()->AnimEnd(data.first);
				}
			}
		}

		// �ʒm
		if (data.second.noticeTime_ != 0.0f &&
			data.second.animTime_ >= data.second.noticeTime_ &&
			!data.second.isNotice_)
		{
			// �ʒm
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
	// �f���^�^�C��
	float delta = SceneManager::GetInstance().GetDeltaTime();

	float animRate = 1.0f;
	VECTOR blendPos = {};

	// �ŗD��A�j���[�V����No
	int priNo = -1;

	for (auto& data : animData_)
	{
		// �ŗD��A�j���[�V�����̒T�����ꏏ�ɍs���Ă���
		if (data.second.isPriority_)
		{
			priNo = data.first;
		}

		// �A�^�b�`����ĂȂ��Ȃ珈�����Ȃ�
		// �܂��A�ŗD��A�j���[�V�����Ȃ珈�����s��Ȃ�
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

			// �J�ڑO�A�j���[�V�����̃u�����h�����Z�b�g
			MV1SetAttachAnimBlendRate(transform_.modelId,
				data.second.animAttachNo_, data.second.animRate_);
			//// �u�����h�������ׂ�0�ɂȂ�����f�^�b�`������
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
			// �J�ڑO�A�j���[�V�����̃u�����h�����Z�b�g
			MV1SetAttachAnimBlendRate(transform_.modelId,
				data.second.animAttachNo_, data.second.animRate_);

			// ���W�ړ��������p���W���u�����h
			blendPos = VAdd(blendPos, VScale(data.second.invalidPos_, data.second.animRate_));

			// �u�����h���������Ă���
			animRate -= data.second.animRate_;
		}
	}

	// �O�̂���
	if (priNo == -1)return;


	// �ŗD��A�j���[�V�����ݒ�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
	auto& pri = animData_.at(priNo);

	// �J�ڌ�A�j���[�V�����̃u�����h�����Z�b�g
	pri.animRate_ = animRate;
	MV1SetAttachAnimBlendRate(transform_.modelId,
		pri.animAttachNo_, pri.animRate_);

	// ���W�ړ��������p���W���u�����h
	blendPos = VAdd(blendPos, VScale(pri.invalidPos_, pri.animRate_));

	// ���W�ړ��������p���W���Z�b�g
	invalidBrendPos_ = blendPos;


	// �����A�ŗD��A�j���[�V�����̃u�����h����
	// 1.0f�ɂȂ��Ă�����J�ڃt���O��OFF
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

	// �f���^�^�C��
	float delta = SceneManager::GetInstance().GetDeltaTime();

	float animRate = 1.0f;
	//VECTOR blendPos = {};

	// �ŗD��A�j���[�V����No
	int priNo = -1;

	for (auto& data : animData_)
	{
		// �ŗD��A�j���[�V�����̒T�����ꏏ�ɍs���Ă���
		if (data.second.isPriorityFrame_[index])
		{
			priNo = data.first;
		}

		// �A�^�b�`����ĂȂ��Ȃ珈�����Ȃ�
		// �܂��A�ŗD��A�j���[�V�����Ȃ珈�����s��Ȃ�
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


			// �J�ڑO�A�j���[�V�����̃u�����h�����Z�b�g
			MV1SetAttachAnimBlendRateToFrame(transform_.modelId,
				data.second.animAttachNo_, splitFrameNo_[index], data.second.animRateFrame_[index]);
		}
		else
		{
			// �J�ڑO�A�j���[�V�����̃u�����h�����Z�b�g
			MV1SetAttachAnimBlendRateToFrame(transform_.modelId,
				data.second.animAttachNo_, splitFrameNo_[index], data.second.animRateFrame_[index]);

			// ���W�ړ��������p���W���u�����h
			//blendPos = VAdd(blendPos, VScale(data.second.invalidPos_, data.second.animRateFrame_[index]));

			// �u�����h���������Ă���
			animRate -= data.second.animRateFrame_[index];
		}
	}



	// �O�̂���
	if (priNo == -1)return;


	// �ŗD��A�j���[�V�����ݒ�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
	auto& pri = animData_.at(priNo);

	// �J�ڌ�A�j���[�V�����̃u�����h�����Z�b�g
	pri.animRateFrame_[index] = animRate;
	MV1SetAttachAnimBlendRateToFrame(transform_.modelId,
		pri.animAttachNo_, splitFrameNo_[index], pri.animRateFrame_[index]);

	// ���W�ړ��������p���W���u�����h
	//blendPos = VAdd(blendPos, VScale(pri.invalidPos_, pri.animRateFrame_[index]));

	// ���W�ړ��������p���W���Z�b�g
	//invalidBrendPos_ = blendPos;


	// �����A�ŗD��A�j���[�V�����̃u�����h����
	// 1.0f�ɂȂ��Ă�����J�ڃt���O��OFF
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

		// �u�����h�������ׂ�0�ɂȂ�����f�^�b�`������
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
