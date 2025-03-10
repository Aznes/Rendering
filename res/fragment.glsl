#version 410

out vec4 out_color;
in vec3 vertex_position;
uniform sampler2D my_texture;
in vec2 uv;

void main()
{
    //out_color = vec4(vertex_position, 1.);

    vec4 texture_color = texture(my_texture, uv);
    out_color = texture_color;
}