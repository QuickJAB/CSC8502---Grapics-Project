#version 460 core
#extension GL_ARB_bindless_texture : enable

uniform vec3 cameraPos;

uniform float fragType;
uniform float textureID;

struct Light {
	vec4 colour;
	vec3 pos_dir;
	float radius;
	vec3 pad1;
	float pad4;
	vec3 pad2;
	float pad5;
};

layout (std140) uniform Lights {
	Light light[6];
};

struct Material {
	sampler2D tex;
	sampler2D bump;
	float specularity;
	float metallic;
	float roughness;
	float reflectiveness;
	int tmp1;
	int tmp2;
	int pad1;
	int pad2;
};

layout(std140) uniform Materials {
	Material terMat[4];
	Material meshMat[5];
};

uniform sampler2D shadowTex;

uniform float[4] layerStart;
uniform float maxHeight;

in Vertex {
	vec3 worldPos;
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
} VERTEX;

uniform samplerCube cubeTex;

in Skybox {
	vec3 viewDir;
} SKYBOX;

in Shadow {
	vec4 shadowProj;
} SHADOW;

out vec4 fragColour;

void blendTerrain(inout vec4 blank, inout vec4 bump, inout float spec, inout float metal, inout float rough, inout float reflective) {
	float heightPercentage = VERTEX.worldPos.y / maxHeight;

	for (int i = 0; i < (4 - 1); i++) {
		if (heightPercentage >= layerStart[i]) {
			float range = layerStart[i + 1] - layerStart[i];
			float blend = clamp((heightPercentage - (layerStart[i] * 1.33f)) / range, 0, 1);
			blank = mix(texture(terMat[i].tex, VERTEX.texCoord), texture(terMat[i + 1].tex, VERTEX.texCoord), blend);
			bump = mix(texture(terMat[i].bump, VERTEX.texCoord), texture(terMat[i + 1].bump, VERTEX.texCoord), blend);
			spec = mix(terMat[i].specularity, terMat[i + 1].specularity, blend);
			metal = mix(terMat[i].metallic, terMat[i + 1].metallic, blend);
			rough = mix(terMat[i].roughness, terMat[i + 1].roughness, blend);
		}
	}
}

vec4 getReflection() {
	vec3 viewDir = normalize(cameraPos - VERTEX.worldPos);

	vec3 reflectDir = reflect(-viewDir, normalize(VERTEX.normal));
	vec4 reflectTex = texture(cubeTex, reflectDir);

	return reflectTex;
}

void directionalLightPBR(inout vec4 blank, in vec4 bump, in float spec, in float metal, in float rough, in float reflective) {	
	vec4 diffuse = blank;

	vec3 bumpNormal = bump.rgb;
	mat3 TBN = mat3(normalize(VERTEX.tangent), normalize(VERTEX.binormal), normalize(VERTEX.normal));
	bumpNormal = normalize(TBN * normalize(bumpNormal * 2.0 - 1.0));

	float shadow = 1.0;
	vec3 shadowNDC = SHADOW.shadowProj.xyz / SHADOW.shadowProj.w;
	if (abs(shadowNDC.x) < 1.0f && abs(shadowNDC.y) < 1.0f && abs(shadowNDC.z) < 1.0f) {
		vec3 biasCoord = shadowNDC * 0.5f + 0.5f;
		float shadowZ = texture(shadowTex, biasCoord.xy).x;
		if (shadowZ < biasCoord.z) {
			shadow = 0.0;
		}
	}

	vec3 incident = normalize(light[0].pos_dir);
	vec3 viewDir = normalize(cameraPos - VERTEX.worldPos);
	vec3 halfDir = normalize(incident + viewDir);

	float nl = max(dot(incident, bumpNormal), 0);
	float nv = max(dot(viewDir, bumpNormal), 0);
	float nh = max(dot(halfDir, bumpNormal), 0);
	float hv = max(dot(halfDir, viewDir), 0);

	float D = pow(rough, 2) / (3.1415 * pow((pow(nh, 2) * (pow(rough, 2) - 1) + 1), 2));

	float F = reflective + (1 - reflective) * pow((1 - nv), 5);

	if (reflective > 0) {
		F = 1 - F;
	}

	float k = pow(rough + 1, 2) / 8;
	float G = nv / (nv * (1 - k) + k);

	float DFG = D * F * G;

	vec4 surface = diffuse * vec4(light[0].colour.rgb, 1) * light[0].colour.a;
	vec4 C = surface * (1 - metal);

	vec4 BRDF = ((1 - F) * (C / 3.1415)) + (DFG / (4 * nl * nv));

	blank = BRDF * nl;

	if (fragType == 4) {
		blank = (getReflection() * (1 - F)) + (blank * 0.25);
	}

	blank.rgb *= shadow;

	blank += blank * 0.1;

	if (reflective == 0) {
		blank.a = diffuse.a;
	}
	else {
		blank.a += 0.5;
		clamp(blank.a, 0.0, 1.0);
	}
}

