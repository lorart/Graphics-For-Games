#include "Renderer.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
 heightMap = new HeightMap(TEXTUREDIR "land.jpg");
 camera = new Camera(-40, 270, Vector3(-2100, 3300, 2000));	
	currentshader = new Shader(SHADERDIR "TexturedVertex.glsl",
	 SHADERDIR "TexturedFragment.glsl");
	
	 if (!currentshader -> LinkProgram()) {
	 return;
	
	}
	
	heightMap -> SetTexture(SOIL_load_OGL_texture(
	 TEXTUREDIR "rusted_east.jpg",SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	
	if (!heightMap -> GetTexture()) {
	return;
	
	}
	 SetTextureRepeating(heightMap -> GetTexture(), true);
	
	
	 projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);
	
	glEnable(GL_DEPTH_TEST);
	 glEnable(GL_CULL_FACE);
	 glCullFace(GL_BACK);
	
	 init = true;
	} Renderer ::~Renderer(void) {
	 delete heightMap;
	 delete camera;
	
}

 void Renderer::UpdateScene(float msec) {
	camera -> UpdateCamera(msec);
	viewMatrix = camera -> BuildViewMatrix();
	}  void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	  glUseProgram(currentshader -> GetProgram());
	UpdateShaderMatrices();
	
	 glUniform1i(glGetUniformLocation(currentshader -> GetProgram(),
	  "diffuseTex"), 0);
	
	 heightMap -> Draw();
	
	 glUseProgram(0);
	SwapBuffers();
	 }