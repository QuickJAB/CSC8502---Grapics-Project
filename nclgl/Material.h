#pragma once

#include <glad/glad.h>

class Material {
public:
	Material() {}
	Material(GLuint texture, GLuint bumpmap, float specularity, float metallic, float roughness, float reflectiveness);
	~Material() {}

	GLuint GetTexture() { return texture; }
	GLuint64 GetTextureHandle() { return textureHndl; }

protected:
	GLuint64 textureHndl, bumpmapHndl;
	float specularity, metallic, roughness, reflectiveness;
	//float specularity;
	GLuint texture, bumpmap, pad1, pad2;
};