#pragma once
#include "..\..\nclgl\scenenode.h"
#include "..\..\nclgl\OBJMesh.h"

class House : public SceneNode{
public:	 House(void);
	  ~House(void) {};
	  virtual void Update(float msec);

	  static void CreateHouse() {
		  OBJMesh* m = new OBJMesh();
		  m->LoadOBJMesh(MESHDIR"House.obj");
		  housemesh = m;

	  }
	  static void DeleteHouse() { delete housemesh; }
	  
protected:
	static Mesh* housemesh;


};




