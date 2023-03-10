#version 330 core
out vec4 FragColor;
  
in vec2 tc;

uniform sampler2D depthMap;

void main()
{             
    float depthValue = texture(depthMap, tc).r;
    FragColor = vec4(vec3(depthValue+0.5), 1.0);
}  