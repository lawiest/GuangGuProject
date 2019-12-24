#pragma once

//#include <phonon/phonon>
//#include <qurl.h>
#include <string>
#include <map>

using namespace std;
//using namespace Phonon;

class CMediaPlayer
{
public:
	CMediaPlayer(void);
	virtual ~CMediaPlayer(void);

	void AddSource(string sKey, string MediaFile);
	
	//播放
	bool PlayMedia(string strKey);
	//停止
	bool StopMedia(string strKey);
	//移除
	bool RemoveMedia(string strKey);
	//直接播放
	void DirectPlayMedia(string strFilePath);

protected:
	map<string, string> m_mapMediaSource;
};

