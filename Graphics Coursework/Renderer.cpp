#include "Renderer.h"
#include "water.h"
#include "string.h"
Renderer::Renderer(Window& parent) :OGLRenderer(parent) {
	camera = new Camera();
	
#pragma region house
	House::CreateHouse();

	houseShader = new Shader(SHADERDIR"HouseVertex1.glsl", SHADERDIR"HouseFragment1.glsl");
	
	/*currentshader = new Shader(SHADERDIR "BumpVertex.glsl ",
		SHADERDIR "BumpFragment.glsl");*/
	//SetTextureRepeating(housemesh->GetTexture(), true);
	if (!houseShader->LinkProgram()) {
		return;
	}
	root = new SceneNode;
	root->AddChild(new House());
#pragma endregion house

#pragma region lighting

	lighting = Mesh::GenerateQuad();

	lightingShader = new Shader(SHADERDIR"lightingVertex.glsl",
		SHADERDIR "lightingFragment.glsl");
	/*reflectShader = new Shader(SHADERDIR"PerPixelVertex.glsl", SHADERDIR "reflectFragment.glsl");*/
	if (!lightingShader->LinkProgram()) {
		return;
	}

	lighting->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "lighting.jpg",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	lighting->GenerateNormals();


	if (!lighting->GetTexture() ) {
		return;

	}

	SetTextureRepeating(lighting->GetTexture(),0);


	




#pragma endregion



	RendererWater();
	
	/*quad = Mesh::GenerateQuad();
	RendererLoader(quad, reflectShader, "PerPixelVertex.glsl", "reflectFragment.glsl", "water.tga", "water.jpg", 1);*/

#pragma region Hightmap
	heightMap = new HeightMap(TEXTUREDIR "/terrain.raw");
	lightShader = new Shader(SHADERDIR "PerPixelVertex.glsl",
		SHADERDIR "PerPixelFragment.glsl");


	if ( !lightShader->LinkProgram()  ){
		return;

	}
	heightMap->SetTexture(SOIL_load_OGL_texture(
		TEXTUREDIR "grass.jpg", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetBumpMap(SOIL_load_OGL_texture(
		TEXTUREDIR "Barren RedsDOT3.JPG ", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!heightMap->GetTexture() ||
		!heightMap->GetBumpMap()) {
		return;

	}
	SetTextureRepeating(heightMap->GetTexture(), true);

#pragma endregion

#pragma region skybox
	skyboxShader = new Shader(SHADERDIR "skyboxVertex.glsl",
		SHADERDIR "skyboxFragment.glsl ");
	if (
		!skyboxShader->LinkProgram()) {
		return;

	}
	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR "rusted_west.jpg", TEXTUREDIR "rusted_east.jpg",
		TEXTUREDIR "rusted_up.jpg", TEXTUREDIR "rusted_down.jpg",
		TEXTUREDIR "rusted_south.jpg", TEXTUREDIR "rusted_north.jpg",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0
	);
	if (!cubeMap || !quad->GetTexture()) {
		return;

	}

#pragma endregion

#pragma region Shadow

	sceneShader = new Shader(SHADERDIR"shadowscenevert.glsl", SHADERDIR"shadowscenefrag.glsl");
	shadowShader = new Shader(SHADERDIR"shadowVert.glsl", SHADERDIR"shadowFrag.glsl");

	if (!sceneShader->LinkProgram() || !shadowShader->LinkProgram()) {
		return;
	}
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#pragma endregion


#pragma region people
//	peopleShader = new Shader(SHADERDIR"skeletonvertex.glsl", SHADERDIR"TexturedFragment.glsl");
	hellData = new MD5FileData(MESHDIR"hellknight.md5mesh");
	hellNode = new MD5Node(*hellData);

	hellData->AddAnim(MESHDIR"idle2.md5anim");
	hellNode->PlayAnim(MESHDIR"idle2.md5anim");
	peopleShader = new Shader(SHADERDIR "TexturedVertex.glsl",
		SHADERDIR "TexturedFragment.glsl");
	if (!peopleShader->LinkProgram()) { 
		return; 
	}

#pragma endregion




	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	camera->SetPosition(Vector3(1000.0f, 500.0f, 1000.0f));



#pragma region light
	light = new Light(Vector3(2000, 800,2000), Vector4(1, 1, 1, 300), 8000.0);

#pragma endregion 

	
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	init = true;
	
}

Renderer::~Renderer(void) {
	delete heightMap;
	delete root;
	House::DeleteHouse();
	delete quad;
	delete light;
	delete camera;
	delete reflectShader;
	delete skyboxShader;
	delete lightShader;
	delete hellData;
	delete hellNode;
	delete lighting;
	currentshader = 0;
}

void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	MoveLight(msec);
	
	waterRotate += msec / 1000.0f;
	skyRotate += msec / 1000.0f;

	viewMatrix = camera->BuildViewMatrix();
	root->Update(msec);

	hellNode->Update(msec);
}


//void Renderer::LoadScene(std::string OBJname) {
//	OBJMesh* tempMesh = new OBJMesh();
//	tempMesh->LoadOBJMesh(OBJname);
//	sceneMesh = tempMesh;
//}
void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	DrawShadowScene();
	DrawCombinedScene();
	
	SwapBuffers();

}


