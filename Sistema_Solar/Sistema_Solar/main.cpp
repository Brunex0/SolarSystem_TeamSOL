// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
using namespace glm;

#include "common/shader.hpp"
#include "common/texture.hpp" 
#include "common/controls.hpp" 
#include "common/objloader.hpp"
#include "common/shader.cpp"
#include "common/texture.cpp" 

int screenWidth, screenHeight;
int currentMenu = 1, selectedMenu1 = 1, selectedMenu3 = 1;

#include "common/controls.cpp" 
#include "common/objloader.cpp"

typedef struct {
	GLuint VertexArrayID;
	GLuint MatrixID;
	GLuint Texture;
	GLuint TextureID;
	GLuint uvbuffer;
	GLuint vertexbuffer;
}INFO;

INFO Background, Sun, Mercury, Venus, Earth, Mars, Jupiter, Saturn, Uranus, Neptune, Ring, Asteroid, Moon, MenuBackground, Menu1, Menu2, Menu3;
INFO MercuryHistory, VenusHistory, EarthHistory, MarsHistory, JupiterHistory, SaturnHistory, UranusHistory, NeptuneHistory, AsteroidBeltHistory, MoonHistory, SunHistory;

typedef struct {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
}VECTORS;

typedef struct {
	glm::mat4 self_rotation;
	glm::mat4 center_rotation;
	glm::mat4 translation;
	glm::mat4 scaling;
	float xPosition;
	float yPosition;
	float zPosition;
}TRANSFORMATIONS;

typedef struct {
	float center_Rotation;
	float self_Rotation;
}planetRotation;

typedef struct {
	planetRotation sun;
	planetRotation mercury;
	planetRotation venus;
	planetRotation earth;
	planetRotation mars;
	planetRotation jupiter;
	planetRotation saturn;
	planetRotation uranus;
	planetRotation neptune;
	planetRotation moon;
	planetRotation asteroidBelt;
}PLANETSROTATIONS;

typedef struct {
	float sun;
	float mercury;
	float venus;
	float earth;
	float mars;
	float jupiter;
	float saturn;
	float uranus;
	float neptune;
	float moon;
	float asteroidBelt;
}DIMENSIONS;


DIMENSIONS Real, Fake, Using;
PLANETSROTATIONS UsingRotation;

VECTORS Sphere, Back, VRing, VAsteroid, Menu;

TRANSFORMATIONS transfBackground;
TRANSFORMATIONS transfSun;
TRANSFORMATIONS transfMercury;
TRANSFORMATIONS transfVenus;
TRANSFORMATIONS transfEarth;
TRANSFORMATIONS transfMars;
TRANSFORMATIONS transfJupiter;
TRANSFORMATIONS transfSaturn;
TRANSFORMATIONS transfUranus;
TRANSFORMATIONS transfNeptune;
TRANSFORMATIONS transfRing;
TRANSFORMATIONS transfAsteroid;
TRANSFORMATIONS transfMoon;
TRANSFORMATIONS transfList;
TRANSFORMATIONS transfHistory;
TRANSFORMATIONS randomAsteroid[100];

int AU = 150;
float rotationScale = 0.5;

#include "transferDataToGPU.hpp"
#include "draw.hpp"
void resetPosPlanetsList();
void selectingMenu1();
void selectingMenu3();
void declareDimensions();
void incrementRotations();
void CelestialBodiesPosition();
void deleteBuffers();

