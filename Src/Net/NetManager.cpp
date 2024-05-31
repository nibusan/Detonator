#include <vector>
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/GameManager.h"
#include "NetBase.h"
#include "NetHost.h"
#include "NetClient.h"
#include "NetSend.h"
#include "NetManager.h"

using namespace std;

NetManager* NetManager::instance_ = nullptr;

void NetManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new NetManager();
	}
	instance_->Init();
}

NetManager& NetManager::GetInstance(void)
{
	return *instance_;
}

void NetManager::Init(void)
{
}

void NetManager::Destroy(void)
{
	instance_->Stop();
	delete instance_;
}

IPDATA NetManager::GetHostIp(void) const
{
	return hostIp_;
}

void NetManager::SetHostIp(IPDATA hostIp)
{
	hostIp_ = hostIp;
}

int NetManager::GetPlayerNum(void)
{
	return pool_.joinUsers_.size();
}

std::string& NetManager::GetPlayerName(void)
{
	std::string name;
	
	for (auto s : pool_.selfJoinUser_.playerName) {
		name.push_back(s);
	}
	return name;
}

int NetManager::GetAlias(void)
{
	return pool_.selfJoinUser_.alias;
}

int NetManager::GetTeam(void)
{
	return pool_.selfJoinUser_.team;
}

void NetManager::SetPlayerName(std::string name)
{
	int i = 0;
	for (auto s : name) {
		pool_.selfJoinUser_.playerName[i] = s;
		i++;
	}
}

void NetManager::SetAlias(int alias)
{
	pool_.selfJoinUser_.alias = alias;
}

void NetManager::SetTeam(int team)
{
	pool_.selfJoinUser_.team = team;
}

void NetManager::SetLoadOut(int gun, int gre1, int gre2)
{
	pool_.selfJoinUser_.gun = gun;
	pool_.selfJoinUser_.grenade[0] = gre1;
	pool_.selfJoinUser_.grenade[1] = gre2;
}

bool NetManager::IsDead(int key)
{
	return pool_.joinUserDeath_.find(key)->second.isDead;
}

void NetManager::SetDead(bool isDead)
{
	pool_.selfDead_.isDead = isDead;
}

NET_MODE NetManager::GetMode(void) const
{
	return mode_;
}

bool NetManager::IsHost(void) const
{
	return mode_ == NET_MODE::HOST;
}

bool NetManager::IsRunning(void) const
{
	return isRunning_;
}

void NetManager::Run(NET_MODE mode)
{
	mode_ = mode;

	// �ʐM�J�n
	isRunning_ = true;

	// Run�ł�邱��
	// �z�X�g�N���C�A���g���Ƃ̏�����
	// �|�[�g�ԍ�
	// �������g�̃��[�U�[���ݒ�
	// �\�P�b�g(����M)�̍쐬
	// �e���[�U�����i�[����pool��
	// �������g���i�[
	// ��M�p�̃X���b�h�����グ
	// ���M�p�N���X�̍쐬
	// 

	// ��U���S�����ō쐬
	// �Q�[���V�[���ȊO�͊��S�����łȂ���OK

	// �z�X�g�ƃN���C�A���g�ňႤ����
	// �|�[�g�ԍ�
	// �l�b�g���[�N��Ԃ��Ƃ̏������s���N���X
	// CONNECTING , GAME , RESULT ��

	int port = 0;
	switch (mode_)
	{
	case NET_MODE::HOST:

		net_ = make_unique<NetHost>(*instance_);
		port = RECEIVE_PORT_HOST;

		break;
	case NET_MODE::CLIENT:

		net_ = make_unique<NetClient>(*instance_);
		port = RECEIVE_PORT_CLIENT_RANGE + GetRand(899);

		break;
	}

	// �����̃l�b�g���[�U���
	auto& self = poolShare_.selfJoinUser_;
	self.mode = mode_;				// �z�X�gor�N���C�A���g
	self.key = GetRand(999999);		// �C�ӂ̃L�[�������_������
	GetMyIPAddress(&self.ip);		// ���炭�����̃A�h���X�����Ă����
	self.port = port;				// �|�[�g
	self.gameState = GAME_STATE::NONE;	// �i�s��

	if (mode_ == NET_MODE::HOST) {
		hostIp_ = self.ip;
		poolShare_.hostJoinUser_ = poolShare_.selfJoinUser_;
	}

	// UDP���g�p�����ʐM���s���\�P�b�g�n���h�����쐬����
	// ����ۂ̃\�P�b�g�Ǝ󂯎��ۂ̃\�P�b�g
	sendSocketId_ = MakeUDPSocket(-1);
	recvSocketId_ = MakeUDPSocket(port);

	// �����̃A�N�V�������
	poolShare_.selfAction_.key = self.key;
	poolShare_.selfActionHis_.key = self.key;

	// �������g���v���C���[�ɒǉ�����
	poolShare_.joinUsers_.emplace(self.key, self);

	hostTime_ = 0.0f;

	// �f�[�^�̃R�s�[
	pool_ = poolShare_;	// �H

	// ��M�������s���X���b�h���N������
	recvThread_ = thread(&NetManager::UdpReceiveThread, this);

	// ���M�p�N���X�̍쐬
	netSend_ = make_unique<NetSend>(*instance_, sendSocketId_);

	frameNo_ = 0;
	// �����t���O(�Q�[�����̂�true)
	isSync_ = false;
	hostIsNotFound_ = false;

	isFirstReceive_ = false;
}

