#version 460

out vec4 color;
in vec2 uv0;

layout(binding=0) uniform sampler2D diffuse;
uniform vec4 baseColor;
uniform bool hasTexture;

void main()
{
    if (hasTexture)
    {
        color = texture2D(diffuse, uv0);
    }
    else
    {
        color = baseColor;
    }
    
}
