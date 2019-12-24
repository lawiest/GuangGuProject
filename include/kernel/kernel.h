#pragma once
//////////////////////////////////////////////////////////////////////////
//  
//   _____                     _____   _                  
//  / ____|                   / ____| | |                 
// | |  __    ___    ___     | (___   | |_    __ _   _ __ 
// | | |_ |  / _ \  / _ \     \___ \  | __|  / _` | | '__|
// | |__| | |  __/ | (_) |    ____) | | |_  | (_| | | |   
//  \_____|  \___|  \___/    |_____/   \__|  \__,_| |_|   
//                                                        
//!  \file kernel.h
//!  \brief GeoStar Kernel GIS内核
//!  \details GIS内核主要模块
//!  \author 
//!  \attention 
//!   Copyright (c) Wuda Geoinformatics Co.,Ltd. <br> 
//!   All Rights Reserved.
//!   版权所有(c) 武大吉奥信息技术有限公司 保留所有权利
//!  \version 1.1
//////////////////////////////////////////////////////////////////////////

#include "preconfig.h"
#include "spatialindex.h"
#include "spatialreference.h"
#include "pyramid.h"
#include "geometry.h"
#include "canvas.h"
#include "symbol.h"
#include "pointsymbol.h"
#include "linesymbol.h"
#include "fillsymbol.h"
#include "weburiparser.h"
#include "geodatabase.h"
#include "map.h"
#include "mapdefine.h"
#include "esrigeodatabase.h"
#include "layoutelement.h"
#include <utility.h>

#include <logger.h>

#ifdef ENABLE_QT 
#include "qt/qtdevice.h"
#endif

#ifdef ENABLE_WIN32
#include "win32/windevice.h"
#endif
/*! \mainpage GeoStar Kernel （GeoStar基础GIS平台内核模块)
 *
 * \section intro_sec 介绍
 *
 * 介绍内容.
 *
 * \section install_sec 安装
 *
 * \subsection step1 Step 
 *  
 * etc...
 */

KERNEL_NS 

