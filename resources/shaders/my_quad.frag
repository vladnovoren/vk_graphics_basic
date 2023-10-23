#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) out vec4 color;

layout (binding = 0) uniform sampler2D colorTex;

layout (location = 0 ) in VS_OUT {
  vec2 texCoord;
} surf;

const vec3 kLuminanceCoeffs = vec3(0.2126, 0.7152, 0.0722);

float CalcLuminance(in vec3 color) {
  return dot(kLuminanceCoeffs, color);
}

bool CompareLuminance(in vec3 first, in vec3 second) {
  return CalcLuminance(first) < CalcLuminance(second);
}

void BubbleSort(inout vec4 array[9]) {
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8 - i; ++j) {
      if (CompareLuminance(vec3(array[j]), vec3(array[j + 1])) == false) {
        vec4 tmp = array[j];
        array[j] = array[j + 1];
        array[j + 1] = tmp;
      }
    }
  }
}

vec4 MedianFilter() {
  vec4 neighbours[9] = vec4[9](
    textureOffset(colorTex, surf.texCoord, ivec2(-1, -1)),
    textureOffset(colorTex, surf.texCoord, ivec2(-1,  0)),
    textureOffset(colorTex, surf.texCoord, ivec2(-1,  1)),
    textureOffset(colorTex, surf.texCoord, ivec2( 0, -1)),
    textureOffset(colorTex, surf.texCoord, ivec2( 0,  0)),
    textureOffset(colorTex, surf.texCoord, ivec2( 0,  1)),
    textureOffset(colorTex, surf.texCoord, ivec2( 1, -1)),
    textureOffset(colorTex, surf.texCoord, ivec2( 1,  0)),
    textureOffset(colorTex, surf.texCoord, ivec2( 1,  1))
  );
  BubbleSort(neighbours);
  return neighbours[4];
}

void main() {
  color = MedianFilter();
}
