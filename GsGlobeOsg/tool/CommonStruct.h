#pragma once
#include <string>
#include <time.h>

//#ifdef MGDBACCESS_EXPORTS
//#define MGDBACCESS_INTERFACE __declspec(dllexport)
//#else
//#define MGDBACCESS_INTERFACE __declspec(dllimport)
//#endif
namespace GG {

typedef struct  tagGeoPoint{
	double x;
	double y;

	tagGeoPoint(){
		x=1e301;
		y=1e301;
	}
	tagGeoPoint(double inX, double inY){
		x=inX;
		y=inY;
	}
	tagGeoPoint(const tagGeoPoint& gp){
		x=gp.x;
		y=gp.y;
	}
	tagGeoPoint& operator=(const tagGeoPoint& gp){
		x=gp.x;
		y=gp.y;
		return *this;
	}

	void SetInvalid(){
		x=1e301;
		y=1e301;
	}
	bool IsInvalid() const{
		if(x>1e300 || y>1e300) return true;
		return false;
	}
}GeoPoint;

typedef struct  tagGeoBox{
	GeoPoint ll;
	GeoPoint tr;

	tagGeoBox(){
		ll.SetInvalid();
		tr.SetInvalid();
	}
	tagGeoBox(double x1, double y1, double x2, double y2){
		ll.x=x1;
		ll.y=y1;
		tr.x=x2;
		tr.y=y2;
	}
	tagGeoBox(GeoPoint inLL, GeoPoint inTR){
		ll.x=inLL.x;
		ll.y=inLL.y;
		tr.x=inTR.x;
		tr.y=inTR.y;
	}
	tagGeoBox(const tagGeoBox& box){
		ll=box.ll;
		tr=box.tr;
	}
	tagGeoBox& operator=(const tagGeoBox& box){
		ll=box.ll;
		tr=box.tr;
		return *this;
	}

	void SetInvalid(){
		ll.SetInvalid();
		tr.SetInvalid();
	}
	bool IsInvalid() const{
		if(ll.IsInvalid() || tr.IsInvalid()) return true;
		return false;
	}
}GeoBox;

class  DateTime{
public:
	short nYear;//��
	short nMonth; //��
	short nDay; //��
	short nHour; //ʱ
	short nMinute; //��
	short nSecond; //��

	DateTime(){//����Ϊ��Чʱ��
		SetAsInvalid();
	}
	DateTime(short nYear, short nMonth, short nDay, short nHour, short nMinute, short nSecond){
		this->nYear=nYear; this->nMonth=nMonth; this->nDay=nDay; 
		this->nHour=nHour; this->nMinute=nMinute; this->nSecond=nSecond;
	}
	DateTime(const DateTime& dt){
		nYear=dt.nYear; nMonth=dt.nMonth; nDay=dt.nDay; 
		nHour=dt.nHour; nMinute=dt.nMinute; nSecond=dt.nSecond;
	}
	DateTime& operator=(const DateTime& dt){
		nYear=dt.nYear; nMonth=dt.nMonth; nDay=dt.nDay; 
		nHour=dt.nHour; nMinute=dt.nMinute; nSecond=dt.nSecond;
		return *this;
	}

	//������Чʱ��㣬�ɽ���������Ϊ����Χ֮���ֵ
	void SetAsInvalid(){
		nYear=nMonth=nDay=nHour=nMinute=nSecond=-1;
	}
	//������Сʱ���
	void SetAsMinimum(){
		nYear=1900; nMonth=0; nDay=1;
		nHour=0; nMinute=0; nSecond=0;
	}
	//�������ʱ���
	void SetAsMaximum(){
		nYear=2999; nMonth=11; nDay=31;
		nHour=23; nMinute=59; nSecond=59;
	}
	//���õ�ǰʱ���
	void SetAsCurrent(){
		*this=FromCurrentDateTime();
	}

	//�Ƿ���Чʱ���
	bool IsInvalid() const{
		if(nYear<1900 || nYear>=3000) return true;
		if(nMonth<0 || nMonth>11) return true;
		if(nDay<1 || nDay>31) return true;
		if(nHour<0 || nHour>23) return true;
		if(nMinute<0 || nMinute>59) return true;
		if(nSecond<0 || nSecond>59) return true;
		return false;
	}

