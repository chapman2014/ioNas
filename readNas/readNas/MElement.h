#ifndef _MELEMENT_H_
#define _MELEMENT_H_

#include <stdio.h>
#include <algorithm>
#include <string>
#include "MVertex.h"
#include "MEdge.h"


class GModel;
// A mesh element.
class MElement
{
 private:
			 // the id number of the element (this number is unique and is
			 // guaranteed never to change once a mesh has been generated)
			 int _num;
			 // the number of the mesh partition the element belongs to
			 short _partition;
			 // a visibility flag
			 char _visible;
 protected:
	 // the tolerance used to determine if a point is inside an element,
	 // in parametric coordinates 	


};

#endif