void NetManager::Update(void)
{
	if (!isRunning_)
	{
		// Run�����s�����܂ŏ������Ȃ�
		return;
	}

	if (isSync_)
	{

		// TODO ���S�����ɂȂ��Ă���̂ŁA������x�񓯊��ł����s�ł���悤�ɂ�����
		while (ProcessMessage() == 0)
		{
			// �ʐM�f�[�^���f
			DataReflection();

			// ���Ԍo��
			UpdateTime();

			// �S���[�U�̍ŐV�t���[���������܂ő҂�
			if (IsSameFrameNo()/* && IsReceiveAll()*/)
			{
				//ResetReceiveAll();
				break;
			}

			// ��ԕʍX�V(��ɑ��M����)
			UpdateState();

			// ��莞�ԑҋ@
			WaitTimer(static_cast<int>(SYNC_TERM_MSEC));
		}

	}
	else
	{
		// �ʐM�f�[�^���f
		DataReflection();

		// ���Ԍo��
		UpdateTime();
	}

	// ��ԕʍX�V(��ɑ��M����)
	UpdateState();


}

void NetManager::DataReflection(void)
{
	// �r������
	// ����Ă邱�Ƃ͂Ȃ�ƂȂ��킩�邪�A�Ȃ������Ȃ�̂��킩���
	std::lock_guard<std::mutex> lock(poolMutex_);

	// �������g�����X�g�X�V
	// �ŗL�f�[�^���狤�L�f�[�^�փR�s�[
	pool_.selfJoinUser_.playerType = poolShare_.selfJoinUser_.playerType;

	// �������g�D��
	poolShare_.selfJoinUser_ = pool_.selfJoinUser_;
	poolShare_.selfAction_ = pool_.selfAction_;
	poolShare_.selfAction_.frameNo = frameNo_;
	poolShare_.selfActionHis_ = pool_.selfActionHis_;

	// �����D��
	ReplaceActionHis(poolShare_.selfActionHis_);

	if (IsHost())
	{
		// ���[���@�\�̓z�X�g�f�[�^�𐳂Ƃ���
		ReplaceUser(poolShare_.selfJoinUser_);
	}
	else
	{
		poolShare_.selfJoinUser_.gameMode = pool_.hostJoinUser_.gameMode;
		if (pool_.selfJoinUser_.gameState == GAME_STATE::GOTO_GAME) {
			poolShare_.selfJoinUser_.team = pool_.joinUsers_.at(pool_.selfJoinUser_.key).team;
		}
	}

	// �f�[�^�̃R�s�[
	pool_ = poolShare_;
}

void NetManager::UpdateEndOfFrame(void)
{
	if (isSync_)
	{
		// �������̃t���[���i�s
		NextFrame();
	}
}

