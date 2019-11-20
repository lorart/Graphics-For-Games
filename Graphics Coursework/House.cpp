#include "House.h"


Mesh* House::housemesh = NULL;
House::House(void)
{
	SceneNode*house = new SceneNode(housemesh, Vector4(1, 0, 0, 1)); // Red !
	house->SetModelScale(Vector3(1, 1, 1));
	house->SetTransform(Matrix4::Translation(Vector3(50, 35, 0)));
	AddChild(house);
	house->SetBoundingRadius(15.0f);

	housemesh->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "House_D.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	housemesh->SetBumpMap(SOIL_load_OGL_texture(
		TEXTUREDIR "HouseNormalMap2.jpg", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if 	(housemesh->GetTexture() || !housemesh->GetBumpMap()) {
		return;
	}
	
	

}

void House::Update(float msec)
{

	/*transform = transform *
		Matrix4::Rotation(msec / 10.0f, Vector3(0, 1, 0));

	
	SceneNode::Update(msec);*/

}

