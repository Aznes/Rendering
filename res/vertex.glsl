#version 410

layout(location = 0) in vec3 in_position;

uniform float aspect_ratio;
uniform float offset;
uniform mat4 view_projection_matrix; // On ajoute une uniforme pour la matrice de vue-projection
out vec3 vertex_position;

void main()
{
    vec3 position = in_position;
    // ...
    position.x += offset;
    //position.x /= aspect_ratio;
    // ...
    //gl_Position = vec4(position, 0.4f, 1.0f); // Ici on utilise maintenant `position` et non plus `in_position`
    gl_Position = view_projection_matrix * vec4(in_position, 1.);
    vertex_position = in_position;
}