void NetManager::UpdateTime(void)
{

	if (IsHost())
	{
		std::vector<int> keys;

		if (joinUserTime_.size() <= 0) return;
		for (auto& user : joinUserTime_){
			user.second += 1.0f / 60.0f;
			if (user.second >= RECEIVE_TIME)
			{
				keys.emplace_back(user.first);
			}
		}

		//if (keys.size() > 0) {
		//	// �v���C���[�ԍ����l�߂�
		//	int i = 0;
		//	for (auto& user : pool_.joinUsers_) {
		//		user.second.playerType = (PLAYER_TYPE)i;
		//		i++;
		//	}
		//}

		for (auto key : keys) {


			if (pool_.joinUsers_.find(key) != pool_.joinUsers_.end()) {
				GameManager::GetInstance().ResetPlayerInfo((int)pool_.joinUsers_.at(key).playerType);
			}

			pool_.joinUserActionHis_.erase(key);
			pool_.joinUsers_.erase(key);
			// �r������
			std::lock_guard<std::mutex> lock(poolMutex_);

			poolShare_.joinUserActionHis_.erase(key);
			poolShare_.joinUsers_.erase(key);

			// ���[�U�������[�U�ɑ��M
			Send(NET_DATA_TYPE::USERS);

		}

		std::erase_if(joinUserTime_, [](const auto& x) { return x.second >= RECEIVE_TIME * 10.0f; });


	}
	else
	{
		hostTime_ += 1.0f / 60.0f;
		if (hostTime_ >= RECEIVE_TIME)
		{
			
			int key = pool_.hostJoinUser_.key;

			if (key != -1) {
				// �z�X�g�Ƃ̐ڑ����؂ꂽ
				pool_.joinUserActionHis_.erase(key);
				pool_.joinUsers_.erase(key);
				pool_.hostJoinUser_.Reset();
				// �r������
				std::lock_guard<std::mutex> lock(poolMutex_);

				poolShare_.joinUserActionHis_.erase(key);
				poolShare_.joinUsers_.erase(key);
				poolShare_.hostJoinUser_.Reset();
				
			}
			else {
				// �z�X�g��������܂���ł���
				hostIsNotFound_ = true;

				pool_.joinUsers_.clear();
				pool_.joinUserActionHis_.clear();
				pool_.joinUserDeath_.clear();
				pool_.hostJoinUser_.Reset();

				// �r������
				std::lock_guard<std::mutex> lock(poolMutex_);
				poolShare_.joinUsers_.clear();
				poolShare_.joinUserActionHis_.clear();
				poolShare_.joinUserDeath_.clear();
				poolShare_.hostJoinUser_.Reset();
			}


		}
	}
}

void NetManager::NextFrame(void)
{
	frameNo_++;
}

void NetManager::Stop(void)
{
	DeleteUDPSocket(sendSocketId_);
	DeleteUDPSocket(recvSocketId_);

	if (isRunning_) {

		isRunning_ = false;

		if (recvThread_.joinable()) {	// 	�X���b�h�Ɋ֘A�t�����Ă��邩�ۂ����擾���� �Ȃɂ���
			recvThread_.join();		// �X���b�h���I������܂őҋ@����
		}

	}

	LOADOUT loadout;
	loadout.gunType_ = (GUN_TYPE)pool_.selfJoinUser_.gun;
	loadout.greType_[0] = (GRENADE_TYPE)pool_.selfJoinUser_.grenade[0];
	loadout.greType_[1] = (GRENADE_TYPE)pool_.selfJoinUser_.grenade[1];
	GameManager::GetInstance().SetLoadOut(0, loadout);
	GameManager::GetInstance().SetTeam(0, GameManager::TEAM::WOLF);
	GameManager::GetInstance().SetTeam(1, GameManager::TEAM::WOLF);
	GameManager::GetInstance().SetTeam(2, GameManager::TEAM::WOLF);
	GameManager::GetInstance().SetTeam(3, GameManager::TEAM::WOLF);

	pool_.selfJoinUser_ = {};
	pool_.selfAction_ = {};
	pool_.selfActionHis_ = {};
	pool_.selfDead_ = {};
	pool_.joinUsers_.clear();
	pool_.joinUserActionHis_.clear();
	pool_.joinUserDeath_.clear();
	pool_.hostJoinUser_.Reset();

	// �r������
	std::lock_guard<std::mutex> lock(poolMutex_);
	poolShare_.selfJoinUser_ = {};
	poolShare_.selfAction_ = {};
	poolShare_.selfActionHis_ = {};
	poolShare_.selfDead_ = {};
	poolShare_.joinUsers_.clear();
	poolShare_.joinUserActionHis_.clear();
	poolShare_.joinUserDeath_.clear();
	poolShare_.hostJoinUser_.Reset();

	joinUserTime_.clear();
	hostTime_ = 0.0f;

	isSync_ = false;
	hostIsNotFound_ = false;
}

