#version 450
#extension GL_EXT_debug_printf : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;
layout(binding = 1) uniform sampler2D texSampler;

void main()
{
    debugPrintfEXT("Distance: %f\n", gl_FragCoord.w);
    outColor = vec4(gl_FragCoord.w, gl_FragCoord.w, gl_FragCoord.w, 1.0);//texture(texSampler, fragTexCoord);
}