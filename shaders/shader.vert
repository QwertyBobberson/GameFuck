#version 450
#extension GL_EXT_debug_printf : enable

layout(set = 0, binding = 0) uniform CamTransform
{
    mat4 view;
    mat4 projection;
} camera;

layout(set = 1, binding = 0) uniform ObjectTransform
{
    mat4 position;
	mat4 rotation;
    mat4 scale;
} object;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 col;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;
layout(location = 0) out vec3 fragColor;
// layout(location = 1) out vec2 fragTexCoord;

void main()
{
    mat4 model = object.position * object.rotation * object.scale;
    gl_Position = camera.view * model * vec4(pos, 1.0);
    vec3 viewspaceNormal = vec3(normalize(camera.view * model * vec4(normal, 1.0)));
    vec3 viewspacePosition = vec3(normalize(gl_Position));
    fragColor = col * dot(viewspaceNormal, viewspacePosition);
    gl_Position = camera.projection * gl_Position;
    //fragTexCoord = inTexCoord;
}