void NetManager::Send(NET_DATA_TYPE type)
{
	netSend_->Send(type);
}

void NetManager::ChangeGameState(GAME_STATE state)
{
	if (pool_.selfJoinUser_.gameState == state) return;

	pool_.selfJoinUser_.gameState = state;

	// �e��Ԃ��Ƃ̏�����
	switch (state)
	{
	case GAME_STATE::NONE:
		break;
		//case GAME_STATE::CONNECTING:
		//	break;
	case GAME_STATE::LOBBY:
		frameNo_ = 0;
		isSync_ = false;
		for (auto& action : pool_.selfActionHis_.actions) {
			action.Reset();
		}
		break;
	case GAME_STATE::GAME_PLAYING:
		frameNo_ = 1;
		isSync_ = true;	// �����J�n(�Q�[���J�n)
		pool_.selfAction_.frameNo = frameNo_;
		break;
	case GAME_STATE::RESULT:
		frameNo_ = 0;
		isSync_ = false;
		for (auto& action : pool_.selfActionHis_.actions) {
			action.Reset();
		}
		break;
	}
}

const NET_JOIN_USER NetManager::GetSelf(void) const
{
	return pool_.selfJoinUser_;
}

const std::map<int, NET_JOIN_USER> NetManager::GetNetUsers(void) const
{
	return pool_.joinUsers_;
}

const std::map<int, NET_ACTION_HIS> NetManager::GetNetUserActionHis(void) const
{
	return pool_.joinUserActionHis_;
}

GAME_STATE NetManager::GetGameStateHost(void) const
{
	return pool_.hostJoinUser_.gameState;
}

GAME_STATE NetManager::GetGameStateSelf(void) const
{
	return pool_.selfJoinUser_.gameState;
}

bool NetManager::IsSameGameState(GAME_STATE state)
{
	if (pool_.joinUsers_.size() == 0)
	{
		return false;
	}

	for (auto& user : pool_.joinUsers_)
	{
		if (user.second.gameState != state)
		{
			return false;
		}
	}

	return true;
}

const NET_ACTION NetManager::GetSelfAction(void) const
{
	return pool_.selfAction_;
}

const DEAD NetManager::GetSelfDead(void) const
{
	return pool_.selfDead_;
}

const std::map<int, NET_ACTION_HIS> NetManager::GetActionHis(void) const
{
	return pool_.joinUserActionHis_;
}

const NET_JOIN_USER& NetManager::GetHostJoinUser(void) const
{
	return pool_.hostJoinUser_;
}

bool NetManager::IsAction(int key, PLAYER_ACTION action)
{
	if (pool_.joinUserActionHis_.find(key) != pool_.joinUserActionHis_.end())
	{

		for (const auto& actionHis : pool_.joinUserActionHis_[key].actions)
		{
			if (frameNo_ == actionHis.frameNo)
			{
				return actionHis.actBits & static_cast<int>(action);
			}
		}

	}
	return false;
}

bool NetManager::IsAction(int key, PLAYER_ACTION action, bool isAction)
{
	if (pool_.selfJoinUser_.key == key)
	{
		return isAction;
	}
	return IsAction(key, action);
}

Vector2<float> NetManager::GetRotatePow(int key, Vector2<float> pow)
{
	if (pool_.selfJoinUser_.key == key)
	{
		return pow;
	}
	if (pool_.joinUserActionHis_.find(key) != pool_.joinUserActionHis_.end())
	{

		for (const auto& actionHis : pool_.joinUserActionHis_[key].actions)
		{
			if (frameNo_ == actionHis.frameNo)
			{
				return actionHis.rotPow_;
			}
		}

	}
	return { 0.0f,0.0f };
}

int NetManager::GetRandSeed(int key, int seed)
{
	if (pool_.selfJoinUser_.key == key)
	{
		return seed;
	}
	if (pool_.joinUserActionHis_.find(key) != pool_.joinUserActionHis_.end())
	{

		for (const auto& actionHis : pool_.joinUserActionHis_[key].actions)
		{
			if (frameNo_ == actionHis.frameNo)
			{
				return actionHis.randSeed;
			}
		}

	}
	return 0;
}

