//// Header
#include "glwindow.h"

//// Definitions
namespace SWPTAS001
{
	//// Imports
	using namespace std;

	//// Utilities (Non-Class Related)
	const char* glGetErrorString(GLenum error)
	{
		// categorise error
		switch (error)
		{
		case GL_NO_ERROR:
			return "GL_NO_ERROR";
		case GL_INVALID_ENUM:
			return "GL_INVALID_ENUM";
		case GL_INVALID_VALUE:
			return "GL_INVALID_VALUE";
		case GL_INVALID_OPERATION:
			return "GL_INVALID_OPERATION";
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			return "GL_INVALID_FRAMEBUFFER_OPERATION";
		case GL_OUT_OF_MEMORY:
			return "GL_OUT_OF_MEMORY";
		default:
			return "UNRECOGNIZED";
		}
	}

	void glPrintError(const char* label = "Unlabelled Error Checkpoint", bool alwaysPrint = false)
	{
		GLenum error = glGetError();
		if (alwaysPrint || (error != GL_NO_ERROR))
		{
			printf("%s: OpenGL error flag is %s\n", label, glGetErrorString(error));
		}
	}

	GLuint loadShader(const char* shaderFilename, GLenum shaderType)
	{
		// check if file is accessible
		FILE* shaderFile = fopen(shaderFilename, "r");
		if (!shaderFile)
		{
			return 0;
		}
		// determine file length
		fseek(shaderFile, 0, SEEK_END);
		long shaderSize = ftell(shaderFile);
		fseek(shaderFile, 0, SEEK_SET);
		// read file
		char* shaderText = new char[shaderSize + 1];
		size_t readCount = fread(shaderText, 1, shaderSize, shaderFile);
		shaderText[readCount] = '\0';
		fclose(shaderFile);
		// create shader
		GLuint shader = glCreateShader(shaderType);
		glShaderSource(shader, 1, (const char**)&shaderText, NULL);
		glCompileShader(shader);
		// free text buffer
		delete[] shaderText;
		// return shader ID
		return shader;
	}

