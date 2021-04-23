#include "openglwindow.hpp"

#include <imgui.h>

#include <cppitertools/itertools.hpp>
#include <glm/ext/vector_int2.hpp>
#include <glm/gtc/matrix_inverse.hpp>

void OpenGLWindow::handleEvent(SDL_Event& event) {
  // Handle keyboard and attribute speed to camera
  if (event.type == SDL_KEYDOWN) {      
    if (!started) {
      started = true;
    } else {
      if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w){
        m_dollySpeed = 1.0f;
        m_cameraLight.mousePress(glm::ivec2(m_dollySpeed, 0.0f));
      }     
      if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) {          
        m_dollySpeed = -1.0f;
        m_cameraLight.mousePress(glm::ivec2(m_dollySpeed, 0.0f));
      }

      if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) {
        m_panSpeed = -1.0f;
        m_cameraLight.mousePress(glm::ivec2(m_panSpeed, 0.0f));
      }

      if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d) {
        m_panSpeed = 1.0f;
        m_cameraLight.mousePress(glm::ivec2(m_panSpeed, 0.0f));
      }

      if (event.key.keysym.sym == SDLK_q) {
        m_truckSpeed = -1.0f;
        m_cameraLight.mousePress(glm::ivec2(m_truckSpeed, 0.0f));
      }
      if (event.key.keysym.sym == SDLK_e) {
        m_truckSpeed = 1.0f;
        m_cameraLight.mousePress(glm::ivec2(m_truckSpeed, 0.0f));
      }
    }
  }
  if (event.type == SDL_KEYUP) {
    if ((event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) && m_dollySpeed > 0)
      m_dollySpeed = 0.0f;
    if ((event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) && m_dollySpeed < 0)
      m_dollySpeed = 0.0f;
    if ((event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) && m_panSpeed < 0)
      m_panSpeed = 0.0f;
    if ((event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d) && m_panSpeed > 0)
      m_panSpeed = 0.0f;
    if (event.key.keysym.sym == SDLK_q && m_truckSpeed < 0) m_truckSpeed = 0.0f;
    if (event.key.keysym.sym == SDLK_e && m_truckSpeed > 0) m_truckSpeed = 0.0f;
  }
}

void OpenGLWindow::initializeGL() {
  glClearColor(0, 0, 0, 1);
  glEnable(GL_DEPTH_TEST);

  // Create program
  auto program{createProgramFromFile(getAssetsPath() + "shaders/normalmapping.vert", getAssetsPath() + "shaders/normalmapping.frag")};
  m_programs.push_back(program);

  // Load default model
  loadModel(getAssetsPath() + "buildUFABC.obj");

  // Load cubemap
  m_model.loadCubeTexture(getAssetsPath() + "maps/cube/");
  m_mappingMode = 3;  // "From mesh" option
  initializeSkybox();

  ImGuiIO &io{ImGui::GetIO()};
  auto filename{getAssetsPath() + "Blackbook.ttf"};
  m_font = io.Fonts->AddFontFromFileTTF(filename.c_str(), 40.0f);
}

void OpenGLWindow::renderSkybox() {
  glUseProgram(m_skyProgram);

  // Get location of uniform variables
  GLint viewMatrixLoc{glGetUniformLocation(m_skyProgram, "viewMatrix")};
  GLint projMatrixLoc{glGetUniformLocation(m_skyProgram, "projMatrix")};
  GLint skyTexLoc{glGetUniformLocation(m_skyProgram, "skyTex")};

  // Set uniform variables
  auto viewMatrix{m_trackBallLight.getRotation()};
  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &viewMatrix[0][0]);
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  glUniform1i(skyTexLoc, 0);

  glBindVertexArray(m_skyVAO);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_model.getCubeTexture());

  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CW);
  glDepthFunc(GL_LEQUAL);
  glDrawArrays(GL_TRIANGLES, 0, m_skyPositions.size());
  glDepthFunc(GL_LESS);

  glBindVertexArray(0);
  glUseProgram(0);
}