bool enterPlanet = true;
int exitProgram = 0;
GLuint programID;

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Open a window and create its OpenGL context
	
	window = glfwCreateWindow(1228, 921, "Solar System", NULL, NULL);
	glfwSetWindowPos(window, (1920 - 1228) / 2, (1080 - 921) / 2); //window Pos center
	
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	// Black background
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// Create and compile our GLSL program from the shaders

	programID = LoadShaders("shaders/TransformVertexShader.vertexshader", "shaders/TextureFragmentShader.fragmentshader");
	// Object---------------------------------------------------------
	const char* objectBackground = "Objetos/background.obj";
	const char* object = "Objetos/sphere.obj";
	const char* objectRing = "Objetos/ring.obj";
	const char* objectAsteroid = "Objetos/Asteroid1.obj";
	const char* objectMenu = "Objetos/Menu1.obj";

	// Texture-------------------------------------------------------
	const char* textureBackGround = "textures/background.DDS";
	const char* textureSun = "textures/sun.DDS";
	const char* textureMercury = "textures/mercury.DDS";
	const char* textureVenus = "textures/venus.DDS";
	const char* textureEarth = "textures/terra.DDS";
	const char* textureMars = "textures/mars.DDS";
	const char* textureJupiter = "textures/jupiter.DDS";
	const char* textureSaturn = "textures/saturn.DDS";
	const char* textureUranus = "textures/uranus.DDS";
	const char* textureNeptune = "textures/neptune.DDS";
	const char* textureRing = "textures/saturn_ring.DDS";
	const char* textureAsteroid = "textures/asteroid.DDS";
	const char* textureMoon = "textures/moon.DDS";

	const char* textureMenu = "textures/Menu/backgroundMenu.DDS";
	const char* textureMenu1 = "textures/Menu/menu1.DDS";
	const char* textureMenu2 = "textures/Menu/menu2.DDS";
	const char* textureMenu3 = "textures/Menu/menu3.DDS";

	//History
	const char* textureMercuryHistory = "textures/Historia/mercury.DDS";
	const char* textureVenusHistory = "textures/Historia/venus.DDS";
	const char* textureEarthHistory = "textures/Historia/earth.DDS";
	const char* textureMarsHistory = "textures/Historia/mars.DDS";
	const char* textureJupiterHistory = "textures/Historia/jupiter.DDS";
	const char* textureSaturnHistory = "textures/Historia/saturn.DDS";
	const char* textureUranusHistory = "textures/Historia/uranus.DDS";
	const char* textureNeptuneHistory = "textures/Historia/neptune.DDS";
	const char* textureSunHistory = "textures/Historia/sun.DDS";
	const char* textureMoonHistory = "textures/Historia/moon.DDS";
	const char* textureAsteroidBeltHistory = "textures/Historia/asteroidBelt.DDS";



	//---------------------------------------------------------
	declareDimensions();
	Using = Real;
	rotationScale = 0.5f;
	CelestialBodiesPosition();


	//---------------------------------------------------------
	transferDataToGPU(&MenuBackground, &Back, programID, textureMenu, objectBackground);
	transferDataToGPU(&Menu1, &Menu, programID, textureMenu1, objectMenu);
	transferDataToGPU(&Menu2, &Menu, programID, textureMenu2, objectMenu);
	transferDataToGPU(&Menu3, &Menu, programID, textureMenu3, objectMenu);
	

	transferDataToGPU(&Background, &Back, programID, textureBackGround, objectBackground);
	transferDataToGPU(&Sun, &Sphere, programID, textureSun, object);
	transferDataToGPU(&Mercury, &Sphere, programID, textureMercury, object);
	transferDataToGPU(&Venus, &Sphere, programID, textureVenus, object);
	transferDataToGPU(&Earth, &Sphere, programID, textureEarth, object);
	transferDataToGPU(&Mars, &Sphere, programID, textureMars, object);
	transferDataToGPU(&Jupiter, &Sphere, programID, textureJupiter, object);
	transferDataToGPU(&Saturn, &Sphere, programID, textureSaturn, object);
	transferDataToGPU(&Uranus, &Sphere, programID, textureUranus, object);
	transferDataToGPU(&Neptune, &Sphere, programID, textureNeptune, object);
	transferDataToGPU(&Ring, &VRing, programID, textureRing, objectRing);
	transferDataToGPU(&Asteroid, &VAsteroid, programID, textureAsteroid, objectAsteroid);
	transferDataToGPU(&Moon, &Sphere, programID, textureMoon, object);

	//History
	transferDataToGPU(&MercuryHistory, &Menu, programID, textureMercuryHistory, objectMenu);
	transferDataToGPU(&VenusHistory, &Menu, programID, textureVenusHistory, objectMenu);
	transferDataToGPU(&EarthHistory, &Menu, programID, textureEarthHistory, objectMenu);
	transferDataToGPU(&MarsHistory, &Menu, programID, textureMarsHistory, objectMenu);
	transferDataToGPU(&JupiterHistory, &Menu, programID, textureJupiterHistory, objectMenu);
	transferDataToGPU(&SaturnHistory, &Menu, programID, textureSaturnHistory, objectMenu);
	transferDataToGPU(&UranusHistory, &Menu, programID, textureUranusHistory, objectMenu);
	transferDataToGPU(&NeptuneHistory, &Menu, programID, textureNeptuneHistory, objectMenu);
	transferDataToGPU(&SunHistory, &Menu, programID, textureSunHistory, objectMenu);
	transferDataToGPU(&MoonHistory, &Menu, programID, textureMoonHistory, objectMenu);
	transferDataToGPU(&AsteroidBeltHistory, &Menu, programID, textureAsteroidBeltHistory, objectMenu);

	
	int j = 0;
	float i = 0;
	//Random seed
	time_t t;
	srand((unsigned) time(&t));
	for (i = 0, j = 0; i < 360; i += 3.6, j++)
	{
		//float cent_Rotation = (((float)(rand()) / RAND_MAX) * i) + i;
		float cent_Rotation = (rand() % 4) + i - 0.4;
		float transl = ((rand() % 10) + AU - 10);
		float scal = ((float)(rand()) / RAND_MAX) * 0.1f;

		randomAsteroid[j].self_rotation = glm::rotate(glm::mat4(1.0), glm::radians(1.0f), glm::vec3(0, 1, 0));
		randomAsteroid[j].center_rotation = glm::rotate(glm::mat4(1.0), glm::radians(cent_Rotation), glm::vec3(0, 1, 0));
		randomAsteroid[j].translation = glm::translate(glm::mat4(1.0), glm::vec3(transl, 0.0f, 0.0f));
		randomAsteroid[j].scaling = glm::scale(glm::mat4(1.0), glm::vec3(scal));
	}

	
	do {
		if (currentMenu == 1)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			draw(&Background, transfBackground, programID, GL_TRIANGLES, &Back.vertices);
			selectingMenu1();
		}
		if (currentMenu == 2)
		{
			// Clear the screen
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) {
				Using = Real;
				rotationScale = 0.5;
			}

			if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
				Using = Fake;
				rotationScale = 0.05;
			}
			if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE) {
				currentMenu = 1;
				position = glm::vec3(112.64f, 1.29f, -8.26f);
				horizontalAngle = -1.51f;
				verticalAngle = 0.0f;
			}
			incrementRotations();
			CelestialBodiesPosition();


			draw(&Background, transfBackground, programID, GL_TRIANGLES, &Back.vertices);
			draw(&Sun, transfSun, programID, GL_TRIANGLES, &Sphere.vertices);
			draw(&Mercury, transfMercury, programID, GL_TRIANGLES, &Sphere.vertices);
			draw(&Venus, transfVenus, programID, GL_TRIANGLES, &Sphere.vertices);
			draw(&Earth, transfEarth, programID, GL_TRIANGLES, &Sphere.vertices);
			draw(&Mars, transfMars, programID, GL_TRIANGLES, &Sphere.vertices);
			draw(&Jupiter, transfJupiter, programID, GL_TRIANGLES, &Sphere.vertices);
			draw(&Saturn, transfSaturn, programID, GL_TRIANGLES, &Sphere.vertices);
			draw(&Uranus, transfUranus, programID, GL_TRIANGLES, &Sphere.vertices);
			draw(&Neptune, transfNeptune, programID, GL_TRIANGLES, &Sphere.vertices);
			draw(&Ring, transfRing, programID, GL_TRIANGLES, &VRing.vertices);
			draw(&Moon, transfMoon, programID, GL_TRIANGLES, &Sphere.vertices);
			for (int i = 0; i < 100; i++)
			{
				draw(&Asteroid, randomAsteroid[i], programID, GL_TRIANGLES, &VAsteroid.vertices);
			}

		}
		if (currentMenu == 3)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) {
				Using = Real;
				rotationScale = 0.5;
			}

			if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
				Using = Fake;
				rotationScale = 0.05;
			}

			//incrementRotations();
			CelestialBodiesPosition();
			selectingMenu3();
		}
		if (currentMenu == 4) {
				glfwSetWindowShouldClose(window, 1);
		}
		
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while ((glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0));


	deleteBuffers();
	

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

