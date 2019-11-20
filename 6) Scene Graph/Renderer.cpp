#include "Renderer.h"
Renderer::Renderer(Window& parent) :OGLRenderer(parent) {
	CubeRobot::CreateCube();
	camera = new Camera();
	currentshader = new Shader(SHADERDIR"SceneVertex.glsl", SHADERDIR"SceneFragment.glsl");
	if (!currentshader->LinkProgram()) {
		return;
	}
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	camera->SetPosition(Vector3(0, 30, 175));
	root = new SceneNode;
	root->AddChild(new CubeRobot());
	
	glEnable(GL_DEPTH_TEST);
	init = true;

}

Renderer::~Renderer(void) {
	delete root;
	CubeRobot::DeleteCube();
}

void Renderer::UpdateScene(float msec) {
	camera -> UpdateCamera(msec);
	 viewMatrix = camera -> BuildViewMatrix();
	 root -> Update(msec);

}

void Renderer::RenderScene() {
	 glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	glUseProgram(currentshader -> GetProgram());
	 UpdateShaderMatrices();
	
	glUniform1i(glGetUniformLocation(currentshader -> GetProgram(),"diffuseTex"), 1);//why
	
	DrawNode(root); 
	
	glUseProgram(0);
	 SwapBuffers();
	
}
 void Renderer::DrawNode(SceneNode * n) {
	 if (n -> GetMesh()) {
		 Matrix4 transform = n -> GetWorldTransform() *Matrix4::Scale(n -> GetModelScale());
		 glUniformMatrix4fv(glGetUniformLocation(currentshader -> GetProgram(),"modelMatrix"), 1, false, (float*)&transform);
		 //location, size count,transpose,value
		
		 glUniform4fv(glGetUniformLocation(currentshader -> GetProgram(),"nodeColour"), 1, (float*)&n -> GetColour());
		 //location, size count,value
		
		 glUniform1i(glGetUniformLocation(currentshader -> GetProgram(),"useTexture"), (int)n -> GetMesh() -> GetTexture());
		 //location valuw
		 n -> Draw(*this);
	}
	
		 for (vector < SceneNode* >::const_iterator i = n -> GetChildIteratorStart();i != n -> GetChildIteratorEnd(); ++i) {
		 DrawNode(*i);
	}
	
}