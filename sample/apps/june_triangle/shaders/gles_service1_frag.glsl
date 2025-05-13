#version 300 es
precision mediump float;
in vec2 vTexCoord;
uniform sampler2D uTexture;
out vec4 fragColor;

void main()
{
    vec4 color = texture(uTexture, vTexCoord);
    fragColor = vec4(color.rgb + 0.02, 1.0);
}
