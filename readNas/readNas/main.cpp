#include "stdafx.h"

#include <string>
#include <iostream>
#include "ioNas.h"
using namespace std;



int _tmain(int argc, _TCHAR* argv[])
{
	string name="D:\\sphere.nas";
	readBDF(name);
	string name2="D:\\sphere1.nas";
	writeBDF(name2);
	return 0;
}