void OpenGLWindow::initializeSkybox() {
  // Create skybox program
  auto path{getAssetsPath() + "shaders/skybox"};
  m_skyProgram = createProgramFromFile(path + ".vert", path + ".frag");

  // Generate VBO
  glGenBuffers(1, &m_skyVBO);
  glBindBuffer(GL_ARRAY_BUFFER, m_skyVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(m_skyPositions), m_skyPositions.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{glGetAttribLocation(m_skyProgram, "inPosition")};

  // Create VAO
  glGenVertexArrays(1, &m_skyVAO);

  // Bind vertex attributes to current VAO
  glBindVertexArray(m_skyVAO);

  glBindBuffer(GL_ARRAY_BUFFER, m_skyVBO);
  glEnableVertexAttribArray(positionAttribute);
  glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  glBindVertexArray(0);
}

void OpenGLWindow::loadModel(std::string_view path) {
  m_model.loadDiffuseTexture(getAssetsPath() + "maps/rough_block_wall_diff_1k.jpg");
  m_model.loadNormalTexture(getAssetsPath() + "maps/rough_block_wall_nor_1k.jpg");
  m_model.loadFromFile(path);
  m_model.setupVAO(m_programs.at(0));
  m_trianglesToDraw = m_model.getNumTriangles();

  // Use material properties from the loaded model
  m_Ka = m_model.getKa();
  m_Kd = m_model.getKd();
  m_Ks = m_model.getKs();
  m_shininess = m_model.getShininess();
}

void OpenGLWindow::paintGL() {
  update();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  // Use currently selected program
  const auto program{m_programs.at(0)};
  glUseProgram(program);

  // Get location of uniform variables
  GLint viewMatrixLoc{glGetUniformLocation(program, "viewMatrix")};
  GLint projMatrixLoc{glGetUniformLocation(program, "projMatrix")};
  GLint modelMatrixLoc{glGetUniformLocation(program, "modelMatrix")};
  GLint normalMatrixLoc{glGetUniformLocation(program, "normalMatrix")};
  GLint lightDirLoc{glGetUniformLocation(program, "lightDirWorldSpace")};
  GLint shininessLoc{glGetUniformLocation(program, "shininess")};
  GLint IaLoc{glGetUniformLocation(program, "Ia")};
  GLint IdLoc{glGetUniformLocation(program, "Id")};
  GLint IsLoc{glGetUniformLocation(program, "Is")};
  GLint KaLoc{glGetUniformLocation(program, "Ka")};
  GLint KdLoc{glGetUniformLocation(program, "Kd")};
  GLint KsLoc{glGetUniformLocation(program, "Ks")};
  GLint diffuseTexLoc{glGetUniformLocation(program, "diffuseTex")};
  GLint normalTexLoc{glGetUniformLocation(program, "normalTex")};
  GLint cubeTexLoc{glGetUniformLocation(program, "cubeTex")};
  GLint mappingModeLoc{glGetUniformLocation(program, "mappingMode")};
  GLint texMatrixLoc{glGetUniformLocation(program, "texMatrix")};

  // Set uniform variables used by every scene object
  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_camera.m_viewMatrix[0][0]);
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_camera.m_projMatrix[0][0]);
  glUniform1i(diffuseTexLoc, 0);
  glUniform1i(normalTexLoc, 1);
  glUniform1i(cubeTexLoc, 2);
  glUniform1i(mappingModeLoc, m_mappingMode);

  glm::mat3 texMatrix{m_trackBallLight.getRotation()};
  glUniformMatrix3fv(texMatrixLoc, 1, GL_TRUE, &texMatrix[0][0]);

  auto lightDirRotated{m_cameraLight.getRotation() * m_lightDir};
  glUniform4fv(lightDirLoc, 1, &lightDirRotated.x);
  glUniform4fv(IaLoc, 1, &m_Ia.x);
  glUniform4fv(IdLoc, 1, &m_Id.x);
  glUniform4fv(IsLoc, 1, &m_Is.x);

  // Set uniform variables of the current object
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &m_modelMatrix[0][0]);
  
  auto modelViewMatrix{glm::mat3(m_viewMatrix * m_modelMatrix)};
  glm::mat3 normalMatrix{glm::inverseTranspose(modelViewMatrix)};
  glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  glUniform1f(shininessLoc, m_shininess);
  glUniform4fv(KaLoc, 1, &m_Ka.x);
  glUniform4fv(KdLoc, 1, &m_Kd.x);
  glUniform4fv(KsLoc, 1, &m_Ks.x);

  m_model.render(m_trianglesToDraw);

  renderSkybox();
  glUseProgram(0);
  glFrontFace(GL_CCW);
  auto aspect{static_cast<float>(m_viewportWidth) / static_cast<float>(m_viewportHeight)};
  m_projMatrix = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 5.0f);

}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  if (!started) {
    auto widgetSize{ImVec2(240, 95)};

    ImGui::SetNextWindowPos(ImVec2((m_viewportWidth/2) - (widgetSize.x/2), (m_viewportHeight/2) - (widgetSize.y/2)));
    ImGui::SetNextWindowSize(widgetSize);
    auto flags{ImGuiWindowFlags_NoDecoration};
    ImGui::Begin("Start Window", nullptr, flags);
    ImGui::PushFont(m_font);
    ImGui::Text("   RUN! from UFABC\nPress any key to start");
    ImGui::PopFont();
    ImGui::End();
  }

  auto widgetSize{ImVec2(400, 190)};
  ImGui::SetNextWindowPos(ImVec2(m_viewportWidth - widgetSize.x - 5, 10));
  ImGui::SetNextWindowSize(widgetSize);
  auto flags{ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground};
  ImGui::Begin("Enjoy Window", nullptr, flags);
  ImGui::PushFont(m_font);
  if (enjoyTheWindow) {      
    ImGui::Text("Enjoy the view of dos Estados, Ave.!");
  } else if (upStair) {
    ImGui::Text("Enjoy the roof with nice view!");
  }
  ImGui::PopFont(); 
  ImGui::End();
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  m_trackBallModel.resizeViewport(width, height);
  m_trackBallLight.resizeViewport(width, height);
  m_camera.computeProjectionMatrix(width, height);
}

