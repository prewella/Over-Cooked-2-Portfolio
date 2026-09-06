#pragma once
#include "Base.h"

#define SOUND_MAX 1.0f
#define SOUND_MIN 0.0f
#define SOUND_DEFAULT 0.5f
#define SOUND_WEIGHT 0.1f

BEGIN(Engine)

class CSound_Manager final : public CBase
{
public:
	enum CHANNELID
	{
		BGM,
		/* COMMON LEVEL */
		CH_LEVEL_TRANSITION,

		/* MAINMENU LEVEL*/
		CH_VAN, CH_UI_HIGHLIGHT, CH_UI_BUTTON, CH_UI_SELECT, CH_UI_START,

		/* GAMEPLAY LEVEL */
		CH_STAGE_TRANSITION,
		CH_AUTOMOBILE, CH_CRASH,
		CH_CHEF_CHOP, CH_CHEF_INTERACT, CH_CHEF_STEP, CH_CHEF_DASH, CH_CHEF_SLIP,
		CH_PLATE, CH_STOVE, CH_COOKER, CH_PASS, CH_SINK, CH_SINK_PLATE, CH_WASTEBIN,
		CH_SCORE, CH_TIMER,
		/* RESULT LEVEL */
		CH_RESULT_STAR, CH_RESULT_SCORE,

		MAXCHANNEL
	};

private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Initialize();

public:
	int  VolumeUp(CHANNELID eID, _float _vol);
	int  VolumeDown(CHANNELID eID, _float _vol);
	int  BGMVolumeUp(_float _vol);
	int  BGMVolumeDown(_float _vol);
	int  Pause(CHANNELID eID);
	void Play_Sound(TCHAR* pSoundKey, CHANNELID eID, _float _vol);
	void Play_BGM(TCHAR* pSoundKey);
	void StopSound(CHANNELID eID);
	void StopAll();

private:
	float m_volume = SOUND_DEFAULT;
	float m_BGMvolume = SOUND_DEFAULT;
	FMOD_BOOL m_bool;

private:
	void LoadSoundFile();

private:
	// 사운드 리소스 정보를 갖는 객체 
	map<TCHAR*, FMOD_SOUND*> m_mapSound;
	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];
	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* m_pSystem;
	_bool		m_bPause = false;

public:
	static CSound_Manager* Create();
	virtual void Free() override;
};

END