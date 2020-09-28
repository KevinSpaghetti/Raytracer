#version 450
#extension GL_ARB_separate_shader_objects : enable

//Add uniform for the dimensions
const int width = 500;
const int height = 500;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D image;

void main() {
    vec2 normFragCoord = ((gl_FragCoord.xy) + vec2(1.0, 1.0));
    vec4 imgcolor = texture(image, normFragCoord.xy / vec2(width, height));

    outColor = vec4(pow(imgcolor.xyz, vec3(1/2.2)), 1.0);
}