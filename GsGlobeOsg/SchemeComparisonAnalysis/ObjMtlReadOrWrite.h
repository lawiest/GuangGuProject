#pragma once

#include "SchemeComparisonAnalysis/CoreOSG.h"


class mvObjModelReadWrite
{
public:

	void TokenizeNextLine(std::ifstream &stream, std::vector< std::string > &tokens)
	{
		if(stream.eof()) return;
		std::string line;
		do{
			std::getline(stream, line);
		}while ((line.length()==0||line[0] == '#') && !stream.eof());  // skip comments and empty lines

		if ((line.length() == 0)||(line[0] == '#') )  // can be true only in the last line of file
			return;

		size_t from		= 0; 
		size_t to			= 0;
		size_t length = line.size();
		tokens.clear();
		do{
			while (from!=length && (line[from]==' ' || line[from]=='\t' || line[from]=='\r') )	from++;
			if(from!=length){
				to = from+1;
				while (to!=length && line[to]!=' ' && line[to] != '\t' && line[to]!='\r')to++;
				tokens.push_back(line.substr(from, to-from).c_str());
				from = to;
			}
		}while (from<length);
	} 

	struct mvMaterial
	{
		std::string name;

		float Ns;
		float Ni;
		float d;
		float Tr;
		osg::Vec3d Tf;
		int illum;
		osg::Vec3d Ka;
		osg::Vec3d Kd;
		osg::Vec3d Ks;
		osg::Vec3d Ke;
		std::string map_Ka;//
		std::string map_Kd;

		mvMaterial(): name(""),Ns(0.0f),Ni(0.0f),d(1.0f),Tr(0.0f),illum(0),map_Ka(""),map_Kd("")
		{

		}
		~mvMaterial()
		{

		}
		mvMaterial& operator=(const mvMaterial& m)
		{
			name=m.name;
			Ns=m.Ns;
			Ni=m.Ni;
			d=m.d;
			Tr=m.Tr;
			illum=m.illum;
			map_Ka=m.map_Ka;
			map_Kd=m.map_Kd;
			int d=3;
			Tf=m.Tf;
			Ka=m.Ka;
			Kd=m.Kd;
			Ks=m.Ks;
			Ke=m.Ke;
			return *this;
		}
	};

	struct mvMaterialLib
	{
		std::string pathname;
		std::vector<mvMaterial> materials;

		void initialize()
		{
			materials.clear();
			pathname="";
		}
	};