void Renderer::MoveLight(float msec)
{
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_G)) {
		light->SetPosition (Vector3(light->GetPosition()) + (Vector3(1, 0, 0) * 100));
		
	}
	
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_J)) {
			light->SetPosition(Vector3(light->GetPosition()) + (Vector3(-1, 0, 0) * 100));

		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_Y)) {
			light->SetPosition(Vector3(light->GetPosition()) + (Vector3(0, 1, 0) * 100));

		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_H)) {
			light->SetPosition(Vector3(light->GetPosition()) + (Vector3(0, -1, 0) * 100));

		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_T)) {
			light->SetPosition(Vector3(light->GetPosition()) + (Vector3(0, 0, 1) * 100));

		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_U)) {
			light->SetPosition(Vector3(light->GetPosition()) + (Vector3(0, 0, -1) * 100));

		
	}
	/*	
		if (autolight)
			light->SetPosition(Vector3(light->GetPosition()) + (Vector3(1, 0, 0) * 100)* msec);


		if (Window::GetKeyboard()->KeyDown(KEYBOARD_X)) {
			this->autolight = false;
			(this->light)->SetPosition( (this->light)->GetPosition());
		}*/

	

}

void Renderer::DrawHouse()
{
	SetCurrentShader(houseShader);
	SetShaderLight(*light);

	glUseProgram(currentshader->GetProgram());
	glUniform1i(glGetUniformLocation(currentshader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentshader->GetProgram(),
		"bumpTex"), 1);

	glUniform3fv(glGetUniformLocation(currentshader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());
	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	modelMatrix =
		Matrix4::Translation(Vector3(3000, 0, 3000)) *
		Matrix4::Scale(Vector3(2, 1, 2)) *
		Matrix4::Rotation(0, Vector3(0.0f, 1.0f, .0f));

	UpdateShaderMatrices();
	SetShaderLight(*light);



	DrawNode(root);

	glUseProgram(0);
}

void Renderer::DrawNode(SceneNode* n) {

	if (n->GetMesh()) {
		Matrix4 transform = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(currentshader->GetProgram(), "modelMatrix"), 1, false, (float*)&transform);
		//location, size count,transpose,value


		glUniform1i(glGetUniformLocation(currentshader->GetProgram(), "useTexture"), (int)n->GetMesh()->GetTexture());
		//location valuw
		n->Draw(*this);
	}

	for (vector < SceneNode* >::const_iterator i = n->GetChildIteratorStart(); i != n->GetChildIteratorEnd(); ++i) {
		DrawNode(*i);
	}

}

void Renderer::DrawWater() {
	SetCurrentShader(reflectShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(currentshader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(currentshader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentshader->GetProgram(),
		"bumpTex"), 1);

	glUniform1i(glGetUniformLocation(currentshader->GetProgram(),
		"cubeTex"), 2);



	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	float heightX = (RAW_WIDTH * HEIGHTMAP_X);

	float heightY = 256 * HEIGHTMAP_Y / 3.0f;

	float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z);

	modelMatrix =
		Matrix4::Translation(Vector3(heightX, heightY, heightZ)) *
		Matrix4::Scale(Vector3(heightX, 1, heightZ)) *
		Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));

	//textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) *
	//	Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));
	textureMatrix = Matrix4::Scale(Vector3(3.0f, 3.0f, 3.0f)) *
		Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));


	UpdateShaderMatrices();

	quad->Draw();

	glUseProgram(0);

}

