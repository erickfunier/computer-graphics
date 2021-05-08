#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>
#include <tiny_obj_loader.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/hash.hpp>
#include <unordered_map>

// Custom specialization of std::hash injected in namespace std
namespace std {
template <>
struct hash<Vertex> {
  size_t operator()(Vertex const& vertex) const noexcept {
    std::size_t h1{std::hash<glm::vec3>()(vertex.position)};
    return h1;
  }
};
}  // namespace std

void OpenGLWindow::initializeGL() {
  glClearColor(0, 0, 0, 1);
  y = 0.0f; // variable used to explode and bouncing
  z = 0.0f; // variable used to explode and bouncing
  colors = {
    {0.502f, 0.827f, 0.404f, 1.0f}, // Green        (body)
    {0.578f, 0.516f, 0.418f, 1.0f}, // Brown        (coat)
    {0.671f, 0.659f, 0.522f, 1.0f}, // Light Brown  (scarf)
    {0.0f, 0.0f, 0.0f, 1.0f},       // Black        (eye)
    {1.0f, 1.0f, 1.0f, 1.0f},       // White
    {1.0f, 0.2f, 0.2f, 1.0f},       // Red
    {0.2f, 0.2f, 1.0f, 1.0f},       // Blue
    {0.2f, 0.7f, 1.0f, 1.0f},       // Light Blue
    {0.4f, 0.4f, 0.4f, 1.0f}        // Gray
  };
  bodyColorIndex = 0;
  bodyColor = colors[bodyColorIndex];
  coatColorIndex = 1;
  coatColor = colors[coatColorIndex];
  scarfColorIndex = 2;
  scarfColor = colors[scarfColorIndex];
  eyeColorIndex = 3;
  eyeColor = colors[eyeColorIndex];
  returningBlink = false;
  returningBody = false;
  blinkingEnable = false;
  explodedVision = false;
  // Enable depth buffering
  glEnable(GL_DEPTH_TEST);

  // Create program
  m_program = createProgramFromFile(getAssetsPath() + "baby_yoda.vert",
                                    getAssetsPath() + "baby_yoda.frag");

  // Load model
  loadModelFromFile(getAssetsPath() + "baby_yoda.obj");
  standardizeBody();

  //loadModelFromFile(getAssetsPath() + "eyes.obj");  
  //standardizeBody();

  m_vertices_ToDraw = m_indices.size();

  // Generate VBO
  glGenBuffers(1, &m_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices[0]) * m_vertices.size(),
               m_vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  glGenBuffers(1, &m_EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices[0]) * m_indices.size(),
               m_indices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Create VAO
  glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  glBindVertexArray(m_VAO);

  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};
  
  glEnableVertexAttribArray(positionAttribute);
  glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

  // End of binding to current VAO
  glBindVertexArray(0);
}

void OpenGLWindow::loadModelFromFile(std::string_view path) {
  tinyobj::ObjReaderConfig readerConfig;
  readerConfig.mtl_search_path =
      getAssetsPath() + "mtl/";  // Path to material files

  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data(), readerConfig)) {
    if (!reader.Error().empty()) {
      throw abcg::Exception{abcg::Exception::Runtime(
          fmt::format("Failed to load model {} ({})", path, reader.Error()))};
    }
    throw abcg::Exception{
        abcg::Exception::Runtime(fmt::format("Failed to load model {}", path))};
  }

  if (!reader.Warning().empty()) {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  const auto& attrib{reader.GetAttrib()};
  const auto& shapes{reader.GetShapes()};

  m_vertices.clear();
  m_indices.clear();

  // A key:value map with key=Vertex and value=index
  std::unordered_map<Vertex, GLuint> hash{};

  // Loop over shapes
  for (const auto& shape : shapes) {
    // Loop over faces(polygon)
    size_t indexOffset{0};
    for (const auto faceNumber :
         iter::range(shape.mesh.num_face_vertices.size())) {
      // Number of vertices composing face f
      std::size_t numFaceVertices{shape.mesh.num_face_vertices[faceNumber]};
      // Loop over vertices in the face
      std::size_t startIndex{};
      for (const auto vertexNumber : iter::range(numFaceVertices)) {
        // Access to vertex
        tinyobj::index_t index{shape.mesh.indices[indexOffset + vertexNumber]};

        // Vertex coordinates
        startIndex = 3 * index.vertex_index;
        tinyobj::real_t vx = attrib.vertices[startIndex + 0];
        tinyobj::real_t vy = attrib.vertices[startIndex + 1];
        tinyobj::real_t vz = attrib.vertices[startIndex + 2];

        Vertex vertex{};
        vertex.position = {vx, vy, vz};

        // If hash doesn't contain this vertex
        if (hash.count(vertex) == 0) {
          // Add this index (size of m_vertices)
          hash[vertex] = m_vertices.size();
          // Add this vertex
          m_vertices.push_back(vertex);
        }

        m_indices.push_back(hash[vertex]);
      }
      indexOffset += numFaceVertices;
    }
  }
}

