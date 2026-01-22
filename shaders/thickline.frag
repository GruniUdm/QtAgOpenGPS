#version 440

// Thick line fragment shader

layout(std140, binding = 0) uniform buf {
    mat4 mvpMatrix;     // projection * modelview
    mat4 ndcMatrix;     // viewport transform
    mat4 windowMatrix;  // final transform to window clip space
    vec4 color;
    vec2 viewportSize;  // viewport width and height in pixels
    float lineWidth;    // line width in pixels
} ubuf;

layout(location = 0) out vec4 fragColor;

void main()
{
    fragColor = vec4(1.0, 0.0, 0.0, 1.0);  // Debug: hardcoded red
    //fragColor = ubuf.color;
}
