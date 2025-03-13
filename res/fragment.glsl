#version 410

out vec4 out_color;
in vec3 vertex_position;
uniform sampler2D my_texture;
in vec2 uv;
in vec3 normal;
//const vec3 light_direction = normalize(vec3(0.2, 0.3, -1.));
uniform vec3 light_direction = normalize(vec3(0.2, 0.3, -1.));;

void main()
{
    vec4 texture_color = texture(my_texture, uv);
    out_color = texture_color * dot(normalize(normal), light_direction);
}