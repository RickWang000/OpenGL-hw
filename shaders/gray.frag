#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{ 
    vec4 color = texture(screenTexture, TexCoords);
    float gray = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    FragColor = vec4(vec3(gray), 1.0);
}