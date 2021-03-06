/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "allmodels.h"
#include "lodepng.h"
#include "shaderprogram.h"

float far = -10.0;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	far += yoffset;
}

float speed = 0.0f;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_A)
		{
			speed = -3.0f;
		}
		else if (key == GLFW_KEY_D)
		{
			speed = 3.0f;
		}
	}
	if(action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_A)
		{
			speed = 0.0f;
		}
		else if (key == GLFW_KEY_D)
		{
			speed = 0.0f;
		}
	}	
}
//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
    initShaders();
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
    freeShaders();
    //************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************
}

//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float angle) {
	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glm::mat4 M = glm::mat4(1.0f);
	glm::mat4 V = glm::lookAt(
		glm::vec3(0.0f, 0.0f, far),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 P = glm::perspective(50.0f * PI / 180.f, 1.0f, 1.0f, 50.0f);
	M = rotate(M, angle, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 M1 = M;
	spLambert->use();
	glUniform4f(spLambert->u("color"), 0.9, 0.6, 0.0, 1.0);
	glUniformMatrix4fv(spLambert->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spLambert->u("V"), 1, false, glm::value_ptr(V));
	int direction = -1;
	for (int i = 0; i < 4; i++)
	{
		glm::mat4 M2 = glm::translate(M1, glm::vec3(0.0f, 0.0f, 1.0f));
		if (i % 2 == 1)
		{
			glm::mat4 MT = M2;
			MT = glm::translate(MT, glm::vec3(1.0f, 0.0f, 0.0f));
			MT = glm::scale(MT, glm::vec3(0.3f, 0.3f, 0.3f));
			MT = glm::translate(MT, glm::vec3(0.0f, 0.0f, 1.0f));
			glm::mat4 TURNED_M = rotate(MT, PI/4 * speed, glm::vec3(0.0f, 1.0f * direction, 0.0f));
			glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(TURNED_M));
			Models::torus.drawWire();
			MT = glm::translate(MT, glm::vec3(-6.67f, 0.0f, 0.0f));
			TURNED_M = rotate(MT, PI / 4 * speed, glm::vec3(0.0f, -1.0f * direction, 0.0f));
			glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(TURNED_M));
			Models::torus.drawWire();
			direction *= -1;
		}
		M2 = glm::scale(M2, glm::vec3(1.0f, 0.2f, 0.2f));
		glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(M2));
		M1 = rotate(M1, PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));
		Models::cube.drawSolid();
	}
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(M1));
	glfwSwapBuffers(window);
}

int main(void)
{
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące

	float angle = 0.0;
	//float speed = PI;
	glfwSetTime(0);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//Główna pętla
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		//key_callback(window, key_callback);
		angle += speed * glfwGetTime();
		glfwSetTime(0);
		drawScene(window, angle); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
