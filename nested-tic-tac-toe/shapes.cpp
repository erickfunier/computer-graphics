#include "shapes.hpp"

void Shapes::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");

  // clang-format off
  std::array<glm::vec2, 96> positions{
      // Vertical left
      glm::vec2{-44.25f, +98.0f}, glm::vec2{-44.25f, -125.0f},
      glm::vec2{-39.5f, -125.0f}, glm::vec2{-39.5f, +98.0f},

      // Vertical right
      glm::vec2{+39.25f, -125.0f}, glm::vec2{+39.25f, +98.0f},
      glm::vec2{+44.25f, +98.0f}, glm::vec2{+44.25f, -125.0f},

      // Horizontal Top
      glm::vec2{-150.0f, 27.0f}, glm::vec2{-150.0f, 22.0f},
      glm::vec2{+150.0f, 22.0f}, glm::vec2{+150.0f, 27.0f},

      // Horizontal Bottom
      glm::vec2{-150.0f, -48.0f}, glm::vec2{-150.0f, -53.0f},
      glm::vec2{+150.0f, -53.0f}, glm::vec2{+150.0f, -48.0f},

      // Horizontal Top Win
      glm::vec2{-100.0f, 65.0f}, glm::vec2{-100.0f, 60.0f},
      glm::vec2{+100.0f, 60.0f}, glm::vec2{+100.0f, 65.0f},

      // Horizontal Mid Win
      glm::vec2{-100.0f, -10.0f}, glm::vec2{-100.0f, -15.0f},
      glm::vec2{+100.0f, -15.0f}, glm::vec2{+100.0f, -10.0f},

      // Horizontal Bottom Win
      glm::vec2{-100.0f, -85.0f}, glm::vec2{-100.0f, -90.0f},
      glm::vec2{+100.0f, -90.0f}, glm::vec2{+100.0f, -85.0f},

      // Vertical Right Win
      glm::vec2{81.1f, -110.0f}, glm::vec2{81.1f, 85.0f},
      glm::vec2{86.1f, 85.0f}, glm::vec2{86.1f, -110.0f},

      // Vertical Mid Win
      glm::vec2{-2.75f, -110.0f}, glm::vec2{-2.75f, 85.0f},
      glm::vec2{2.25f, 85.0f}, glm::vec2{2.25f, -110.0f},

      // Vertical Left Win
      glm::vec2{-81.25f, -110.0f}, glm::vec2{-81.25f, 85.0f},
      glm::vec2{-86.25f, 85.0f}, glm::vec2{-86.25f, -110.0f},

      // Diag Bottom to Top Win
      glm::vec2{-104.5f, -110.5f}, glm::vec2{110.0f, 82.0f},
      glm::vec2{106.5f, 85.5f}, glm::vec2{-108.0f, -107.0f},

      // Diag Top to Bottom Win
      glm::vec2{-104.5f, 85.5f}, glm::vec2{110.0f, -107.0f},
      glm::vec2{106.5f, -110.5f}, glm::vec2{-108.0f, 82.0f}
      };

  // Normalize
  for (auto &position :positions) {
    position /= glm::vec2{15.5f, 15.5f};
  }

  std::array indices2{
          // Cannons
          0, 1, 2,
          0, 2, 3,
          4, 5, 6,
          4, 6, 7,
          8, 9, 10,
          8, 10, 11,
          12, 13, 14,
          12, 14, 15,
          winnerLine[0], winnerLine[1], winnerLine[2],
          winnerLine[3], winnerLine[4], winnerLine[5]
        };


  // Generate VBO
  glGenBuffers(1, &m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  glGenBuffers(1, &m_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), indices2.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  glBindVertexArray(m_vao);

  glEnableVertexAttribArray(positionAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

  // End of binding to current VAO
  glBindVertexArray(0);
}

void Shapes::paintGL() {
  glUseProgram(m_program);
  glBindVertexArray(m_vao);

  glUniform1f(m_scaleLoc, m_scale);
  glUniform4fv(m_colorLoc, 1, &m_color.r);
  glDrawElements(GL_TRIANGLES, 10 * 3, GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);
  glUseProgram(0);
}

void Shapes::terminateGL() {
  glDeleteBuffers(1, &m_vbo);
  glDeleteBuffers(1, &m_ebo);
  glDeleteVertexArrays(1, &m_vao);
}