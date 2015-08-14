#include <sstream>
#include <algorithm>
#include "GModel.h"
#include "GVertex.h"

//#include "MPoint.h"

GVertex::GVertex(GModel *m, int tag, double ms) : GEntity(m, tag), meshSize(ms)
{
}