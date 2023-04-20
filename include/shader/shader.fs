#version 460 core

struct Material
{
    vec3 ambient;
    // vec3 diffuse;
    sampler2D diffuse;
    // vec3 specular;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Light
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    vec3 position;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
out vec4 FragColor;

uniform Material material;
uniform Light light;
uniform float time;

uniform vec3 viewPos;

void main()
{
    // 环境光
    // vec3 ambient = light.ambient * material.ambient;
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    // 漫反射
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * vec3(texture(material.diffuse, TexCoords));

    // 镜面反射
    float Shininess = material.shininess;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Shininess);
    vec3 specular = spec * light.specular * vec3(texture(material.specular, TexCoords));

    float weight = smoothstep(1.0, 0.2, diff);
    vec3 emission = weight * vec3(texture(material.emission, vec2(TexCoords.x, TexCoords.y + time * 0.3)));

    vec3 result = ambient + diffuse + specular + emission;
    FragColor = vec4(result, 1.0);
}