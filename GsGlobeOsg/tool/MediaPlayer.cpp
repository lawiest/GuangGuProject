#include "MediaPlayer.h"
 #include "windows.h"

#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

CMediaPlayer::CMediaPlayer(void)
{
}

CMediaPlayer::~CMediaPlayer(void)
{
	//释放内存
	map<string, string>::const_iterator it = m_mapMediaSource.begin();
	for (; it!=m_mapMediaSource.end(); it++)
	{
		string sCmd = "close " + it->first;
		mciSendStringA(sCmd.c_str(), NULL, 0, 0);
	}
	m_mapMediaSource.clear();
}

void CMediaPlayer::AddSource(string sKey, string MediaFile)
{
	string sCmd = "open " + MediaFile + " alias " + sKey;
	MCIERROR mciError = mciSendStringA(sCmd.c_str(), NULL, 0, 0);
	if (mciError == 0)
		m_mapMediaSource.insert(make_pair(sKey, MediaFile));
}

bool CMediaPlayer::PlayMedia(string strKey)
{
	map<string, string>::const_iterator it = m_mapMediaSource.find(strKey);
	if (it == m_mapMediaSource.end())
		return false;

	string sCmd = "play " + strKey;
	mciSendStringA(sCmd.c_str(), NULL, 0, 0);
	return true;
}

bool CMediaPlayer::RemoveMedia(string strKey)
{
	map<string, string>::const_iterator it = m_mapMediaSource.find(strKey);
	if (it == m_mapMediaSource.end())
		return false;

	//执行close后释放内存
	string sCmd = "close " + strKey;
	mciSendStringA(sCmd.c_str(), NULL, 0, 0);
	m_mapMediaSource.erase(it);
	return true;
}

bool CMediaPlayer::StopMedia(string strKey)
{
	map<string, string>::const_iterator it = m_mapMediaSource.find(strKey);
	if (it == m_mapMediaSource.end())
		return false;

	//执行close后释放内存
	string sCmd = "stop " + strKey;
	mciSendStringA(sCmd.c_str(), NULL, 0, 0);
	return true;
}

void CMediaPlayer::DirectPlayMedia(string strFilePath)
{
	string sCmd = "play " + strFilePath;
	mciSendStringA(sCmd.c_str(), NULL, 0, 0);
}
