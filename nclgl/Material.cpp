#include "Material.h"

Material::Material(GLuint texture, GLuint bumpmap, float specularity, float metallic, float roughness, float reflectiveness) : 
					texture(texture), bumpmap(bumpmap), specularity(specularity), metallic(metallic), roughness(roughness), reflectiveness(reflectiveness) {
	glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameterf(texture, GL_TEXTURE_MAX_ANISOTROPY, 16);
	glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);

	textureHndl = glGetTextureHandleARB(texture);
	glMakeTextureHandleResidentARB(textureHndl);

	if (bumpmap != NULL) {
		glTextureParameteri(bumpmap, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(bumpmap, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameterf(bumpmap, GL_TEXTURE_MAX_ANISOTROPY, 16);
		glTextureParameteri(bumpmap, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(bumpmap, GL_TEXTURE_WRAP_T, GL_REPEAT);

		bumpmapHndl = glGetTextureHandleARB(bumpmap);
		glMakeTextureHandleResidentARB(bumpmapHndl);
	}
	else {
		bumpmapHndl = NULL;
	}
}