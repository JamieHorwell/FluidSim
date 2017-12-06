#pragma once
#include <vector>
#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/camera.h"
#include "../../nclgl/Fluid.h"
#include "../../nclgl/TextMesh.h"
#include "../../nclgl/RenderObject.h"
#include "../../nclgl/OBJMesh.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/Light.h"


class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);



	virtual void Render(const RenderObject &o);

	virtual void RenderScene();

	virtual void UpdateScene(float msec);

	void	DrawText(const std::string &text, const Vector3 &position, const float size = 10.0f);

	void	AddRenderObject(RenderObject* r) {
		renderObjects.push_back(r);
	}

	void deleteRenderObjects();
	
	void setFluid(Fluid* fluid) { this->fluid = fluid; };

	static GLuint LoadTexture(string name);

	vector<RenderObject*> renderObjects;
protected:
	Fluid* fluid = NULL;
	Font*	font;	
	Camera * camera;
	RenderObject* textToDisplay;
	GLint text; 
		int count = 0;	double totalFluid = 0;	int maxfps = 0;	float totaltime = 0;	Light* light;	ofstream myfile;	std::string fps;	std::string fluidTotal;	Shader* fluidShader;	Shader* sphereShader;	Shader* textShader;};