/// \brief 内核模块
enum GsKernelModule
{
	eKernelDatabase=1,
	eKernelSymbol = 2,
	eKernelElement = 4,
	eKernelLogFile = 0x20000000,
	eKernelDebug = 0x40000000,
	eKernelUser =    0x80000000,
	eAllKernelModule = 0x7,
};
/// \brief 内核初始化
class GsKernel
{
	static void RegisterDatabase(GsKernelModule module)
	{
		if (module & eKernelDebug)  GS_D << "Register Sqlite";
		REG_CLASS_CREATE(GsSqliteGeoDatabaseFactory);
		REG_CLASS_CREATE_ALIAS(GsSqliteGeoDatabaseFactory, "SqliteGeoDatabaseFactory");

		if (module & eKernelDebug)  GS_D << "Register SHP";
		REG_CLASS_CREATE(GsShpGeoDatabaseFactory);
		REG_CLASS_CREATE_ALIAS(GsShpGeoDatabaseFactory, "ShpGeoDatabaseFactory");

		if (module & eKernelDebug)  GS_D << "Register File";
		REG_CLASS_CREATE(GsFileGeoDatabaseFactory);
		REG_CLASS_CREATE_ALIAS(GsFileGeoDatabaseFactory, "FileGeoDatabaseFactory");
		
		if (module & eKernelDebug)  GS_D << "Register GeoPackage";
		REG_CLASS_CREATE(GsGeoPackageGeoDatabaseFactory);
		REG_CLASS_CREATE_ALIAS(GsGeoPackageGeoDatabaseFactory, "GeoPackageGeoDatabaseFactory");
		
		if (module & eKernelDebug)  GS_D << "Register Web";
		REG_CLASS_CREATE(GsWebGeoDatabaseFactory);
		REG_CLASS_CREATE_ALIAS(GsWebGeoDatabaseFactory, "WebGeoDatabaseFactory");
		
		if (module & eKernelDebug)  GS_D << "Register Memory";
		REG_CLASS_CREATE(GsMemoryGeoDatabaseFactory);
		REG_CLASS_CREATE_ALIAS(GsMemoryGeoDatabaseFactory, "MemoryGeoDatabaseFactory");
	}
	static void RegisterSymbol(GsKernelModule module)
	{
		//点
		if (module & eKernelDebug)  GS_D << "Register MultiPointSymbol";
		REG_CLASS_CREATE(GsMultiPointSymbol);
		REG_CLASS_CREATE_ALIAS(GsMultiPointSymbol,"{F6E15656-6573-4B93-BF6F-0CD1A3B05426}");
		REG_CLASS_CREATE_ALIAS(GsMultiPointSymbol,"GeoStarCore.MultiPointSymbol.1");
		REG_CLASS_CREATE_ALIAS(GsMultiPointSymbol, "GeoStarCore.MultiPointSymbol");
		REG_CLASS_CREATE_ALIAS(GsMultiPointSymbol,"MultiPointSymbol");

		if (module & eKernelDebug)  GS_D << "Register SimplePointSymbol";
		REG_CLASS_CREATE(GsSimplePointSymbol);
		REG_CLASS_CREATE_ALIAS(GsSimplePointSymbol,"{F6E15656-6573-4B93-BF6F-0CD1A3B05415}");
		REG_CLASS_CREATE_ALIAS(GsSimplePointSymbol,"GeoStarCore.SimplePointSymbol.1"); 
		REG_CLASS_CREATE_ALIAS(GsSimplePointSymbol,"SimplePoint");

		if (module & eKernelDebug)  GS_D << "Register LinePointSymbol";
		REG_CLASS_CREATE(GsLinePointSymbol);
		REG_CLASS_CREATE_ALIAS(GsLinePointSymbol,"{F6E15656-6573-4B93-BF6F-0CD1A3B05416}");
		REG_CLASS_CREATE_ALIAS(GsLinePointSymbol,"GeoStarCore.LinePointSymbol.1");
		REG_CLASS_CREATE_ALIAS(GsLinePointSymbol,"LinePoint");

		if (module & eKernelDebug)  GS_D << "Register PolylinePointSymbol";
		REG_CLASS_CREATE(GsPolylinePointSymbol);
		REG_CLASS_CREATE_ALIAS(GsPolylinePointSymbol,"{F6E15656-6573-4B93-BF6F-0CD1A3B05417}");
		REG_CLASS_CREATE_ALIAS(GsPolylinePointSymbol,"GeoStarCore.PolylinePointSymbol.1");
		REG_CLASS_CREATE_ALIAS(GsPolylinePointSymbol,"PolylinePoint");

		if (module & eKernelDebug)  GS_D << "Register PicturePointSymbol";
		REG_CLASS_CREATE(GsPicturePointSymbol);
		REG_CLASS_CREATE_ALIAS(GsPicturePointSymbol,"{0C6D50E9-24CA-4D80-91AE-491C8053F0A5}");
		REG_CLASS_CREATE_ALIAS(GsPicturePointSymbol,"GeoStarCore.PicturePointSymbol.1");
		REG_CLASS_CREATE_ALIAS(GsPicturePointSymbol,"PicturePoint");

		if (module & eKernelDebug)  GS_D << "Register EllipsePointSymbol";
		REG_CLASS_CREATE(GsEllipsePointSymbol);
		REG_CLASS_CREATE_ALIAS(GsEllipsePointSymbol,"{F6E15656-6573-4B93-BF6F-0CD1A3B0541B}");
		REG_CLASS_CREATE_ALIAS(GsEllipsePointSymbol,"GeoStarCore.EllipsePointSymbol.1");
		REG_CLASS_CREATE_ALIAS(GsEllipsePointSymbol,"EllipsePoint");

		if (module & eKernelDebug)  GS_D << "Register ArcPointSymbol";
		REG_CLASS_CREATE(GsArcPointSymbol);
		REG_CLASS_CREATE_ALIAS(GsArcPointSymbol,"{F6E15656-6573-4B93-BF6F-0CD1A3B05425}");
		REG_CLASS_CREATE_ALIAS(GsArcPointSymbol,"GeoStarCore.ArcPointSymbol.1");
		REG_CLASS_CREATE_ALIAS(GsArcPointSymbol,"ArcPoint");

		if (module & eKernelDebug)  GS_D << "Register TextPointSymbol";
		REG_CLASS_CREATE(GsTextPointSymbol);
		REG_CLASS_CREATE_ALIAS(GsTextPointSymbol,"{F6E15656-6573-4B93-BF6F-0CD1A3B05420}");
		REG_CLASS_CREATE_ALIAS(GsTextPointSymbol,"GeoStarCore.TextPointSymbol.1");
		REG_CLASS_CREATE_ALIAS(GsTextPointSymbol,"TextPoint");

		if (module & eKernelDebug)  GS_D << "Register CurvelinePointSymbol";
		REG_CLASS_CREATE(GsCurvelinePointSymbol);
		REG_CLASS_CREATE_ALIAS(GsCurvelinePointSymbol,"{F6E15656-6573-4B93-BF6F-0CD1A3B05419}");
		REG_CLASS_CREATE_ALIAS(GsCurvelinePointSymbol,"GeoStarCore.CurvelinePointSymbol.1");
		REG_CLASS_CREATE_ALIAS(GsCurvelinePointSymbol,"CurvelinePoint");

		if (module & eKernelDebug)  GS_D << "Register CurvegonPointSymbol";
		REG_CLASS_CREATE(GsCurvegonPointSymbol);
		REG_CLASS_CREATE_ALIAS(GsCurvegonPointSymbol,"{F6E15656-6573-4B93-BF6F-0CD1A3B0541A}");
		REG_CLASS_CREATE_ALIAS(GsCurvegonPointSymbol,"GeoStarCore.CurvegonPointSymbol.1");
		 REG_CLASS_CREATE_ALIAS(GsCurvegonPointSymbol,"CurvegonPoint");

		 if (module & eKernelDebug)  GS_D << "Register AsteriskPointSymbol";
		REG_CLASS_CREATE(GsAsteriskPointSymbol);
		REG_CLASS_CREATE_ALIAS(GsAsteriskPointSymbol,"{F6E15656-6573-4B93-BF6F-0CD1A3B0541F}");
		REG_CLASS_CREATE_ALIAS(GsAsteriskPointSymbol,"GeoStarCore.AsteriskPointSymbol.1");
		REG_CLASS_CREATE_ALIAS(GsAsteriskPointSymbol,"AsteriskPoint");

		if (module & eKernelDebug)  GS_D << "Register QRCodePointSymbol";
		REG_CLASS_CREATE(GsQRCodePointSymbol);
		REG_CLASS_CREATE_ALIAS(GsQRCodePointSymbol,"QRCodePoint"); 
		

		//PolygonPoint
		if (module & eKernelDebug)  GS_D << "Register PolygonPointSymbol";
		REG_CLASS_CREATE(GsPolygonPointSymbol);
		REG_CLASS_CREATE_ALIAS(GsPolygonPointSymbol,"{F6E15656-6573-4B93-BF6F-0CD1A3B05418}");
		REG_CLASS_CREATE_ALIAS(GsPolygonPointSymbol,"GeoStarCore.PolygonPointSymbol.1");
		REG_CLASS_CREATE_ALIAS(GsPolygonPointSymbol,"PolygonPoint");

		//ChordPoint
		if (module & eKernelDebug)  GS_D << "Register ChordPointSymbol";
		REG_CLASS_CREATE(GsChordPointSymbol);
		REG_CLASS_CREATE_ALIAS(GsChordPointSymbol,"{F6E15656-6573-4B93-BF6F-0CD1A3B0541E}");
		REG_CLASS_CREATE_ALIAS(GsChordPointSymbol,"GeoStarCore.ChordPointSymbol.1");
		REG_CLASS_CREATE_ALIAS(GsChordPointSymbol,"ChordPoint");

		if (module & eKernelDebug)  GS_D << "Register PiePointSymbol";
		REG_CLASS_CREATE(GsPiePointSymbol);
		REG_CLASS_CREATE_ALIAS(GsPiePointSymbol,"{F6E15656-6573-4B93-BF6F-0CD1A3B0541D}");
		REG_CLASS_CREATE_ALIAS(GsPiePointSymbol,"GeoStarCore.GsPiePointSymbol.1");
		REG_CLASS_CREATE_ALIAS(GsPiePointSymbol,"PiePoint");

		//RectanglePoint
		if (module & eKernelDebug)  GS_D << "Register RectanglePointSymbol";
		REG_CLASS_CREATE(GsRectanglePointSymbol);
		REG_CLASS_CREATE_ALIAS(GsRectanglePointSymbol,"{F6E15656-6573-4B93-BF6F-0CD1A3B0541C}");
		REG_CLASS_CREATE_ALIAS(GsRectanglePointSymbol,"GeoStarCore.RectanglePointSymbol.1");
		REG_CLASS_CREATE_ALIAS(GsRectanglePointSymbol,"RectanglePoint");

		//线
		if (module & eKernelDebug)  GS_D << "Register MultiLineSymbol";
		REG_CLASS_CREATE(GsMultiLineSymbol);
		REG_CLASS_CREATE_ALIAS(GsMultiLineSymbol,"{C6D520D5-6EC3-4C93-9C5A-9AFFAB0EED0F}");
		REG_CLASS_CREATE_ALIAS(GsMultiLineSymbol,"GeoStarCore.MultiLineSymbol.1");
		REG_CLASS_CREATE_ALIAS(GsMultiLineSymbol,"MultiLineSymbol");

		if (module & eKernelDebug)  GS_D << "Register SimpleLineSymbol";
		REG_CLASS_CREATE(GsSimpleLineSymbol);
		REG_CLASS_CREATE_ALIAS(GsSimpleLineSymbol,"{C6D520D5-6EC3-4C93-9C5A-9AFFAB0EED0E}");
		REG_CLASS_CREATE_ALIAS(GsSimpleLineSymbol,"GeoStarCore.SimpleLineSymbol.1");
		REG_CLASS_CREATE_ALIAS(GsSimpleLineSymbol,"SimpleLine");

		if (module & eKernelDebug)  GS_D << "Register PointLineSymbol";
		REG_CLASS_CREATE(GsPointLineSymbol);
		REG_CLASS_CREATE_ALIAS(GsPointLineSymbol,"{260F5830-5562-4D8F-BE80-6AB5805204E5}");
		REG_CLASS_CREATE_ALIAS(GsPointLineSymbol,"GeoStarCore.PointLineSymbol.1");
		REG_CLASS_CREATE_ALIAS(GsPointLineSymbol,"PointLine");

		if (module & eKernelDebug)  GS_D << "Register HashLineSymbol";
		REG_CLASS_CREATE(GsHashLineSymbol);
		REG_CLASS_CREATE_ALIAS(GsHashLineSymbol,"{8E427709-7B01-4580-9239-EFD176966615}");
		REG_CLASS_CREATE_ALIAS(GsHashLineSymbol,"GeoStarCore.HashLineSymbol.1");
		REG_CLASS_CREATE_ALIAS(GsHashLineSymbol,"HashLine");

		if (module & eKernelDebug)  GS_D << "Register CartographicLineSymbol";
		REG_CLASS_CREATE(GsCartographicLineSymbol);
		REG_CLASS_CREATE_ALIAS(GsCartographicLineSymbol,"{C6D520D5-6EC3-4C93-9C5A-9AFFAB0EED0D}");
		REG_CLASS_CREATE_ALIAS(GsCartographicLineSymbol,"GeoStarCore.CartographicLineSymbol.1");
		REG_CLASS_CREATE_ALIAS(GsCartographicLineSymbol,"CartographicLine");

		if (module & eKernelDebug)  GS_D << "Register LocatePointLineSymbol";
		REG_CLASS_CREATE(GsLocatePointLineSymbol);
		REG_CLASS_CREATE_ALIAS(GsLocatePointLineSymbol,"{2474E6D1-8FE4-42E8-A20C-C07B68B9628D}");
		REG_CLASS_CREATE_ALIAS(GsLocatePointLineSymbol,"GeoStarCore.LocatePointLineSymbol.1");
		REG_CLASS_CREATE_ALIAS(GsLocatePointLineSymbol,"LocatePointLine");

		if (module & eKernelDebug)  GS_D << "Register LeadConnectedLineSymbol";
		REG_CLASS_CREATE(GsLeadConnectedLineSymbol);
		REG_CLASS_CREATE_ALIAS(GsLeadConnectedLineSymbol,"{B44C3816-A707-441C-B8EC-EA5D890DBECE}");
		REG_CLASS_CREATE_ALIAS(GsLeadConnectedLineSymbol,"GeoStarCore.LeadConnectedLineSymbol.1");
		REG_CLASS_CREATE_ALIAS(GsLeadConnectedLineSymbol,"LeadConnectedLine");

		//面
		if (module & eKernelDebug)  GS_D << "Register MultiFillSymbol";
		REG_CLASS_CREATE(GsMultiFillSymbol);
		REG_CLASS_CREATE_ALIAS(GsMultiFillSymbol,"{AF129AAF-F709-4F59-9E67-47904923B6A9}");
		REG_CLASS_CREATE_ALIAS(GsMultiFillSymbol,"GeoStarCore.MultiFillSymbol.1");
		REG_CLASS_CREATE_ALIAS(GsMultiFillSymbol,"MultiAreaSymbol");

		if (module & eKernelDebug)  GS_D << "Register SimpleFillSymbol"; 
		REG_CLASS_CREATE(GsSimpleFillSymbol);
		REG_CLASS_CREATE_ALIAS(GsSimpleFillSymbol,"{171538B1-4DAB-435D-9C48-8CBBF33307EA}");
		REG_CLASS_CREATE_ALIAS(GsSimpleFillSymbol,"GeoStarCore.SimpleFillSymbol.1");
		REG_CLASS_CREATE_ALIAS(GsSimpleFillSymbol,"SimpleArea");

		if (module & eKernelDebug)  GS_D << "Register PointFillSymbol";
		REG_CLASS_CREATE(GsPointFillSymbol);
		REG_CLASS_CREATE_ALIAS(GsPointFillSymbol,"{23D65545-D01A-4398-9DA7-0701AA896B32}");
		REG_CLASS_CREATE_ALIAS(GsPointFillSymbol,"GeoStarCore.PointFillSymbol.1");
		REG_CLASS_CREATE_ALIAS(GsPointFillSymbol,"PointArea");

		if (module & eKernelDebug)  GS_D << "Register LineFillSymbol";
		REG_CLASS_CREATE(GsLineFillSymbol);
		REG_CLASS_CREATE_ALIAS(GsLineFillSymbol,"{3A92B7B6-54C9-4D13-ABBA-A8118D740BD0}");
		REG_CLASS_CREATE_ALIAS(GsLineFillSymbol,"GeoStarCore.LineFillSymbol.1");
		REG_CLASS_CREATE_ALIAS(GsLineFillSymbol,"LineArea");

		if (module & eKernelDebug)  GS_D << "Register PictureFillSymbol";
		REG_CLASS_CREATE(GsPictureFillSymbol);
		REG_CLASS_CREATE_ALIAS(GsPictureFillSymbol,"{7a0aa58b-2446-43dc-a282-0470d759de86}");
		REG_CLASS_CREATE_ALIAS(GsPictureFillSymbol,"GeoStarCore.PictureFillSymbol.1");
		REG_CLASS_CREATE_ALIAS(GsPictureFillSymbol,"PictureArea");

		if (module & eKernelDebug)  GS_D << "Register GradsFillSymbol";
		REG_CLASS_CREATE(GsGradsFillSymbol);
		REG_CLASS_CREATE_ALIAS(GsGradsFillSymbol,"{55196cc6-effb-45df-a992-1e27167011d6}");
		REG_CLASS_CREATE_ALIAS(GsGradsFillSymbol,"GeoStarCore.GradsFillSymbol.1");
		REG_CLASS_CREATE_ALIAS(GsGradsFillSymbol,"GradientArea");

		if (module & eKernelDebug)  GS_D << "Register TextSymbol";
		REG_CLASS_CREATE(GsTextSymbol);
		REG_CLASS_CREATE_ALIAS(GsTextSymbol,"{F947691D-CED4-4600-A1EC-CBC0C01EB039}");
		REG_CLASS_CREATE_ALIAS(GsTextSymbol,"GeoStarCore.TextSymbol.1");
		REG_CLASS_CREATE_ALIAS(GsTextSymbol,"TextSymbol");

		//Canvas 注册
#ifdef ENABLE_QT 
		REG_CLASS_CREATE_NS(GsQTCanvasFactory, QT);
#endif
		
#ifdef ENABLE_WIN32  
		if (module & eKernelDebug)  GS_D << "Register Win32CanvasFactory";
		REG_CLASS_CREATE_NS(GsWin32CanvasFactory,Win32); 
#endif
	}
	static void RegisterReflect(GsKernelModule module)
	{
		GS_REGISTER_REFLECT(GsPT);
	}
	static void RegisterUser(GsKernelModule module)
	{

	}

