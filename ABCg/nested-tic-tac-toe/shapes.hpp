#ifndef SHAPES_HPP_
#define SHAPES_HPP_

#include "abcg.hpp"

class OpenGLWindow;

class Shapes {
  public:
    void initializeGL(GLuint program);
    void paintGL();
    void terminateGL();
    std::array<int, 6> winnerLine = {0, 0, 0, 0, 0, 0};
    
  private:
    friend OpenGLWindow;

    GLuint m_program{};
    GLint m_colorLoc{};
    GLint m_scaleLoc{};

    GLuint m_vao{};
    GLuint m_vbo{};
    GLuint m_ebo{};

    glm::vec4 m_color{1};
    float m_scale{0.125f};
};

#endif