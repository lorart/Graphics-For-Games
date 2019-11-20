#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/SceneNode.h"
#include "House.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/MD5Mesh.h"
#include "../../nclgl/MD5Node.h"


#define SHADOWSIZE 2048

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	void LoadMaster();

	virtual void UpdateScene(float msec);
	void LoadScene(std::string OBJname);
	virtual void RenderScene();
	 void MoveLight(float msec);
	


protected:
	void DrawMesh();
	void DrawNode(SceneNode* n);
	void DrawHouse();
	void DrawHeightmap();
	void DrawWater();
	void DrawSkybox();
	void DrawShadowScene();
	void DrawCombinedScene();
	void DrawLighting();

	void RendererWater();
	void RendererLoader(Mesh* mesh, Shader* shader_Program, string vertex_shader, string Fragment_shader, string D_texture_name,string N_texture_name,  bool texture_repeat);

	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* houseShader;
	Shader* sceneShader;
	Shader* shadowShader;
	Shader* peopleShader;
	Shader* lightingShader;

	HeightMap* heightMap;
	Mesh* quad;
	Mesh* lighting;


	MD5FileData* hellData;
	MD5Node* hellNode;


	SceneNode* root;

	Camera* camera;
	Light* light;

	GLuint cubeMap;
	GLuint shadowTex;
	GLuint shadowFBO;

	float waterRotate;
	float skyRotate;
	bool autolight=TRUE;

};