//CAMERA_INFO NetManager::GetCameraInfo(int key, CAMERA_INFO info)
//{
//	if (pool_.selfJoinUser_.key == key)
//	{
//		return info;
//	}
//	if (pool_.joinUserActionHis_.find(key) != pool_.joinUserActionHis_.end())
//	{
//
//		for (const auto& actionHis : pool_.joinUserActionHis_[key].actions)
//		{
//			if (frameNo_ == actionHis.frameNo)
//			{
//				return actionHis.cameraInfo_;
//			}
//		}
//
//	}
//	return CAMERA_INFO();
//}

//VECTOR NetManager::GetTargetPos(int key, VECTOR pos)
//{
//	if (pool_.selfJoinUser_.key == key)
//	{
//		return pos;
//	}
//	if (pool_.joinUserActionHis_.find(key) != pool_.joinUserActionHis_.end())
//	{
//		for (const auto& actionHis : pool_.joinUserActionHis_[key].actions)
//		{
//			if (frameNo_ == actionHis.frameNo)
//			{
//				return actionHis.targetPos_;
//			}
//		}
//	}
//	return { 0.0f,0.0f ,0.0f };
//}
//
//VECTOR NetManager::GetBulletDir(int key, VECTOR vec)
//{
//	if (pool_.selfJoinUser_.key == key)
//	{
//		return vec;
//	}
//	if (pool_.joinUserActionHis_.find(key) != pool_.joinUserActionHis_.end())
//	{
//		for (const auto& actionHis : pool_.joinUserActionHis_[key].actions)
//		{
//			if (frameNo_ == actionHis.frameNo)
//			{
//				return actionHis.bulletDir_;
//			}
//		}
//	}
//	return { 0.0f,0.0f ,0.0f };
//}

void NetManager::SetAction(PLAYER_ACTION act)
{
	pool_.selfAction_.actBits |= static_cast<int>(act);
}

void NetManager::SetRotPow(Vector2<float> pow)
{
	pool_.selfAction_.rotPow_ = pow;
}

void NetManager::SetRandSeed(int seed)
{
	pool_.selfAction_.randSeed = seed;
}

//void NetManager::SetCameraInfo(CAMERA_INFO info)
//{
//	pool_.selfAction_.cameraInfo_ = info;
//}

//void NetManager::SetTargetPos(VECTOR pos)
//{
//	pool_.selfAction_.targetPos_ = pos;
//}
//
//void NetManager::SetBulletDir(VECTOR vec)
//{
//	pool_.selfAction_.bulletDir_ = vec;
//}

void NetManager::SetGameMode(int mode)
{
	pool_.selfJoinUser_.gameMode = mode;
}

bool NetManager::IsSameFrameNo(void)
{
	bool ret = false;

	if (pool_.selfJoinUser_.gameState != GAME_STATE::GAME_PLAYING)
	{
		return true;
	}

	if (pool_.joinUserActionHis_.size() != pool_.joinUsers_.size())	// �H
	{
		return false;
	}

	unsigned int frameNo = pool_.selfAction_.frameNo;
	if (frameNo == 0)
	{
		return false;
	}

	int cnt = 0;
	for (const auto& his : pool_.joinUserActionHis_)
	{
		for (const auto& act : his.second.actions)
		{
			if (frameNo == act.frameNo)
			{
				cnt += 1;
			}
		}
	}

	if (cnt == pool_.joinUsers_.size())
	{
		ret = true;
	}

	return ret;
}

bool NetManager::IsReceiveAll(void)
{
	bool ret = true;
	for (const auto& his : pool_.joinUserActionHis_)
	{
		if (his.second.key == GetSelf().key) {
			ret = true;
		}
		else ret = his.second.isReceive;
		if (!ret)break;
	}

	return ret;
}

void NetManager::ResetReceiveAll(void)
{
	for (auto& his : pool_.joinUserActionHis_)
	{
		his.second.isReceive = false;
	}
}

const NET_ACTION_HIS NetManager::GetSelfActionHis(void) const
{
	return pool_.selfActionHis_;
}