void pointLightPBR(inout vec4 blank, in vec4 bump, in float spec, in float metal, in float rough, in float reflective) {	
	vec4 totalColour = vec4(0, 0, 0, 1);
	vec4 diffuse = blank;

	vec3 bumpNormal = bump.rgb;
	mat3 TBN = mat3(normalize(VERTEX.tangent), normalize(VERTEX.binormal), normalize(VERTEX.normal));
	bumpNormal = normalize(TBN * normalize(bumpNormal * 2.0 - 1.0));

	for (int i = 1; i < 2; i++) {
		vec3 incident = normalize(light[i].pos_dir - VERTEX.worldPos);
		vec3 viewDir = normalize(cameraPos - VERTEX.worldPos);
		vec3 halfDir = normalize(incident + viewDir);

		float nl = max(dot(incident, bumpNormal), 0);
		float nv = max(dot(viewDir, bumpNormal), 0);
		float nh = max(dot(halfDir, bumpNormal), 0);
		float hv = max(dot(halfDir, viewDir), 0);

		float D = pow(rough, 2) / (3.1415 * pow((pow(nh, 2) * (pow(rough, 2) - 1) + 1), 2));

		float F = reflective + (1 - reflective) * pow((1 - nv), 5);

		if (reflective > 0) {
			F = 1 - F;
		}

		float k = pow(rough + 1, 2) / 8;
		float G = nv / (nv * (1 - k) + k);

		float DFG = D * F * G;

		vec4 surface = diffuse * vec4(light[i].colour.rgb, 1) * light[i].colour.a;
		vec4 C = surface * (1 - metal);

		vec4 BRDF = ((1 - F) * (C / 3.1415)) + (DFG / (4 * nl * nv));

		if (fragType == 4) {
			blank = (getReflection() * (1 - F)) + (blank * 0.25);
		}

		float distance = length(light[i].pos_dir - VERTEX.worldPos);
		float attenuation = 1.0 - clamp(distance / light[i].radius, 0.0, 1.0f);

		totalColour += BRDF * nl * attenuation;
	}
	
	blank = mix(blank, totalColour, 0.5);

	if (reflective == 0) {
		blank.a = diffuse.a;
	}
	else {
		blank.a += 0.5;
		clamp(blank.a, 0, 1);
	}
}

void directional(inout vec4 finalColour, in vec4 diffuse, in vec3 bumpNormal, in float metal, in float rough, in float reflective) {
	vec3 incident = normalize(light[0].pos_dir);
	vec3 viewDir = normalize(cameraPos - VERTEX.worldPos);
	vec3 halfDir = normalize(incident + viewDir);

	float nl = max(dot(incident, bumpNormal), 0);
	float nv = max(dot(viewDir, bumpNormal), 0);
	float nh = max(dot(halfDir, bumpNormal), 0);
	float hv = max(dot(halfDir, viewDir), 0);

	float D = pow(rough, 2) / (3.1415 * pow((pow(nh, 2) * (pow(rough, 2) - 1) + 1), 2));

	float F = reflective + (1 - reflective) * pow((1 - nv), 5);

	if (reflective > 0) {
		F = 1 - F;
	}

	float k = pow(rough + 1, 2) / 8;
	float G = nv / (nv * (1 - k) + k);

	float DFG = D * F * G;

	vec4 surface = diffuse * vec4(light[0].colour.rgb, 1) * light[0].colour.a;
	vec4 C = surface * (1 - metal);

	vec4 BRDF = ((1 - F) * (C / 3.1415)) + (DFG / (4 * nl * nv));

	finalColour = BRDF * nl;

	if (fragType == 4) {
		finalColour.rgb = mix(getReflection().rgb * (1 - F), finalColour.rgb, 0.25);
	}
}

