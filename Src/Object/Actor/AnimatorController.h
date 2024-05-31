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

	//// �A�j���[�V������K�p�����镔��
	//enum class BODY
	//{
	//	UP,
	//	DOWN,
	//	FULL
	//};

	struct ANIM_DATA
	{
		// �ς��Ȃ�����------------------------------
		// �A�j���[�V�����n���h��
		int animHandle = -1;

		// �A�j���[�V�����X�s�[�h
		float animSpeed_ = 60.0f;

		// �A�j���[�V�������Đ�����
		float animTotalTime_ = 0.0f;

		// ���W�ړ��������p
		VECTOR invalidPos_ = {};

		// �Đ��I����ɍĐ�������A�j���[�V����
		int nextAnimNo_ = -1;

		// �Đ��I����A�����Ƀf�^�b�`������
		bool endDetach_ = false;

		// �Đ��I����A�e�I�u�W�F�N�g��AnimEnd���Ă�
		bool endCallAnimEnd_ = false;

		// �u�����h���������x
		float detachSpeed_ = 2.0f;

		// ���̍Đ����ԂɂȂ�����e�ɒʒm�𑗂�(0�b�ڂ͑���Ȃ�)
		float noticeTime_ = 0.0f;

		// �A�j���[�V�����J�n����
		float startTime_ = 0.0f;

		// �K�p������t���[��
		int applyFrameIndex_ = -1;

		// �ς��\���̂������----------------------
		// �A�j���[�V�������ݎ���
		float animTime_ = 0.0f;

		// �A�j���[�V�����A�^�b�`No(���f���ƃA�j���[�V�����̊Ǘ��ԍ�)
		int animAttachNo_ = -1;

		// �u�����h�p
		float animRate_ = 0.0f;

		// �Đ��D�悳���邩
		bool isPriority_ = false;

		// �A�j���[�V���������f���t���[�����Ƃɕ�������ꍇ�g��
		std::vector<float>animRateFrame_ = {};
		std::vector<bool>isPriorityFrame_ = {};

		// �ʒm�𑗂�����
		bool isNotice_ = false;

		// �A�j���[�V�����Đ����X�g�b�v
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

	// �w��̃A�j���[�V�������Đ������ǂ���
	bool IsAnimation(int animNo);

	float GetAnimRate(int animNo);

	void Release(void);

	void Reset(void);

	void SetValidAnimMove(bool valid);

	void SetInvalidFrameNo(int no);

	void DrawDebug(void);

private:

	// �J�ڎ���
	static constexpr float ANIM_CHANGE_TIME = 0.5f;

	// �e�̃|�C���^
	std::weak_ptr<ActorBase> parent_;

	const Transform& transform_;

	// �t���[��������
	int splitFrameNum_;

	// 
	std::vector<int> splitFrameNo_;

	// �A�j���[�V�����f�[�^
	std::unordered_map<int, ANIM_DATA> animData_;

	// �J�ڒ����ǂ���
	bool isChanging_;
	std::vector<bool>isChangingFrame_;

	// �A�j���[�V�����ɂ����W�ړ��𖳌�������ۂ̃I�t�Z�b�g
	VECTOR invalidBrendPos_;
	
	int invalidFrameNo_;

	// �A�j���[�V�����ɂ����W�ړ���L����
	bool validAnimMove_;

	// �A�j���[�V�����f�^�b�`
	void AnimationDettach(int animNo);

	// �A�j���[�V����
	void Animation(void);

	// �A�j���[�V�����J��
	void ChangingAnimation(void);
	void ChangingAnimationFrame(int index);

	void CheckDettach(void);

	// �A�j���[�V�����ɂ����W�ړ�������
	void InvalidAnimMove(void);

};

