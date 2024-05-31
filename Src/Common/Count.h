#pragma once

struct Count
{
	float cnt_ = 0.0f;
	float max_ = 0.0f;
	float sub_ = 0.0f;
	bool isStop_ = false;

	void Init(float max, float sub)
	{
		cnt_ = 0.0f;
		max_ = max;
		sub_ = sub;
		isStop_ = false;
	}

	// �J�E���g���I�����Ă����true��Ԃ�
	bool Update()
	{
		if (isStop_) return false;

		if (cnt_ <= 0.0f) return true;

		cnt_ -= sub_;
		if (cnt_ < 0.0f) cnt_ = 0.0f;
		return false;
	}

	void Set()
	{
		cnt_ = max_;
		isStop_ = false;
	}

	bool IsCount()
	{
		return(cnt_ > 0.0f);
	}

	// �J�E���g���X�g�b�v������
	void Stop() 
	{
		isStop_ = true;
	}

	// �J�E���g���ĊJ
	void Start()
	{
		isStop_ = false;
	}
};
