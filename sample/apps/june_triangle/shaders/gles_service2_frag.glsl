precision mediump float;
varying vec2 vTexCoord;
uniform sampler2D uTexture;
const int texWidth = 32;
const int texHeight = 32;

void main()
{
    // 기준 색상을 texture의 첫번째 texel에서 샘플링
    vec4 refColor = texture2D(
        uTexture,
        vec2(0.5 / float(texWidth), 0.5 / float(texHeight)));

    bool isUniform = true;
    for (int y = 0; y < texHeight; y++)
    {
        for (int x = 0; x < texWidth; x++)
        {
            vec2 coord = vec2(
                (float(x) + 0.5) / float(texWidth),
                (float(y) + 0.5) / float(texHeight));
            vec4 currentColor = texture2D(uTexture, coord);
            // 기준 색상과의 차이가 아주 작으면 동일하다고 판단
            if (distance(currentColor, refColor) > 0.001)
            {
                isUniform = false;
            }
        }
    }

    // texture의 모든 색상이 동일하면 원래 texture 색상을, 그렇지 않으면 파란색 출력
    if (isUniform)
    {
        gl_FragColor = texture2D(uTexture, vTexCoord);
    }
    else
    {
        gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);
    }
}
