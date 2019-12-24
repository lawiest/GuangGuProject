#ifndef GEOGLOBE_TOOL_DOWNLOAD
#define GEOGLOBE_TOOL_DOWNLOAD 1
#include <osg/Referenced>
#include <string>
#include <osgEarth/URI>


//#include <Tool/Request.h>
namespace GeoGlobe
{

	namespace Tool
	{

		class Download:public osg::Referenced
		{
		public:
			//std::string url;
			/**下载数据*/
			const char* data;
            /**下载数据的大小*/
			int   size;
			/////////window///////////////////
			std::string temp;
            /**POST方式下载，适用于URL链接很长的下载*/
			bool downloadData(std::string url)
			{
				temp=osgEarth::URI(url).readString().getString();
				if (temp.size()>0)
				{
					data=temp.c_str();
					size=temp.size();
					return true;
				}
				else
					return false;

			}
			///////////////Android///////////////////////////
			/**根据URL链接下载数据*/
			//Request myRequest;
			//bool downloadData(std::string url)
			//{
			//	
			//	bool isPost = false;//GET²Ù×÷
			//	int m_nHeaderLen = 0;
			//	int m_nBodyLen = 0;
			//	unsigned char* rawpsHeaderReceive = NULL;
			//	unsigned char* rawpsMessage = NULL;
			//	unsigned char* psHeaderReceive =NULL;
			//	unsigned char* psMessage = NULL;

			//	std::string strHeaderSend;
			//	int result =myRequest.SendRequest(isPost,url,strHeaderSend,rawpsHeaderReceive,rawpsMessage,psHeaderReceive,psMessage,&m_nHeaderLen,&m_nBodyLen);

			//	if(result == 2)
			//	{
   //                 return false;
			//	}

			//	if (result==1&&m_nBodyLen>0)
			//	{
		 //          //cache->AddTileData((unsigned char*)rawpsMessage, m_nBodyLen, tileinfo);
			//	   data=rawpsMessage;
			//	   size=m_nBodyLen;
			//		return true;

			//	}
			//}
		};
	}

}


#endif