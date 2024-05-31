#include "UIBase.h"
#include "UIUtility.h"

void UIUtility::InitBaseUI(
	std::shared_ptr<UIBase>& ui,
	Vector2<float> pos, 
	Vector2<int> size, 
	std::shared_ptr<Collision2D> collision,
	UIBase::DRAW_POSITION_TYPE positionType,
	std::function<void(void)> onClick,
	bool isActive
){
	ui->Init();
	ui->CreateDrawScreen(size);
	ui->SetPos(pos);
	ui->SetTotalPos(pos);
	ui->SetAlpha(1.0f);
	ui->SetScale(1.0f);
	ui->SetAngle(0.0f);
	ui->SetIsActive(true);
	ui->SetIsClickable(true);
	ui->SetDrawPositionType(positionType);
	ui->CreateCollisionData(positionType, collision);
	ui->SetIsActive(isActive);
}
