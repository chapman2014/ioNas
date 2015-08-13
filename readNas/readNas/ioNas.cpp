
#include "StdAfx.h"
#include <stdio.h>
#include <iostream>
#include "ioNas.h"
#include <map>
#include <vector>

map<int, MVertex*> vertexData;

map<int, std::vector<MTriangle*> > triangleData;

map<int,MLine*> lineData;
static int getFormatBDF(char *buffer, int &keySize)
{
	if(buffer[keySize] == '*'){ keySize++; return 2; } // long fields
	for(unsigned int i = 0; i < strlen(buffer); i++)
		if(buffer[i] == ',') return 0; // free fields
	return 1; // small fields;
}

static double atofBDF(char *str)
{
	int len = strlen(str);
	// classic numbers with E or D exponent notation
	for(int i = 0; i < len; i++){
		if(str[i] == 'E' || str[i] == 'e') {
			return atof(str);
		}
		else if(str[i] == 'D' || str[i] == 'd'){
			str[i] = 'E';
			return atof(str);
		}
	}
	// special Nastran floating point format (e.g. "-7.-1" instead of
	// "-7.E-01" or "2.3+2" instead of "2.3E+02")
	char tmp[32];
	int j = 0, leading_minus = 1;
	for(int i = 0; i < len; i++){
		if(leading_minus && str[i] != ' '  && str[i] != '-') leading_minus = 0;//遇到的第一个不是负号
		if(!leading_minus && str[i] == '-') tmp[j++] = 'E';
		if(str[i] == '+') tmp[j++] = 'E';
		tmp[j++] = str[i];
	}
	tmp[j] = '\0';
	return atof(tmp);
}


static int readVertexBDF(FILE *fp, char *buffer, int keySize,
	int *num, double *x, double *y, double *z)
{
	char tmp[5][32];
	int j = keySize;

	switch(getFormatBDF(buffer, keySize)){
	case 0: // free field
		for(int i = 0; i < 5; i++){
			tmp[i][16] = '\0';
			strncpy(tmp[i], &buffer[j + 1], 16);
			for(int k = 0; k < 16; k++){ if(tmp[i][k] == ',') tmp[i][k] = '\0'; }
			j++;
			while(j < (int)strlen(buffer) && buffer[j] != ',') j++;
		}
		break;
	case 1: // small field
		for(int i = 0; i < 5; i++) tmp[i][8] = '\0';
		strncpy(tmp[0], &buffer[8], 8);
		strncpy(tmp[2], &buffer[24], 8);
		strncpy(tmp[3], &buffer[32], 8);
		strncpy(tmp[4], &buffer[40], 8);
		break;
	case 2: // long field
		for(int i = 0; i < 5; i++) tmp[i][16] = '\0';
		strncpy(tmp[0], &buffer[8], 16);
		strncpy(tmp[2], &buffer[40], 16);
		strncpy(tmp[3], &buffer[56], 16);
		char buffer2[256];
		for(unsigned int i = 0; i < sizeof(buffer2); i++) buffer2[i] = '\0';
		if(!fgets(buffer2, sizeof(buffer2), fp)) return 0;
		strncpy(tmp[4], &buffer2[8], 16);
		break;
	}

	*num = atoi(tmp[0]);
	*x = atofBDF(tmp[2]);
	*y = atofBDF(tmp[3]);
	*z = atofBDF(tmp[4]);
	return 1;
}



static bool emptyFieldBDF(char *field, int length)
{
	for(int i = 0; i < length; i++)
		if(field[i] != '\0' && field[i] != ' ' && field[i] != '\n' && field[i] != '\r')
			return false;
	return true;
}


static void readLineBDF(char *buffer, int format,vector<char*> &fields)
{
	int cmax = (format == 2) ? 16 : 8; // max char per (center) field
	int nmax = (format == 2) ? 4 : 8; // max num of (center) fields per line

	if(format == 0){ // free fields
		for(unsigned int i = 0; i < strlen(buffer); i++){
			if(buffer[i] == ',') fields.push_back(&buffer[i + 1]);
		}
	}
	else{ // small or long fields
		for(int i = 0; i < nmax + 1; i++){
			if(!emptyFieldBDF(&buffer[8 + cmax * i], cmax))
				fields.push_back(&buffer[8 + cmax * i]);
		}
	}
}



