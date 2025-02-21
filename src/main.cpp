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
            .layout = {gl::VertexAttribute::Position3D{0}},
            .data   = {
                
                // Front face
                -0.5f, -0.5f, +0.5f,
                +0.5f, -0.5f, +0.5f,
                +0.5f, +0.5f, +0.5f,
                -0.5f, +0.5f, +0.5f,
                // Back face
                -0.5f, -0.5f, -0.5f,
                +0.5f, -0.5f, -0.5f,
                +0.5f, +0.5f, -0.5f,
                -0.5f, +0.5f, -0.5f,
                // Left face
                -0.5f, -0.5f, -0.5f,
                -0.5f, -0.5f, +0.5f,
                -0.5f, +0.5f, +0.5f,
                -0.5f, +0.5f, -0.5f,
                // Right face
                +0.5f, -0.5f, -0.5f,
                +0.5f, -0.5f, +0.5f,
                +0.5f, +0.5f, +0.5f,
                +0.5f, +0.5f, -0.5f,
                // Top face
                -0.5f, +0.5f, +0.5f,
                +0.5f, +0.5f, +0.5f,
                +0.5f, +0.5f, -0.5f,
                -0.5f, +0.5f, -0.5f,
                // Bottom face
                -0.5f, -0.5f, +0.5f,
                +0.5f, -0.5f, +0.5f,
                +0.5f, -0.5f, -0.5f,
                -0.5f, -0.5f, -0.5f,
            }
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

    while (gl::window_is_open())
    {
        glEnable(GL_DEPTH_TEST); // Active le test de profondeur. C'est ce qui permet de ne pas dessiner les faces cachées par d'autres faces.
        glClearColor(0.f, 0.f, 1.f, 1.f); // Choisis la couleur à utiliser. Les paramètres sont R, G, B, A avec des valeurs qui vont de 0 à 1
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Vient remplacer glClear(GL_COLOR_BUFFER_BIT);

        shader.bind(); // On a besoin qu'un shader soit bind (i.e. "actif") avant de draw(). On en reparle dans la section d'après.
        //shader.set_uniform("aspect_ratio", gl::framebuffer_aspect_ratio());
        shader.set_uniform("offset", gl::time_in_seconds());
        cube_mesh.draw(); // C'est ce qu'on appelle un "draw call" : on envoie l'instruction à la carte graphique de dessiner notre mesh.
        
        glm::mat4 const view_matrix = camera.view_matrix();
        glm::mat4 const projection_matrix = glm::infinitePerspective(glm::radians(75.f) /*field of view in radians*/, gl::framebuffer_aspect_ratio() /*aspect ratio*/, 0.1f /*near plane*/);
        
        glm::mat4 const orthogonal_projection_matrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
        
        glm::mat4 const rotation = glm::rotate(glm::mat4{1.f}, gl::time_in_seconds() /*angle de la rotation*/, glm::vec3{0.f, 0.f, 1.f} /* axe autour duquel on tourne */);
        glm::mat4 const translation = glm::translate(glm::mat4{1.f}, glm::vec3{0.f, 1.f, 0.f} /* déplacement */);    
        
        glm::mat4 const model_view_projection_matrix = projection_matrix * view_matrix * translation * rotation;

        shader.set_uniform("view_projection_matrix", model_view_projection_matrix);
    }
}