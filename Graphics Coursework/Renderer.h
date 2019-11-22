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
	 void UpdateHeightmap(float msec);

	


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
	void DrawFloor();



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
	Shader* mapShader;


	HeightMap* heightMap;
	Mesh* quad;
	Mesh* lighting;
	Mesh* floor;
	Mesh* map;


	MD5FileData* hellData;
	MD5Node* hellNode;


	SceneNode* root;

	Camera* camera;
	Camera* mapcamera;
	Light* light;

	GLuint cubeMap;
	GLuint shadowTex;
	GLuint shadowFBO;


	GLuint miniMapTex;
	GLuint miniMapFBO;
	void DrawMiniMapFBO();
	void DrawMiniMap();
	Mesh* miniMapQuad;
	GLuint miniMapDepthTex;

	float waterRotate;
	float skyRotate;
	float timeValue ;



	bool autolight=TRUE;
	bool usingMap=TRUE;



	
};