void CelestialBodiesPosition() {
	//Planets in the list
	transfList.center_rotation = glm::rotate(glm::mat4(1.0), glm::radians(1.0f), glm::vec3(0, 1, 0));
	transfList.self_rotation = glm::rotate(glm::mat4(1.0), glm::radians(1.0f), glm::vec3(0, 1, 0));
	transfList.translation = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 0.0f));
	transfList.scaling = glm::scale(glm::mat4(1.0), glm::vec3(15.0f));
	
	//Position of the history
	transfHistory.center_rotation = glm::rotate(glm::mat4(1.0), glm::radians(1.0f), glm::vec3(0, 1, 0));
	transfHistory.self_rotation = glm::rotate(glm::mat4(1.0), glm::radians(1.0f), glm::vec3(0, 1, 0));
	transfHistory.translation = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 80.0f));
	transfHistory.scaling = glm::scale(glm::mat4(1.0), glm::vec3(20.0f));

	//Background
	transfBackground.center_rotation = glm::rotate(glm::mat4(1.0), glm::radians(1.0f), glm::vec3(0, 1, 0));
	transfBackground.self_rotation = glm::rotate(glm::mat4(1.0), glm::radians(1.0f), glm::vec3(0, 1, 0));
	transfBackground.translation = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 0.0f));
	transfBackground.scaling = glm::scale(glm::mat4(1.0), glm::vec3(400.0f));

	//Sol
	transfSun.center_rotation = glm::rotate(glm::mat4(1.0), glm::radians(UsingRotation.sun.center_Rotation), glm::vec3(0, 1, 0));
	transfSun.self_rotation = glm::rotate(glm::mat4(1.0), glm::radians(UsingRotation.sun.self_Rotation), glm::vec3(0, 1, 0));
	transfSun.translation = glm::translate(glm::mat4(1.0), glm::vec3(Using.sun, 0.0f, 0.0f));
	transfSun.scaling = glm::scale(glm::mat4(1.0), glm::vec3(19.0f));

	//Mercury
	transfMercury.center_rotation = glm::rotate(glm::mat4(1.0), glm::radians(UsingRotation.mercury.center_Rotation), glm::vec3(0, 1, 0));
	transfMercury.self_rotation = glm::rotate(glm::mat4(1.0), glm::radians(UsingRotation.mercury.self_Rotation), glm::vec3(0, 1, 0));
	transfMercury.translation = glm::translate(glm::mat4(1.0), glm::vec3(Using.mercury, 0.0f, 0.0f));
	transfMercury.scaling = glm::scale(glm::mat4(1.0), glm::vec3(0.4f));

	//Venus
	transfVenus.center_rotation = glm::rotate(glm::mat4(1.0), glm::radians(UsingRotation.venus.center_Rotation), glm::vec3(0, 1, 0));
	transfVenus.self_rotation = glm::rotate(glm::mat4(1.0), glm::radians(UsingRotation.venus.self_Rotation), glm::vec3(0, 1, 0));
	transfVenus.translation = glm::translate(glm::mat4(1.0), glm::vec3(Using.venus, 0.0f, 0.0f));
	transfVenus.scaling = glm::scale(glm::mat4(1.0), glm::vec3(0.9f));

	//Terra
	transfEarth.self_rotation = glm::rotate(glm::mat4(1.0), glm::radians(UsingRotation.earth.self_Rotation), glm::vec3(0, 1, 0));
	transfEarth.center_rotation = glm::rotate(glm::mat4(1.0), glm::radians(UsingRotation.earth.center_Rotation), glm::vec3(0, 1, 0));
	transfEarth.translation = glm::translate(glm::mat4(1.0), glm::vec3(Using.earth, 0.0f, 0.0f));
	transfEarth.scaling = glm::scale(glm::mat4(1.0), glm::vec3(1.0f));

	//Moon 
	//mvp *centerRot(around sun) * trans(original pos around earth) * centerRot(around earth) * trans(back to earth's orbit)
	glm::mat4 moontransRot = glm::mat4(1.0);
	moontransRot = glm::rotate(moontransRot, glm::radians(UsingRotation.moon.self_Rotation), glm::vec3(0, 1, 0));
	transfMoon.self_rotation = glm::translate(moontransRot, glm::vec3(0.0, 0.0f, 3.0f));
	transfMoon.self_rotation = glm::rotate(transfMoon.self_rotation, glm::radians(UsingRotation.moon.self_Rotation), glm::vec3(0, 1, 0));

	transfMoon.center_rotation = glm::rotate(glm::mat4(1.0), glm::radians(UsingRotation.moon.center_Rotation), glm::vec3(0, 1, 0));
	transfMoon.translation = glm::translate(glm::mat4(1.0), glm::vec3(Using.moon, 0.0f, 0.0f));
	transfMoon.scaling = glm::scale(glm::mat4(1.0), glm::vec3(0.25f));


	//Marte
	transfMars.self_rotation = glm::rotate(glm::mat4(1.0), glm::radians(UsingRotation.mars.self_Rotation), glm::vec3(0, 1, 0));
	transfMars.center_rotation = glm::rotate(glm::mat4(1.0), glm::radians(UsingRotation.mars.center_Rotation), glm::vec3(0, 1, 0));
	transfMars.translation = glm::translate(glm::mat4(1.0), glm::vec3(Using.mars, 0.0f, 0.0f));
	transfMars.scaling = glm::scale(glm::mat4(1.0), glm::vec3(0.5f));

	//Jupiter
	transfJupiter.self_rotation = glm::rotate(glm::mat4(1.0), glm::radians(UsingRotation.jupiter.self_Rotation), glm::vec3(0, 1, 0));
	transfJupiter.center_rotation = glm::rotate(glm::mat4(1.0), glm::radians(UsingRotation.jupiter.center_Rotation), glm::vec3(0, 1, 0));
	transfJupiter.translation = glm::translate(glm::mat4(1.0), glm::vec3(Using.jupiter, 0.0f, .0f));
	transfJupiter.scaling = glm::scale(glm::mat4(1.0), glm::vec3(11.0f));

	//Saturno
	glm::mat4 saturnInc  = glm::mat4(1.0);
	saturnInc = glm::rotate(saturnInc, glm::radians(45.0f), glm::vec3(0, 0, 1));
	transfSaturn.self_rotation = glm::rotate(saturnInc, glm::radians(UsingRotation.saturn.self_Rotation), glm::vec3(0, 1, 0));
	transfSaturn.center_rotation = glm::rotate(glm::mat4(1.0), glm::radians(UsingRotation.saturn.center_Rotation), glm::vec3(0, 1, 0));
	transfSaturn.translation = glm::translate(glm::mat4(1.0), glm::vec3(Using.saturn, 0.0f, 0.0f));
	transfSaturn.scaling = glm::scale(glm::mat4(1.0), glm::vec3(9.0f));

	//Saturn Ring
	transfRing.center_rotation = glm::rotate(glm::mat4(1.0), glm::radians(UsingRotation.saturn.center_Rotation), glm::vec3(0, 1, 0));
	transfRing.self_rotation = glm::rotate(glm::mat4(1.0), glm::radians(45.0f), glm::vec3(0, 0, 1));
	transfRing.translation = glm::translate(glm::mat4(1.0), glm::vec3(Using.saturn, 0.0f, 0.0f));
	transfRing.scaling = glm::scale(glm::mat4(1.0), glm::vec3(25.0f, 1.0f, 25.0f));

	//Urano
	transfUranus.self_rotation = glm::rotate(glm::mat4(1.0), glm::radians(UsingRotation.uranus.self_Rotation), glm::vec3(0, 1, 0));
	transfUranus.center_rotation = glm::rotate(glm::mat4(1.0), glm::radians(UsingRotation.uranus.center_Rotation), glm::vec3(0, 1, 0));
	transfUranus.translation = glm::translate(glm::mat4(1.0), glm::vec3(Using.uranus, 0.0f, 0.0f));
	transfUranus.scaling = glm::scale(glm::mat4(1.0), glm::vec3(4.0f));

	//Neptuno
	transfNeptune.self_rotation = glm::rotate(glm::mat4(1.0), glm::radians(UsingRotation.neptune.self_Rotation), glm::vec3(0, 1, 0));
	transfNeptune.center_rotation = glm::rotate(glm::mat4(1.0), glm::radians(UsingRotation.neptune.center_Rotation), glm::vec3(0, 1, 0));
	transfNeptune.translation = glm::translate(glm::mat4(1.0), glm::vec3(Using.neptune, 0.0f, 0.0f));
	transfNeptune.scaling = glm::scale(glm::mat4(1.0), glm::vec3(3.0f));

	//Asteroid Belt
	transfAsteroid.self_rotation = glm::rotate(glm::mat4(1.0), glm::radians(UsingRotation.asteroidBelt.self_Rotation), glm::vec3(0, 1, 0));
	transfAsteroid.center_rotation = glm::rotate(glm::mat4(1.0), glm::radians(UsingRotation.asteroidBelt.center_Rotation), glm::vec3(0, 1, 0));
	transfAsteroid.translation = glm::translate(glm::mat4(1.0), glm::vec3(Using.asteroidBelt, 0.0f, 0.0f));
	transfAsteroid.scaling = glm::scale(glm::mat4(1.0), glm::vec3(0.1f));


}

