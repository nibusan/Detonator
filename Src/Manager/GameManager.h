#pragma once
#include <string>
#include "LoadOut.h"
#include "../Object/ObjectID.h"
#include "../UI/Parts/Game/LogMessageArea/UI_LogMessageArea.h"
#include "../Common/Handle/Font/Font.h"
#include "../Common/Count.h"

class NetManager;
class ObjectBase;
class EnemyCreator;

class GameManager
{

public:

	// �V�[�����Ƃ̃^�C�v
	enum class STATE
	{
		NONE,
		sLOBBY,
		mLOBBY,
		sGAME,
		mGAME,
	};

	// �Q�[�����[�h
	enum class MODE
	{
		NONE,
		TIMEATTACK,
		SCOREATTACK,
		HOST,
		CLIENT,
		SETTINGS,
	};

	enum class TEAM
	{
		WOLF,
		EAGLE,
		MAX
	};

	// �v���C���[�̍�
	enum class ALIAS
	{
		NONE,
		MVP,
		KILLER,
		LASTKILL,
		BOMMER,
		UNDEAD,
		HITMAN,
		COMEDIAN,
	};

	struct PLAYER_INFO
	{
		LOADOUT loadOut = { GUN_TYPE::AK47,GRENADE_TYPE::HAND,GRENADE_TYPE::SMOKE };
		int kill = 0;
		int death = 0;
		int bom = 0;
		int headShot = 0;
		int score = 0;
		int magazine = 0;
		TEAM team = TEAM::WOLF;
		GRENADE_TYPE greType = GRENADE_TYPE::HAND;
		int greNum = 0;
		float moveControlPosZ = 0.0f;
		float actionControlPosZ = 0.0f;
		std::string playerName = "�v���C���[";
		ALIAS alias = ALIAS::NONE;
		VECTOR respawnPos = {};
		VECTOR respawnRot = {};
		bool isHide = false;
		void Reset() {
			kill = 0;
			death = 0;
			bom = 0;
			headShot = 0;
			score = 0;
			magazine = 0;
			moveControlPosZ = 0.0f;
			actionControlPosZ = 0.0f;
			playerName = "�v���C���[";
			alias = ALIAS::NONE;
			isHide = false;
		}
		void GameReset() {
			kill = 0;
			death = 0;
			bom = 0;
			headShot = 0;
			score = 0;
			magazine = 0;
			alias = ALIAS::NONE;
			isHide = false;
		}
	};

	// �^�����_���[�W
	struct DamageData {
		int damage_;
		VECTOR pos;
		float time_;
		VECTOR color_;
	};

	static constexpr int MAX_PLAYER = (4);

	//�G�t�F�N�g�̍ő吔
	static constexpr int EFFECT_MAX_NUM = 8000;

	// �^�C���A�b�v����
	static constexpr float GAME_TIME_SCOREATTACK = (60.0f);
	static constexpr float GAME_TIME_TDM = (180.0f);

	// �Q�[���J�n�܂ł̃J�E���g�_�E������
	static constexpr float TIME_TO_START = 5.0f;

	// �Q�[���I���ҋ@����
	static constexpr float TIME_TO_END = 1.0f;

	// TDM�ڕW�X�R�A
	static constexpr int TDM_GOAL_SCORE[3] = { 5,8,12 };

	// �̍�
	static constexpr int INFO_MAX = 5;

	// ���X�|�[���ʒu
	static constexpr VECTOR SINGLE_RESPAWN_POS = { -1500.0f,0.0f,-3100.0f };
	static constexpr VECTOR MULTI_RESPAWN_POS_WOLF[2] = { { -1500.0f,0.0f,-3100.0f },{ 800.0f,0.0f,-3100.0f } };
	static constexpr VECTOR MULTI_RESPAWN_POS_EAGLE[2] = { { 800.0f,0.0f,2600.0f },{ -1500.0f,0.0f,2600.0f} };