	static void RegisterElement(GsKernelModule module)
	{
		if (module & eKernelDebug)  GS_D << "Register GsMapElement";
		REG_CLASS_CREATE(GsMapElement);
		REG_CLASS_CREATE_ALIAS(GsMapElement, "MapElement");

		if (module & eKernelDebug)  GS_D << "Register GsShapeElement";
		REG_CLASS_CREATE(GsShapeElement);
		REG_CLASS_CREATE_ALIAS(GsShapeElement, "ShapeElement");

		if (module & eKernelDebug)  GS_D << "Register GsGeometryElement";
		REG_CLASS_CREATE(GsGeometryElement);
		REG_CLASS_CREATE_ALIAS(GsGeometryElement,"GeometryElement");

		if (module & eKernelDebug)  GS_D << "Register GsEnvelopeElement";
		REG_CLASS_CREATE(GsEnvelopeElement);
		REG_CLASS_CREATE_ALIAS(GsEnvelopeElement, "EnvelopeElement");

		if (module & eKernelDebug)  GS_D << "Register GsNorthArrow";
		REG_CLASS_CREATE(GsNorthArrow);
		REG_CLASS_CREATE_ALIAS(GsNorthArrow, "NorthArrow");

		if (module & eKernelDebug)  GS_D << "Register GsPictureElement";
		REG_CLASS_CREATE(GsPictureElement);
		REG_CLASS_CREATE_ALIAS(GsPictureElement, "PictureElement");

		if (module & eKernelDebug)  GS_D << "Register GsJoinMapTableElement";
		REG_CLASS_CREATE(GsJoinMapTableElement);
		REG_CLASS_CREATE_ALIAS(GsJoinMapTableElement, "JoinMapTableElement");

		if (module & eKernelDebug)  GS_D << "Register GsSlopeRulerElement";
		REG_CLASS_CREATE(GsSlopeRulerElement);
		REG_CLASS_CREATE_ALIAS(GsSlopeRulerElement, "SlopeRulerElement");

		if (module & eKernelDebug)  GS_D << "Register GsTextElement";
		REG_CLASS_CREATE(GsTextElement);
		REG_CLASS_CREATE_ALIAS(GsTextElement, "TextElement");

		if (module & eKernelDebug)  GS_D << "Register GsGridElement";
		REG_CLASS_CREATE(GsGridElement);
		REG_CLASS_CREATE_ALIAS(GsGridElement, "GridElement");

		if (module & eKernelDebug)  GS_D << "Register GsScaleBarElement";
		REG_CLASS_CREATE(GsScaleBarElement);
		REG_CLASS_CREATE_ALIAS(GsScaleBarElement, "ScaleBarElement");

		if (module & eKernelDebug)  GS_D << "Register GsThreeNorthElement";
		REG_CLASS_CREATE(GsThreeNorthElement);
		REG_CLASS_CREATE_ALIAS(GsThreeNorthElement, "ThreeNorthElement");

		if (module & eKernelDebug)  GS_D << "Register GsLegendsElement";
		REG_CLASS_CREATE(GsLegendsElement);
		REG_CLASS_CREATE_ALIAS(GsLegendsElement, "LegendsElement");
	}
	static void InitLogFile(const char* file = NULL)
	{
		static bool bInit = false;
		if (bInit)
			return;
		Utility::GsString strFile = "GeoStar.Kernel.Log";
		if (file)
			strFile = file;
		Utility::GsLogger::Default().CustomOutput(new Utility::GsFileCustomLogOutput(strFile.c_str()));
		bInit = true;
	}
public:
	static void InitializeLogFile(const char* file, Utility::GsLogLevel level)
	{
		Utility::GsLogger::Default().LogLevel(level);
		InitLogFile(file);
	}

	/// \brief 创建类的实例
	static Utility::GsString Version()
	{ 
		return Utility::GsString("1.1.0");
	}

	/// \brief 初始化
	static void Initialize(GsKernelModule module)
	{

		static bool bInit = false;
		static Utility::GsLock lock;
		if (bInit)
			return;
		{
			Utility::GsAutoLocker l(lock);
			if (module & eKernelLogFile)
				InitLogFile();
			if (module & eKernelSymbol)
				RegisterSymbol(module);
			if (module & eKernelElement)
				RegisterElement(module);
			if (module & eKernelDatabase)
				RegisterDatabase(module);
			
			if (module & eKernelUser)
				RegisterUser(module);


			RegisterReflect(module);

			bInit = true;
		}
	}
	/// \brief 初始化
	static void Initialize()
	{
		Initialize(eAllKernelModule);
	}


};
KERNEL_ENDNS 
