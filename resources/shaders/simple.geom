#version 450

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

layout(push_constant) uniform params_t
{
    mat4 mProjView;
    mat4 mModel;
} params;

layout(location = 0) in VS_OUT
{
    vec3 wPos;
    vec3 wNorm;
    vec3 wTangent;
    vec2 texCoord;
} vOut[];

layout(location = 0) out GS_OUT
{
    vec3 wPos;
    vec3 wNorm;
    vec3 wTangent;
    vec2 texCoord;
} gOut;

void main()
{
    for (int i = 0; i < 3; ++i)
    {
        gOut.wPos = vOut[i].wPos;
        gOut.wNorm = vOut[i].wNorm;
        gOut.wTangent = vOut[i].wTangent;
        gOut.texCoord = vOut[i].texCoord;
        gl_Position = params.mProjView * vec4(gOut.wPos, 1.0);
        EmitVertex();
    }
    EndPrimitive();

    for (int i = 0; i < 3; ++i)
    {
        vec3 edge1 = vOut[(i + 1) % 3].wPos - vOut[i].wPos;
        vec3 edge2 = vOut[(i + 2) % 3].wPos - vOut[i].wPos;
        vec3 normal = normalize(cross(edge1, edge2));

        for (float j = 0.1; j <= 1.0; j += 0.4)
        {
            gOut.wPos = vOut[i].wPos + normal * j;
            gOut.wNorm = normal;
            gOut.wTangent = vOut[i].wTangent;
            gOut.texCoord = vOut[i].texCoord;
            gl_Position = params.mProjView * vec4(gOut.wPos, 1.0);
            EmitVertex();
        }
    }
    EndPrimitive();
}
