#version 330 core


out vec4 FragColor;

in vec2 uv;

uniform sampler2D u_diffuse;
uniform vec4 u_color;

void main()
{
    FragColor = texture(u_diffuse,uv);
    // FragColor = vec4(uv,0.0f,1.0f);

}



