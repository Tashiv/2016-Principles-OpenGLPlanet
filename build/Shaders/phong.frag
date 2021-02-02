#version 330 core

//// Inputs
in vec3 N;
in vec3 T;
in vec3 B;
in vec3 L[2];
in vec3 E[2];
in vec2 UV;

//// Uniforms
uniform int renderType;
uniform vec3 modelColor;
uniform vec3 lightColor[2];
uniform float shine;
uniform float ambprod;
uniform float diffprod[2];
uniform float specprod[2];
uniform sampler2D modelTexture;
uniform sampler2D modelBumpMap;

//// Run Loop
void main()
{
	// Initialize
	vec3 amb = modelColor * ambprod;
	vec3 diff = vec3(0.0f, 0.0f, 0.0f);
	vec3 spec = vec3(0.0f, 0.0f, 0.0f);
	// Shading Process
	if(renderType == 0) // case: Light Source Shading
	{
		// process all light sources
		for (int i = 0; i < 2; i++)
		{
			// enhance ambient colour
			amb = modelColor * 2.0f;
			// phong model preparation
			vec3 NN = normalize(N);
			vec3 EE = normalize(E[i]);
			vec3 LL = normalize(L[i]);
			vec3 H = normalize(LL+EE);
			// phong model
			float kd = max(dot(LL,NN), 0.0f);
			float ks = pow(max(dot(NN,H),0.0f), shine);
			// accumulative lighting
			diff += lightColor[i] * kd*diffprod[i];
			spec += lightColor[i] * ks*specprod[i];
		}
		// set fragment color
		gl_FragColor = vec4((amb+diff+spec).rgb, 1.0f);
	}
	else if (renderType == 1) // case: Plain Shading
	{
		// process all light sources
		for (int i = 0; i < 2; i++)
		{
			// phong model preparation
			vec3 NN = normalize(N);
			vec3 EE = normalize(E[i]);
			vec3 LL = normalize(L[i]);
			vec3 H = normalize(LL+EE);
			// phong model
			float kd = max(dot(LL,NN), 0.0f);
			float ks = pow(max(dot(NN,H),0.0f), shine);
			// accumulative lighting
			diff += lightColor[i] * kd*diffprod[i];
			spec += lightColor[i] * ks*specprod[i];
		}
		// set fragment color
		gl_FragColor = vec4((amb+diff+spec).rgb, 1.0f);
	}
	else if (renderType == 2) // case: Texture Shading
	{
		// process all light sources
		for (int i = 0; i < 2; i++)
		{
			// phong model preparation
			vec3 NN = normalize(N);
			vec3 EE = normalize(E[i]);
			vec3 LL = normalize(L[i]);
			vec3 H = normalize(LL+EE);
			// phong model
			float kd = max(dot(LL,NN), 0.0f);
			float ks = pow(max(dot(NN,H),0.0f), shine);
			// accumulative lighting
			diff += lightColor[i] * kd*diffprod[i];
			spec += lightColor[i] * ks*specprod[i];
		}
		// set fragment color
		gl_FragColor = vec4(texture2D(modelTexture, UV ).rgb * (amb+diff), 1.0f) + vec4((spec).rgb, 1.0f);
	}
	else if (renderType == 3) // case: Bump Texture Shading
	{
		// process all light sources
		for (int i = 0; i < 2; i++)
		{
			// generate tangent space matrix
			mat3 TBN = transpose(mat3(T,B,N));
			// sample bump map
			vec3 bumpNormal = normalize(texture2D( modelBumpMap, UV ).rgb * 2 - 1);
			// phong model preparation
			vec3 NN = bumpNormal;
			vec3 EE = TBN * normalize(E[i]);
			vec3 LL = TBN * normalize(L[i]);
			vec3 H = normalize(LL+EE);
			// phong model
			float kd = max(dot(LL,NN), 0.0f);
			float ks = pow(max(dot(NN,H),0.0f), shine);
			// accumulative lighting
			diff += lightColor[i] * kd*diffprod[i];
			spec += lightColor[i] * ks*specprod[i];
		}
		// set fragment color
		gl_FragColor = vec4(texture2D( modelTexture, UV ).rgb * (amb+diff), 1.0f) + vec4((spec).rgb, 1.0f);
	}
}
