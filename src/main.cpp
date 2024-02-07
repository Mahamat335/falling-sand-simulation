#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <Shader.h>
#include <ctime>
#include <vector>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void activateCells(int y, int x);

const unsigned int width = 800;
const unsigned int height = 600;
const unsigned int PIXEL_SIZE = 2;
const unsigned int gridSize = 100;
const unsigned int numObjects = (width * height) / (PIXEL_SIZE * PIXEL_SIZE);
const int radius = 10;
bool isPressing = false;
bool isSand = true;
unsigned int *EBOs = new unsigned int[numObjects];
unsigned int VAO, VBO;
std::vector<std::vector<int>> sandMatrix(height / PIXEL_SIZE, std::vector<int>(width / PIXEL_SIZE, 0));
std::vector<std::vector<int>> sandMatrix2(height / PIXEL_SIZE, std::vector<int>(width / PIXEL_SIZE, 0));
std::vector<std::vector<int>> tiles(height / gridSize, std::vector<int>(width / gridSize, 0));
std::vector<std::vector<int>> nextTiles(height / gridSize, std::vector<int>(width / gridSize, 0));
int numVertices = ((width / PIXEL_SIZE) + 1) * ((height / PIXEL_SIZE) + 1);
float *vertices = new float[numVertices * 3];
std::vector<std::vector<unsigned int>> indices(numVertices, std::vector<unsigned int>(6, 0));
Shader *shaderProgram;
Shader *shaderProgram2;
void mouse_callback(GLFWwindow *window, int button, int action, int mods)
{

	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		isSand = true;
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			isPressing = true;
		}
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		{
			isPressing = false;
		}
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		isSand = false;
		if (action == GLFW_PRESS)
		{
			isPressing = true;
		}
		if (action == GLFW_RELEASE)
		{
			isPressing = false;
		}
	}
}
void updateMatrix()
{
	for (int y = 1; y < height / PIXEL_SIZE; y++)
	{
		for (int x = 0; x < width / PIXEL_SIZE; x++)
		{

			// Is it sand?
			if (sandMatrix[y][x] == 1)
			{
				// Is it sand underneath?
				if (sandMatrix[y - 1][x] == 0)
				{
					// sandMatrix[y][x] = 0;
					// sandMatrix[y - 1][x] = 1;

					int distance = 1;
					bool isFound = false;
					while (distance < 5 && !isFound)
					{
						if (y - distance > 0 && sandMatrix[y - distance][x] == 0)
						{
							distance++;
						}
						else
						{
							isFound = true;
						}
					}
					if (distance > 1)
						distance--;
					sandMatrix[y][x] = 0;
					sandMatrix[y - distance][x] = 1;
				}
				// Is it water underneath?
				else if (sandMatrix[y - 1][x] == 2)
				{
					if (y + 1 < height / PIXEL_SIZE && sandMatrix[y + 1][x] == 1)
					{
						if (x + 1 < width / PIXEL_SIZE && sandMatrix[y][x + 1] == 0 && x > 0 && sandMatrix[y][x - 1] == 0)
						{
							if (std::rand() % 2 == 0)
							{
								if (x + 1 < width / PIXEL_SIZE && sandMatrix[y][x + 1] == 0)
								{
									sandMatrix[y][x + 1] = 2;
									sandMatrix[y - 1][x] = 1;
								}
							}
							else
							{
								if (x > 0 && sandMatrix[y][x - 1] == 0)
								{
									sandMatrix[y][x - 1] = 2;
									sandMatrix[y - 1][x] = 1;
								}
							}
						}
						else if (x + 1 < width / PIXEL_SIZE && sandMatrix[y][x + 1] == 0)
						{
							sandMatrix[y][x + 1] = 2;
							sandMatrix[y - 1][x] = 1;
						}
						else if (x > 0 && sandMatrix[y][x - 1] == 0)
						{
							sandMatrix[y][x - 1] = 2;
							sandMatrix[y - 1][x] = 1;
						}
						else
						{
							sandMatrix[y][x] = 2;
							sandMatrix[y - 1][x] = 1;
						}
					}
					else
					{

						sandMatrix[y][x] = 2;
						sandMatrix[y - 1][x] = 1;
					}
				}
				else if (sandMatrix[y - 1][x] == 1)
				{
					// Random check for diagonals
					if (std::rand() % 2 == 0)
					{
						if (x > 0 && sandMatrix[y - 1][x - 1] == 0)
						{
							int distance = 1;
							bool isFound = false;
							while (distance < 5 && !isFound)
							{
								if (y - distance > 0 && x - distance > 0 && sandMatrix[y - distance][x - distance] == 0)
								{
									distance++;
								}
								else
								{
									isFound = true;
								}
							}
							if (distance > 1)
								distance--;
							sandMatrix[y][x] = 0;
							sandMatrix[y - distance][x - distance] = 1;
						}
						else if (x > 0 && sandMatrix[y - 1][x - 1] == 2)
						{

							sandMatrix[y][x] = 2;
							sandMatrix[y - 1][x - 1] = 1;
						}
						else if (x < -1 + width / PIXEL_SIZE && sandMatrix[y - 1][x + 1] == 0)
						{
							int distance = 1;
							bool isFound = false;
							while (distance < 5 && !isFound)
							{
								if (y - distance > 0 && x + distance < width / PIXEL_SIZE && sandMatrix[y - distance][x + distance] == 0)
								{
									distance++;
								}
								else
								{
									isFound = true;
								}
							}
							if (distance > 1)
								distance--;
							sandMatrix[y][x] = 0;
							sandMatrix[y - distance][x + distance] = 1;
						}
						else if (x < -1 + width / PIXEL_SIZE && sandMatrix[y - 1][x + 1] == 2)
						{
							sandMatrix[y][x] = 2;
							sandMatrix[y - 1][x + 1] = 1;
						}
					}
					else
					{
						if (x < -1 + width / PIXEL_SIZE && sandMatrix[y - 1][x + 1] == 0)
						{
							int distance = 1;
							bool isFound = false;
							while (distance < 5 && !isFound)
							{
								if (y - distance > 0 && x + distance < width / PIXEL_SIZE && sandMatrix[y - distance][x + distance] == 0)
								{
									distance++;
								}
								else
								{
									isFound = true;
								}
							}
							if (distance > 1)
								distance--;
							sandMatrix[y][x] = 0;
							sandMatrix[y - distance][x + distance] = 1;
						}
						else if (x < -1 + width / PIXEL_SIZE && sandMatrix[y - 1][x + 1] == 2)
						{
							sandMatrix[y][x] = 2;
							sandMatrix[y - 1][x + 1] = 1;
						}
						else if (x > 0 && sandMatrix[y - 1][x - 1] == 0)
						{
							int distance = 1;
							bool isFound = false;
							while (distance < 5 && !isFound)
							{
								if (y - distance > 0 && x - distance > 0 && sandMatrix[y - distance][x - distance] == 0)
								{
									distance++;
								}
								else
								{
									isFound = true;
								}
							}
							if (distance > 1)
								distance--;
							sandMatrix[y][x] = 0;
							sandMatrix[y - distance][x - distance] = 1;
						}
						else if (x > 0 && sandMatrix[y - 1][x - 1] == 2)
						{
							sandMatrix[y][x] = 2;
							sandMatrix[y - 1][x - 1] = 1;
						}
					}
				}
			}
			// Is it water?
			else if (sandMatrix[y][x] == 2)
			{
				if (sandMatrix[y - 1][x] == 0)
				{
					int distance = 1;
					bool isFound = false;
					while (distance < 5 && !isFound)
					{
						if (y - distance > 0 && sandMatrix[y - distance][x] == 0)
						{
							distance++;
						}
						else
						{
							isFound = true;
						}
					}
					if (distance > 1)
						distance--;
					sandMatrix[y][x] = 0;
					sandMatrix[y - distance][x] = 2;
				}
				// Check the neighbors on the sides.
				else if ((x > 0 && sandMatrix[y][x - 1] == 0) || (x < -1 + width / PIXEL_SIZE && sandMatrix[y][x + 1] == 0))
				{
					if (std::rand() % 2 == 0)
					{

						if (x > 0 && sandMatrix[y][x - 1] == 0)
						{
							int distance = 1;
							bool isFound = false;
							while (distance < 5 && !isFound)
							{
								if (x - distance >= 0 && sandMatrix[y][x - distance] == 0)
								{
									distance++;
								}
								else
								{
									isFound = true;
								}
							}
							if (distance > 1)
								distance--;
							for (int c = 0; c < distance; c++)
							{
								if (x + c < -1 + width / PIXEL_SIZE && sandMatrix[y][x + c] == 2)
								{
									sandMatrix[y][x + c] = 0;
									sandMatrix[y][x - distance + c] = 2;
								}
								else
									break;
							}
						}
						else if (x < -1 + width / PIXEL_SIZE && sandMatrix[y][x + 1] == 0)
						{
							int distance = 1;
							bool isFound = false;
							while (distance < 5 && !isFound)
							{
								if (x + distance < -1 + width / PIXEL_SIZE && sandMatrix[y][x + distance] == 0)
								{
									distance++;
								}
								else
								{
									isFound = true;
								}
							}
							if (distance > 1)
								distance--;
							for (int c = 0; c < distance; c++)
							{
								if (x - c >= 0 && sandMatrix[y][x - c] == 2)
								{
									sandMatrix[y][x - c] = 0;
									sandMatrix[y][x + distance - c] = 2;
								}
								else
									break;
							}
						}
					}
					else
					{
						if (x < -1 + width / PIXEL_SIZE && sandMatrix[y][x + 1] == 0)
						{
							int distance = 1;
							bool isFound = false;
							while (distance < 5 && !isFound)
							{
								if (x + distance < -1 + width / PIXEL_SIZE && sandMatrix[y][x + distance] == 0)
								{
									distance++;
								}
								else
								{
									isFound = true;
								}
							}
							if (distance > 1)
								distance--;
							sandMatrix[y][x] = 0;
							sandMatrix[y][x + distance] = 2;
						}
						else if (x > 0 && sandMatrix[y][x - 1] == 0)
						{
							int distance = 1;
							bool isFound = false;
							while (distance < 5 && !isFound)
							{
								if (x - distance >= 0 && sandMatrix[y][x - distance] == 0)
								{
									distance++;
								}
								else
								{
									isFound = true;
								}
							}
							if (distance > 1)
								distance--;
							for (int c = 0; c < distance; c++)
							{
								if (x + c < -1 + width / PIXEL_SIZE && sandMatrix[y][x + c] == 2)
								{
									sandMatrix[y][x + c] = 0;
									sandMatrix[y][x - distance + c] = 2;
								}
								else
									break;
							}
						}
					}
				}
				else if (sandMatrix[y - 1][x] >= 1)
				{
					if (std::rand() % 2 == 0)
					{
						if (x > 0 && sandMatrix[y - 1][x - 1] == 0)
						{
							sandMatrix[y][x] = 0;
							sandMatrix[y - 1][x - 1] = 2;
						}
						else if (x < -1 + width / PIXEL_SIZE && sandMatrix[y - 1][x + 1] == 0)
						{
							sandMatrix[y][x] = 0;
							sandMatrix[y - 1][x + 1] = 2;
						}
					}
					else
					{
						if (x < -1 + width / PIXEL_SIZE && sandMatrix[y - 1][x + 1] == 0)
						{
							sandMatrix[y][x] = 0;
							sandMatrix[y - 1][x + 1] = 2;
						}
						else if (x > 0 && sandMatrix[y - 1][x - 1] == 0)
						{
							sandMatrix[y][x] = 0;
							sandMatrix[y - 1][x - 1] = 2;
						}
					}
				}
			}
			// Setting the order for reading the matrix.
			if (x == (((width / PIXEL_SIZE) / 2) - 1))
			{
				continue;
			}
			else if (x == ((width / PIXEL_SIZE) / 2))
			{
				x = width;
			}
			else if (x < ((width / PIXEL_SIZE) / 2))
			{
				x = (width / PIXEL_SIZE) - x - 2;
			}
			else
			{
				x = (width / PIXEL_SIZE) - x - 1;
			}
		}
	}
}

