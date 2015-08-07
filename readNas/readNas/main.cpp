//                            _ooOoo_
//                           o8888888o
//                           88" . "88
//                           (| -_- |)
//                            O\ = /O
//                        ____/`---'\____
//                      .   ' \\| |// `.
//                       / \\||| : |||// \
//                     / _||||| -:- |||||- \
//                       | | \\\ - /// | |
//                     | \_| ''\---/'' | |
//                      \ .-\__ `-` ___/-. /
//                   ___`. .' /--.--\ `. . __
//                ."" '< `.___\_<|>_/___.' >'"".
//               | | : `- \`.;`\ _ /`;.`/ - ` : | |
//                 \ \ `-. \_ __\ /__ _/ .-` / /
//         ======`-.____`-.___\_____/___.-`____.-'======
//                            `=---='
//
//         .............................................
//                  ·ð×æ±£ÓÓ             ÓÀÎÞBUG
#include "stdafx.h"
#include <string>
#include <iostream>
#include "ioNas.h"
#include "meshGFace.h"
using namespace std;



int _tmain(int argc, _TCHAR* argv[])
{
	string name="C:\\Users\\lin\\Desktop\\plane.nas";
	readBDF(name);
	meshGenerator();
	string name2="C:\\Users\\lin\\Desktop\\plane2.nas";
	writeBDF(name2);
	return 0;
}

