#version 410

out vec4 out_color;
in vec3 vertex_position;
uniform sampler2D my_texture;
in vec2 uv;
in vec3 normal;
//const vec3 light_direction = normalize(vec3(0.2, 0.3, -1.));
uniform vec3 light_direction = normalize(vec3(0.2, 0.3, -1.));
uniform vec3 light_position = vec3(2., 0., 0.);
uniform float light_intensity = 5.;


void main()
{
    vec4 texture_color = texture(my_texture, uv);
    float squared_light_distance = distance(vertex_position, light_position) * distance(vertex_position, light_position);
    vec3 light_direction = normalize(vertex_position-light_position);
    vec4 lighted_color = texture_color * (light_intensity * clamp(-dot(normalize(normal), light_direction) / squared_light_distance,0 ,1) + 0.3);
    out_color = lighted_color;
}