void declareDimensions() {
	//Real scale
	Real.sun = 0.0f * AU;
	Real.mercury = 0.28f * AU;
	Real.venus = 0.367f * AU;
	Real.earth = 0.457f * AU;
	Real.moon = 0.457f * AU;
	Real.mars = 0.627f * AU;
	Real.jupiter = 1.857 * AU;
	Real.saturn = 3.297f * AU;
	Real.uranus = 6.527 * AU;
	Real.neptune = 10.157f * AU;
	Real.asteroidBelt = 1.0f * AU;
	
	//Fake scale
	Fake.sun = 0.0f * AU;
	Fake.mercury = 0.28f * AU;
	Fake.venus = 0.367f * AU;
	Fake.earth = 0.457f * AU;
	Fake.moon = 0.457f * AU;
	Fake.mars = 0.627f * AU;
	Fake.jupiter = 1.300 * AU;
	Fake.saturn = 2.297f * AU;
	Fake.uranus = 3.527 * AU;
	Fake.neptune = 5.157f * AU;
	Fake.asteroidBelt = 1.0f * AU;

	//Atribui valor random inicial
	UsingRotation.sun.center_Rotation = (float)(rand() % 365);
	UsingRotation.mercury.center_Rotation = (float)(rand() % 365);
	UsingRotation.venus.center_Rotation = (float)(rand() % 365);
	UsingRotation.earth.center_Rotation = (float)(rand() % 365);
	UsingRotation.moon.center_Rotation = UsingRotation.earth.center_Rotation;
	UsingRotation.mars.center_Rotation = (float)(rand() % 365);
	UsingRotation.jupiter.center_Rotation = (float)(rand() % 365);
	UsingRotation.saturn.center_Rotation = (float)(rand() % 365);
	UsingRotation.uranus.center_Rotation = (float)(rand() % 365);
	UsingRotation.neptune.center_Rotation = (float)(rand() % 365);
	UsingRotation.asteroidBelt.center_Rotation = (float)(rand() % 365);


	UsingRotation.sun.self_Rotation = 0.0f;
	UsingRotation.mercury.self_Rotation = 0.0f;
	UsingRotation.venus.self_Rotation = 0.0f;
	UsingRotation.earth.self_Rotation = 0.0f;
	UsingRotation.moon.self_Rotation = 0.0f;
	UsingRotation.mars.self_Rotation = 0.0f;
	UsingRotation.jupiter.self_Rotation = 0.0f;
	UsingRotation.saturn.self_Rotation = 0.0f;
	UsingRotation.uranus.self_Rotation = 0.0f;
	UsingRotation.neptune.self_Rotation = 0.0f;
	UsingRotation.asteroidBelt.self_Rotation = 0.0f;
}