static int readElementBDF(FILE *fp, char *buffer, int keySize, int numVertices,
	int &num, int &region, std::vector<MVertex*> &vertices,
	std::map<int, MVertex*> &vertexMap)
{
	char buffer2[256], buffer3[256];
	std::vector<char*> fields;
	int format = getFormatBDF(buffer, keySize);

	for(unsigned int i = 0; i < sizeof(buffer2); i++) buffer2[i] = buffer3[i] = '\0';

	readLineBDF(buffer, format, fields);

	if(((int)fields.size() - 2 < abs(numVertices)) ||
		(numVertices < 0 && (fields.size() == 9))){
			if(fields.size() == 9) fields.pop_back();
			if(!fgets(buffer2, sizeof(buffer2), fp)) return 0;
			readLineBDF(buffer2, format, fields);
	}

	if(((int)fields.size() - 2 < abs(numVertices)) ||
		(numVertices < 0 && (fields.size() == 17))){
			if(fields.size() == 17) fields.pop_back();
			if(!fgets(buffer3, sizeof(buffer3), fp)) return 0;
			readLineBDF(buffer3, format, fields);
	}

	// negative 'numVertices' gives the minimum required number of vertices
	if((int)fields.size() - 2 < abs(numVertices)){
		printf("Wrong number of vertices %d for element", fields.size() - 2);
		return 0;
	}

	int n[30], cmax = (format == 2) ? 16 : 8; // max char per (center) field
	char tmp[32];
	tmp[cmax] = '\0';
	strncpy(tmp, fields[0], cmax); num = atoi(tmp);
	strncpy(tmp, fields[1], cmax); region = atoi(tmp);
	for(unsigned int i = 2; i < fields.size(); i++){
		strncpy(tmp, fields[i], cmax); n[i - 2] = atoi(tmp);
	}

	// ignore the extra fields when we know how many vertices we need
	int numCheck = (numVertices > 0) ? numVertices : fields.size() - 2;

	for(int i = 0; i < numCheck; i++){
		std::map<int, MVertex*>::iterator it = vertexMap.find(n[i]);
		if(it == vertexMap.end()){
			printf("Wrong vertex index %d", n[i]);
			return 0;
		}
		vertices.push_back(it->second);
	}
	return 1;
}

int readBDF(const string &name)
{
	FILE *fp = fopen(name.c_str(), "r");
	if(!fp){
		printf("Unable to open file '%s'", name.c_str());
		return 0;
	}
	vertexData.clear();
	triangleData.clear();
	char buffer[256];
	std::map<int, MVertex*> vertexMap;
    std::map<int, std::vector<MTriangle*> > elements1;
	std::map<int, std::vector<MLine*> > elements0;
	// nodes can be defined after elements, so parse the file twice

	while(!feof(fp)) {
		for(unsigned int i = 0; i < sizeof(buffer); i++) buffer[i] = '\0';
		if(!fgets(buffer, sizeof(buffer), fp)) break;
		if(buffer[0] != '$'){ // skip comments
			if(!strncmp(buffer, "GRID", 4)){
				int num;
				double x, y, z;
				if(!readVertexBDF(fp, buffer, 4, &num, &x, &y, &z)) break;
				vertexMap[num] = new MVertex(x, y, z, num);
			}
		}
	}
	printf("%d vertices\n", vertexMap.size());
	 vertexData=vertexMap;
	
	//read triangle
	rewind(fp);
	while(!feof(fp)) {
		for(unsigned int i = 0; i < sizeof(buffer); i++) buffer[i] = '\0';
		if(!fgets(buffer, sizeof(buffer), fp)) break;
		if(buffer[0] != '$'){ // skip comments
			int num, region;
			std::vector<MVertex*> vertices;
			if(!strncmp(buffer, "CBAR", 4)){
				if(readElementBDF(fp, buffer, 4, 2, num, region, vertices, vertexMap))
					elements0[region].push_back(new MLine(vertices, num));
			}
			else if(!strncmp(buffer, "CROD", 4)){
				if(readElementBDF(fp, buffer, 4, 2, num, region, vertices, vertexMap))
					elements0[region].push_back(new MLine(vertices, num));
			}
		    else if(!strncmp(buffer, "CTRIA3", 6)){
				if(readElementBDF(fp, buffer, 6, 3, num, region, vertices, vertexMap))
					elements1[region].push_back(new MTriangle(vertices,num));
			}
		}
	}
    triangleData=elements1;
	fclose(fp);
	return 1;
}



