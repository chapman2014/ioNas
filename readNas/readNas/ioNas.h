#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include "MVertex.h"
#include "MTriangle.h"
#include "MLine.h"
#include "GModel.h"
using namespace std;

int readBDF(const string &name,GModel *m);
int writeBDF(const string&name);