void OpenGLWindow::standardizeBody() {
  // Center to origin and normalize largest bound to [-1, 1]

  // Get bounds
  glm::vec3 max(std::numeric_limits<float>::lowest());
  glm::vec3 min(std::numeric_limits<float>::max());
  for (const auto& vertex : m_vertices) {
    max.x = std::max(max.x, vertex.position.x);
    max.y = std::max(max.y, vertex.position.y);
    max.z = std::max(max.z, vertex.position.z);
    min.x = std::min(min.x, vertex.position.x);
    min.y = std::min(min.y, vertex.position.y);
    min.z = std::min(min.z, vertex.position.z);
  }

  // Center and scale
  const auto center{(min + max) / 2.0f};
  const auto scaling{2.0f / glm::length(max - min)};
  for (auto& vertex : m_vertices) {
    vertex.position = (vertex.position - center) * scaling;
  }
}

void OpenGLWindow::paintGL() {
  // Animate angle by 15 degrees per second
  float deltaTime{static_cast<float>(getDeltaTime())};
  m_angle = glm::wrapAngle(m_angle + glm::radians(15.0f) * deltaTime);

  // Clear color buffer and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  GLint posX{glGetUniformLocation(m_program, "posX")};
  GLint posY{glGetUniformLocation(m_program, "posY")};
  GLint posZ{glGetUniformLocation(m_program, "posZ")};
  GLint colorLoc{glGetUniformLocation(m_program, "color")};
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  glUseProgram(m_program);
  glBindVertexArray(m_VAO);
  

  // used for bouncing baby yoda
  if (bouncingEnable) {
    if (returningBody) {
      if (y < 0) {
        returningBody = false;
      } else {
        //y -= 0.0001f;
        y -= 0.001f; // use this for web
      }      
    } else {
      if (y > 0.5f) {
       returningBody = true;
      } else {
        //y += 0.0001f;
        y += 0.001f; // use this for web
      }
    }
  } else {
    y = 0.0f;
  } 
  
  glUniform1f(posX, x); 
  glUniform1f(posY, y); 
  glUniform1f(posZ, z); 


  // used for blink eye when check box is checked
  if (blinkingEnable) {
    if (returningBlink) {    
      if (eyeColor[0] < 0.0f) {
        returningBlink = false;        
      } else {
        //eyeColor[0]-=0.001f;
        //eyeColor[1]-=0.001f;
        //eyeColor[2]-=0.001f;
        eyeColor[0]-=0.05f; // use this for web
        eyeColor[1]-=0.05f; // use this for web
        eyeColor[2]-=0.05f; // use this for web
      }    
    } else {    
      if (eyeColor[0] > 1.0f) { 
        returningBlink = true;
      } else {
        //eyeColor[0]+=0.001f;
        //eyeColor[1]+=0.001f;
        //eyeColor[2]+=0.001f;
        eyeColor[0]+=0.05f; // use this for web
        eyeColor[1]+=0.05f; // use this for web
        eyeColor[2]+=0.05f; // use this for web
      }
    }
  }
  
  
  // Update uniform variable
  GLint angleLoc{glGetUniformLocation(m_program, "angle")};
  glUniform1f(angleLoc, m_angle);

  // Draw COAT
  if (explodedVision) {   // for exploded view set new coat position to vert
    glUniform1f(posY, y-0.3f);
  }     
  glUniform4f(colorLoc, coatColor[0], coatColor[1], coatColor[2], coatColor[3]);  // set current selected coat color to frag
  m_vertices_ToDraw = 11460; // set limit of coat vertices
  glDrawElements(GL_TRIANGLES, m_vertices_ToDraw, GL_UNSIGNED_INT, nullptr); // draw coat vertices starting vertices vector in 0

  // SCARF
  if (explodedVision) {   // for exploded view set new scarf position to vert
    glUniform1f(posY, y-0.1f);
  }  
  glUniform4f(colorLoc, scarfColor[0], scarfColor[1], scarfColor[2], scarfColor[3]);  // set current selected scarf color to frag
  m_vertices_ToDraw = 2100; // set limit of scarf vertices
  glDrawElements(GL_TRIANGLES, m_vertices_ToDraw, GL_UNSIGNED_INT, (void*)(11460 * sizeof(GLuint))); // draw scarf vertices starting vertices vector in 11460

  // BODY
  if (explodedVision) {   // for exploded view set new body position to vert
    glUniform1f(posY, y+0.2f);
  }  
  glUniform4f(colorLoc, bodyColor[0], bodyColor[1], bodyColor[2], bodyColor[3]); // set current selected body color to frag
  m_vertices_ToDraw = 20406; // set limit of body vertices (head and hands)
  glDrawElements(GL_TRIANGLES, m_vertices_ToDraw, GL_UNSIGNED_INT, (void*)(13560 * sizeof(GLuint))); // draw body vertices starting vertices vector in 13560

  // EYE
  if (explodedVision) {   // for exploded view set new eye position to vert
    glUniform1f(posZ, 0.4f);
  }
  glUniform4f(colorLoc, eyeColor[0], eyeColor[1], eyeColor[2], eyeColor[3]); // set current selected eye color to frag
  m_vertices_ToDraw = 4560; // set limit of eye vertices
  glDrawElements(GL_TRIANGLES, m_vertices_ToDraw, GL_UNSIGNED_INT, (void*)(33966 * sizeof(GLuint))); // draw eye vertices starting vertices vector in 33966

  glBindVertexArray(0);
  glUseProgram(0);
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();
  // Create a window for the other widgets
  {
    auto widgetSize{ImVec2(172, 190)};
    ImGui::SetNextWindowPos(ImVec2(m_viewportWidth - widgetSize.x - 5, 5));
    ImGui::SetNextWindowSize(widgetSize);
    ImGui::Begin("Widget window", nullptr, ImGuiWindowFlags_NoDecoration);
    {
      ImGui::Checkbox("Bouncing Baby", &bouncingEnable);

      std::vector<std::string> comboItems{"Green", "Brown", "Light Brown", "Black", 
                                          "White", "Red", "Blue", "Light Blue",
                                          "Gray"};

      ImGui::PushItemWidth(70);
      if (ImGui::BeginCombo("Body", comboItems.at(bodyColorIndex).c_str())) {
        for (int index : iter::range(comboItems.size())) {
          const bool isSelected{bodyColorIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected)) {
            bodyColorIndex = index;
            bodyColor = colors[bodyColorIndex];
          }            
          if (isSelected) {
            ImGui::SetItemDefaultFocus();
          } 
        }
        ImGui::EndCombo();
      }
      ImGui::PushItemWidth(70);
      if (ImGui::BeginCombo("Coat", comboItems.at(coatColorIndex).c_str())) {
        for (int index : iter::range(comboItems.size())) {
          const bool isSelected{coatColorIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected)) {
            coatColorIndex = index;
            coatColor = colors[coatColorIndex];
          }            
          if (isSelected) {
            ImGui::SetItemDefaultFocus();
          } 
        }
        ImGui::EndCombo();
      }
      ImGui::PushItemWidth(70);
      if (ImGui::BeginCombo("Scarf", comboItems.at(scarfColorIndex).c_str())) {
        for (int index : iter::range(comboItems.size())) {
          const bool isSelected{scarfColorIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected)) {
            scarfColorIndex = index;
            scarfColor = colors[scarfColorIndex];
          }            
          if (isSelected) {
            ImGui::SetItemDefaultFocus();
          } 
        }
        ImGui::EndCombo();
      }
      
      ImGui::Checkbox("Blinking Eye", &blinkingEnable);
      ImGui::PushItemWidth(70);
      if (ImGui::BeginCombo("Eye", comboItems.at(eyeColorIndex).c_str())) {
        for (int index : iter::range(comboItems.size())) {
          const bool isSelected{eyeColorIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected)) {
            eyeColorIndex = index;
            eyeColor = colors[index];
          }            
          if (isSelected) {
            ImGui::SetItemDefaultFocus();
          } 
        }
        ImGui::EndCombo();
      }

      ImGui::Checkbox("Exploded Vision", &explodedVision);
      ImGui::PopItemWidth();
      ImGui::PopItemWidth();
      ImGui::PopItemWidth();
      ImGui::PopItemWidth();
    }
    ImGui::End();
  }
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;
}

void OpenGLWindow::terminateGL() {
  glDeleteProgram(m_program);
  glDeleteBuffers(1, &m_EBO);
  glDeleteBuffers(1, &m_VBO);
  glDeleteVertexArrays(1, &m_VAO);
}