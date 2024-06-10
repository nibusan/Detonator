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

	/// @brief UILayer���v�b�V������
	/// @param uiLayer �Ώۂ�UILayer
	void PushUILayer(std::unique_ptr<UILayerBase> uiLayer);

	/// @brief UILayer���|�b�v����
	void PopUILayer(void);

	/// @brief UILayer�̐���Ԃ� 
	/// @return UILayer�̐�
	int GetUILayerCount(void) const;

	/// @brief �擪��UILayer���A�N�e�B�u�ɂ��邩���Z�b�g����
	/// @param flag �A�N�e�B�u�ɂ��邩
	void SetIsActive_FrontUILayer(bool flag);

	/// @brief UILayer���폜���� 
	void DeleteUILayer(void);
private:
	// �Ǘ�����UILayer(�L���[�̂悤�Ɉ����Ă܂�)
	std::vector<std::unique_ptr<UILayerBase>> uiLayers_;

	// �폜����UILayer��ID
	std::vector<int> deleteUILayerID_;

	// �폜�����
	int deleteCount_;
};