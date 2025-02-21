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

    auto const square_mesh = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {gl::VertexAttribute::Position2D{0}},
            .data   = {
                -0.5f, -0.5f, // Position2D du 1er sommet
                +0.5f, -0.5f, // Position2D du 2ème sommet
                +0.5f, +0.5f, // Position2D du 3ème sommet
                -0.5f, +0.5f  // Position2D du 4ème sommet
            },
        }},
        .index_buffer   = {
            0, 1, 2, // Indices du premier triangle : on utilise le 1er, 2ème et 3ème sommet
            0, 2, 3  // Indices du deuxième triangle : on utilise le 1er, 3ème et 4ème sommet
        },
    }};

    while (gl::window_is_open())
    {
        glClearColor(0.f, 0.f, 1.f, 1.f); // Choisis la couleur à utiliser. Les paramètres sont R, G, B, A avec des valeurs qui vont de 0 à 1
        glClear(GL_COLOR_BUFFER_BIT); // Exécute concrètement l'action d'appliquer sur tout l'écran la couleur choisie au-dessus

        shader.bind(); // On a besoin qu'un shader soit bind (i.e. "actif") avant de draw(). On en reparle dans la section d'après.
        //shader.set_uniform("aspect_ratio", gl::framebuffer_aspect_ratio());
        shader.set_uniform("offset", gl::time_in_seconds());
        square_mesh.draw(); // C'est ce qu'on appelle un "draw call" : on envoie l'instruction à la carte graphique de dessiner notre mesh.
        
        glm::mat4 const view_matrix = camera.view_matrix();
        glm::mat4 const projection_matrix = glm::infinitePerspective(glm::radians(75.f) /*field of view in radians*/, gl::framebuffer_aspect_ratio() /*aspect ratio*/, 0.1f /*near plane*/);
        glm::mat4 const orthogonal_projection_matrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
        glm::mat4 const rotation = glm::rotate(glm::mat4{1.f}, gl::time_in_seconds() /*angle de la rotation*/, glm::vec3{0.f, 0.f, 1.f} /* axe autour duquel on tourne */);
        glm::mat4 const translation = glm::translate(glm::mat4{1.f}, glm::vec3{0.f, 1.f, 0.f} /* déplacement */);    
        glm::mat4 const model_view_projection_matrix = projection_matrix * view_matrix * translation * rotation;

        shader.set_uniform("view_projection_matrix", model_view_projection_matrix);
    }
}