#pragma once
#include "..\..\nclgl\OBJMesh.h"

class water
{

public:	 water(void);
	    ~water(void) ;
	  virtual void Update(float msec);

	
	 // ststic 

protected:
	static Mesh* quad;

	float waterRotate;
};

