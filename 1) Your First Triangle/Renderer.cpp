#include "Renderer.h"

Renderer::Renderer(Window& parent) :OGLRenderer(parent) {
	triangle = Mesh::GenerateTrangle();
	currentShader = new Shader(SHADERDIR"basicVertex.glsl", SHADERDIR"colourFragment.glsl");

	if (!currentShader->LinkProgram()) {
		return;
	}
	init = true;
}

Renderer::~Renderer(void) {
	delete triangle;
}