int writeBDF(const string&name)
{
	FILE *fp =fopen(name.c_str(), "w");
	if(!fp){
		printf("Unable to write file  '%s'\n", name.c_str());
		return 0;
	}

	// write vertex;
	if(vertexData.size()!=0)
	{
		char buffer[256];
		map<int,MVertex*>::iterator it;
		for(it=vertexData.begin();it!=vertexData.end();it++)
		{
			for(unsigned int i = 0; i < sizeof(buffer); i++) buffer[i] = '\0';
			//GRID    1       0       1.0000000.00E+000.00E+00
			//012345678901234567890123456789012345678901234567
			
			strncpy(buffer,"GRID    ",8);
			int id=it->first ;
			sprintf(buffer+8,"%-8d",id);
			sprintf(buffer+16,"%-8d",0);

			//add x,y,z;
			double x,y,z;
			x=it->second->_x;
			y=it->second->_y;
			z=it->second->_z;

			if(x<0)
			{
			sprintf(buffer+24,"-%.5f",abs(x));
			}else
			{
			sprintf(buffer+24,"%.6f",x);
			}

			if(y<0)
			{
				sprintf(buffer+32,"-%.5f",abs(y));
			}else
			{
				sprintf(buffer+32,"%.6f",y);
			}

			if(z<0)
			{
				sprintf(buffer+40,"-%.5f",abs(z));
			}else
			{
				sprintf(buffer+40,"%.6f",z);
			}
			strcat(buffer,"\n");
			fputs(buffer,fp);
	   }	
	}
	else
	{
	printf("No vertex\n");
	return 0;
	}
	//end of write vertex

	//start writing triangle
	//CTRIA3  98      14      21      31      25      
	//012345678901234567890123456789012345678901234567

	if(triangleData.size()!=0)
	{
	   char buffer[256];
	   
	   map<int, std::vector<MTriangle*>>::iterator it;
	   for(it=triangleData.begin();it!=triangleData.end();it++)
	   {
			 
			 int num,region;
			 region=it->first;
			 sprintf(buffer+16,"%-8d",region);
			 vector<MTriangle*>::iterator itMT;
			 for(itMT=it->second.begin();itMT!=it->second.end();itMT++)
			 {
				 for(unsigned int i = 0; i < sizeof(buffer); i++) buffer[i] = '\0';
				 strncpy(buffer,"CTRIA3  ",8);
				 num=(*itMT)->_num;
				 MVertex *v0,*v1,*v2;
				 v0=(*itMT)->_v0;
				 v1=(*itMT)->_v1;
				 v2=(*itMT)->_v2;
				 int num0,num1,num2;
				 num0=v0->_index;
				 num1=v1->_index;
				 num2=v2->_index;
				 sprintf(buffer+8,"%-8d",num);
				 sprintf(buffer+16,"%-8d",region);
				 sprintf(buffer+24,"%-8d",num0);
				 sprintf(buffer+32,"%-8d",num1);
				 sprintf(buffer+40,"%-8d",num2);
				 strcat(buffer,"\n");
				 fputs(buffer,fp);
			 }

	   }

	}else
	{
	printf("No triangle \n");
	}
	fputs("ENDDATA",fp);
	fclose(fp);
	return 1;
}





