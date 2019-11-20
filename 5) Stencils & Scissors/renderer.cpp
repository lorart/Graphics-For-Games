#include "renderer.h"

Renderer::Renderer(Window& parent) :OGLRenderer(parent) {
	triangle = Mesh::GenerateTrangle();
	quad = Mesh::GenerateQuad();
	currentshader = new Shader(SHADERDIR"TexturedVertex.glsl", 
		 SHADERDIR"StencilFragment.glsl");
	if (!currentshader->LinkProgram()) {
		 return;
		  } 
	 triangle->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"brick.tga",  SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	  quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"chessboard.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));

	  if (!triangle->GetTexture() || !quad->GetTexture()) { 
		  return;  }

	  usingScissor = false;
	  usingStencil = false;
	  init = true; 
}

Renderer::~Renderer(void) { 
	delete triangle; 
	delete quad;
}

void Renderer::ToggleScissor() { 
	 usingScissor = !usingScissor; 
	 } 

 void Renderer::ToggleStencil() { 
	 usingStencil = !usingStencil; 
	 }

 void Renderer::RenderScene() {
	 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);//cannot without bit

	 if (usingScissor) {
		 glEnable(GL_SCISSOR_TEST);//enable scissor test
		 glScissor((float)width/2.5f,(float)height/2.5f, (float)width / 5.0f, (float)height / 5.0f);//start location x,y,width,height
	 }

	 glUseProgram(currentshader->GetProgram());
	 UpdateShaderMatrices();

	 glUniform1i(glGetUniformLocation(currentshader->GetProgram(), "diffuseTex"), 0);

	 if (usingStencil) {
		 glEnable(GL_STENCIL_TEST);
		 
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		 //先关闭colour通道写入
		 glStencilFunc(GL_ALWAYS, 2, ~0);
		 // 写到深度2号通道（一共256个通道） ~0是除了0之外最大的全1
		 // ref is clamped to the range 0 2 n - 1 , 
		 //where n is the number of bitplanes in the stencil buffer. The initial value is 0.
		 //~0 not zero but the max number it can be

		 glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		 // 无论如何 保证蒙版
		 //Sets the stencil buffer value to ref, as specified by glStencilFunc. 

		 quad->Draw();
		 // 画的时候抛弃0的步伐  （在此步写入深度蒙版）

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		// 打开颜色通道
		 
		 glStencilFunc(GL_EQUAL, 2, ~0);
		 // 做与运算 如果深度通道等于2 （有内容） 就保存

		 glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		 //Keeps the current value. 
	 }
	 triangle->Draw(); 
	 glUseProgram(0);
	 glDisable(GL_SCISSOR_TEST);
	 glDisable(GL_STENCIL_TEST);
	 SwapBuffers();
 }
