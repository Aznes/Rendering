#version 410

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;


uniform float aspect_ratio;
uniform float offset;
uniform mat4 view_projection_matrix; // On ajoute une uniforme pour la matrice de vue-projection

out vec3 vertex_position;
out vec2 uv;
out vec3 normal;

void main()
{
    vec3 position = in_position;

    position.x += offset;

    gl_Position = view_projection_matrix * vec4(in_position, 1.);
    vertex_position = in_position;
    uv = in_uv;
    normal = in_normal;
}