void incrementRotations() {
	//Center rotation
	UsingRotation.sun.center_Rotation += 0.0f;
	UsingRotation.mercury.center_Rotation += 4.20f * rotationScale;
	UsingRotation.venus.center_Rotation += 1.43f * rotationScale;
	UsingRotation.earth.center_Rotation += 1.0f * rotationScale;
	UsingRotation.moon.center_Rotation += 1.0f * rotationScale;
	UsingRotation.mars.center_Rotation += 0.53f * rotationScale;
	UsingRotation.jupiter.center_Rotation += 0.083f * rotationScale;
	UsingRotation.saturn.center_Rotation += 0.033f * rotationScale;
	UsingRotation.uranus.center_Rotation += 0.006f * rotationScale;
	UsingRotation.neptune.center_Rotation += 0.005f * rotationScale;
	UsingRotation.asteroidBelt.center_Rotation += 0.05f * rotationScale;

	//Self rotation
	UsingRotation.sun.self_Rotation += 0.0115f;
	UsingRotation.mercury.self_Rotation += 0.017f;
	UsingRotation.venus.self_Rotation -= 0.004f;
	UsingRotation.earth.self_Rotation += 1.0f;
	UsingRotation.moon.self_Rotation += 0.037f;
	UsingRotation.mars.self_Rotation += 0.98f;
	UsingRotation.jupiter.self_Rotation += 2.513f;
	UsingRotation.saturn.self_Rotation += 2.367f;
	UsingRotation.uranus.self_Rotation += 1.412f;
	UsingRotation.neptune.self_Rotation += 1.5f;
}

