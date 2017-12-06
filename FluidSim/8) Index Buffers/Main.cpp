#include "../../nclgl/window.h"
#include "Renderer.h"
#include <iostream>
#include "../../nclgl/Fluid.h"
#include "Entity.h"
#include "PhysicsLoop.h"
#include <fstream>

#pragma comment(lib, "nclgl.lib")

using namespace std;

/*values to handle returning normalized mouse co-ordinates*/


/*
TODO:

1.Real-time lighting 2 (bumpmaps)

2. Cube-mapping (skybox, reflection)

3. animate textures by manipulating tex-coords

4. edge cases for Sphere, fix fluid loss

5. sphere, sphere interaction, add/delete spheres

6. move spheres with mouse

7. independent shader variables for objects

8. clean it all up, limit frame rate


*/

void getInput(Fluid *& fluid1, Shader* shader, RenderObject* fluidObj, Renderer* r, Entity*& sphere) {
	
	//if (Window::GetKeyboard()->KeyDown(KEYBOARD_1)) {
	//	r->deleteRenderObjects();
	//	fluid1 = nullptr;
	//	fluid1 = new HeightMap("../../Textures/terrain.raw", false, false);
	//	fluid1->SetTexture(SOIL_load_OGL_texture(
	//		"../../Textures/lava.png",
	//		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	//	fluidObj = new RenderObject(fluid1, shader, SOIL_load_OGL_texture(
	//		"../../Textures/lava.png",
	//		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	//	fluidObj->SetModelMatrix(Matrix4::Translation(Vector3(0, 0, 0)));
	//	r->AddRenderObject(fluidObj);
	//	
	//}
	//if (Window::GetKeyboard()->KeyDown(KEYBOARD_2)) {
	//	r->deleteRenderObjects();
	//	//delete fluid1;
	//	delete sphere;
	//	sphere = nullptr;
	//	//fluid1 = nullptr;
	//	//HeightMap* newFluid = new HeightMap("../../Textures/terrain.raw", false, true);
	//	fluid1 = new HeightMap("../../Textures/terrain.raw", false, true);
	//	//*fluid1 = *(new HeightMap("../../Textures/terrain.raw", false, true));
	//	fluid1->SetTexture(SOIL_load_OGL_texture(
	//		"../../Textures/lava.png",
	//		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	//	fluid1->updateFluid();
	//	r->setFluid(fluid1);
	//	shader = new Shader("../../Shaders/TexturedVertex.glsl",
	//		"../../Shaders/TexturedFragment.glsl");
	//	fluidObj = new RenderObject(fluid1, shader, SOIL_load_OGL_texture(
	//		"../../Textures/lava.png",
	//		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	//	fluidObj->SetModelMatrix(Matrix4::Translation(Vector3(0, 0, 0)));
	//	r->AddRenderObject(fluidObj);
	//}
	//if (Window::GetKeyboard()->KeyDown(KEYBOARD_3)) {
	//	r->deleteRenderObjects();
	//	fluid1 = new HeightMap("../../Textures/terrain.raw", true, true);
	//	fluid1->SetTexture(SOIL_load_OGL_texture(
	//		"../../Textures/lava.png",
	//		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	//	shader = new Shader("../../Shaders/TexturedVertex.glsl",
	//		"../../Shaders/TexturedFragment.glsl");
	//	fluidObj = new RenderObject(fluid1, shader, SOIL_load_OGL_texture(
	//		"../../Textures/lava.png",
	//		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	//	fluidObj->SetModelMatrix(Matrix4::Translation(Vector3(0, 0, 0)));
	//	r->AddRenderObject(fluidObj);
	//	
	//}
	//if (Window::GetKeyboard()->KeyDown(KEYBOARD_4)) {

	//}
	

}




//move back (s) and up (shift) to see heightmap
int main() {
	Window w("Index Buffers!", 1280, 720, false);
	if (!w.HasInitialised()) {
		return -1;
	}
	_controlfp(_MCW_PC, _PC_24);
	Renderer* renderer = new Renderer(w);
	if (!renderer->HasInitialised()) {
		return -1;
	}

	
	w.LockMouseToWindow(false);
	w.ShowOSPointer(true);
	



	//Setup Shaders
	Shader* defaultShader = new Shader("../../Shaders/vertNormals.glsl", "../../Shaders/phong.glsl");
	defaultShader->LinkProgram();

	Shader* fluidShader = new Shader("../../Shaders/TexturedVertex.glsl", "../../Shaders/TexturedFragment.glsl");
	fluidShader->LinkProgram();

	Shader* textShader = new Shader("../../Shaders/TexturedVertex.glsl","../../Shaders/TexturedFragment.glsl");
	textShader->LinkProgram();

	Shader* sphereShader = new Shader("../../Shaders/TexturedVertex.glsl", "../../Shaders/TexturedFragment.glsl");
	sphereShader->LinkProgram();

	//Setup Sphere
	RenderObject* sphereObject = new RenderObject(new OBJMesh(MESHDIR"sphere.obj"), sphereShader, Renderer::LoadTexture("../../Textures/lava.png"));
	sphereObject->GetMesh()->SetTexture(SOIL_load_OGL_texture("../../Textures/lava.png",SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	//Setup Fluid
	/*CHANGE PARAMETERS HERE FOR DIFFERENT FLUID REPRESENTATIONS*/
	Fluid* fluid = new Fluid("../../Textures/terrain.raw", Processor::CPU, true, true, 257);
	fluid->SetTexture(Renderer::LoadTexture("../../Textures/lava.png"));
	fluid->SetBumpMap(SOIL_load_OGL_texture("../Textures/lava_NRM.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	RenderObject* fluidObject = new RenderObject(fluid, fluidShader, Renderer::LoadTexture("../../Textures/lava.png"));
	fluidObject->SetModelMatrix(Matrix4::Translation(Vector3(0, 0, 0)));
	

	//Entity for sphere to communicate physics updates to renderer of sphere
	Entity* sphereEnt = new Entity(Vector3(100,55,100), 10, 10, 13, sphereObject);
	


	//add renderObjects
	renderer->AddRenderObject(fluidObject);
	renderer->setFluid(fluid);
	renderer->AddRenderObject(sphereEnt->getRendObj());

	//Setup Physics
	PhysicsLoop pl;
	pl.sphere = sphereEnt;
	pl.fluid = fluid;
	
	bool updatePhys = true;

	float t = 0;
	while (true) {
		if (fluid != nullptr) {
			w.GetTimer()->GetTimedMS();
			fluid->updateFluid();
			t = w.GetTimer()->GetTimedMS();
			
			if(updatePhys) pl.updatePhysics(0.0033);
			
		}
		getInput(fluid, defaultShader, fluidObject, renderer, sphereEnt);
		if (w.GetKeyboard()->KeyTriggered(KEYBOARD_H)) {
			updatePhys = false;
		}
		renderer->UpdateScene(t);
		renderer->RenderScene();
		w.UpdateWindow();
	}



	delete fluid;
	
	
	return 0;
}