int NetManager::GetHostPort(void) const
{
	return RECEIVE_PORT_HOST;
}

void NetManager::ResetAction(void)
{
	pool_.selfAction_.actBits = 0;
	pool_.selfAction_.rotPow_ = { 0.0f,0.0f };
}

unsigned int NetManager::GetFrameNo(void) const
{
	return frameNo_;
}


void NetManager::MakeActionHis(NET_ACTION action)
{
	bool isExists = false;
	for (int i = 0; i < NUM_FRAME; i++)
	{
		if (action.frameNo == pool_.selfActionHis_.actions[i].frameNo)
		{
			return;
		}
	}

	unsigned int min = 4294967295;
	int idx = -1;
	for (int i = 0; i < NUM_FRAME; i++)
	{
		if (min > pool_.selfActionHis_.actions[i].frameNo)
		{
			min = pool_.selfActionHis_.actions[i].frameNo;
			idx = i;
		}
	}

	pool_.selfActionHis_.actions[idx] = action;
}

bool NetManager::IsSync(void)
{
	return isSync_;
}

bool NetManager::HostIsNotFound(void)
{
	return hostIsNotFound_;
}

NetManager::NetManager(void)
{
	isRunning_ = false;
	frameNo_ = 0;
	mode_ = NET_MODE::NONE;
	net_ = nullptr;
	netSend_ = nullptr;
	isRunning_ = false;
	recvSocketId_ = -1;
	sendSocketId_ = -1;
	isSync_ = false;
}

NetManager::NetManager(const NetManager& ins)
{
	isRunning_ = false;
	frameNo_ = 0;
	mode_ = NET_MODE::NONE;
	net_ = nullptr;
	netSend_ = nullptr;
	isRunning_ = false;
	recvSocketId_ = -1;
	sendSocketId_ = -1;
	isSync_ = false;
}

void NetManager::UpdateState(void)
{
	if (hostIsNotFound_) return;

	switch (pool_.selfJoinUser_.gameState)
	{
	//case GAME_STATE::CONNECTING:
	//	net_->UpdateConnecting();
	//	break;
	//case GAME_STATE::GOTO_LOBBY:
	//	net_->UpdateGotoLobby();
	//	break;
	case GAME_STATE::LOBBY:
		net_->UpdateLobby();
		break;
	case GAME_STATE::GOTO_GAME:
		net_->UpdateGotoGame();
		break;
	case GAME_STATE::GAME_PLAYING:
		net_->UpdateGamePlaying();
		break;
	case GAME_STATE::GOTO_RESULT:
		net_->UpdateGotoResult();
		break;
	case GAME_STATE::RESULT:
		net_->UpdateResult();
		break;
	}

}

void NetManager::UdpReceiveThread(void)
{
	while (isRunning_)
	{

		{
			// �r������
			std::lock_guard<std::mutex> lock(poolMutex_);

			// �f�[�^�̎�M
			UdpReceiveData();
		}

		GAME_STATE state = GetGameStateSelf();

		switch (state)
		{
		//case GAME_STATE::CONNECTING:
		//	net_->UdpReceiveThreadConnecting();
		//	break;
		//case GAME_STATE::GOTO_LOBBY:
		//	net_->UdpReceiveThreadGotoLobby();
		//	break;
		case GAME_STATE::LOBBY:
			net_->UdpReceiveThreadLobby();
			break;
		case GAME_STATE::GOTO_GAME:
			net_->UdpReceiveThreadGotoGame();
			break;
		case GAME_STATE::GAME_PLAYING:
			net_->UdpReceiveThreadGamePlaying();
			break;
		case GAME_STATE::GOTO_RESULT:
			net_->UdpReceiveThreadGotoResult();
			break;
		case GAME_STATE::RESULT:
			net_->UdpReceiveThreadResult();
			break;
		}

		// ��M���[�v�̑ҋ@
		WaitTimer(static_cast<int>(NetManager::RECEIVE_TERM_MSEC));

	}
}