void selectingMenu1()
{
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE)
	{
		if (selectedMenu1 == 3)
			selectedMenu1 = 1;
		else
			selectedMenu1++;
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE)
	{
		if (selectedMenu1 == 1)
			selectedMenu1 = 3;
		else
			selectedMenu1--;
	}
	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE)
	{
		if (selectedMenu1 == 1)
			currentMenu = 2;
		else if (selectedMenu1 == 2) {
			resetPosPlanetsList();
			currentMenu = 3;
		}

		else if (selectedMenu1 == 3)
			currentMenu = 4;
	}
	if (selectedMenu1 == 1) {

		transfList.scaling = glm::scale(glm::mat4(1.0), glm::vec3(30.0f));
		draw(&Menu1, transfList, programID, GL_TRIANGLES, &Menu.vertices);
	}
	else if (selectedMenu1 == 2) {
		transfList.scaling = glm::scale(glm::mat4(1.0), glm::vec3(30.0f));
		draw(&Menu2, transfList, programID, GL_TRIANGLES, &Menu.vertices);
	}
	else if (selectedMenu1 == 3) {
		transfList.scaling = glm::scale(glm::mat4(1.0), glm::vec3(30.0f));
		draw(&Menu3, transfList, programID, GL_TRIANGLES, &Menu.vertices);
	}
}

void selectingMenu3()
{

	draw(&Background, transfBackground, programID, GL_TRIANGLES, &Back.vertices);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE)
	{
		resetPosPlanetsList();
		if (selectedMenu3 == 11)
			selectedMenu3 = 1;
		else
			selectedMenu3++;
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE)
	{
		resetPosPlanetsList();
		if (selectedMenu3 == 1)
			selectedMenu3 = 11;
		else
			selectedMenu3--;
	}
	if (enterPlanet)
	{
		switch (selectedMenu3)
		{
		case 1:
			draw(&SunHistory, transfHistory, programID, GL_TRIANGLES, &Menu.vertices);
			draw(&Sun, transfList, programID, GL_TRIANGLES, &Sphere.vertices);
			break;
		case 2:
			draw(&MercuryHistory, transfHistory, programID, GL_TRIANGLES, &Menu.vertices);
			draw(&Mercury, transfList, programID, GL_TRIANGLES, &Sphere.vertices);
			break;
		case 3:
			draw(&VenusHistory, transfHistory, programID, GL_TRIANGLES, &Menu.vertices);
			draw(&Venus, transfList, programID, GL_TRIANGLES, &Sphere.vertices);
			break;
		case 4:
			draw(&EarthHistory, transfHistory, programID, GL_TRIANGLES, &Menu.vertices);
			draw(&Earth, transfList, programID, GL_TRIANGLES, &Sphere.vertices);
			break;
		case 5:
			draw(&MarsHistory, transfHistory, programID, GL_TRIANGLES, &Menu.vertices);
			draw(&Mars, transfList, programID, GL_TRIANGLES, &Sphere.vertices);
			break;
		case 6:
			draw(&JupiterHistory, transfHistory, programID, GL_TRIANGLES, &Menu.vertices);
			draw(&Jupiter, transfList, programID, GL_TRIANGLES, &Sphere.vertices);
			break;
		case 7:
			draw(&SaturnHistory, transfHistory, programID, GL_TRIANGLES, &Menu.vertices);
			transfList.self_rotation = glm::rotate(glm::mat4(1.0), glm::radians(45.0f), glm::vec3(1, 1, 0));
			draw(&Saturn, transfList, programID, GL_TRIANGLES, &Sphere.vertices);
			transfList.self_rotation = glm::rotate(glm::mat4(1.0), glm::radians(45.0f), glm::vec3(1, 1, 0));
			transfList.scaling = glm::scale(glm::mat4(1.0), glm::vec3(45.0f, 1.0f, 60.0f));
			draw(&Ring, transfList, programID, GL_TRIANGLES, &VRing.vertices);
			transfList.self_rotation = glm::rotate(glm::mat4(1.0), glm::radians(1.0f), glm::vec3(1, 1, 0));
			break;
		case 8:
			draw(&UranusHistory, transfHistory, programID, GL_TRIANGLES, &Menu.vertices);
			draw(&Uranus, transfList, programID, GL_TRIANGLES, &Sphere.vertices);
			break;
		case 9:
			draw(&NeptuneHistory, transfHistory, programID, GL_TRIANGLES, &Menu.vertices);
			draw(&Neptune, transfList, programID, GL_TRIANGLES, &Sphere.vertices);
			break;
		case 10:
			draw(&MoonHistory, transfHistory, programID, GL_TRIANGLES, &Menu.vertices);
			draw(&Moon, transfList, programID, GL_TRIANGLES, &Sphere.vertices);
			break;
		case 11:
			draw(&AsteroidBeltHistory, transfHistory, programID, GL_TRIANGLES, &Menu.vertices);
			transfList.scaling = glm::scale(glm::mat4(1.0), glm::vec3(0.6f));
			draw(&Asteroid, transfList, programID, GL_TRIANGLES, &VAsteroid.vertices);
			break;
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE) {
			currentMenu = 1;
			position = glm::vec3(112.64f, 1.29f, -8.26f);
			horizontalAngle = -1.51f;
			verticalAngle = 0.0f;
		}
	}

}