void initializeOpenGLObjects()
{
	unsigned int i = 0;
	for (int y = 0; y < 1 + (height / PIXEL_SIZE); y++)
	{
		for (int x = 0; x < 1 + (width / PIXEL_SIZE); x++)
		{
			i = (width / PIXEL_SIZE) * y + x;
			i *= 3;
			vertices[i] = -1.0f + (float)(x * PIXEL_SIZE * 2) / width;
			vertices[i + 1] = -1.0f + (float)(y * PIXEL_SIZE * 2) / height;
			vertices[i + 2] = 0.0f;
		}
	}
	int counter = 0;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVertices * 3, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void *)0);
	glEnableVertexAttribArray(0);
	for (int y = 0; y < (height / PIXEL_SIZE); y++)
	{
		for (int x = 0; x < (width / PIXEL_SIZE); x++)
		{
			i = (0 + (width / PIXEL_SIZE)) * y + x;
			indices[i][0] = i;
			indices[i][1] = i + 1;
			indices[i][2] = i + 1 + width / PIXEL_SIZE;
			indices[i][3] = i;
			indices[i][4] = i + 1 + width / PIXEL_SIZE;
			indices[i][5] = i + width / PIXEL_SIZE;
			glGenBuffers(1, &EBOs[i]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices[i].data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	}
}

void drawSand(int x, int y)
{

	shaderProgram->use();
	unsigned int i = (width / PIXEL_SIZE) * y + x;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
void drawWater(int x, int y)
{

	shaderProgram2->use();
	unsigned int i = (width / PIXEL_SIZE) * y + x;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	std::srand(std::time(0));
	GLFWwindow *window = glfwCreateWindow(width, height, "First OpenGL Project", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD!" << std::endl;
		return -1;
	}
	glViewport(0, 0, width, height);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetMouseButtonCallback(window, mouse_callback);

	initializeOpenGLObjects();
	shaderProgram = new Shader("resources/shaders/shader.vert", "resources/shaders/shader.frag");
	shaderProgram2 = new Shader("resources/shaders/shader.vert", "resources/shaders/shader2.frag");

	while (!glfwWindowShouldClose(window))
	{

		processInput(window);

		glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		if (isPressing)
		{

			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);

			int pixelX = static_cast<int>(mouseX / PIXEL_SIZE);
			int pixelY = static_cast<int>((height - mouseY) / PIXEL_SIZE);
			int x = 0;
			for (int i = -radius; i <= radius; i++)
			{
				for (int j = -radius; j <= radius; j++)
				{
					int newX = pixelX + i;
					int newY = pixelY + j;
					if (newX >= 0 && newX < width / PIXEL_SIZE && newY >= 0 && newY < height / PIXEL_SIZE)
					{
						double distance = sqrt(static_cast<double>(i * i + j * j));
						if (distance <= radius)
						{
							if (x == 6)
							{
								x = 0;
								if (isSand == 1)
									sandMatrix[newY][newX] = 1;
								else
									sandMatrix[newY][newX] = 2;
							}
							else
								x++;
						}
					}
				}
			}
		}

		updateMatrix();

		for (int i = 0; i < height / PIXEL_SIZE; i++)
		{
			for (int j = 0; j < -1 + width / PIXEL_SIZE; j++)
			{
				if (sandMatrix[i][j] == 1)
					drawSand(j, i);
				else if (sandMatrix[i][j] == 2)
					drawWater(j, i);
			}
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(numObjects, EBOs);
	shaderProgram->deleteProgram();
	shaderProgram2->deleteProgram();
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}