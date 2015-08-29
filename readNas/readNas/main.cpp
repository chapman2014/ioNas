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
//                  佛祖保佑             永无BUG
#include "stdafx.h"
#include <string>
#include <iostream>
#include "ioNas.h"
#include "meshGFace.h"
#include "Generator.h"
#include "GModel.h"
using namespace std;



int _tmain(int argc, _TCHAR* argv[])
{
	string name="C:\\Users\\lin\\Desktop\\plane.nas";
	GModel *m;
	readBDF(name,m);//将数据读入m
	meshGenerator();
	string name2="C:\\Users\\lin\\Desktop\\plane2.nas";
	writeBDF(name2);
	return 0;
}

