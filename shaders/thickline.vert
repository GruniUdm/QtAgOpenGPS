#version 440

// Thick line vertex shader
// Expands line segments to quads with constant screen-pixel width
// Uses standard MVP clip space for calculations, then applies NDC matrix at the end

//layout(location = 0) in vec3 pos;

layout(location = 0) in vec3 pos;       // Current vertex
layout(location = 1) in vec3 nextPos;   // Neighbor vertex
layout(location = 2) in float side;     // -1.0 or 1.0 (left/right side of quad)

layout(std140, binding = 0) uniform buf {
    mat4 mvpMatrix;     // projection * modelview (standard clip space, NDC is [-1,1])
    //mat4 ndcMatrix;     // viewport transform (applied at the end)
    vec4 color;
    //vec2 viewportSize;  // viewport width and height in pixels
    float lineWidth;    // line width in pixels
} ubuf;

out gl_PerVertex {
    vec4 gl_Position;
    float gl_PointSize;
};

void main()
{
    // Step 1: Transform to standard MVP clip space (NDC will be [-1, 1])
    vec4 currClip = ubuf.mvpMatrix * pos;
/*
    vec4 nextClip = ubuf.mvpMatrix * nextPos;
    // Step 2: Calculate line direction using homogeneous coordinates
    // This works correctly even when w is negative (vertex behind camera)
    vec2 lineVec = nextClip.xy * currClip.w - currClip.xy * nextClip.w;
    float len = length(lineVec);

    // Guard against degenerate lines (avoid NaN from normalize)
    vec2 dir = (len > 0.0001) ? (lineVec / len) : vec2(1.0, 0.0);
    vec2 normal = vec2(-dir.y, dir.x);

    // Step 3: Calculate offset in standard clip space
    // In NDC, range is [-1, 1] = 2 units for viewport dimensions
    // 1 pixel = 2 / viewportSize NDC units
    // In clip space: offset = pixelOffset * (2 / viewportSize) * abs(w)
    float halfWidth = ubuf.lineWidth * 0.5;
    vec2 pixelToNDC = vec2(2.0 / ubuf.viewportSize.x, 2.0 / ubuf.viewportSize.y);
    vec2 ndcOffset = normal * halfWidth * pixelToNDC * side;
    vec2 clipOffset = ndcOffset * abs(currClip.w);

    // Apply offset in clip space
    currClip.xy += clipOffset;
*/
    // Step 4: Apply NDC matrix to convert to scene graph's expected clip space
    //gl_Position = ubuf.ndcMatrix * currClip;
    gl_PointSize = 1;
    gl_Position = currClip;
}
