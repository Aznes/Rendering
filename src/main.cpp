#include "opengl-framework/opengl-framework.hpp" // Inclue la librairie qui va nous servir à faire du rendu
#include "glm/ext/matrix_clip_space.hpp" // Inclue la librairie qui va nous servir à créer des matrices de projection
#include "glm/ext/matrix_transform.hpp" // Inclue la librairie qui va nous servir à créer des matrices de transformation
#include "iostream"

auto load_mesh(std::filesystem::path const& path) -> gl::Mesh
{
    // On lit le fichier avec tinyobj
    auto reader = tinyobj::ObjReader{};
    reader.ParseFromFile(gl::make_absolute_path(path).string(), {});

    if (!reader.Error().empty())
        throw std::runtime_error("Failed to read 3D model:\n" + reader.Error());
    if (!reader.Warning().empty())
        std::cout << "Warning while reading 3D model:\n" + reader.Warning();

    // On met tous les attributs dans un tableau
    auto vertices = std::vector<float>{};
    for (auto const& shape : reader.GetShapes())
    {
        for (auto const& idx : shape.mesh.indices)
        {
            // Position
            vertices.push_back(reader.GetAttrib().vertices[3 * idx.vertex_index + 0]);
            vertices.push_back(reader.GetAttrib().vertices[3 * idx.vertex_index + 2]);
            vertices.push_back(reader.GetAttrib().vertices[3 * idx.vertex_index + 1]);

            // UV
            vertices.push_back(reader.GetAttrib().texcoords[2 * idx.texcoord_index + 0]);
            vertices.push_back(reader.GetAttrib().texcoords[2 * idx.texcoord_index + 1]);

            // Normale
            vertices.push_back(reader.GetAttrib().normals[3 * idx.normal_index + 0]);
            vertices.push_back(reader.GetAttrib().normals[3 * idx.normal_index + 2]);
            vertices.push_back(reader.GetAttrib().normals[3 * idx.normal_index + 1]);
        };
    }

    // TODO créer et return un gl::mesh, qui utilisera le tableau `vertices` en tant que `data` pour son vertex buffer.
    // Attention, il faudra bien spécifier le layout pour qu'il corresponde à l'ordre des attributs dans le tableau `vertices`.

    return gl::Mesh{{
        .vertex_buffers = {{
            .layout = {gl::VertexAttribute::Position3D{0}, gl::VertexAttribute::UV{1}, gl::VertexAttribute::Normal3D{2}},
            .data = vertices,
        }},
    }};
}

