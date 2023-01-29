#version 460 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;
uniform mat4 shadowMatrix;

uniform float fragType;

uniform vec3 sunPosition;

uniform float time;

in vec3 position;
in vec4 colour;
in vec2 texCoord;
in vec3 normal;
in vec4 tangent;

out Vertex {
	vec3 worldPos;
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
} VERTEX;

out Skybox {
	vec3 viewDir;
} SKYBOX;

out Shadow {
	vec4 shadowProj;
} SHADOW;

vec3 GerstnerWave(in vec4 wave, in vec3 p) {
	float steepness = wave.z;
	float wavelength = wave.w;

	float k = 2 * 3.1415 / wavelength;
	float c = sqrt(98 / k);
	vec2 d = normalize(wave.xy);
	float f = k * (dot(d, p.xz) - c * time);
	float a = steepness / k;

	return vec3(d.x * a * cos(f), a * sin(f), d.y * a * cos(f));
}

void applyWave(in mat4 mvp, in mat3 normalMatrix) {
	vec4 waveA = vec4(1.0, 0.0, 0.1, 500.0);
	vec4 waveB = vec4(0.0, 1.0, 0.2, 500.0);
	vec4 waveC = vec4(1.0, 1.0, 0.15, 250.0);
	
	vec3 gridPoint = position;
	vec3 p = gridPoint;

	p += GerstnerWave(waveA, gridPoint);
	p += GerstnerWave(waveB, gridPoint);
	p += GerstnerWave(waveC, gridPoint);

	VERTEX.worldPos = (modelMatrix * vec4(p, 1)).xyz;

	gl_Position = mvp * vec4(p, 1.0);
}

void main(void) {
	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	VERTEX.worldPos = (modelMatrix * vec4(position, 1)).xyz;
	VERTEX.colour = colour;
	VERTEX.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	VERTEX.normal = normalize(normalMatrix * normalize(normal));
	VERTEX.tangent = normalize(normalMatrix * normalize(tangent.xyz));
	VERTEX.binormal = cross(VERTEX.tangent, VERTEX.normal) * tangent.w;

	vec3 pos = position;
	mat4 invproj = inverse(projMatrix);
	pos.xy *= vec2(invproj [0][0] , invproj [1][1]);
	pos.z = -1.0f;
	SKYBOX.viewDir = transpose(mat3(viewMatrix)) * normalize(pos);

	vec3 shadowDir = normalize(sunPosition - VERTEX.worldPos);
	vec4 pushVal = vec4(VERTEX.normal, 0) * dot(shadowDir, VERTEX.normal);
	SHADOW.shadowProj = shadowMatrix * ((modelMatrix * vec4(position, 1)) + pushVal);

	if (fragType == 0) {
		gl_Position = vec4(position , 1.0);
	}
	else if (fragType == 4) {
		applyWave(mvp, normalMatrix);
	}
	else {
		gl_Position = mvp * vec4(position, 1.0);
	}
}