void resetPosPlanetsList() {
	position = glm::vec3(145.78f, 2.13f, 39.61f);
	horizontalAngle = -1.55f;
	verticalAngle = -0.03f;
}

void deleteBuffers() {
	//Delete Program
	glDeleteProgram(programID);
	
	//Delete Textures
	glDeleteTextures(1, &Background.Texture);
	glDeleteTextures(1, &Sun.Texture);
	glDeleteTextures(1, &Mercury.Texture);
	glDeleteTextures(1, &Venus.Texture);
	glDeleteTextures(1, &Earth.Texture);
	glDeleteTextures(1, &Moon.Texture);
	glDeleteTextures(1, &Mars.Texture);
	glDeleteTextures(1, &Jupiter.Texture);
	glDeleteTextures(1, &Saturn.Texture);
	glDeleteTextures(1, &Ring.Texture);
	glDeleteTextures(1, &Uranus.Texture);
	glDeleteTextures(1, &Neptune.Texture);
	glDeleteTextures(1, &Asteroid.Texture);
	glDeleteTextures(1, &Menu1.Texture);
	glDeleteTextures(1, &Menu2.Texture);
	glDeleteTextures(1, &Menu3.Texture);
	glDeleteTextures(1, &SunHistory.Texture);
	glDeleteTextures(1, &MercuryHistory.Texture);
	glDeleteTextures(1, &VenusHistory.Texture);
	glDeleteTextures(1, &EarthHistory.Texture);
	glDeleteTextures(1, &MoonHistory.Texture);
	glDeleteTextures(1, &MarsHistory.Texture);
	glDeleteTextures(1, &JupiterHistory.Texture);
	glDeleteTextures(1, &SaturnHistory.Texture);
	glDeleteTextures(1, &UranusHistory.Texture);
	glDeleteTextures(1, &NeptuneHistory.Texture);
	glDeleteTextures(1, &AsteroidBeltHistory.Texture);
	//Delete buffers
	glDeleteBuffers(1, &Background.vertexbuffer);
	glDeleteBuffers(1, &Background.uvbuffer);
	glDeleteBuffers(1, &Sun.vertexbuffer);
	glDeleteBuffers(1, &Sun.uvbuffer);
	glDeleteBuffers(1, &Mercury.vertexbuffer);
	glDeleteBuffers(1, &Mercury.uvbuffer);
	glDeleteBuffers(1, &Venus.vertexbuffer);
	glDeleteBuffers(1, &Venus.uvbuffer);
	glDeleteBuffers(1, &Earth.vertexbuffer);
	glDeleteBuffers(1, &Earth.uvbuffer);
	glDeleteBuffers(1, &Moon.vertexbuffer);
	glDeleteBuffers(1, &Moon.uvbuffer);
	glDeleteBuffers(1, &Mars.vertexbuffer);
	glDeleteBuffers(1, &Mars.uvbuffer);
	glDeleteBuffers(1, &Jupiter.vertexbuffer);
	glDeleteBuffers(1, &Jupiter.uvbuffer);
	glDeleteBuffers(1, &Saturn.vertexbuffer);
	glDeleteBuffers(1, &Saturn.uvbuffer);
	glDeleteBuffers(1, &Ring.vertexbuffer);
	glDeleteBuffers(1, &Ring.uvbuffer);
	glDeleteBuffers(1, &Uranus.vertexbuffer);
	glDeleteBuffers(1, &Uranus.uvbuffer);
	glDeleteBuffers(1, &Neptune.vertexbuffer);
	glDeleteBuffers(1, &Neptune.uvbuffer);
	glDeleteBuffers(1, &Asteroid.vertexbuffer);
	glDeleteBuffers(1, &Asteroid.uvbuffer);
	glDeleteBuffers(1, &Menu1.vertexbuffer);
	glDeleteBuffers(1, &Menu1.uvbuffer);
	glDeleteBuffers(1, &Menu2.vertexbuffer);
	glDeleteBuffers(1, &Menu2.uvbuffer);
	glDeleteBuffers(1, &Menu3.vertexbuffer);
	glDeleteBuffers(1, &Menu3.uvbuffer);
	glDeleteBuffers(1, &SunHistory.vertexbuffer);
	glDeleteBuffers(1, &SunHistory.uvbuffer);
	glDeleteBuffers(1, &MercuryHistory.vertexbuffer);
	glDeleteBuffers(1, &MercuryHistory.uvbuffer);
	glDeleteBuffers(1, &VenusHistory.vertexbuffer);
	glDeleteBuffers(1, &VenusHistory.uvbuffer);
	glDeleteBuffers(1, &EarthHistory.vertexbuffer);
	glDeleteBuffers(1, &EarthHistory.uvbuffer);
	glDeleteBuffers(1, &MoonHistory.vertexbuffer);
	glDeleteBuffers(1, &MoonHistory.uvbuffer);
	glDeleteBuffers(1, &MarsHistory.vertexbuffer);
	glDeleteBuffers(1, &MarsHistory.uvbuffer);
	glDeleteBuffers(1, &JupiterHistory.vertexbuffer);
	glDeleteBuffers(1, &JupiterHistory.uvbuffer);
	glDeleteBuffers(1, &SaturnHistory.vertexbuffer);
	glDeleteBuffers(1, &SaturnHistory.uvbuffer);
	glDeleteBuffers(1, &UranusHistory.vertexbuffer);
	glDeleteBuffers(1, &UranusHistory.uvbuffer);
	glDeleteBuffers(1, &NeptuneHistory.vertexbuffer);
	glDeleteBuffers(1, &NeptuneHistory.uvbuffer);
	glDeleteBuffers(1, &AsteroidBeltHistory.vertexbuffer);
	glDeleteBuffers(1, &AsteroidBeltHistory.uvbuffer);

	//Delete VertexArrays
	glDeleteVertexArrays(1, &Background.VertexArrayID);
	glDeleteVertexArrays(1, &Sun.VertexArrayID);
	glDeleteVertexArrays(1, &Mercury.VertexArrayID);
	glDeleteVertexArrays(1, &Venus.VertexArrayID);
	glDeleteVertexArrays(1, &Earth.VertexArrayID);
	glDeleteVertexArrays(1, &Moon.VertexArrayID);
	glDeleteVertexArrays(1, &Mars.VertexArrayID);
	glDeleteVertexArrays(1, &Jupiter.VertexArrayID);
	glDeleteVertexArrays(1, &Saturn.VertexArrayID);
	glDeleteVertexArrays(1, &Ring.VertexArrayID);
	glDeleteVertexArrays(1, &Uranus.VertexArrayID);
	glDeleteVertexArrays(1, &Neptune.VertexArrayID);
	glDeleteVertexArrays(1, &Asteroid.VertexArrayID);
	glDeleteVertexArrays(1, &Menu1.VertexArrayID);
	glDeleteVertexArrays(1, &Menu2.VertexArrayID);
	glDeleteVertexArrays(1, &Menu3.VertexArrayID);
	glDeleteVertexArrays(1, &SunHistory.VertexArrayID);
	glDeleteVertexArrays(1, &MercuryHistory.VertexArrayID);
	glDeleteVertexArrays(1, &VenusHistory.VertexArrayID);
	glDeleteVertexArrays(1, &EarthHistory.VertexArrayID);
	glDeleteVertexArrays(1, &MoonHistory.VertexArrayID);
	glDeleteVertexArrays(1, &MarsHistory.VertexArrayID);
	glDeleteVertexArrays(1, &JupiterHistory.VertexArrayID);
	glDeleteVertexArrays(1, &SaturnHistory.VertexArrayID);
	glDeleteVertexArrays(1, &UranusHistory.VertexArrayID);
	glDeleteVertexArrays(1, &NeptuneHistory.VertexArrayID);
	glDeleteVertexArrays(1, &AsteroidBeltHistory.VertexArrayID);
}