int main()
{
    // Initialisation
    gl::init("TPs de Rendering"); // On crée une fenêtre et on choisit son nom
    gl::maximize_window(); // On peut la maximiser si on veut
    auto camera = gl::Camera{};
    gl::set_events_callbacks({camera.events_callbacks()});

    auto const shader = gl::Shader{{
        .vertex   = gl::ShaderSource::File{"res/vertex.glsl"},
        .fragment = gl::ShaderSource::File{"res/fragment.glsl"},
    }};

    auto const cube_mesh = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {gl::VertexAttribute::Position3D{0}, gl::VertexAttribute::UV{1}},
            .data = {
                // Position3D          |   UV Coordinates
                // Front face
                -0.5f, -0.5f, +0.5f, -1.0f, -1.0f, // Bottom-left-front
                +0.5f, -0.5f, +0.5f, 2.0f, -1.0f, // Bottom-right-front
                +0.5f, +0.5f, +0.5f, 2.0f, 2.0f, // Top-right-front
                -0.5f, +0.5f, +0.5f, -1.0f, 2.0f, // Top-left-front

                // Back face
                -0.5f, -0.5f, -0.5f, 2.0f, -1.0f, // Bottom-left-back
                +0.5f, -0.5f, -0.5f, -1.0f, -1.0f, // Bottom-right-back
                +0.5f, +0.5f, -0.5f, -1.0f, 2.0f, // Top-right-back
                -0.5f, +0.5f, -0.5f, 2.0f, 2.0f, // Top-left-back

                // Left face
                -0.5f, -0.5f, -0.5f, -1.0f, -1.0f, // Bottom-left-back
                -0.5f, +0.5f, -0.5f, 2.0f, -1.0f, // Top-left-back
                -0.5f, +0.5f, +0.5f, 2.0f, 2.0f, // Top-left-front
                -0.5f, -0.5f, +0.5f, -1.0f, 2.0f, // Bottom-left-front

                // Right face
                +0.5f, -0.5f, -0.5f, 2.0f, -1.0f, // Bottom-right-back
                +0.5f, +0.5f, -0.5f, -1.0f, -1.0f, // Top-right-back
                +0.5f, +0.5f, +0.5f, -1.0f, 2.0f, // Top-right-front
                +0.5f, -0.5f, +0.5f, 2.0f, 2.0f, // Bottom-right-front

                // Top face
                -0.5f, +0.5f, -0.5f, -1.0f, -1.0f, // Top-left-back
                +0.5f, +0.5f, -0.5f, 2.0f, -1.0f, // Top-right-back
                +0.5f, +0.5f, +0.5f, 2.0f, 2.0f, // Top-right-front
                -0.5f, +0.5f, +0.5f, -1.0f, 2.0f, // Top-left-front

                // Bottom face
                -0.5f, -0.5f, -0.5f, 2.0f, 2.0f, // Bottom-left-back
                +0.5f, -0.5f, -0.5f, -1.0f, 2.0f, // Bottom-right-back
                +0.5f, -0.5f, +0.5f, -1.0f, -1.0f, // Bottom-right-front
                -0.5f, -0.5f, +0.5f, 2.0f, -1.0f, // Bottom-left-front
            },
        }},
        .index_buffer = {
            // Front face
            0, 1, 2, 2, 3, 0,
            // Back face
            4, 5, 6, 6, 7, 4,
            // Left face
            8, 9, 10, 10, 11, 8,
            // Right face
            12, 13, 14, 14, 15, 12,
            // Top face
            16, 17, 18, 18, 19, 16,
            // Bottom face
            20, 21, 22, 22, 23, 20,
        }
    }};

    auto const texture = gl::Texture{
        gl::TextureSource::File{ // Peut être un fichier, ou directement un tableau de pixels
            .path           = "res/bato/fourareen2K_albedo.jpg",
            .flip_y         = true, // Il n'y a pas de convention universelle sur la direction de l'axe Y. Les fichiers (.png, .jpeg) utilisent souvent une direction différente de celle attendue par OpenGL. Ce booléen flip_y est là pour inverser la texture si jamais elle n'apparaît pas dans le bon sens.
            .texture_format = gl::InternalFormat::RGBA8, // Format dans lequel la texture sera stockée. On pourrait par exemple utiliser RGBA16 si on voulait 16 bits par canal de couleur au lieu de 8. (Mais ça ne sert à rien dans notre cas car notre fichier ne contient que 8 bits par canal, donc on ne gagnerait pas de précision). On pourrait aussi stocker en RGB8 si on ne voulait pas de canal alpha. On utilise aussi parfois des textures avec un seul canal (R8) pour des usages spécifiques.
        },
        gl::TextureOptions{
            .minification_filter  = gl::Filter::Linear, // Comment on va moyenner les pixels quand on voit l'image de loin ?
            .magnification_filter = gl::Filter::Linear, // Comment on va interpoler entre les pixels quand on zoom dans l'image ?
            .wrap_x               = gl::Wrap::MirroredRepeat,   // Quelle couleur va-t-on lire si jamais on essaye de lire en dehors de la texture ?
            .wrap_y               = gl::Wrap::MirroredRepeat,   // Idem, mais sur l'axe Y. En général on met le même wrap mode sur les deux axes.
        }
    };

    auto render_target = gl::RenderTarget{gl::RenderTarget_Descriptor{
        .width          = gl::framebuffer_width_in_pixels(),
        .height         = gl::framebuffer_height_in_pixels(),
        .color_textures = {
            gl::ColorAttachment_Descriptor{
                .format  = gl::InternalFormat_Color::RGBA8,
                .options = {
                    .minification_filter  = gl::Filter::NearestNeighbour, // On va toujours afficher la texture à la taille de l'écran,
                    .magnification_filter = gl::Filter::NearestNeighbour, // donc les filtres n'auront pas d'effet. Tant qu'à faire on choisit le moins coûteux.
                    .wrap_x               = gl::Wrap::ClampToEdge,
                    .wrap_y               = gl::Wrap::ClampToEdge,
                },
            },
        },
        .depth_stencil_texture = gl::DepthStencilAttachment_Descriptor{
            .format  = gl::InternalFormat_DepthStencil::Depth32F,
            .options = {
                .minification_filter  = gl::Filter::NearestNeighbour,
                .magnification_filter = gl::Filter::NearestNeighbour,
                .wrap_x               = gl::Wrap::ClampToEdge,
                .wrap_y               = gl::Wrap::ClampToEdge,
            },
        },
    }};
    
    gl::set_events_callbacks({
        camera.events_callbacks(),
        {.on_framebuffer_resized = [&](gl::FramebufferResizedEvent const& e) {
            if(e.width_in_pixels != 0 && e.height_in_pixels != 0) // OpenGL crash si on tente de faire une render target avec une taille de 0
                render_target.resize(e.width_in_pixels, e.height_in_pixels);
        }},
    });
    
    // Load le bato
    auto boatMesh = load_mesh("res/bato/fourareen.obj");

    while (gl::window_is_open())
    {
        glEnable(GL_DEPTH_TEST); // Active le test de profondeur. C'est ce qui permet de ne pas dessiner les faces cachées par d'autres faces.
        glClearColor(0.f, 0.f, 1.f, 1.f); // Choisis la couleur à utiliser. Les paramètres sont R, G, B, A avec des valeurs qui vont de 0 à 1
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Vient remplacer glClear(GL_COLOR_BUFFER_BIT);

        shader.bind(); // On a besoin qu'un shader soit bind (i.e. "actif") avant de draw(). On en reparle dans la section d'après.
        //shader.set_uniform("aspect_ratio", gl::framebuffer_aspect_ratio());
        shader.set_uniform("offset", gl::time_in_seconds());
        shader.set_uniform("my_texture", texture);
        
        glm::mat4 const view_matrix = camera.view_matrix();
        glm::mat4 const projection_matrix = glm::infinitePerspective(glm::radians(75.f) /*field of view in radians*/, gl::framebuffer_aspect_ratio() /*aspect ratio*/, 0.1f /*near plane*/);
        
        glm::mat4 const orthogonal_projection_matrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
        
        glm::mat4 const rotation = glm::rotate(glm::mat4{1.f}, gl::time_in_seconds() /*angle de la rotation*/, glm::vec3{0.f, 0.f, 1.f} /* axe autour duquel on tourne */);
        glm::mat4 const translation = glm::translate(glm::mat4{1.f}, glm::vec3{0.f, 1.f, 0.f} /* déplacement */);    
        
        glm::mat4 const model_view_projection_matrix = projection_matrix * view_matrix * translation * rotation;
        
        shader.set_uniform("view_projection_matrix_ws", model_view_projection_matrix);
        
        shader.set_uniform("model_matrix_ws", translation * rotation);
        shader.set_uniform("normal_matrix_ws", glm::inverse(glm::transpose( translation * rotation)));

        render_target.render([&]() {
            glClearColor(1.f, 0.f, 0.f, 1.f); // Dessine du rouge, non pas à l'écran, mais sur notre render target
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            // ... mettez tout votre code de rendu ici
        });

        boatMesh.draw(); 
    }
}