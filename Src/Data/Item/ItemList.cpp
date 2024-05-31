#include "ItemList.h"

ItemList* ItemList::instance_ = nullptr;

ItemList::ItemList(void)
{
	
}

ItemList::ItemList(const ItemList& ins)
{

}

ItemList::~ItemList(void)
{
	delete instance_;
}

void ItemList::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new ItemList();
	}
}

ItemList& ItemList::GetInstance(void)
{
	return *instance_;
}

void ItemList::Init(void)
{

}

const ItemList::ITEM_DATA& ItemList::GetItemData(ITEM_CATEGORY category, int id) const
{
	return itemList_[static_cast<int>(category)].at(id);
}
