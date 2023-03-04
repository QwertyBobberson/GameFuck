#version 450
#extension GL_EXT_debug_printf : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;
layout(set = 1, binding = 1) uniform sampler2D texSampler;

void main()
{
    // if(fragTexCoord.x > 0.4 && fragTexCoord.x < 0.6 && fragTexCoord.y > 0.4 && fragTexCoord.y < 0.6)
    // {
    //     debugPrintfEXT("Position: %v4f\n", texture(texSampler, fragTexCoord));
    // }
    outColor = vec4(fragColor, 1.0);//texture(texSampler, fragTexCoord); //vec4(gl_FragCoord.w, gl_FragCoord.w, gl_FragCoord.w, 1.0);//
}