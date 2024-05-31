#include "../../Manager/SceneManager.h"
#include "HandleBase.h"

HandleBase::HandleBase(void) :
handle_(-1),
isAutoDeleteHandle_(false) {

}

HandleBase::~HandleBase() {

}

int HandleBase::GetHandle(void) const {
	return handle_;
}

void HandleBase::SetHandle(int handle) {
	handle_ = handle;
}

bool HandleBase::IsAutoDeleteHandle(void) const {
	return isAutoDeleteHandle_;
}

void HandleBase::SetIsAutoDeleteHandle(bool flag) {
	isAutoDeleteHandle_ = flag;
}