	// ���X�|�[������
	static constexpr VECTOR SINGLE_RESPAWN_ROT = { 0.0f,DX_PI_F * 45.0f / 180.0f,0.0f };
	static constexpr VECTOR MULTI_RESPAWN_ROT_WOLF[2] = { { 0.0f,DX_PI_F * 45.0f / 180.0f,0.0f },
														  { 0.0f,DX_PI_F * -45.0f / 180.0f,0.0f } };
	static constexpr VECTOR MULTI_RESPAWN_ROT_EAGLE[2] = { { 0.0f,DX_PI_F * -135.0f / 180.0f,0.0f },
														   { 0.0f,DX_PI_F * 135.0f / 180.0f,0.0f } };

	// TDM���ʒu����
	static constexpr float WOLF_CONTROL_POS_Z = 1980.0f;
	static constexpr float EAGLE_CONTROL_POS_Z = -2470.0f;

	// �C���X�^���X�̐���
	static void CreateInstance(void);

	// �C���X�^���X�̎擾
	static GameManager& GetInstance(void);

	void Init(void);
	void Update(void);
	void NetUpdate(void);

	// ���\�[�X�̔j��
	void Destroy(void);

	const float GetDPI(void) const;
	void SetDPI(float dpi);

	// �X�R�A
	const int GetScore(int no)const;
	void AddScore(unsigned int score, int no);

	// �{��
	void AddBom(int no);

	// �w�b�h�V���b�g
	void AddHeadShot(int no);

	// ���S
	void AddDead(int no);
	int GetDead(int no);

	// �e��
	void SetMagazine(int no, int magazine);
	int GetMagazine(int no);

	// �L�����擾
	const int GetKill(int no)const;

	// �O���l�[�h
	const GRENADE_TYPE GetGrenadeType(int no,int slot)const;

	// �O���l�[�h��
	int GetGrenadeNum(int no) const;
	void SetGrenadeNum(int no,int num);

	void SetName(int no, std::string name);

	const std::string GetPlayerName(int no)const;
	const std::string GetName(ID id)const;
	const TEAM GetTeam(int no)const;

	const bool IsHide(int no)const;
	void SetIsHide(int no , bool isHide);

	const GRENADE_TYPE GetNowGreType(int no)const;
	void SetGreType(int no, GRENADE_TYPE type);
	
	// �c�莞�Ԏ擾
	const float GetGameTime(void)const;

	// �����������Ԏ擾
	const float GetAttackTime(void)const;

	// �Q�[���J�n���Ԏ擾
	const float GetTimeToStart(void)const;

	/// <summary>
	/// �L��������
	/// </summary>
	/// <param name="die">���񂾂��</param>
	/// <param name="killed">�E�������</param>
	void Kill(ID die, ID killed);

	// �E���Ă�����̖��O
	std::string GetKillerName(void);
	std::string GetDieName(void);

	// ���j��
	int GetEliminationNum(ID id);
	void AddElimination(ID id);

	// �V�[���擾
	STATE GetState(void);

	// �V�[���̕ύX
	void ChangeState(STATE state);

	// �Q�[�����[�h�擾
	MODE GetMode(void);

	// �Q�[�����[�h�̕ύX
	void ChangeMode(MODE mode);

	// ����^�C�v�擾
	void SetLoadOut(int no, LOADOUT loadOut);
	LOADOUT GetLoadOut(int no);

	void SetTeam(int no, TEAM team);

	// �}���`�̃v���C���[���擾
	PLAYER_INFO& GetPlayerInfo(int no);

	// �����`�[���擾
	const TEAM& GetWinner(void)const;

	const bool WinPlayer(void)const;

	const int& GetTDMScore(TEAM team)const;
	int GetGoalTDMScore(void)const;

	// �}���`�̃v���C���[���ύX
	void ChangePlayerInfo(PLAYER_INFO info, int no);

	// �`�[�����ϓ��ɕ�����Ă��邩�ǂ���
	bool IsDivideTeam(void);

	// �Q�[�����p���\���ǂ���
	void SustainableGame(void);

	// �Q�[���X�^�[�g�������ǂ���
	bool IsStart(void);

	bool IsEnd(void);

	bool IsEndGame(void);

	IPDATA GetHostIP(void);
	void SetHostIP(IPDATA ip);

	std::string GetName(void);
	void SetName(std::string name);

	void ResetPlayerInfo(int no);

	// �f�o�b�O
	void DrawDebug(void);

	void SetEnemyFollow(std::weak_ptr<ObjectBase> follow);
	void DrawEnemy(void);

