#version 330 core

//// Inputs
in vec3 position;
in vec3 normal;
in vec2 textureUV;
in vec3 tangent;
in vec3 bitangent;

//// Uniforms
uniform int renderType;
uniform vec3 lightpositions[2];
uniform mat4 transformM;
uniform mat4 transformV;
uniform mat4 transformMVP;
uniform mat3 transformN;

//// Outputs
out vec3 N;
out vec3 T;
out vec3 B;
out vec3 L[2];
out vec3 E[2];
out vec2 UV;

//// Run Loop
void main()
{
	// Generate Model View Matrix
    mat4 transformMV = transformV * transformM;
	// Generate vectors for Tangent Space
	N = normalize((transformMV * vec4(normal, 0.0f)).xyz);
	T = normalize((transformMV * vec4(tangent, 0.0f)).xyz);
	B = normalize((transformMV * vec4(bitangent, 0.0f)).xyz);
	// Process Light Source INteractions
	for(int i=0; i<2; i++)
	{
		L[i] = normalize((transformV * vec4(lightpositions[i],1.0f)).xyz - (transformMV * vec4(position, 1.0f)).xyz);
		E[i] = normalize((transformMV * vec4(-1.0f * position, 1.0f)).xyz);
	}
	// Pass through UV coordinates
   	UV = textureUV;
   	// set vertex position
   	gl_Position = transformMVP * vec4(position, 1.0f);
}
