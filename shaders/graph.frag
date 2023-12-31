#version 330 core
out vec4 PixelColor;
in vec2 UV;
in vec4 Color;
vec2 Resolution = vec2(520.0,420.0); // TODO(): PASS THIS AS AN UNIFORM
uniform sampler2D Texture;
uniform vec2 CameraOffset;
float Grid(float Subdivisions, float Thickness, vec2 UV)
{
    UV-=0.5;
    UV += (Thickness / 4.0);
    UV = fract(UV*Subdivisions);
    UV = step(Thickness*(Subdivisions*0.5),UV);
    float Result = min(UV.x,UV.y);
    return 1.0-Result;
}
float F(float X)
{
    float Result = -(sin(X)*(X));
    return Result;
}
float Plot(float Thickness,vec2 UV)
{
    vec2 PixelSize = (1.0/(Resolution.xy));
    float Y = F(UV.x);
    float YMin = Y - F(UV.x + PixelSize.x);
    float YMax = Y - F(UV.x - PixelSize.x);
    float MinDelta = max(PixelSize.y, max(YMin,YMax)); 
    float Distance = distance(UV.y,Y);
    float Result = smoothstep(0.0,MinDelta*Thickness,Distance);
    return 1.0-Result;
}
void main()
{
    const float ZoomFactor = 15.0;
    const float Subdivs = 2.0 + (ZoomFactor-1.0) + floor(1.0-ZoomFactor);
    vec2 TransformedUV = UV + CameraOffset;
    PixelColor = Color;
    PixelColor = mix(PixelColor, vec4(225.0/255.0),Grid(Subdivs*5.0, 0.005,TransformedUV ));
    PixelColor = mix(PixelColor, vec4(190.0/255.0),Grid(Subdivs, 0.0098,TransformedUV ));
    PixelColor = mix(PixelColor, vec4(255.0/255,56.0/255.0,155.0/255.0,1.0),Plot(4.0*ZoomFactor,((TransformedUV -0.5)*2.0)*ZoomFactor));
}