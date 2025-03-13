#version 410

layout(location = 0) in vec3 in_position_os;
layout(location = 1) in vec2 in_uv_os;
layout(location = 2) in vec3 in_normal_os;

uniform float aspect_ratio;
uniform float offset;
uniform mat4 view_projection_matrix_ws; // On ajoute une uniforme pour la matrice de vue-projection
uniform mat4 model_matrix_ws;
uniform mat4 normal_matrix_ws;


out vec3 vertex_position;
out vec2 uv;
out vec3 normal;

vec3 apply_matrix_to_position(mat4 matrix, vec3 point)
{
    vec4 tmp = matrix * vec4(point, 1.);
    return tmp.xyz / tmp.w;
}

vec3 apply_matrix_to_direction(mat4 matrix, vec3 direction)
{
    vec4 tmp = matrix * vec4(direction, 0.);
    return normalize(tmp.xyz);
}


void main()
{
    vec3 position = in_position_os;

    position.x += offset;

    gl_Position = view_projection_matrix_ws * vec4(in_position_os, 1.);
    vertex_position = apply_matrix_to_position(model_matrix_ws, in_position_os);
    uv = in_uv_os;
    normal = apply_matrix_to_direction(normal_matrix_ws, in_normal_os);
}