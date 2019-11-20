 #include "Renderer.h"

 Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	CubeRobot::CreateCube();
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
	 (float)width / (float)height, 45.0f);
	
	 camera = new Camera();
	camera -> SetPosition(Vector3(0, 100, 750.0f));
	
	 currentshader = new Shader(SHADERDIR "SceneVertex.glsl",SHADERDIR "SceneFragment.glsl");
	
	 quad = Mesh::GenerateQuad();
	 quad -> SetTexture(SOIL_load_OGL_texture(
	 TEXTUREDIR "stainedglass.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	
	 if (!currentshader -> LinkProgram() || !quad -> GetTexture()) {
	 return;
		}	 root = new SceneNode();
	
	for (int i = 0; i < 5; ++i) {
		SceneNode * s = new SceneNode();
		s -> SetColour(Vector4(1.0f, 1.0f, 1.0f, 0.5f));
		s -> SetTransform(Matrix4::Translation(Vector3(0, 100.0f, -300.0f + 100.0f + 100 * i)));
		s -> SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
		s -> SetBoundingRadius(100.0f);
		s -> SetMesh(quad);
		root -> AddChild(s);
		
	 }
	
		  root -> AddChild(new CubeRobot());		 glEnable(GL_DEPTH_TEST);
		  glEnable(GL_BLEND);
		  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		  init = true;
		  } Renderer ::~Renderer(void) {
	 delete root;
	 delete quad;
	 delete camera;
	 CubeRobot::DeleteCube();
	 }  void Renderer::UpdateScene(float msec) {
	 camera -> UpdateCamera(msec);
	 viewMatrix = camera -> BuildViewMatrix();
	 frameFrustum.FromMatrix(projMatrix * viewMatrix);//set frameFrustum
	
	 root -> Update(msec);
	 }  void Renderer::BuildNodeLists(SceneNode * from) {
	   if (frameFrustum.InsideFrustum(*from)) {
		  Vector3 dir = from -> GetWorldTransform().GetPositionVector() - camera -> GetPosition();
		  from -> SetCameraDistance(Vector3::Dot(dir, dir));//not nomolize   distence=|dir|*|dir|
		 
			  if (from -> GetColour().w < 1.0f) {//whether it is transparent or not
			  transparentNodeList.push_back(from);
			 
		    }
		      else {
			   nodeList.push_back(from);
			  
		      }
		  
	  }
	  
		   for (vector < SceneNode* >::const_iterator i = from -> GetChildIteratorStart(); i != from -> GetChildIteratorEnd(); ++i) {
		   BuildNodeLists((*i));
		  
	  }
	    }   void Renderer::SortNodeLists() {
	  std::sort(transparentNodeList.begin(), transparentNodeList.end(),
	   SceneNode::CompareByCameraDistance);
	  std::sort(nodeList.begin(),nodeList.end(),
	   SceneNode::CompareByCameraDistance);
	   }   void Renderer::DrawNodes() {
	    for (vector < SceneNode* >::const_iterator i = nodeList.begin();                       //draw object from near to far
		    i != nodeList.end(); ++i) {
		    DrawNode((*i));
		   
	   }
	   
		    for (vector < SceneNode* >::const_reverse_iterator i =                              //draw  object form  far to near
			   transparentNodeList.rbegin();
			   i != transparentNodeList.rend(); ++i) {
		    DrawNode((*i));
		   
	   }
	      }   void Renderer::DrawNode(SceneNode * n) {
	if (n -> GetMesh()) {
		glUniformMatrix4fv(glGetUniformLocation(
		currentshader -> GetProgram(), " modelMatrix "), 
			1, false,
		(float*) & (n -> GetWorldTransform()* Matrix4::Scale(n -> GetModelScale()))
		);
		
		glUniform4fv(glGetUniformLocation(currentshader -> GetProgram(),
		 "nodeColour"), 1, (float*)&n -> GetColour());
		
		 glUniform1i(glGetUniformLocation(currentshader -> GetProgram(),
		 "useTexture"), (int)n -> GetMesh() -> GetTexture());
		
		 n -> Draw(*this);
		
	   }
	   
   }


   void Renderer::RenderScene() {
	BuildNodeLists(root);
	SortNodeLists();
	
	 glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	 glUseProgram(currentshader -> GetProgram());
	UpdateShaderMatrices();
	
	 glUniform1i(glGetUniformLocation(currentshader -> GetProgram(),
	   "diffuseTex"), 0);
	DrawNodes();
	
	 glUseProgram(0);
	SwapBuffers();
	ClearNodeLists();
	
   }
    void Renderer::ClearNodeLists() {                
	   transparentNodeList.clear();
	   nodeList.clear();
	   
   }