#include "opengl-framework/opengl-framework.hpp" // Inclue la librairie qui va nous servir à faire du rendu
#include "glm/ext/matrix_clip_space.hpp" // Inclue la librairie qui va nous servir à créer des matrices de projection
#include "glm/ext/matrix_transform.hpp" // Inclue la librairie qui va nous servir à créer des matrices de transformation

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
                -0.5f, -0.5f, +0.5f, 0.0f, 0.0f, // Bottom-left-front
                +0.5f, -0.5f, +0.5f, 1.0f, 0.0f, // Bottom-right-front
                +0.5f, +0.5f, +0.5f, 1.0f, 1.0f, // Top-right-front
                -0.5f, +0.5f, +0.5f, 0.0f, 1.0f, // Top-left-front

                // Back face
                -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // Bottom-left-back
                +0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // Bottom-right-back
                +0.5f, +0.5f, -0.5f, 0.0f, 1.0f, // Top-right-back
                -0.5f, +0.5f, -0.5f, 1.0f, 1.0f, // Top-left-back

                // Left face
                -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // Bottom-left-back
                -0.5f, +0.5f, -0.5f, 1.0f, 0.0f, // Top-left-back
                -0.5f, +0.5f, +0.5f, 1.0f, 1.0f, // Top-left-front
                -0.5f, -0.5f, +0.5f, 0.0f, 1.0f, // Bottom-left-front

                // Right face
                +0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // Bottom-right-back
                +0.5f, +0.5f, -0.5f, 0.0f, 0.0f, // Top-right-back
                +0.5f, +0.5f, +0.5f, 0.0f, 1.0f, // Top-right-front
                +0.5f, -0.5f, +0.5f, 1.0f, 1.0f, // Bottom-right-front

                // Top face
                -0.5f, +0.5f, -0.5f, 0.0f, 0.0f, // Top-left-back
                +0.5f, +0.5f, -0.5f, 1.0f, 0.0f, // Top-right-back
                +0.5f, +0.5f, +0.5f, 1.0f, 1.0f, // Top-right-front
                -0.5f, +0.5f, +0.5f, 0.0f, 1.0f, // Top-left-front

                // Bottom face
                -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, // Bottom-left-back
                +0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // Bottom-right-back
                +0.5f, -0.5f, +0.5f, 0.0f, 0.0f, // Bottom-right-front
                -0.5f, -0.5f, +0.5f, 1.0f, 0.0f, // Bottom-left-front
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
            .path           = "res/texture.png",
            .flip_y         = true, // Il n'y a pas de convention universelle sur la direction de l'axe Y. Les fichiers (.png, .jpeg) utilisent souvent une direction différente de celle attendue par OpenGL. Ce booléen flip_y est là pour inverser la texture si jamais elle n'apparaît pas dans le bon sens.
            .texture_format = gl::InternalFormat::RGBA8, // Format dans lequel la texture sera stockée. On pourrait par exemple utiliser RGBA16 si on voulait 16 bits par canal de couleur au lieu de 8. (Mais ça ne sert à rien dans notre cas car notre fichier ne contient que 8 bits par canal, donc on ne gagnerait pas de précision). On pourrait aussi stocker en RGB8 si on ne voulait pas de canal alpha. On utilise aussi parfois des textures avec un seul canal (R8) pour des usages spécifiques.
        },
        gl::TextureOptions{
            .minification_filter  = gl::Filter::Linear, // Comment on va moyenner les pixels quand on voit l'image de loin ?
            .magnification_filter = gl::Filter::Linear, // Comment on va interpoler entre les pixels quand on zoom dans l'image ?
            .wrap_x               = gl::Wrap::Repeat,   // Quelle couleur va-t-on lire si jamais on essaye de lire en dehors de la texture ?
            .wrap_y               = gl::Wrap::Repeat,   // Idem, mais sur l'axe Y. En général on met le même wrap mode sur les deux axes.
        }
    };

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
        
        shader.set_uniform("view_projection_matrix", model_view_projection_matrix);
        cube_mesh.draw(); // C'est ce qu'on appelle un "draw call" : on envoie l'instruction à la carte graphique de dessiner notre mesh.
    }
}