void NetManager::UdpReceiveData(void)
{
	const int SIZE_BASIC = sizeof(NET_BASIC_DATA);

	for (int i = 0; i < NetManager::MAX_RECEIVE_NUM; i++)
	{

		char buf[MAX_SEND_BYTES] = { 0 };
		char* bufptr = buf;

		// UDP�f�[�^����M
		int bufSize = NetWorkRecvUDP(
			recvSocketId_, nullptr, nullptr, bufptr, MAX_SEND_BYTES, false);

		if (bufSize == -3)
		{
			// ��M�f�[�^���Ȃ�
			break;
		}
		if (bufSize == -1 || bufSize == -2)
		{
			// -1 �G���[���� �|�Q�F�G���[�����i��M�f�[�^���o�b�t�@�̃T�C�Y�̕����������j
			continue;
		}

		// ��{�f�[�^�T�C�Y�`�F�b�N
		int headSize = SIZE_BASIC;
		if (bufSize >= headSize)
		{

			// ��{�f�[�^���Ɏ擾���ĒʐM��ʂŏ�����U�蕪����
			NET_BASIC_DATA data = GetBasicData(bufptr);
			bufptr += SIZE_BASIC;

			switch (data.type)
			{
				case NET_DATA_TYPE::USER:
				{
					if (bufSize == SIZE_BASIC + sizeof(NET_JOIN_USER))
					{
						NET_JOIN_USER* cast = reinterpret_cast<NET_JOIN_USER*>(bufptr);
						// CRC�`�F�b�N
						//std::uint32_t crc = CRC::Calculate(cast, sizeof(NET_JOIN_USER), CRC::CRC_32());
						//if (crc == data.crc)
						//{
							ReplaceUser(*cast);
						//}
					}
					break;
				}
				case NET_DATA_TYPE::USERS:
				{
					if (bufSize == SIZE_BASIC + sizeof(NET_JOIN_USERS))
					{
						if (!IsHost() && data.key == pool_.hostJoinUser_.key)
						{
							//SceneManager::GetInstance().SetTotalGameTime(data.gameTime);
						}
						NET_JOIN_USERS* cast = reinterpret_cast<NET_JOIN_USERS*>(bufptr);
						// CRC�`�F�b�N
						//std::uint32_t crc = CRC::Calculate(cast, sizeof(NET_JOIN_USERS), CRC::CRC_32());
						//if (crc == data.crc)
						//{
							ReplaceUsers(*cast);
						//}
					}
					break;
				}
				case NET_DATA_TYPE::ACTION_HIS_ALL:
				{
					if (bufSize == SIZE_BASIC + sizeof(NET_ACTION_HIS))
					{
						if (!IsHost() && data.key == pool_.hostJoinUser_.key)
						{
							/*SceneManager::GetInstance().SetTotalGameTime(data.gameTime);*/
						}
						NET_ACTION_HIS* cast = reinterpret_cast<NET_ACTION_HIS*>(bufptr);
						//// CRC�`�F�b�N
						//std::uint32_t crc = CRC::Calculate(cast, sizeof(NET_ACTION_HIS), CRC::CRC_32());
						//if (crc == data.crc)
						//{
							ReplaceActionHis(*cast);
						//}
					}
					break;
				}
			}

		}

	}
}

NET_BASIC_DATA NetManager::GetBasicData(const char* bufptr)
{
	NET_BASIC_DATA ret;
	int size;

	// �ʐM���
	size = sizeof(ret.type);
	unsigned char* pType = reinterpret_cast<unsigned char*>(&ret.type);
	memcpy(pType, bufptr, size);
	// memcpy(&ret.type, bufptr, size);���ꂶ�Ⴞ�߁H
	bufptr += size;	// �|�C���^���T�C�Y���ړ�

	// ���[�U�L�[
	size = sizeof(ret.key);
	unsigned char* pKey = reinterpret_cast<unsigned char*>(&ret.key);
	memcpy(pKey, bufptr, size);
	bufptr += size;

	// �Q�[������
	//size = sizeof(ret.gameTime);
	//unsigned char* pTime = reinterpret_cast<unsigned char*>(&ret.gameTime);
	//memcpy(pTime, bufptr, size);
	//bufptr += size;

	//// CRC
	//size = sizeof(ret.crc);
	//unsigned char* pCRC = reinterpret_cast<unsigned char*>(&ret.crc);
	//memcpy(pCRC, bufptr, size);
	//bufptr += size;

	return ret;
}

