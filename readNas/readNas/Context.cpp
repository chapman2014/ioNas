#include "StdAfx.h"
#include <stdlib.h>
#include <string.h>

#include "Context.h"

CTX::CTX()
{
  // initialize everything that has no default value in DefaultOptions.h
	lc = 1.;
	  mesh.draw = 1;
}