	bool readMTL(char* fileName,mvMaterialLib& mtlLib)
	{
		//char *a=strcat((char*)pathName,fileName);
		std::ifstream stream(fileName);
		if (stream.fail())	return false;

		std::vector< std::string > tokens;
		std::string	header;

		mtlLib.initialize();
		mtlLib.pathname=std::string(fileName);

		mvMaterial currentMaterial;

		bool first = true;
		while (!stream.eof()){
			tokens.clear();
			TokenizeNextLine(stream, tokens);

			if (tokens.size() > 0){
				header.clear();
				header = tokens[0];

				if (header.compare("newmtl")==0){
					if (!first){
						mtlLib.materials.push_back(currentMaterial);
						currentMaterial = mvMaterial();
					}else
						first = false;

					//if(tokens.size() < 2)return false; 
					currentMaterial.name=tokens[1];
				}else if (header.compare("Ka")==0){
					float r = (float) atof(tokens[1].c_str());
					float g = (float) atof(tokens[2].c_str());
					float b = (float) atof(tokens[3].c_str());
					currentMaterial.Ka = osg::Vec3d(r, g, b); 
				}else if (header.compare("Kd")==0){
					float r = (float) atof(tokens[1].c_str());
					float g = (float) atof(tokens[2].c_str());
					float b = (float) atof(tokens[3].c_str());
					currentMaterial.Kd = osg::Vec3d(r, g, b); 
				}else if (header.compare("Ks")==0){
					float r = (float) atof(tokens[1].c_str());
					float g = (float) atof(tokens[2].c_str());
					float b = (float) atof(tokens[3].c_str());
					currentMaterial.Ks = osg::Vec3d(r, g, b); 
				}else if(header.compare("Ke")==0){
					float r = (float) atof(tokens[1].c_str());
					float g = (float) atof(tokens[2].c_str());
					float b = (float) atof(tokens[3].c_str());
					currentMaterial.Ke = osg::Vec3d(r, g, b); 
				}else if (header.compare("Tr")==0	){	// alpha
					currentMaterial.Tr = (float) atof(tokens[1].c_str());
				}else if (header.compare("Tf")==0	){
					float r = (float) atof(tokens[1].c_str());
					float g = (float) atof(tokens[2].c_str());
					float b = (float) atof(tokens[3].c_str());
					currentMaterial.Tf = osg::Vec3d(r, g, b); 
				}else if (header.compare("d")==0){
					currentMaterial.d = (float) atof(tokens[1].c_str());
				}else if (header.compare("Ns")==0){  // shininess        
					currentMaterial.Ns = float(atof(tokens[1].c_str()));
				}else if (header.compare("Ni")==0){  // shininess        
					currentMaterial.Ni = float(atof(tokens[1].c_str()));
				}else if (header.compare("illum")==0){	// specular illumination on/off
					int illumination = atoi(tokens[1].c_str());
					currentMaterial.illum = illumination;
				}else if(header.compare("map_Kd")==0 && tokens.size()>1){

					currentMaterial.map_Kd=tokens[1];
				}else if(header.compare("map_Ka")==0 && tokens.size()>1){
					currentMaterial.map_Ka=tokens[1];
				}
				// we simply ignore other situations
			}
		}
		mtlLib.materials.push_back(currentMaterial);  // add last read material
		stream.close();
		return true;
	}

	bool saveMTL(const char* fullPathName,mvMaterialLib& mtlLib)
	{
		std::ofstream stream(fullPathName);
		if(stream.fail())return false;

		for(int i=0;i<mtlLib.materials.size();++i){
			mvMaterial& material=mtlLib.materials[i];
			stream<<"newmtl "<<material.name.c_str()<<std::endl;
			stream<<"Ns "<<material.Ns<<std::endl;
			stream<<"Ni "<<material.Ni<<std::endl;
			stream<<"d "<<material.d<<std::endl;
			stream<<"Tr "<<material.Tr<<std::endl;
			stream<<"Tf "<<material.Tf.x()<<" "<<material.Tf.y()<<" "<<material.Tf.z()<<std::endl;
			stream<<"illum "<<material.illum<<std::endl;
			stream<<"Ka "<<material.Ka.x()<<" "<<material.Ka.y()<<" "<<material.Ka.z()<<std::endl;
			stream<<"Kd "<<material.Kd.x()<<" "<<material.Kd.y()<<" "<<material.Kd.z()<<std::endl;
			stream<<"Ks "<<material.Ks.x()<<" "<<material.Ks.y()<<" "<<material.Ks.z()<<std::endl;
			stream<<"Ke "<<material.Ke.x()<<" "<<material.Ke.y()<<" "<<material.Ke.z()<<std::endl;
			if (!material.map_Ka.empty()) stream<<"map_Ka "<<material.map_Ka<<std::endl;
			if (!material.map_Kd.empty()) stream<<"map_Kd "<<material.map_Kd<<std::endl;
			stream<<std::endl;
		}
		stream.close();
		return true;
	}


	int findMaterialIndexByName(mvMaterialLib& mtlLib,std::string & mtlName)
	{
		for(int i=0;i<mtlLib.materials.size();++i){
			if(mtlLib.materials[i].name==mtlName)
				return i;
		}
		return -1;
	}

	bool changeTex(mvMaterial& changMaterial,std::string Texname)
	{
		changMaterial.map_Ka=Texname;
		changMaterial.map_Kd=Texname;

		return true;
	}
};