	GLuint loadShaderProgram(const char* vertShaderFilename, const char* fragShaderFilename)
	{
		// load shaders
		GLuint vertShader = loadShader(vertShaderFilename, GL_VERTEX_SHADER);
		GLuint fragShader = loadShader(fragShaderFilename, GL_FRAGMENT_SHADER);
		// compile shaders
		GLuint program = glCreateProgram();
		glAttachShader(program, vertShader);
		glAttachShader(program, fragShader);
		glLinkProgram(program);
		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
		// check if compiled
		GLint linkStatus;
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE)
		{
			GLsizei logLength = 0;
			GLchar message[1024];
			glGetProgramInfoLog(program, 1024, &logLength, message);
			cout << "Shader load error: " << message << endl;
			return 0;
		}
		// return program ID
		return program;
	}

	//// Constructors
	OpenGLWindow::OpenGLWindow()
	{
		// Setup SDL
		setupLibraries();
	}

	//// Core Routines
	void OpenGLWindow::setupLibraries()
	{
		// setup GL Context
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		// sdl config
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
		// create view window
		sdlWin = SDL_CreateWindow("Advanced OpenGL Prac - SWPTAS001", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, fWidth, fHeight, SDL_WINDOW_OPENGL);
		// check if window created
		if (!sdlWin) // case: not created
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Error", "Unable to create window", 0);
		}
		// SDL Settings
		SDL_GLContext glc = SDL_GL_CreateContext(sdlWin);
		SDL_GL_MakeCurrent(sdlWin, glc);
		SDL_GL_SetSwapInterval(1);
		// GLEW Settings
		glewExperimental = true;
		GLenum glewInitResult = glewInit();
		glGetError(); // Consume the error erroneously set by glewInit()
		if (glewInitResult != GLEW_OK)
		{
			const GLubyte* errorString = glewGetErrorString(glewInitResult);
			cout << "Unable to initialize glew: " << errorString;
		}
		// Report
		int glMajorVersion;
		int glMinorVersion;
		glGetIntegerv(GL_MAJOR_VERSION, &glMajorVersion);
		glGetIntegerv(GL_MINOR_VERSION, &glMinorVersion);
		cout << "\n - Loaded OpenGL " << glMajorVersion << "." << glMinorVersion << " with:" << endl;
		cout << "    - Vendor: " << glGetString(GL_VENDOR) << endl;
		cout << "    - Renderer: " << glGetString(GL_RENDERER) << endl;
		cout << "    - Version: " << glGetString(GL_VERSION) << endl;
		cout << "    - GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
	}

	void OpenGLWindow::initGL()
	{
		///////////
		// Setup //
		///////////

		// configure GL
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);
		glClearColor(0, 0, 0, 1);
		
		// Set Shader
		bufferBindMap["phongShader"] = loadShaderProgram("Shaders/phong.vert", "Shaders/phong.frag");
		glUseProgram(bufferBindMap["phongShader"]);

		////////////////////
		// VAO 1  - Model //
		////////////////////

		// Make 1 VAO
		glGenVertexArrays(1, &bufferBindMap["modelVAO"]);
		glBindVertexArray(bufferBindMap["modelVAO"]);
		
		/////////////////////////
		// VBO 1 - Model Vertex//
		/////////////////////////
		
		// Load Geometry
		fModelGeometry.loadFromOBJFile("Objects/planet.obj");
		
		// read vertex positions
		glGenBuffers(1, &bufferBindMap["modelVertexBuffer"]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferBindMap["modelVertexBuffer"]);
		glBufferData(GL_ARRAY_BUFFER, (fModelGeometry.vertexCount() * 3 * sizeof(float)), fModelGeometry.vertexData(), GL_STATIC_DRAW);
		
		// bind vertex positions
		shaderBindMap["modelposition"] = glGetAttribLocation(bufferBindMap["phongShader"], "position");
		glVertexAttribPointer(shaderBindMap["modelposition"], 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(shaderBindMap["modelposition"]);
		
		//////////////////////////
		// VBO 2 - Model Normal //		
		//////////////////////////

		// read normal positions
		glGenBuffers(1, &bufferBindMap["modelNormalBuffer"]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferBindMap["modelNormalBuffer"]);
		glBufferData(GL_ARRAY_BUFFER, ((fModelGeometry.normalCount()) * 3  * sizeof(float)), fModelGeometry.normalData(), GL_STATIC_DRAW);
		
		// bind normal positions
		shaderBindMap["modelnormal"] = glGetAttribLocation(bufferBindMap["phongShader"], "normal");
		glVertexAttribPointer(shaderBindMap["modelnormal"], 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(shaderBindMap["modelnormal"]);
		
		//////////////////////
		// VBO 3 - Model UV //
		//////////////////////

		// read textureUV positions
		glGenBuffers(1, &bufferBindMap["modelTextureCoordBuffer"]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferBindMap["modelTextureCoordBuffer"]);
		glBufferData(GL_ARRAY_BUFFER, ((fModelGeometry.textureCoordCount()) * 2  * sizeof(float)), fModelGeometry.textureCoordData(), GL_STATIC_DRAW);
		
		// bind textureUV positions
		shaderBindMap["modeltexturecoord"] = glGetAttribLocation(bufferBindMap["phongShader"], "textureUV");
		glVertexAttribPointer(shaderBindMap["modeltexturecoord"], 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(shaderBindMap["modeltexturecoord"]);

		////////////////////////////
		// VBO 3 - Model Tangents //
		////////////////////////////

		// read normal positions
		glGenBuffers(1, &bufferBindMap["modelTangentBuffer"]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferBindMap["modelTangentBuffer"]);
		glBufferData(GL_ARRAY_BUFFER, ((fModelGeometry.tangentCount()) * 3  * sizeof(float)), fModelGeometry.tangentData(), GL_STATIC_DRAW);
		
		// bind normal positions
		shaderBindMap["modeltangent"] = glGetAttribLocation(bufferBindMap["phongShader"], "tangent");
		glVertexAttribPointer(shaderBindMap["modeltangent"], 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(shaderBindMap["modeltangent"]);

		/////////////////////////////
		// VBO 3 - Model Bitangent //
		/////////////////////////////

		// read normal positions
		glGenBuffers(1, &bufferBindMap["modelBitangetBuffer"]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferBindMap["modelBitangetBuffer"]);
		glBufferData(GL_ARRAY_BUFFER, ((fModelGeometry.bitangentCount()) * 3  * sizeof(float)), fModelGeometry.bitangentData(), GL_STATIC_DRAW);
		
		// bind normal positions
		shaderBindMap["modelbitangent"] = glGetAttribLocation(bufferBindMap["phongShader"], "bitangent");
		glVertexAttribPointer(shaderBindMap["modelbitangent"], 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(shaderBindMap["modelbitangent"]);

		////////////////////////////
		// Texture - Model Texture//
		////////////////////////////

		// activate
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &bufferBindMap["modelTexture"]);
		glBindTexture(GL_TEXTURE_2D, bufferBindMap["modelTexture"]);

		// load texture
		int width, height, bpp;
 		unsigned char * textureData = stbi_load( "Textures/venusmap.png", &width, &height, &bpp, 4);

		// config
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// upload image
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);

		// setup sampler
		shaderBindMap["modeltexture"] = glGetUniformLocation(bufferBindMap["phongShader"], "modelTexture");
		glUniform1i(shaderBindMap["modeltexture"], 0);

		// clean up
		stbi_image_free(textureData);

		/////////////////////////////
		// Texture - Model BumpMap //
		/////////////////////////////

		// activate
		glActiveTexture(GL_TEXTURE1);
		glGenTextures(1, &bufferBindMap["modelBumpMap"]);
		glBindTexture(GL_TEXTURE_2D, bufferBindMap["modelBumpMap"]);

		// load texture
 		unsigned char * bumpMapData = stbi_load( "Textures/venusbump.png", &width, &height, &bpp, 4);

		// config
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// upload image
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bumpMapData);

		// setup sampler
		shaderBindMap["modelBumpMap"] = glGetUniformLocation(bufferBindMap["phongShader"], "modelBumpMap");
		glUniform1i(shaderBindMap["modelBumpMap"], 1);

		// clean up
		stbi_image_free(bumpMapData);

		///////////////////
		// VAO 2 - Light //
		///////////////////

		// Make 1 VAO
		glGenVertexArrays(1, &bufferBindMap["lightVAO"]);
		glBindVertexArray(bufferBindMap["lightVAO"]);
		
		//////////////////////////
		// VBO 1 - Light Vertex //
		//////////////////////////

		// Load Geometry
		fLightGeometry.loadFromOBJFile("Objects/sphere.obj");
		
		// read vertex positions
		glGenBuffers(1, &bufferBindMap["lightVertexBuffer"]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferBindMap["lightVertexBuffer"]);
		glBufferData(GL_ARRAY_BUFFER, (fLightGeometry.vertexCount() * 3 * sizeof(float)), fLightGeometry.vertexData(), GL_STATIC_DRAW);
		
		// bind vertex positions
		shaderBindMap["lightposition"] = glGetAttribLocation(bufferBindMap["phongShader"], "position");
		glVertexAttribPointer(shaderBindMap["lightposition"], 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(shaderBindMap["lightposition"]);

		//////////////////////////
		// VBO 2 - Light Normal //		
		//////////////////////////

		// read normal positions
		glGenBuffers(1, &bufferBindMap["lightNormalBuffer"]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferBindMap["lightNormalBuffer"]);
		glBufferData(GL_ARRAY_BUFFER, ((fLightGeometry.normalCount()) * 3  * sizeof(float)), fLightGeometry.normalData(), GL_STATIC_DRAW);
		
		// bind normal positions
		shaderBindMap["lightnormal"] = glGetAttribLocation(bufferBindMap["phongShader"], "normal");
		glVertexAttribPointer(shaderBindMap["lightnormal"], 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(shaderBindMap["lightnormal"]);

		//////////////////////
		// Uniforms - Model //
		//////////////////////

		// Set Color Vec in Shader
		shaderBindMap["modelColor"] = glGetUniformLocation(bufferBindMap["phongShader"], "modelColor");
		glUniform3fv(shaderBindMap["modelColor"], 1, &fModelColorBuffer[0]);

		//////////////////////
		// Uniforms - Light //
		//////////////////////

		// set light source colours
		shaderBindMap["lightColor"] = glGetUniformLocation(bufferBindMap["phongShader"], "lightColor");
		glUniform3fv(shaderBindMap["lightColor"], sizeof(fLightColorBuffer), &fLightColorBuffer[0][0]);
		
		// set light source positions
		shaderBindMap["lightpositions"] = glGetUniformLocation(bufferBindMap["phongShader"], "lightpositions");
		glUniform3fv(shaderBindMap["lightpositions"], sizeof(fLightPositions), &fLightPositions[0][0]);

		//////////////////////
		// Uniforms - Phong //
		//////////////////////
		
		// render mode
		shaderBindMap["renderType"] = glGetUniformLocation(bufferBindMap["phongShader"], "renderType");;
		glUniform1i(shaderBindMap["renderType"], fRenderType);

		// object shine
		shaderBindMap["shine"] = glGetUniformLocation(bufferBindMap["phongShader"], "shine");;
		glUniform1f(shaderBindMap["shine"],fShineBuffer);

		// ambient light
		shaderBindMap["ambprod"] = glGetUniformLocation(bufferBindMap["phongShader"], "ambprod");
		glUniform1f(shaderBindMap["ambprod"], fAmbBuffer);
		
		// diffuse lights
		shaderBindMap["diffprod"] = glGetUniformLocation(bufferBindMap["phongShader"], "diffprod");
		glUniform1fv(shaderBindMap["diffprod"], sizeof(fDiffBuffer), fDiffBuffer);
		
		// specular lights
		shaderBindMap["specprod"] = glGetUniformLocation(bufferBindMap["phongShader"], "specprod");
		glUniform1fv(shaderBindMap["specprod"], sizeof(fSpecBuffer), fSpecBuffer);

		///////////////////////
		// Uniforms - Camera //
		///////////////////////

		// Set up Transform Vec in Shader
		shaderBindMap["transformMVP"] = glGetUniformLocation(bufferBindMap["phongShader"], "transformMVP");
		glUniformMatrix4fv(shaderBindMap["transformMVP"], 1, GL_FALSE, &fMVPTransformBuffer[0][0]);

		shaderBindMap["transformM"] = glGetUniformLocation(bufferBindMap["phongShader"], "transformM");
		glUniformMatrix4fv(shaderBindMap["transformM"], 1, GL_FALSE, &fMTransformBuffer[0][0]);

		shaderBindMap["transformV"] = glGetUniformLocation(bufferBindMap["phongShader"], "transformV");
		glUniformMatrix4fv(shaderBindMap["transformV"], 1, GL_FALSE, &fVTransformBuffer[0][0]);
		
		shaderBindMap["transformN"] = glGetUniformLocation(bufferBindMap["phongShader"], "transformN");
		glUniformMatrix3fv(shaderBindMap["transformN"], 1, GL_FALSE, &fNTransformBuffer[0][0]);

		////////////
		// Camera //
		////////////

		// setup static camera
		fProjectionMatrix = getProjectionMatrix();
		fViewMatrix = getViewMatrix();

		//////////
		// Done //
		//////////

		// show success status
		glPrintError("    = Setup complete", true);
	}
	
	bool OpenGLWindow::handleEvent(SDL_Event e)
	{
		// initialize
		const Uint8 * iKeyState = SDL_GetKeyboardState(NULL);
		float dDelta = 0.2f;
		glm::vec3 vTransformDelta = {0,0,0};

		if (fControlMode == CAMERA)
		{
			dDelta = 2.5f;
		}
		else
		{
			dDelta = 0.2f;
		}

		if (e.type == SDL_KEYDOWN)
		{
			//////////////////////
			// General Controls //
			//////////////////////

			// ESC to exit
			if (e.key.keysym.sym == SDLK_ESCAPE)
			{
				// The program will exit if this function returns false
				return false;
			}

			// Reset Scene
			if (e.key.keysym.sym == SDLK_d) // dump light data
			{
				std::cout << "\n - Lighting Configuration\n";
				std::cout << "    - Global Settings\n";
				std::cout << "      - Shine: " << fShineBuffer << "\n";
				std::cout << "      - Ambient: " << fAmbBuffer << "\n";
				std::cout << "    - Light 1 Settings\n";
				std::cout << "      - Color: " << fLightColorBuffer[0].x << ", " << fLightColorBuffer[0].y << ", " << fLightColorBuffer[0].z << "\n";
				std::cout << "      - Diffuse: " << fDiffBuffer[0] << "\n";
				std::cout << "      - Specular: " << fSpecBuffer[0] << "\n";
				std::cout << "    - Light 2 Settings\n";
				std::cout << "      - Color: " << fLightColorBuffer[1].x << ", " << fLightColorBuffer[1].y << ", " << fLightColorBuffer[1].z << "\n";
				std::cout << "      - Diffuse: " << fDiffBuffer[1] << "\n";
				std::cout << "      - Specular: " << fSpecBuffer[1] << "\n";
			}

			//////////////////
			// Render Modes //
			//////////////////

			if (e.key.keysym.sym == SDLK_q)
			{
				fRenderMode = MESH;
			}
			else if (e.key.keysym.sym == SDLK_w)
			{
				fRenderMode = PLAIN;
			}
			else if (e.key.keysym.sym == SDLK_e)
			{
				if (fModelGeometry.textureCoordCount() == 0)
				{
					std::cout << "\n - Object has no texture coordinates, reverting to PLAIN rendering mode.\n";
					fRenderMode = PLAIN;
				}
				else
				{
					fRenderMode = TEXTURED;
				}
			}
			else if (e.key.keysym.sym == SDLK_r)
			{
				if (fModelGeometry.textureCoordCount() == 0)
				{
					std::cout << "\n - Object has no texture coordinates, reverting to PLAIN rendering mode.\n";
					fRenderMode = PLAIN;
				}
				else
				{
					fRenderMode = BUMPMAPPED;
				}
			}
			else if (e.key.keysym.sym == SDLK_a)
			{
				fAutoRotate = !fAutoRotate;
			}
			
			//////////////////
			// Control Mode //
			//////////////////

			if (e.key.keysym.sym == SDLK_1)
			{
				fControlMode = CAMERA;
			}
			else if (e.key.keysym.sym == SDLK_2)
			{
				fControlMode = MODEL;
			}
			else if (e.key.keysym.sym == SDLK_3)
			{
				fControlMode = LIGHT1;
			}
			else if (e.key.keysym.sym == SDLK_4)
			{
				fControlMode = LIGHT2;
			}

			/////////////////
			// Input Modes //
			/////////////////

			if (iKeyState[SDL_SCANCODE_LALT] && iKeyState[SDL_SCANCODE_LCTRL])
			{
				fInputMode = SCALE;
			}
			else if (iKeyState[SDL_SCANCODE_LALT])
			{
				fInputMode = TRANSLATE;
			}
			else if (iKeyState[SDL_SCANCODE_LCTRL])
			{
				fInputMode = ROTATE;
			}
			else
			{
				fInputMode = DISABLED;
			}
		}
		else if (e.type == SDL_MOUSEMOTION) // case: Mouse Controls
		{
			////////////////////
			// Mouse Controls //
			////////////////////

			// check if wheel or mouse motion
			if (e.type == SDL_MOUSEMOTION)
			{
				if (e.motion.x < fMouseMoveX)
				{
					vTransformDelta.x += dDelta;
				}
				else if(e.motion.x > fMouseMoveX)
				{
					vTransformDelta.x -= dDelta;
				}
				if (e.motion.y < fMouseMoveY)
				{
					vTransformDelta.y += dDelta;
				}
				else if (e.motion.y > fMouseMoveY)
				{
					vTransformDelta.y -= dDelta;
				}
				// update data
				fMouseMoveX = e.motion.x;
				fMouseMoveY = e.motion.y;
			}
		}
		else if (e.type == SDL_MOUSEWHEEL)
		{
			// check type of change
			if (e.wheel.y == 1)
			{
				vTransformDelta.z += dDelta;
			}
			else if (e.wheel.y == -1)
			{
				vTransformDelta.z -= dDelta;
			}
		}
		else
		{
			fInputMode = DISABLED;
		}

		// apply controls
		if (fControlMode == CAMERA)
		{
			switch(fInputMode)
			{
				case TRANSLATE:
					fCameraPosition += glm::vec3(vTransformDelta.x, 0.0f, 0.0f);
					break;
				case ROTATE:
					fCameraRotate += (vTransformDelta * 0.05f);
					break;
				case SCALE:
					fAmbBuffer += (vTransformDelta.z * 0.01f);
					clampFloat(fAmbBuffer, 0.0f, 10.0f);
					glUniform1f(shaderBindMap["ambprod"], fAmbBuffer);
			}
		}
		else if (fControlMode == MODEL)
		{
			switch(fInputMode)
			{
				case TRANSLATE:
					fModelPosition += vTransformDelta;
					break;
				case ROTATE:
					fModelRotate += vTransformDelta;
					break;
				case SCALE:
					fModelColor += vTransformDelta;
					clampVector(fModelColor, 0.0f, 1.0f);
					break;
			}
		}
		else if (fControlMode == LIGHT1)
		{
			switch(fInputMode)
			{
				case TRANSLATE:
					fLightPositions[0] += vTransformDelta;
					glUniform3fv(shaderBindMap["lightpositions"], sizeof(fLightPositions), &fLightPositions[0][0]);
					break;
				case ROTATE:
					fLightColorBuffer[0] += vTransformDelta;
					clampVector(fLightColorBuffer[0], 0.0f, 1.0f);
					glUniform3fv(shaderBindMap["lightColor"], sizeof(fLightColorBuffer), &fLightColorBuffer[0][0]);
					break;
				case SCALE:
					fShineBuffer += (vTransformDelta.z);
					clampFloat(fShineBuffer, 0.0f, 15.0f);
					glUniform1f(shaderBindMap["shine"],fShineBuffer);
					fDiffBuffer[0] += vTransformDelta.y;
					clampFloat(fDiffBuffer[0], 0.0f, 10.0f);
					glUniform1fv(shaderBindMap["diffprod"], sizeof(fDiffBuffer), fDiffBuffer);
					fSpecBuffer[0] += vTransformDelta.x;
					clampFloat(fSpecBuffer[0], 0.0f, 10.0f);
					glUniform1fv(shaderBindMap["specprod"], sizeof(fSpecBuffer), fSpecBuffer);
					break;
			}
		}
		else if (fControlMode == LIGHT2)
		{
			switch(fInputMode)
			{
				case TRANSLATE:
					fLightPositions[1] += vTransformDelta;
					glUniform3fv(shaderBindMap["lightpositions"], sizeof(fLightPositions), &fLightPositions[0][0]);
					break;
				case ROTATE:
					fLightColorBuffer[1] += vTransformDelta;
					clampVector(fLightColorBuffer[1], 0.0f, 1.0f);
					glUniform3fv(shaderBindMap["lightColor"], sizeof(fLightColorBuffer), &fLightColorBuffer[0][0]);
					break;
				case SCALE:
					fShineBuffer += (vTransformDelta.z);
					clampFloat(fShineBuffer, 0.0f, 15.0f);
					glUniform1f(shaderBindMap["shine"],fShineBuffer);
					fDiffBuffer[1] += vTransformDelta.y;
					glUniform1fv(shaderBindMap["diffprod"], sizeof(fDiffBuffer), fDiffBuffer);
					clampFloat(fDiffBuffer[1], 0.0f, 10.0f);
					fSpecBuffer[1] += vTransformDelta.x;
					clampFloat(fSpecBuffer[1], 0.0f, 10.0f);
					glUniform1fv(shaderBindMap["specprod"], sizeof(fSpecBuffer), fSpecBuffer);
					break;
			}
		}

		//////////
		// Done //
		//////////

		return true;
	}

	void OpenGLWindow::render()
	{
		////////////
		// Config //
		////////////

		// clear screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		////////////////
		// AutoRotate //
		////////////////

		if (fAutoRotate == true)
		{
			// increment orbit angle
			fRotationAngle += 0.02f;
			// orbit light rource 1
			fLightPositions[0].x = sin(fRotationAngle) * fOrbitDistace[0];
			fLightPositions[0].y = cos(fRotationAngle) * fOrbitDistace[0];
			fLightPositions[0].z = sin(fRotationAngle) * fOrbitDistace[0];
			// orbit light rource 2
			fLightPositions[1].x = cos(fRotationAngle) * fOrbitDistace[1];
			fLightPositions[1].y = sin(fRotationAngle) * fOrbitDistace[1];
			fLightPositions[1].z = cos(fRotationAngle) * fOrbitDistace[1];
			// clamp
			if (fRotationAngle > 360.5f)
			{
				fRotationAngle = 0.0f;
			}
			// upload new positions
			glUniform3fv(shaderBindMap["lightpositions"], sizeof(fLightPositions), &fLightPositions[0][0]);
			// camera rotation
			fCameraRotate.y += 0.008f;
			if (fCameraRotate.y > 360.f)
			{
				fCameraRotate.y = 0.0f;
			}
		}

		///////////
		// Model //
		///////////
		
		// enable model VAO
		glBindVertexArray(bufferBindMap["modelVAO"]);

		// generate transformation matrix for parent
		fProjectionMatrix = getProjectionMatrix();
		fViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = {{1,0,0,0} ,{0,1,0,0} ,{0,0,1,0} ,{0,0,0,1}};
		ModelMatrix *= glm::translate(fModelPosition);
		ModelMatrix *= glm::rotate(fModelRotate.x, glm::vec3(1.0f,0.0f,0.0f));		
		ModelMatrix *= glm::rotate(fModelRotate.y, glm::vec3(0.0f,1.0f,0.0f)); 
		ModelMatrix *= glm::rotate(fModelRotate.z, glm::vec3(0.0f,0.0f,1.0f)); 
		ModelMatrix *= glm::scale(fModelScale);
		fillMTransformBuffer(ModelMatrix);
		fillVTransformBuffer(fViewMatrix);
		fillMVPTransformBuffer(fProjectionMatrix * fViewMatrix * ModelMatrix);
		
		// Generate Normal Transformation Matrix
		glm::mat3 NormalMatrix = glm::transpose(glm::inverse(glm::mat3(fViewMatrix * ModelMatrix)));
		fillNTransformBuffer(NormalMatrix);
		
		// Set Model Color
		fillColorBuffer({ fModelColor.x, fModelColor.y , fModelColor.z });

		// render model
		switch(fRenderMode)
		{
			case MESH:
				setRenderType(1);
				glDrawArrays(GL_LINES, 0, fModelGeometry.vertexCount());
				break;
			case PLAIN:
				setRenderType(1);
				glDrawArrays(GL_TRIANGLES, 0, fModelGeometry.vertexCount());
				break;
			case TEXTURED:
				setRenderType(2);
				glDrawArrays(GL_TRIANGLES, 0, fModelGeometry.vertexCount());
				break;
			case BUMPMAPPED:
				setRenderType(3);
				glDrawArrays(GL_TRIANGLES, 0, fModelGeometry.vertexCount());
				break;
		}
		
		/////////////
		// Light 1 //
		/////////////

		// enable light VAO
		glBindVertexArray(bufferBindMap["lightVAO"]);

		// generate transformation matrix for light
		fProjectionMatrix = getProjectionMatrix();
		fViewMatrix = getViewMatrix();
		ModelMatrix = {{1,0,0,0} ,{0,1,0,0} ,{0,0,1,0} ,{0,0,0,1}};
		ModelMatrix *= glm::translate(fLightPositions[0]);
		ModelMatrix *= glm::scale(fLightScale);
		fillMTransformBuffer(ModelMatrix);
		fillVTransformBuffer(fViewMatrix);
		fillMVPTransformBuffer(fProjectionMatrix * fViewMatrix * ModelMatrix);

		// Generate Normal Transformation Matrix
		NormalMatrix = glm::transpose(glm::inverse(glm::mat3(fViewMatrix * ModelMatrix)));
		fillNTransformBuffer(NormalMatrix);
		
		// set colour
		setRenderType(0);
		fillColorBuffer({ fLightColorBuffer[0].x, fLightColorBuffer[0].y, fLightColorBuffer[0].z});
		
		// render light
		glDrawArrays(GL_TRIANGLES, 0, fLightGeometry.vertexCount());

		/////////////
		// Light 2 //
		/////////////

		// enable light VAO
		glBindVertexArray(bufferBindMap["lightVAO"]);

		// generate transformation matrix for light
		fProjectionMatrix = getProjectionMatrix();
		fViewMatrix = getViewMatrix();
		ModelMatrix = {{1,0,0,0} ,{0,1,0,0} ,{0,0,1,0} ,{0,0,0,1}};
		ModelMatrix *= glm::translate(fLightPositions[1]);
		ModelMatrix *= glm::scale(fLightScale);
		fillMTransformBuffer(ModelMatrix);
		fillVTransformBuffer(fViewMatrix);
		fillMVPTransformBuffer(fProjectionMatrix * fViewMatrix * ModelMatrix);

		// Generate Normal Transformation Matrix
		NormalMatrix = glm::transpose(glm::inverse(glm::mat3(fViewMatrix * ModelMatrix)));
		fillNTransformBuffer(NormalMatrix);
		
		// set colour
		setRenderType(0);
		fillColorBuffer({ fLightColorBuffer[1].x, fLightColorBuffer[1].y, fLightColorBuffer[1].z});
		
		// render light
		glDrawArrays(GL_TRIANGLES, 0, fLightGeometry.vertexCount());

		//////////
		// Done //
		//////////

		// output buffer to screen
		SDL_GL_SwapWindow(sdlWin);
	}

	void OpenGLWindow::cleanup()
	{
		// clear VBOs
		glDeleteBuffers(1, &bufferBindMap["modelVertexBuffer"]);
		glDeleteBuffers(1, &bufferBindMap["modelNormalBuffer"]);
		glDeleteBuffers(1, &bufferBindMap["modelTextureCoordBuffer"]);
		glDeleteBuffers(1, &bufferBindMap["modelTangentBuffer"]);
		glDeleteBuffers(1, &bufferBindMap["modelBitangetBuffer"]);
		glDeleteBuffers(1, &bufferBindMap["lightVertexBuffer"]);
		glDeleteBuffers(1, &bufferBindMap["lightNormalBuffer"]);
		// clear VAO
		glDeleteVertexArrays(1, &bufferBindMap["modelVAO"]);
		glDeleteVertexArrays(1, &bufferBindMap["lightVAO"]);
		// clear textures
		glDeleteTextures(1, &bufferBindMap["modelTexture"]);
		glDeleteTextures(1, &bufferBindMap["modelBumpMap"]);
		// destroy window
		SDL_DestroyWindow(sdlWin);
	}
	
	//// Utilities
	void OpenGLWindow::fillMVPTransformBuffer(glm::mat4 theData)
	{
		fMVPTransformBuffer = theData;
		glUniformMatrix4fv(shaderBindMap["transformMVP"], 1, GL_FALSE, &fMVPTransformBuffer[0][0]);
	}

	void OpenGLWindow::fillMTransformBuffer(glm::mat4 theData)
	{
		fMTransformBuffer = theData;
		glUniformMatrix4fv(shaderBindMap["transformM"], 1, GL_FALSE, &fMTransformBuffer[0][0]);
	}

	void OpenGLWindow::fillVTransformBuffer(glm::mat4 theData)
	{
		fVTransformBuffer = theData;
		glUniformMatrix4fv(shaderBindMap["transformV"], 1, GL_FALSE, &fVTransformBuffer[0][0]);
	}

	void OpenGLWindow::fillNTransformBuffer(glm::mat3 theData)
	{
		fNTransformBuffer = theData;
		glUniformMatrix4fv(shaderBindMap["transformN"], 1, GL_FALSE, &fNTransformBuffer[0][0]);
	}

	void OpenGLWindow::fillColorBuffer(glm::vec3 theColor)
	{
		fModelColorBuffer = theColor;
		glUniform3fv(shaderBindMap["modelColor"], 1, &fModelColorBuffer[0]);
	}

	void OpenGLWindow::setRenderType(int theRenderType)
	{
		fRenderType = theRenderType;
		glUniform1i(shaderBindMap["renderType"], fRenderType);
	}

	glm::mat4 OpenGLWindow::getViewMatrix()
	{
		// recalculate
		glm::mat4 viewMatrix = glm::lookAt(fCameraPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		viewMatrix *= glm::rotate(fCameraRotate.x, glm::vec3(1.0f,0.0f,0.0f));		
		viewMatrix *= glm::rotate(fCameraRotate.y, glm::vec3(0.0f,1.0f,0.0f)); 
		viewMatrix *= glm::rotate(fCameraRotate.z, glm::vec3(0.0f,0.0f,1.0f)); 
		// done
		return viewMatrix;

	}
	
	glm::mat4 OpenGLWindow::getProjectionMatrix()
	{
		// recalculate
		glm::mat4 projectionMatrix = glm::perspective(120.0f, ((float)fWidth) / fHeight, 0.001f, 300.0f);
		// done
		return projectionMatrix;
	}

	void OpenGLWindow::clampVector(glm::vec3 & theVector, float minValue, float maxValue)
	{
		for(int i = 0; i < 3; ++i)
		{
			if (theVector[i] < minValue)
			{
				theVector[i] = minValue;	
			}
			else if (theVector[i] > maxValue)
			{
				theVector[i] = maxValue;
			}
		}
	}

	void OpenGLWindow::clampFloat(float & theValue, float minValue, float maxValue)
	{
		if (theValue < minValue)
		{
			theValue = minValue;	
		}
		else if (theValue > maxValue)
		{
			theValue = maxValue;
		}
	}
}