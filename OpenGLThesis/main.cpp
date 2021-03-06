#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <iostream>

const GLchar* vertexSource =
"#version 440\n"
"in vec3 position;"
"in vec3 color;"
"in vec2 texcoord;"
"in vec3 normal;"
"out vec3 Color;"
"out vec2 Texcoord;"
"out vec3 FragPos;"
"out vec3 Normal;"
"uniform mat4 model;"
"uniform mat4 view;"
"uniform mat4 proj;"
"void main()"
"{"
"Color = color;"
"Texcoord = texcoord;"
"FragPos = vec3(model * vec4(position, 1.0));"
"Normal = mat3(transpose(inverse(model))) * normal;"
"gl_Position = proj * view * model * vec4(FragPos, 1.0);"
"}";

const GLchar* fragmentSource =
"#version 440\n"
"in vec3 Color;"
"in vec2 Texcoord;"
"in vec3 Normal;"
"in vec3 FragPos;"
"out vec4 outColor;"
"uniform sampler2D tex;"
"uniform vec3 lightColor;"
"uniform vec3 lightPos;"
"uniform vec3 viewPos;"
"void main()"
"{"
"float ambientStrength = 0.15;"
"vec3 ambient = ambientStrength * lightColor;"
"vec3 norm = normalize(Normal);"
"vec3 lightDir = normalize(lightPos - FragPos);"
"float diff = max(dot(norm, lightDir), 0.0);"
"vec3 diffuse = diff * lightColor;"
"float specularStrength = 1.5;"
"vec3 viewDir = normalize(normalize(viewPos) - FragPos);"
"vec3 reflectDir = reflect(-lightDir, norm);"
"float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);"
"vec3 specular = specularStrength * spec * lightColor;"
"vec3 result = (ambient + diffuse + specular) * texture(tex, Texcoord*6).rgb;"
//"vec3 result = (ambient + diffuse + specular) * Color;"
//"outColor = texture(tex, Texcoord);"
"outColor = vec4(result, 1.0);"
"}";

int main(int argc, char *argv[])
{
	//auto t_start = std::chrono::high_resolution_clock::now();

	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_Window* window = SDL_CreateWindow("OpenGL", 100, 100, 1024, 768, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
	glewInit();

	// Z-buffer
	glEnable(GL_DEPTH_TEST);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);


	/*float vertices[] = {
    //  Position			Color             Texture coordinates
		-0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
		 0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
		 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
		-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Bottom-left
	};*/

	GLfloat vertices[] = {
	//  Position			Color             Texcoord.    Normal
		-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.1f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.1f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.1f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.1f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.1f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.1f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.1f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.1f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.1f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.1f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.1f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.1f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.1f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.1f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

		 0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.1f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.1f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.1f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.1f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.1f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.1f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.1f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.1f, 1.0f, -1.0f, 0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.1f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.1f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.1f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.1f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f,

		-0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.1f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.1f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.1f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.1f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.1f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.1f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f
	};

	GLfloat lightVertices[] = {
		//  Position			Color             Texture coordinates
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f
	};

	GLuint elements[] = {
		0, 1, 2,
		2, 3, 0
	};

	GLuint vbo; // Vertex buffer object
	glGenBuffers(1, &vbo);

	GLuint ebo; // Element buffer object
	glGenBuffers(1, &ebo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	//Tarkista varjostinten compailaus t�ss�!
	GLint status;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (status == GL_TRUE)
	{
		std::cout << "Vertex compiled!" << std::endl;
	}
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (status == GL_TRUE)
	{
		std::cout << "Fragment compiled!" << std::endl;
	}

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 11*sizeof(float), 0);
	
	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3*sizeof(float)));

	GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	
	GLint normalAttrib = glGetAttribLocation(shaderProgram, "normal");
	glEnableVertexAttribArray(normalAttrib);
	glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));

	// Textures 
	GLuint textures[2];
	glGenTextures(2, textures);

	// Loading
	int width, height;
	unsigned char* image;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	image = SOIL_load_image("dottest.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	//glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	/*glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	image = SOIL_load_image("sample2.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glUniform1i(glGetUniformLocation(shaderProgram, "texPuppy"), 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/

	//glGenerateMipmap(GL_TEXTURE_2D);

	// Transformations
	/*glm::mat4 model;
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	GLint uniModel = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));*/

	glm::vec3 viewPos(-1.0f, 0.8f, 1.5f);
	GLint uniViewPos = glGetUniformLocation(shaderProgram, "viewPos");
	glUniform3fv(uniViewPos, 1, glm::value_ptr(viewPos));

	glm::mat4 view = glm::lookAt(
		viewPos,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	GLint uniView = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 proj = glm::perspective(glm::radians(80.0f), 1024.0f / 768.0f, 0.5f, 10.0f);
	GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	//Lighting

	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
	GLint uniLightColor = glGetUniformLocation(shaderProgram, "lightColor");
	glUniform3fv(uniLightColor, 1, glm::value_ptr(lightColor));

	glm::vec3 lightPos(-0.2f, 0.8f, 1.5f);
	GLint uniLightPos = glGetUniformLocation(shaderProgram, "lightPos");
	glUniform3fv(uniLightPos, 1, glm::value_ptr(lightPos));

	// Main event loop
	SDL_Event windowEvent;
	while (true)
	{
		if (SDL_PollEvent(&windowEvent))
		{
			if (windowEvent.type == SDL_QUIT) break;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//auto t_now = std::chrono::high_resolution_clock::now();
		//float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();

		//glUniform3f(uniColor, (sin(time*4.0f) + 1.0f) / 2.0f, 0.0f, 0.0f);

		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glm::mat4 model;
		//model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		GLint uniModel = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_TRIANGLES, 0, 36);

		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(context);
	SDL_Quit();

	return 0;
}