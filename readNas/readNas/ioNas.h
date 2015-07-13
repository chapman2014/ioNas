#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include "MVertex.h"
#include "MTriangle.h"
using namespace std;

int readBDF(const string &name);
int writeBDF(const string&name);


static map<int, MVertex*> vertexData;

static map<int, std::vector<MTriangle*> > triangleData;