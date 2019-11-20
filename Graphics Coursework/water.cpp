#include "water.h"

water::water(void)
{
	quad = Mesh::GenerateQuad();
	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "water.TGA",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	waterRotate = 0.0f;

	if (quad->GetTexture() ) {
		return;
	}
	//SetTextureRepeating(quad->GetTexture(), true);
}

water::~water(void) {
	delete quad;
};
void water::Update(float msec)
{
}
