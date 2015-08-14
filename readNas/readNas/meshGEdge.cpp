

#include "GmshConfig.h"
#include "GModel.h"
#include "meshGEdge.h"
#include "GEdge.h"
#include "MLine.h"
#include "BackgroundMesh.h"
#include "boundaryLayersData.h"
#include "Numeric.h"
#include "GmshMessage.h"
#include "Context.h"
#include "STensor3.h"
#include "Field.h"
#include "OS.h"

#define SQU(a)      ((a)*(a))