void Renderer::DrawMesh()
{
	SetCurrentShader(reflectShader);
	//SetCurrentShader(peopleShader);
	SetShaderLight(*light);
	modelMatrix.ToIdentity();
	//modelMatrix.SetPositionVector(Vector3(2000,100,2000));

	modelMatrix =
		Matrix4::Translation(Vector3(800, 400, 800)) *
		Matrix4::Scale(Vector3(1, 1, 1)) *
		Matrix4::Rotation(160, Vector3(0.0f, 1.0f, .0f));



	UpdateShaderMatrices();
	Matrix4 tempMatrix = textureMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(currentshader->GetProgram(), "textureMatrix"), 1, false, *&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentshader->GetProgram(), "modelMatrix"), 1, false, *&modelMatrix.values);

	


	hellNode->Draw(*this);

	glUseProgram(0);
}
void Renderer::DrawHeightmap(){
	SetCurrentShader(lightShader);
	SetShaderLight(*light)
		;


	glUniform3fv(glGetUniformLocation(currentshader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(currentshader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentshader->GetProgram(),
		"bumpTex"), 1);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();


	modelMatrix =
		Matrix4::Translation(Vector3(-1000, 0, -1000)) *
		Matrix4::Scale(Vector3(2, 1, 2)) *
		Matrix4::Rotation(0, Vector3(0.0f, 1.0f, .0f));

	UpdateShaderMatrices();

	heightMap->Draw();

	glUseProgram(0);

}
void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);

	UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);

}

void Renderer::DrawLighting()
{
	SetCurrentShader(lightingShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(currentshader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(currentshader->GetProgram(),
		"diffuseTex"), 0);



	modelMatrix =
		Matrix4::Translation(Vector3(30,1000,30)) *
		Matrix4::Scale(Vector3(100,100,100)) *
		Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));

	//textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) *
	//	Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));
	/*textureMatrix = Matrix4::Scale(Vector3(3.0f, 3.0f, 3.0f)) *
		Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));*/


	UpdateShaderMatrices();

	lighting->Draw();

	glUseProgram(0);


}


void Renderer::DrawShadowScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	SetCurrentShader(shadowShader);

	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3(0, 0, 0));
	textureMatrix = biasMatrix * (projMatrix * viewMatrix);
	UpdateShaderMatrices();

	//DrawHouse();
	//DrawMesh();
	//DrawWater();
	//DrawHeightmap();

	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawCombinedScene()
{
	SetCurrentShader(sceneShader);
	glUniform1i(glGetUniformLocation(currentshader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentshader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentshader->GetProgram(), "shadowTex"), 2);

	glUniform3fv(glGetUniformLocation(currentshader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	SetShaderLight(*light);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();
	DrawSkybox();

	/*DrawHouse();
	DrawMesh();
	DrawWater();
	DrawHeightmap();*/
	DrawLighting();


	glUseProgram(0);
}














/*quad = Mesh::GenerateQuad();
	RendererLoader(quad, reflectShader, "PerPixelVertex.glsl", "reflectFragment.glsl", "water.tga", "water.jpg", 1);*/

void Renderer::RendererWater()
{
	quad = Mesh::GenerateQuad();
	
	reflectShader = new Shader(SHADERDIR"PerPixelVertex1.glsl",
		SHADERDIR "reflectFragment1.glsl");
	/*reflectShader = new Shader(SHADERDIR"PerPixelVertex.glsl", SHADERDIR "reflectFragment.glsl");*/
	if (!reflectShader->LinkProgram()) {
		return;
	}
	
	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "water.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	quad->SetBumpMap(SOIL_load_OGL_texture(
		TEXTUREDIR "water.jpg", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));


	if (!quad->GetTexture() || !quad->GetBumpMap()) {
		return;

	}
	quad->GenerateNormals();
	quad->GenerateTangents();
	SetTextureRepeating(quad->GetTexture(),1);
	SetTextureRepeating(quad->GetBumpMap(),1);
	
	waterRotate = 0.0f;
}

void Renderer::RendererLoader(Mesh* mesh, Shader* shader_Program, string vertex_shader,
	string Fragment_shader, string D_texture, string N_texture,bool texture_repeat)
{
	shader_Program = new Shader(SHADERDIR + vertex_shader, SHADERDIR + Fragment_shader);
	if (!shader_Program->LinkProgram()) {
		return;
	}
	string D_texture_p = TEXTUREDIR + D_texture;
	string N_texture_p = TEXTUREDIR + N_texture;

	mesh->SetTexture(SOIL_load_OGL_texture(D_texture_p.c_str(),
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	mesh->SetBumpMap(SOIL_load_OGL_texture(
		N_texture_p.c_str(), SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!mesh->GetTexture() ) {
		cout << "GetTexture failed"<<endl;
		return;

	}
	else if (!mesh->GetBumpMap() ){
		cout << "GetTexture failed" << endl;
			return;

	}

	SetTextureRepeating(mesh->GetTexture(), true);
}

