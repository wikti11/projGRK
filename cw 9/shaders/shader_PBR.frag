#version 430 core

#define M_PI 3.1415926535897932384626433832795

vec3 AMBIENT = vec3(0.2);

uniform vec3 pointLights[10];
uniform vec3 pointLightsColors[10];

uniform float exposition;

in vec2 vecTex;
in vec3 worldPos;

in vec3 viewDirTS;

in mat3 TBN;

out vec4 outColor;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D aoMap;
uniform sampler2D metalMap;
uniform sampler2D roughMap;
uniform sampler2D displacementMap;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
	float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float denom = NdotH2 * (a2 - 1.0) + 1.0;
    denom = M_PI * denom * denom;
	
    return a2 / max(denom, 0.0000001);
}

float GeometrySchlickGGX(float NdotV, float k)
{
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}
  
float GeometrySmith(vec3 N, vec3 V, vec3 L, float k)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
	
    return ggx1 * ggx2;
}


vec3 fresnelSchlick(float HdotV, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - HdotV, 5.0);
}

vec3 PBR (vec3 lightDir, vec3 lightColor, vec3 normal, vec3 viewDir, float roughness, float metalness, vec3 lPos) {
	// tłumienie - attenuation
	// tłumienie dla koloru
	vec3 attenuatedlightColor = lightColor/pow(length(lPos-worldPos),2)*3;

	// F0
	vec3 F0 = vec3(0.04);
	vec3 albedo = texture2D(albedoMap, vecTex).xyz; // tekstura koloru

	F0 = mix(F0, albedo.rgb, metalness);

	// Half Vector
	vec3 halfVec = normalize(viewDir + lightDir);

	// odległość od źródła światła
	float distance = length(lPos - worldPos);

	// tłumienie po odwrotności odległości
	//float attenuation = 1.0 / distance;

	// tłumienie z wykorzystaniem odwrotności kwadratu odległości
	//float attenuation = 1.0 / (distance * distance);

	// tłumienie z wykorzystaniem interpolacji
	// float t1 = 15.0;
	// float t2 = 100.0;
	// if (distance <= t1) {
	// 	attenuation = 1.0;
	// } else if (t1 < distance && distance < t2) {
	// 	attenuation = (t2 - distance)/(t2 - t1);
	// } else {
	// 	attenuation = 0.0;
	// }

	// tłumienie z wykorzystaniem współczynników a i b
	float a = 0.05;
	float b = 0.01;
	float attenuation = 1.0 / (1.0 + a * distance + b * distance * distance);

	// Cook-Torrance BRDF
	float HdotV = max(dot(halfVec, viewDir), 0.0);
	float NdotH = max(dot(normal, halfVec), 0.0);
	float NdotV = max(dot(normal, viewDir), 0.0000001);
	float NdotL = max(dot(normal, lightDir), 0.0000001);

	float k = pow((roughness+1),2)/8;

	float D = DistributionGGX(normal, halfVec, roughness);
	float G = GeometrySmith(normal, viewDir, lightDir, k);
	vec3 F = fresnelSchlick(HdotV, F0);

	vec3 specular = D * G * F;
	specular /= 4.0 * NdotV * NdotL;

	vec3 kD = vec3(1.0) - F;

	kD *= 1.0 - metalness;

	vec3 Flambert = albedo/M_PI;

	// Bez wygaszania energii światła
	//vec3 Lo = (kD * Flambert + specular) * attenuatedlightColor * NdotL;

	// Wygaszanie energii wraz z odleg�o�ci�
	vec3 Lo = (kD * Flambert + specular) * attenuatedlightColor * attenuation * NdotL;

	return Lo;
}

void main()
{
	vec3 viewDir = normalize(viewDirTS);

	// Normal mapping z tekstury
	vec3 normal = texture2D(normalMap, vecTex).xyz;
	normal = normal * 2.0f - 1.0f;
	normal = normalize(normal);

	// kolor, potrzebny do o�wietlenia ambient
	vec3 albedo = texture2D(albedoMap, vecTex).xyz;

	// Tekstury roughness, metalness, ambient occlusion (tekstura ARM -> w tej kolejno�ci!)
	float r = texture2D(roughMap, vecTex).g;
	float m = texture2D(metalMap, vecTex).b;
	float ao = texture2D(aoMap, vecTex).r;

	vec3 ambient = AMBIENT*albedo*ao;

	vec3 ilumination;

	vec3 col = vec3(0.0f, 0.0f, 0.0f);

	// wiele źródeł światła
	for (int i = 0; i < pointLights.length(); i++) {
		vec3 L = normalize(pointLights[i]-worldPos);
		col += PBR(normalize(TBN * L), pointLightsColors[i], normal, viewDir, r, m, pointLights[i]);
	}

	//gamma correction
	col = col / (albedo + vec3(1.0));
	col = pow(col, vec3(1.0/2.2)); 

	ilumination = ambient+col;
	
	outColor = vec4(1.0 - exp(-ilumination*exposition), 1.0f);
}