void point(in int i, inout vec4 finalColour, in vec4 diffuse, in vec3 bumpNormal, in float metal, in float rough, in float reflective) {
	vec3 incident = normalize(light[i].pos_dir - VERTEX.worldPos);
	vec3 viewDir = normalize(cameraPos - VERTEX.worldPos);
	vec3 halfDir = normalize(incident + viewDir);

	float nl = max(dot(incident, bumpNormal), 0);
	float nv = max(dot(viewDir, bumpNormal), 0);
	float nh = max(dot(halfDir, bumpNormal), 0);
	float hv = max(dot(halfDir, viewDir), 0);

	float D = pow(rough, 2) / (3.1415 * pow((pow(nh, 2) * (pow(rough, 2) - 1) + 1), 2));

	float F = reflective + (1 - reflective) * pow((1 - hv), 5);

	if (reflective > 0) {
		F = 1 - F;
	}

	float k = pow(rough + 1, 2) / 8;
	float G = nv / (nv * (1 - k) + k);

	float DFG = D * F * G;

	vec4 surface = diffuse * vec4(light[i].colour.rgb, 1) * light[i].colour.a;
	vec4 C = surface * (1 - metal);

	vec4 BRDF = ((1 - F) * (C / 3.1415)) + (DFG / (4 * nl * nv));

	float dist = length(light[i].pos_dir - VERTEX.worldPos);
	float attenuation = clamp(1 - (pow(dist, 2) / pow(light[i].radius, 2)), 0, 1);
	attenuation *= attenuation;

	finalColour += BRDF * nl * attenuation;
}

void castLight(inout vec4 blank, in vec4 bump, in float metal, in float rough, in float reflective) {
	vec4 finalColour = vec4(0, 0, 0, 1);
	vec4 diffuse = blank;

	float shadow = 1.0;
	vec3 shadowNDC = SHADOW.shadowProj.xyz / SHADOW.shadowProj.w;
	if (abs(shadowNDC.x) < 1.0f && abs(shadowNDC.y) < 1.0f && abs(shadowNDC.z) < 1.0f) {
		vec3 biasCoord = shadowNDC * 0.5f + 0.5f;
		float shadowZ = texture(shadowTex, biasCoord.xy).x;
		if (shadowZ < biasCoord.z) {
			shadow = 0.0;
		}
	}

	vec3 bumpNormal = bump.rgb;
	mat3 TBN = mat3(normalize(VERTEX.tangent), normalize(VERTEX.binormal), normalize(VERTEX.normal));
	bumpNormal = normalize(TBN * normalize(bumpNormal * 2.0 - 1.0));

	directional(finalColour, diffuse, bumpNormal, metal, rough, reflective);

	finalColour *= shadow;

	for (int i = 1; i < 6; i++) {
		point(i, finalColour, diffuse, bumpNormal, metal, rough, reflective);
	}

	if (reflective == 0) {
		finalColour.a = diffuse.a;
	}
	else {
		finalColour.a += 0.4;
		clamp(finalColour.a, 0.4, 1.0);
	}

	finalColour += diffuse * 0.1;
	blank = finalColour;
}

void applyFog(inout vec4 blank) {
	float dist = distance(VERTEX.worldPos, cameraPos);
	if (dist > 15000) {
		dist = clamp((dist - 15000) / 10000, 0, 1);
		blank += vec4(0, 0.2, 0.6, 1) * dist;
	}
}

void textureMesh(inout vec4 blank, inout vec4 bump, inout float spec, inout float metal, inout float rough, inout float reflective) {
	blank = texture(meshMat[int(textureID)].tex, VERTEX.texCoord);
	bump = texture(meshMat[int(textureID)].bump, VERTEX.texCoord);
	spec = meshMat[int(textureID)].specularity;
	metal = meshMat[int(textureID)].metallic;
	rough = meshMat[int(textureID)].roughness;
	reflective = meshMat[int(textureID)].reflectiveness;
}

void main(void) {
	vec4 blank = vec4(0, 0, 0, 1);
	vec4 bump = vec4(0, 0, 0, 1);
	float spec = 0.0f;
	float metal = 0.0f;
	float rough = 0.0f;
	float reflective = 0.0f;

	switch (int(fragType)) {
	case 0:
		blank = mix(texture(cubeTex, normalize(SKYBOX.viewDir)), vec4(0, 0, 0, 1), 0.33f);
		break;
	case 1:
		blendTerrain(blank, bump, spec, metal, rough, reflective);
		castLight(blank, bump, metal, rough, reflective);
		applyFog(blank);
		break;
	case 2:
		textureMesh(blank, bump, spec, metal, rough, reflective);
		castLight(blank, bump, metal, rough, reflective);
		applyFog(blank);
		break;
	case 3:
		blank = vec4(1.0);
		applyFog(blank);
		break;
	case 4:
		textureMesh(blank, bump, spec, metal, rough, reflective);
		castLight(blank, bump, metal, rough, reflective);
		applyFog(blank);
		break;
	}

	fragColour = blank;
}