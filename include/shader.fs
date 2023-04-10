#version 460 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float weight;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
    // FragColor=mix(texture(texture1,TexCoord),texture(texture2,TexCoord),weight);
    FragColor = vec4(lightColor * objectColor, 1.0);
}