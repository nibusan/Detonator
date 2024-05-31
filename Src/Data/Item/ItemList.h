#pragma once
#include <array>
#include <map>
#include <string>

class ItemList
{

public:

	//�A�C�e���̃J�e�S��
	enum ITEM_CATEGORY 
	{
		GUN = 0,	//�e
		BULLET,		//�e��
		BOMB,		//�{��
		MAX
	};

	//�A�C�e���̃f�[�^
	struct ITEM_DATA 
	{
		ITEM_CATEGORY category;	//�J�e�S��ID
		int id;	//�A�C�e��ID
		std::wstring name;	//�A�C�e���̖��O
		int maxStackNum;	//�X�^�b�N�ł���ő吔
	};

	// �����I�ɃC���X�^���X�𐶐�����
	static void CreateInstance(void);

	// �ÓI�C���X�^���X�̎擾
	static ItemList& GetInstance(void);

	// ����������
	void Init(void);

	/// <summary>
	/// �w�肳�ꂽ�A�C�e���̃f�[�^��Ԃ�
	/// </summary>
	/// <param name="category"> �J�e�S��ID </param>
	/// <param name="id"> �A�C�e��ID </param>
	/// <returns> �A�C�e���̃f�[�^ </returns>
	[[nodiscard]]
	const ITEM_DATA& GetItemData(ITEM_CATEGORY category, int id) const;

private:
	// �ÓI�C���X�^���X
	static ItemList* instance_;

	// �A�C�e�����X�g
	std::array<std::map<int, ITEM_DATA>, static_cast<int>(ITEM_CATEGORY::MAX)> itemList_;

	// �R���X�g���N�^
	ItemList(void);

	// �R�s�[�R���X�g���N�^
	ItemList(const ItemList& ins);

	// �f�X�g���N�^
	~ItemList(void);
};