void NetManager::ReplaceUser(NET_JOIN_USER entity)
{
	if (poolShare_.joinUsers_.find(entity.key) == poolShare_.joinUsers_.end())
	{
		// �z��ɂȂ��̂Œǉ�
		// ���Ԗڂ̃v���C���[����joinUsers�̃T�C�Y�Ŕ���
		// �T�C�Y0��PLAYER_1 (0)
		// �T�C�Y1��PLAYER_2 (1)

		int i = 0;
		bool isSame = false;
		for (i; i < 3; i++) {
			isSame = false;
			for (auto& user : poolShare_.joinUsers_) {
				if ((int)user.second.playerType == i)isSame = true;
			}
			if (!isSame)break;
		}


		//size_t size = poolShare_.joinUsers_.size();
		//entity.playerType = static_cast<PLAYER_TYPE>(size);
		entity.playerType = static_cast<PLAYER_TYPE>(i);
		poolShare_.joinUsers_.emplace(entity.key, entity);
		LOADOUT loadout;
		loadout.gunType_ = (GUN_TYPE)entity.gun;
		loadout.greType_[0] = (GRENADE_TYPE)entity.grenade[0];
		loadout.greType_[1] = (GRENADE_TYPE)entity.grenade[1];
		GameManager::GetInstance().SetLoadOut(i, loadout);

		if (IsHost())
		{
			if (entity.key != pool_.selfJoinUser_.key)
			{
				joinUserTime_.emplace(entity.key, 0.0f);
			}
			
		}
		
	}
	else
	{
		//// GameState�͖߂�Ȃ��悤��
		//if (poolShare_.joinUsers_[entity.key].gameState <= entity.gameState)
		//{
			// �v���C���[��ʂ̓z�X�g�D��
			PLAYER_TYPE type = poolShare_.joinUsers_[entity.key].playerType;
			entity.playerType = type;
			poolShare_.joinUsers_[entity.key] = entity;
		//}

	}

	if (IsHost() && joinUserTime_.size() != 0)
	{
		if (entity.key != pool_.selfJoinUser_.key){
			joinUserTime_.at(entity.key) = 0.0f;
		}
	}
}

void NetManager::ReplaceUsers(NET_JOIN_USERS entities)
{
	if (!IsHost()) {
		std::vector<int> keys;

		for (const auto& user : poolShare_.joinUsers_) {
			int key = user.second.key;
			if (key == -1)continue;
			bool a = false;
			for (const auto& user2 : entities.users) {
				if (key == user2.key)a = true;
			}
			if (!a)keys.emplace_back(key);
		}
		for (auto key : keys) {
			poolShare_.joinUserActionHis_.erase(key);
			poolShare_.joinUsers_.erase(key);
		}
	}
	 
	poolShare_.joinUsers_.clear();

	for (const auto& user : entities.users)
	{

		if (user.mode == NET_MODE::HOST
			|| user.mode == NET_MODE::CLIENT)
		{
			poolShare_.joinUsers_.emplace(user.key, user);
		}

		if (user.key == poolShare_.selfJoinUser_.key)
		{
			poolShare_.selfJoinUser_ = user;
			if (!isFirstReceive_) {
				LOADOUT loadout;
				loadout.gunType_ = (GUN_TYPE)user.gun;
				loadout.greType_[0] = (GRENADE_TYPE)user.grenade[0];
				loadout.greType_[1] = (GRENADE_TYPE)user.grenade[1];
				GameManager::GetInstance().SetLoadOut((int)user.playerType, loadout);
				isFirstReceive_ = true;
			}
		}

		if (user.mode == NET_MODE::HOST)
		{
			poolShare_.hostJoinUser_ = user;
			hostIp_ = user.ip;
			hostTime_ = 0.0f;
		}

	}

	



}

void NetManager::ReplaceActionHis(NET_ACTION_HIS entity)
{
	auto& map = poolShare_.joinUserActionHis_;
	if (map.find(entity.key) == map.end())
	{
		map.emplace(entity.key, entity);
	}
	else
	{
		map[entity.key] = entity;
	}

	map[entity.key].isReceive = true;

	auto& time = joinUserTime_;
	if (time.find(entity.key) == time.end()){
		if (pool_.hostJoinUser_.key == entity.key){
			hostTime_ = 0.0f;
		}
	}else{
		time.at(entity.key) = 0.0f;
	}
}
