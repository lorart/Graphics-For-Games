#include "renderer.h"

Renderer::Renderer(Window& parent): OGLRenderer(parent) {
	triangle = Mesh::GenerateTrangle();
	triangle->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"brick.tga",SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0)); 	if (!triangle->GetTexture()) {		return;	}

	currentshader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR "texturedfragment.glsl");

	if (!currentshader->LinkProgram()) { return; }

	init = true;
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, 1, -1);

	filtering = true;
	repeating = false;
}


Renderer::~Renderer(void) {
	delete triangle;
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//each fame need to clean and update
	glUseProgram(currentshader->GetProgram());
	
	UpdateShaderMatrices();//each fame need to clean and update


	glUniform1i(glGetUniformLocation(currentshader -> GetProgram(),"diffuseTex"), 0);//1i means int

	

	
	triangle->Draw();

	glUseProgram(0);//needed to prove every thing is done
	SwapBuffers(); //Swap

}

void Renderer::UpdateTextureMatrix(float value)
{
	Matrix4 pushPos = Matrix4::Translation(Vector3(0.5f, 0.5f, 0));
	 Matrix4 popPos = Matrix4::Translation(Vector3(-0.5f, -0.5f, 0));
	 Matrix4 rotation = Matrix4::Rotation(value, Vector3(0, 0, 1));
	 textureMatrix = pushPos*rotation*popPos;

}

void Renderer::ToggleRepeating() {//是否重复贴图
	repeating = !repeating;
	glBindTexture(GL_TEXTURE_2D, triangle->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeating ? GL_REPEAT : GL_CLAMP);  //x axis
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeating ? GL_REPEAT : GL_CLAMP); //y axis 
	glBindTexture(GL_TEXTURE_2D, 0);
}



void Renderer::ToggleFiltering() {//切换边界蒙版
	filtering = !filtering;
	glBindTexture(GL_TEXTURE_2D, triangle->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,filtering ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,filtering ? GL_LINEAR : GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);
}
