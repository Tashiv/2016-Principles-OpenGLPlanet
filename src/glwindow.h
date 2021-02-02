//// Include Gaurds
#ifndef GL_WINDOW_H
#define GL_WINDOW_H

//// OS Specific Imports
#ifndef __linux__
#include <Windows.h>
#endif

//// General Imports
#include <SDL/SDL.h>
#include <GL/glew.h>
#include <GL/glew.h>
#include <GLM/glm.hpp>
#include <GLM/vec3.hpp>
#include <GLM/gtx/transform.hpp>

#include <iostream>
#include <stdio.h>
#include <map>

#include "stb_image.h"
#include "geometry.h"

//// Classes Declarations
namespace SWPTAS001
{
	//// Enums
	enum ControlMode {CAMERA, MODEL, LIGHT1, LIGHT2};
	enum InputMode {DISABLED, TRANSLATE, ALLSCALE, SCALE, ROTATE};
	enum RenderMode {MESH, PLAIN, TEXTURED, BUMPMAPPED};
	
	//// Classes
	class OpenGLWindow
	{
		//// Constructors
		public:
			OpenGLWindow();
		
		//// OpenGL Variables
		private:
			SDL_Window* sdlWin;
			std::map<std::string, GLuint> shaderBindMap;
			std::map<std::string, GLuint> bufferBindMap;
			GeometryData fModelGeometry;
			GeometryData fLightGeometry;
			
		//// Buffers
		private:
			glm::mat4 fMVPTransformBuffer = {{1,0,0,0} ,{0,1,0,0} ,{0,0,1,0} ,{0,0,0,1}};			
			glm::mat4 fVTransformBuffer = {{1,0,0,0} ,{0,1,0,0} ,{0,0,1,0} ,{0,0,0,1}};	
			glm::mat4 fMTransformBuffer = {{1,0,0,0} ,{0,1,0,0} ,{0,0,1,0} ,{0,0,0,1}};	
			glm::mat3 fNTransformBuffer = {{1,0,0} ,{0,1,0} ,{0,0,1}};
			glm::vec3 fModelColorBuffer = {0.0f, 0.0f, 1.0f};
			glm::vec3 fLightColorBuffer[2] = {{0.5f, 0.5f, 0.1f}, {0.1f, 0.1f, 0.5f}};
			float fShineBuffer = 1.8f;
			float fAmbBuffer = 0.13f;
			float fDiffBuffer[2] = {2.6f, 2.6f};
			float fSpecBuffer[2] = {8.0f, 8.0f};

		//// Camera
		private:
			glm::vec3 fCameraPosition = {0.0f, 0.0f, -20.0f};
			glm::vec3 fCameraRotate = {0.0f, 0.0f, 0.0f};
			int fWidth = 800;
			int fHeight = 600;
			glm::mat4 fProjectionMatrix;
			glm::mat4 fViewMatrix;
			bool fAutoRotate = true;
			float fRotationAngle = 0;
			float fOrbitDistace[2] = {6.5f, 7.5f};
			
		//// 3D World
		private:
			glm::vec3 fModelColor = {0.5f, 0.2f, 0.2f};
			glm::vec3 fModelPosition = {0.0f, 0.0f, 0.0f};
			glm::vec3 fModelRotate = {0.0f, 0.0f, 0.0f};
			glm::vec3 fModelScale = {2.0f, 2.0f, 2.0f};
			glm::vec3 fLightPositions[2] = {{0.0f, 7.0f, 0.0f}, {7.0f, 0.0f, 0.0f}};
			glm::vec3 fLightScale = {0.5f, 0.5f, 0.5f};

		//// Controls
		private:
			int fRenderType = 0;
			InputMode fInputMode = DISABLED;
			RenderMode fRenderMode = BUMPMAPPED;
			ControlMode fControlMode = CAMERA;
			int fMouseMoveX = 0;
			int fMouseMoveY = 0;
		
		//// Core Routines
		public:
			void initGL();
			void setupLibraries();
			bool handleEvent(SDL_Event e);
			void render();
			void cleanup();
			
		//// Utilities
		public:
			void fillMVPTransformBuffer(glm::mat4 theData);
			void fillMTransformBuffer(glm::mat4 theData);
			void fillVTransformBuffer(glm::mat4 theData);
			void fillNTransformBuffer(glm::mat3 theData);
			void fillColorBuffer(glm::vec3 theColor);
			void setRenderType(int theRenderType);
			glm::mat4 getViewMatrix();
			glm::mat4 getProjectionMatrix();
			void clampVector(glm::vec3 & theVector, float minValue, float maxValue);
			void clampFloat(float & theValue, float minValue, float maxValue);
	};
}
#endif
