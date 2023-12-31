#version 330 core
out vec4 PixelColor;
in vec2 UV;
in vec4 Color;
uniform sampler2D Texture;

void main()
{
    if(Color.a<=0.5)
    {
        PixelColor = vec4(Color.x,Color.y,Color.z,1.0);
    }
    else
    {
        PixelColor = texture2D(Texture,UV) * Color;
    }
}