void OpenGLWindow::terminateGL() {
  for (const auto& program : m_programs) {
    glDeleteProgram(program);
  }
  terminateSkybox();
}

void OpenGLWindow::terminateSkybox() {
  glDeleteProgram(m_skyProgram);
  glDeleteBuffers(1, &m_skyVBO);
  glDeleteVertexArrays(1, &m_skyVAO);
}

void OpenGLWindow::update() {
  m_modelMatrix = m_trackBallModel.getRotation();

  // Handle camera position following the map range
  /*
    first level
    (-0.55, -0.05, -0.51) --------------- window (0.55, -0.05, -0.51)

    (-0.55, -0.05, 0.50)  start --------- stairs (0.55, -0.05, 0.50)

    second level
    (-0.55, 0.25, -0.51) --------------- window (0.55, 0.25, -0.51)

    (-0.55, 0.25, 0.50)  start --------- stairs (0.55, 0.25, 0.50)

    stairs
    (0.41 < x <= 0.55, -0.05, 0.1 < z <= 0.2) ground
    (0.41 < x <= 0.55,  0.05, 0.2 < z <= 0.3) 1st step
    (0.41 < x <= 0.55,  0.15, 0.3 < z <= 0.5) 2nd step
    (x < 0.45,          0.25, 0.3 < z <= 0.5) 1st level (sety upstair to true)
  
  */
  float deltaTime{static_cast<float>(getDeltaTime())};

  // If is in upstair set y camera position to next level of build
  if (upStair) {
    m_camera.yPos = 0.25f;
  }

  m_camera.m_eye[1] = m_camera.yPos;
  m_camera.m_at[1] = m_camera.yPos;
  m_camera.m_eye[0] = glm::clamp(m_camera.m_eye[0], -0.55f, 0.55f); // limit range of camera x
  m_camera.m_eye[2] = glm::clamp(m_camera.m_eye[2], -0.51f, 0.50f); // limit range of camera z

  // handle stairs steps  
  if (m_camera.m_eye[0] > 0.41f && m_camera.m_eye[0] <= 0.55f && m_camera.m_eye[2] > 0.1f && m_camera.m_eye[2] <= 0.2f) {
    m_camera.yPos = 0.05f;
    upStair = false;
  } else if (m_camera.m_eye[0] > 0.41f && m_camera.m_eye[0] <= 0.55f && m_camera.m_eye[2] > 0.2f && m_camera.m_eye[2] <= 0.3f) {
    m_camera.yPos = 0.1f;
    upStair = false;
  } else if (m_camera.m_eye[0] > 0.41f && m_camera.m_eye[0] <= 0.55f && m_camera.m_eye[2] > 0.3f && m_camera.m_eye[2] <= 0.50f) {
    m_camera.yPos = 0.15f;
    upStair = false;
  } else if (m_camera.m_eye[0] < 0.45f && m_camera.m_eye[2] > 0.3f && m_camera.m_eye[2] <= 0.50f && m_camera.yPos > 0.05f) {
    upStair = true;
  } else if (m_camera.m_eye[0] > 0.40f && m_camera.m_eye[2] < -0.33f) {
    enjoyTheWindow = true;
  } else {    
    m_camera.yPos = -0.05f;
    enjoyTheWindow = false;
  }
  
  m_camera.dolly(m_dollySpeed * deltaTime);  
  m_camera.truck(m_truckSpeed * deltaTime);
  m_camera.pan(m_panSpeed * deltaTime);
 
  m_viewMatrix =
      glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f + m_zoom),
                  glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}