	//��������ʱ���֮ǰ
	bool IsBefore(const DateTime& dt) const{
		if(nYear>dt.nYear) return false;
		else if(nYear<dt.nYear) return true;

		if(nMonth>dt.nMonth) return false;
		else if(nMonth<dt.nMonth) return true;

		if(nDay>dt.nDay) return false;
		else if(nDay<dt.nDay) return true;

		if(nHour>dt.nHour) return false;
		else if(nHour<dt.nHour) return true;

		if(nMinute>dt.nMinute) return false;
		else if(nMinute<dt.nMinute) return true;

		if(nSecond>dt.nSecond) return false;
		else if(nSecond<dt.nSecond) return true;

		return false;
	}
	//��������ʱ���֮��
	bool IsAfter(const DateTime& dt) const{
		if(nYear<dt.nYear) return false;
		else if(nYear>dt.nYear) return true;

		if(nMonth<dt.nMonth) return false;
		else if(nMonth>dt.nMonth) return true;

		if(nDay<dt.nDay) return false;
		else if(nDay>dt.nDay) return true;

		if(nHour<dt.nHour) return false;
		else if(nHour>dt.nHour) return true;

		if(nMinute<dt.nMinute) return false;
		else if(nMinute>dt.nMinute) return true;

		if(nSecond<dt.nSecond) return false;
		else if(nSecond>dt.nSecond) return true;

		return false;
	}
	//�Ƿ�ͬһʱ��㣬�ɽ��Ƚ�ǰnComBitsλ
	bool IsTheSame(const DateTime& dt, int nComBits=6) const{
		if(nComBits<0 || nComBits>6) nComBits=6;
		int num=nComBits;

		num--;
		if(nYear!=dt.nYear) return false;
		if(0==num) return true;
			
		num--;
		if(nMonth!=dt.nMonth) return false;
		if(0==num) return true;

		num--;
		if(nDay!=dt.nDay) return false;
		if(0==num) return true;

		num--;
		if(nHour!=dt.nHour) return false;
		if(0==num) return true;

		num--;
		if(nMinute!=dt.nMinute) return false;
		if(0==num) return true;

		num--;
		if(nSecond!=dt.nSecond) return false;
		return true;
	}

	std::string ToString() const{
		char strBuf[256];
		sprintf_s(strBuf, "%d-%d-%d %d:%d:%d", nYear, nMonth, nDay, nHour, nMinute, nSecond);
		return std::string(strBuf);
	}
	time_t ToTimeT() const{
		struct tm tmp;
		tmp.tm_year=nYear-1900;
		tmp.tm_mon=nMonth;
		tmp.tm_mday=nDay;
		tmp.tm_hour=nHour;
		tmp.tm_min=nMinute;
		tmp.tm_sec=nSecond;
		return mktime(&tmp);
	}
	long long ToLongLong() const{
		struct tm tmp;
		tmp.tm_year=nYear-1900;
		tmp.tm_mon=nMonth;
		tmp.tm_mday=nDay;
		tmp.tm_hour=nHour;
		tmp.tm_min=nMinute;
		tmp.tm_sec=nSecond;
		return (long long)(mktime(&tmp));
	}

	void Forward(int nSeconds){
		time_t tt=ToTimeT();
		tt+=nSeconds;
		*this=FromTimeT(tt);
	}
	void BackWard(int nSeconds){
		time_t tt=ToTimeT();
		tt-=nSeconds;
		*this=FromTimeT(tt);
	}

	static DateTime FromTimeT(time_t tt)
	{
		DateTime dt;
		struct tm *tmp;
		time(&tt);
		tmp = localtime(&tt);

		dt.nYear=tmp->tm_year+1900;
		dt.nMonth=tmp->tm_mon;
		dt.nDay=tmp->tm_mday;
		dt.nHour=tmp->tm_hour;
		dt.nMinute=tmp->tm_min;
		dt.nSecond=tmp->tm_sec;

		return dt;
	}
	static DateTime FromLongLong(long long ll)
	{
		DateTime dt;
		dt.SetAsInvalid();

		if(ll<0) return dt;

		time_t tt = (time_t)ll;
		struct tm *tmp;
		time(&tt);
		tmp = localtime(&tt);

		dt.nYear=tmp->tm_year+1900;
		dt.nMonth=tmp->tm_mon;
		dt.nDay=tmp->tm_mday;
		dt.nHour=tmp->tm_hour;
		dt.nMinute=tmp->tm_min;
		dt.nSecond=tmp->tm_sec;

		return dt;
	}

	static DateTime FromCurrentDateTime()
	{
		time_t ct;
		time(&ct);
		return FromTimeT(ct);
	}
};
}
