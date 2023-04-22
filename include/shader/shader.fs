#version 460 core

struct Material
{
    // vec3 diffuse;
    sampler2D diffuse;
    // vec3 specular;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct PointLight
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    vec3 position;

    float a, b, c;
};

struct DirLight
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    vec3 direction;
};

struct SpotLight
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float a, b, c;

    vec3 position;
    vec3 direction;
    float cutOff;
    float otherCutOff;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
out vec4 FragColor;

uniform Material material;
uniform DirLight dirLight;
uniform PointLight[4] pointLights;
uniform SpotLight spotLight;
uniform float time;

uniform vec3 viewPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * vec3(texture(texture_diffuse1, TexCoords));

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * light.specular * vec3(texture(texture_specular1, TexCoords));

    return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 FragPos)
{
    float dis = length(light.position - FragPos);
    float weight = 1.0 / (light.a * (dis * dis) + light.b * dis + light.c);
    vec3 lightDir = normalize(light.position - FragPos);

    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * vec3(texture(texture_diffuse1, TexCoords));

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * light.specular * vec3(texture(texture_specular1, TexCoords));

    return (ambient + diffuse + specular) * weight;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 FragPos)
{
    vec3 lightDir = normalize(light.position - FragPos);
    float theta = dot(normalize(-light.direction), lightDir);
    // 在范围之内
    float epsilon = light.cutOff - light.otherCutOff;
    float intensity = clamp((theta - light.otherCutOff) / epsilon, 0.0, 1.0);

    float dis = length(light.position - FragPos);
    float weight = 1.0 / (light.a * (dis * dis) + light.b * dis + light.c);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * vec3(texture(material.diffuse, TexCoords));

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * light.specular * vec3(texture(material.specular, TexCoords));

    return (ambient + diffuse + specular) * intensity * weight;
}

void main()
{
    vec3 result = vec3(0.0, 0.0, 0.0);

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // 平行光
    result += calcDirLight(dirLight, norm, viewDir);
    // 点光源
    for(int i = 0; i < 1; i++)
    {
        result += calcPointLight(pointLights[i], norm, viewDir, FragPos);
    }
    // 聚合光
    result += calcSpotLight(spotLight, norm, viewDir, FragPos);

    // float weight = 0.3;
    // vec3 emission = weight * vec3(texture(material.emission, vec2(TexCoords.x, TexCoords.y + time * 0.3)));

    // result += emission;
    FragColor = vec4(result, 1.0);
    // FragColor = texture(texture_diffuse1, TexCoords);
}