	void UpdateEnemy();

	bool IsActive(void);
	void SetActive(bool isActive);

	// ���
	void ReleseEnemy(void);

	void Back(void);

	void AddDamageData(DamageData data);
	std::vector<DamageData>& GetDamageData(void);

	// ���O���b�Z�[�W���i�[
	void SetNormalLogMessage(const std::string& message, unsigned int color);
	void SetKillLogMessage(const std::string& by, unsigned int by_color, const std::string& killed, unsigned int killed_color);

	// ���O���b�Z�[�W���擾 & ���Z�b�g
	const std::shared_ptr<UI_LogMessageArea::MESSAGE_DATA_BASE> GetLogMessageData(void);

	void DrawKillMessage(void);
	void DrawHelpPessage(void);

private:

	int imgHelp_;
	int imgHelpClose_;
	bool helpActive_;

	bool isActive_;

	// �ÓI�C���X�^���X
	static GameManager* instance_;

	NetManager& netManager_;

	std::weak_ptr<ObjectBase> enemyFollow_;
	std::shared_ptr<EnemyCreator> enemyCreator_;

	std::vector<std::shared_ptr<ObjectBase>> enemys_;

	int remainingTime_;			//�Q�[���̎c�莞��
	//PLAYER_INFO winnerPlayer_;	//���������v���C���[�̏��

	// �v���C���[���
	PLAYER_INFO playerInfo_[MAX_PLAYER];

	// �V�[�����Ƃ̃^�C�v
	STATE state_;

	// �Q�[�����[�h
	MODE mode_;

	// �}�E�X���x
	float dpi_;

	//// ����̃^�C�v
	//LoadOut loadOut_;

	// �Ō�ɃL������ID
	//ID ID_;

	//// �X�R�A
	//int score_;

	//// �L����
	//int kill_;

	// �^�����_���[�W�̃f�[�^
	std::vector<DamageData> damages_;

	// �^�C���A�b�v����
	float gameTime_;

	// �G��|������܂ł̎���
	float attackTime_;

	// �Q�[���J�n�܂ł̎���
	float timeToStart_;
	float timeToEnd_;

	// TDM�ڕW�X�R�A
	int goalScore_;

	// TEAM�X�R�A
	int tdmScore_[(int)TEAM::MAX];

	TEAM winTeam_;

	// �Ō�ɃL�������l
	int finishKill_;

	// �����̍�����
	int cnt_[MAX_PLAYER];
	int idx_;

	bool isChanging_;

	// ���j��
	int eliminationNum_[(int)ID::ENEMY_GEORGE + 1];

	// �ۑ��p�z�X�gIP�A�h���X
	IPDATA hostIp_;
	std::string name_;

	// �Q�[�����ɏo�Ă���L�����O�Ȃǂ��ꎞ�I�Ɋi�[
	std::shared_ptr<UI_LogMessageArea::MESSAGE_DATA_BASE> logMessageData_;

	//�@�L�����Ă������ID
	ID killerID_;
	//�@�L���������ID
	ID dieID_;

	Count killMessage_;
	std::unique_ptr<Font> fontKill_;
	std::unique_ptr<Font> fontDPI_;
	int killAlpha_;

	// �f�t�H���g�R���X�g���N�^��private�ɂ��āA
	// �O�����琶���ł��Ȃ��l�ɂ���
	GameManager(void);

	// �R�s�[�R���X�g���N�^�����l
	GameManager(const GameManager&);

	// �f�X�g���N�^�����l
	~GameManager(void);

	// ������
	// �V���O�����r�[
	void sLobbyInit(void);

	// �}���`���r�[
	void mLobbyInit(void);

	// �V���O���Q�[��
	void sGameInit(void);

	// �}���`�Q�[��
	void mGameInit(void);

	// �̍��t�^
	void Alias(void);
	void SetAlias(int idx, int no);
	void SetMVP(void);


	void ChangeDPI(void);

	// �X�V����
	// �V���O�����r�[
	void sLobbyUpdate(void);

	// �}���`���r�[
	void mLobbyUpdate(void);

	// �V���O���Q�[��
	void sGameUpdate(void);

	// �}���`�Q�[��
	void mGameUpdate(void);



};