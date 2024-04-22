#version 410 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

#define POINT_LIGHTS_NUM 4

struct Material {
	sampler2D diffuseMap;
	sampler2D specularMap;
	float shininess;
};

struct DirLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 pos;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
};

struct SpotLight {
	vec3 pos;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float innerCutOff;
	float outerCutOff;
	float constant;
	float linear;
	float quadratic;
};

uniform vec3 viewPos;
uniform Material material;
uniform PointLight pointLights[POINT_LIGHTS_NUM];
uniform SpotLight spotLight;
uniform DirLight dirLight;

vec3 calcDirLight(DirLight dirLight, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight pointLight, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calcSpotLight(SpotLight spotLight, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
	vec3 normal = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 result = vec3(0.0f);
	//result = calcDirLight(dirLight, normal, viewDir);
	//for (int i = 0; i < POINT_LIGHTS_NUM; i++) {
	//	result += calcPointLight(pointLights[i], normal, FragPos, viewDir);
	//}
	result += calcSpotLight(spotLight, normal, FragPos, viewDir);

	FragColor = vec4(result, 1.0);
}

vec3 calcDirLight(DirLight dirLight, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(-dirLight.direction);
	float diff_contrib = max(dot(normal, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec_degree = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	vec3 ambient = dirLight.ambient * texture(material.diffuseMap, TexCoords).rgb;
	vec3 diffuse = dirLight.diffuse * diff_contrib * texture(material.diffuseMap, TexCoords).rgb;
	vec3 specular = dirLight.specular * spec_degree * texture(material.specularMap, TexCoords).rgb;
	return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight pointLight, vec3 normal, vec3 fragPos, vec3 viewDir) {
	vec3 lightDir = normalize(pointLight.pos - fragPos);
	float diff_contrib = max(dot(normal, lightDir), 0.0);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec_degree = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	float distance = length(pointLight.pos - fragPos);
	float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));

	vec3 ambient = pointLight.ambient * texture(material.diffuseMap, TexCoords).rgb;
	vec3 diffuse = pointLight.diffuse * diff_contrib * texture(material.diffuseMap, TexCoords).rgb;
	vec3 specular = pointLight.specular * spec_degree * texture(material.specularMap, TexCoords).rgb;
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	vec3 lightDir = normalize(light.pos - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.pos - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * vec3(texture(material.diffuseMap, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuseMap, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specularMap, TexCoords));

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

	return (ambient + diffuse + specular);
}
