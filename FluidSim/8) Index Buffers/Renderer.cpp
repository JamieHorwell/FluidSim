#include "Renderer.h"
#include <algorithm>
#include "../../nclgl/Matrix3.h"
Renderer::Renderer(Window & parent) : OGLRenderer(parent) {

	//setup camera
	camera = new Camera(-14, 266, Vector3(-167, 100, 71));
	

	//setup basic shaders
	currentShader = new Shader("../../Shaders/bumpVertex.glsl",
		"../../Shaders/bumpFragment.glsl");



	
	text = LoadTexture("../../Textures/lava.png");
	

	font = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

	if (!currentShader->LinkProgram()) {
		return;

	}

	light = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f),
		 500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)),
		 Vector4(1, 1, 1, 1), (RAW_WIDTH * HEIGHTMAP_X) * 5.0f);

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	myfile.open("FluidResults.txt");
	//SetShaderLight(Vector3(0, 200.0f, 0), Vector3(1, 1, 1), 2000.0f);

	init = true;

}


Renderer::~Renderer(void) {
	myfile.close();
	delete fluid;
	delete camera;
	delete font;
	delete textToDisplay;
	
}





void	Renderer::Render(const RenderObject &o) {
	Matrix3 rotation = Matrix3(viewMatrix);
	 Vector3 invCamPos = viewMatrix.GetPositionVector();
	
	Vector3 camPos = rotation * -invCamPos; // invert the
	
		


	modelMatrix = o.GetWorldTransform();
	if (o.GetShader() && o.GetMesh() && o.GetTexture()) {
		SetTextureRepeating(o.GetMesh()->GetTexture(), true);
		glUseProgram(currentShader->GetProgram());
		
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
		glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1,(float*)&camPos);
		UpdateShaderMatrices();
		
		o.GetMesh()->Draw();
	}

	for (vector<RenderObject*>::const_iterator i = o.GetChildren().begin(); i != o.GetChildren().end(); ++i) {
		Render(*(*i));
	}
}









void Renderer::UpdateScene(float msec) {
	
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SetShaderLight(*light);
	

	for (vector<RenderObject*>::iterator i = renderObjects.begin(); i != renderObjects.end(); ++i) {
		if ((*i) != nullptr) {
			if ((*i)->GetMesh() != NULL) {

				(*i)->Update(msec);
				Render(*(*i));
			}
		}
		
	}
	totaltime += msec;
	int newfps = 1 / (msec / 1000);
	if (newfps > maxfps) {
		maxfps = newfps;
	}
	string textToDisplay = "FPS: " + std::to_string(maxfps);
	//textToDisplay = "msec: " + std::to_string(msec);
	DrawText(textToDisplay, Vector3(0, 22, 0), 18);

	//check total fluid every so often
	count++;
	if (fluid != nullptr) {
		if (count > 100) {
			totalFluid = fluid->totalFluid();
			if (totaltime < 30000) {
				myfile << totalFluid << "\n";
			}
			else {
				myfile.close();
			}
			count = 0;
		}
		textToDisplay = "Total Fluid: " + std::to_string(totalFluid);
		DrawText(textToDisplay, Vector3(0, 42, 0), 18);
		textToDisplay = fluid->getTitle();
		DrawText(textToDisplay, Vector3(0, 0, 0), 20);
	}
	


	SwapBuffers();


}






void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(currentShader->GetProgram());
	UpdateShaderMatrices();
	SetShaderLight(*light);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"bumpTex"), 1);
	glEnable(GL_DEPTH_TEST);



	glUseProgram(0);
	SwapBuffers();

}


void Renderer::DrawText(const std::string &text, const Vector3 &position, const float size) {
	
	//Create a new temporary TextMesh, using our line of text and our font
	TextMesh* mesh = new TextMesh(text, *font);


		//store current matrices
	Matrix4 tempMM = modelMatrix;
	Matrix4 tempVM = viewMatrix;
	Matrix4 tempPM = projMatrix;


		modelMatrix = Matrix4::Translation(Vector3(position.x, height - position.y, position.z)) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
	
	//Either way, we update the matrices, and draw the mesh
	UpdateShaderMatrices();
	//draw our text
	mesh->Draw();

	//reset our matrices
	modelMatrix = tempMM;
	viewMatrix = tempVM;
	projMatrix = tempPM;
	UpdateShaderMatrices();

	delete mesh; //Once it's drawn, we don't need it anymore!
}

void Renderer::deleteRenderObjects()
{
	for (int i = 0; i< renderObjects.size(); i++)
	{
		delete (renderObjects[i]);
	}
	renderObjects.clear();
}





GLuint Renderer::LoadTexture(string name)
{
	GLuint textName = SOIL_load_OGL_texture(name.c_str(), SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	glBindTexture(GL_TEXTURE_2D, textName);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	return textName;
}






