#pragma once
#include <memory>
#include <vector>
#include "../../Common/Handle/Graphic/Graphic.h"
#include "../../Data/UI/UILayerList.h"

class UIBase;
class UILayerBase {
public:
	UILayerBase(void);
	virtual ~UILayerBase(void) = default;

	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	/// @brief UI��ǉ�
	/// @param ui UI
	void AddUI(std::shared_ptr<UIBase> ui);

	bool IsActive(void) const;
	void SetIsActive(bool flag);
protected:
	// UI���i�[
	std::vector<std::shared_ptr<UIBase>> uis_;

	// �A�N�e�B�u��Ԃ��ǂ���
	bool isActive_;

	// �`��p�X�N���[��
	Unique_Graphic drawScreen_;

	void UpdateUI(std::shared_ptr<UIBase> ui);
	void DrawUI(std::shared_ptr<UIBase> ui);
	void ReleaseUI(std::shared_ptr<UIBase> ui);
	virtual void SetDefaultUI(void) = 0;

	// UI�̐�������
	std::shared_ptr<UIBase> CreateUI(std::shared_ptr<UILayerList::DEFAULT_UI_DATA> uiData);
};

