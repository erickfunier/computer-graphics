#include "openglwindow.hpp"

#include <imgui.h>
#include "abcg.hpp"

void OpenGLWindow::initializeGL() {
    // Load a new font
    ImGuiIO &io{ImGui::GetIO()};
    auto filename{getAssetsPath() + "Inconsolata-Medium.ttf"};
    m_font = io.Fonts->AddFontFromFileTTF(filename.c_str(), 30.0f);
    if (m_font == nullptr) {
        throw abcg::Exception{abcg::Exception::Runtime("Cannot load font file")};
    }
    m_font2 = io.Fonts->AddFontFromFileTTF(filename.c_str(), 17.0f);
    m_font3 = io.Fonts->AddFontFromFileTTF(filename.c_str(), 260.0f);
    // Create program to render the objects
    m_objectsProgram = createProgramFromFile(getAssetsPath() + "objects.vert",
                                            getAssetsPath() + "objects.frag");

    // Set the clear color
    glClearColor(0, 0, 0, 1);
    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    #if !defined(__EMSCRIPTEN__)
        glEnable(GL_PROGRAM_POINT_SIZE);
    #endif

    restart();
}

// Restart the game
void OpenGLWindow::restart() {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            board[i][j] = "";            
        }
        winnerStr[i] = "";
        turnCount[i] = 0;
    }
    playerTurn = 0;
    winner = -1;
    for (int i = 0; i < 7; i++) {
        m_shapes.winnerLine[i] = 0;
    }
    m_shapes.initializeGL(m_objectsProgram);
}

// Insert text centralized in viewport
void OpenGLWindow::insertTextCenter(std::string text, int posX, int posY) {
    float font_size = ImGui::GetFontSize() * text.size() / 2;
    ImGui::SetCursorPosX(posX - font_size + (font_size / 2));
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - posY);
    ImGui::Text(text.c_str());
}

// Insert text in position x and position y
void OpenGLWindow::insertText(std::string text, int posX, int posY) {
    ImGui::SetCursorPosX(posX);
    ImGui::SetCursorPosY(posY);
    ImGui::Text(text.c_str());
}

// Check if game ended
void OpenGLWindow::checkEndGame(int game) {
    // Verify small boards for end game
    for (int i = 0; i < 8; i++) {
        std::string combination = "";

        // 8 combinations possible
        // 0 1 2
        // 3 4 5
        // 6 7 8
        // 0 3 6
        // 1 4 7
        // 2 5 8
        // 0 4 8
        // 2 4 6

        switch (i) { 
            case 0: 
                combination = board[game][0] + board[game][1] + board[game][2]; 
                break; 
            case 1: 
                combination = board[game][3] + board[game][4] + board[game][5]; 
                break; 
            case 2: 
                combination = board[game][6] + board[game][7] + board[game][8]; 
                break; 
            case 3: 
                combination = board[game][0] + board[game][3] + board[game][6]; 
                break; 
            case 4: 
                combination = board[game][1] + board[game][4] + board[game][7]; 
                break; 
            case 5: 
                combination = board[game][2] + board[game][5] + board[game][8]; 
                break; 
            case 6: 
                combination = board[game][0] + board[game][4] + board[game][8]; 
                break; 
            case 7: 
                combination = board[game][2] + board[game][4] + board[game][6]; 
                break; 
        }
        
        // Set the winner board variable
        if (combination == "XXX") { 
            winnerStr[game] = "X"; 
            break;
        } else if (combination == "OOO") { 
            winnerStr[game] = "O"; 
            break;
        } 
    }
    // Set winner variable if Draw
    if (turnCount[game] > 8 && winnerStr[game] == "") {
        winnerStr[game] = "D";
    }

    // Verify big tic-tac-toe if end game
    for (int i = 0; i < 8; i++) {
        std::string combination = "";

        // 8 combinations possible
        // 0 1 2
        // 3 4 5
        // 6 7 8
        // 0 3 6
        // 1 4 7
        // 2 5 8
        // 0 4 8
        // 2 4 6
        switch (i) { 
            case 0: 
                combination = winnerStr[0] + winnerStr[1] + winnerStr[2];
                // Insert winner line to m_shapes, then next PaintGL will draw winner line
                m_shapes.winnerLine = {
                    16, 17, 18, 
                    16, 18,19
                };
                break; 
            case 1: 
                combination = winnerStr[3] + winnerStr[4] + winnerStr[5];
                // Insert winner line to m_shapes, then next PaintGL will draw winner line
                m_shapes.winnerLine = {
                    20, 21, 22, 
                    20, 22, 23
                };
                break; 
            case 2: 
                combination = winnerStr[6] + winnerStr[7] + winnerStr[8];
                // Insert winner line to m_shapes, then next PaintGL will draw winner line
                m_shapes.winnerLine = {
                    24, 25, 26, 
                    24, 26, 27
                };
                break; 
            case 3: 
                combination = winnerStr[0] + winnerStr[3] + winnerStr[6];
                // Insert winner line to m_shapes, then next PaintGL will draw winner line
                m_shapes.winnerLine = {
                    36, 37, 38, 
                    36, 38, 39
                };
 
                break; 
            case 4: 
                combination = winnerStr[1] + winnerStr[4] + winnerStr[7];
                // Insert winner line to m_shapes, then next PaintGL will draw winner line
                m_shapes.winnerLine = {
                    32, 33, 34, 
                    32, 34, 35
                };
  
                break; 
            case 5: 
                combination = winnerStr[2] + winnerStr[5] + winnerStr[8];
                // Insert winner line to m_shapes, then next PaintGL will draw winner line
                m_shapes.winnerLine = {
                    28, 29, 30, 
                    38, 30, 31
                };

                break; 
            case 6: 
                combination = winnerStr[0] + winnerStr[4] + winnerStr[8];
                // Insert winner line to m_shapes, then next PaintGL will draw winner line
                m_shapes.winnerLine = {
                    44, 45, 46, 
                    44, 46, 47
                };

                break; 
            case 7: 
                combination = winnerStr[2] + winnerStr[4] + winnerStr[6];
                // Insert winner line to m_shapes, then next PaintGL will draw winner line
                m_shapes.winnerLine = {
                    40, 41, 42, 
                    40, 42, 43
                };

                break; 
        }
        
        // Set the winner board variable
        if (combination == "XXX") { 
            winner = 0; 
            return;
        } else if (combination == "OOO") { 
            winner = 1; 
            return;
        } 
    }

    // Set winner variable if Draw
    int count = 0;
    for (int i = 0; i < 9; i++) {        
        if (winnerStr[i] != "") {
            count++;
        }
    }
    if (count > 8) {
        winner = 2;
    }
}

void OpenGLWindow::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, m_viewportWidth, m_viewportHeight);
    m_shapes.m_color = {0.0f, 255.0f, 0.0f, 0.75f};
    m_shapes.paintGL();
}

void OpenGLWindow::paintUI() {

    bool gameChanged[9] = {false};

    ImGui::SetNextWindowSize(ImVec2(600, 64));
    
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    auto flagTitleBarWindow{ImGuiWindowFlags_NoScrollbar | 
            ImGuiWindowFlags_NoBringToFrontOnFocus | 
                ImGuiWindowFlags_NoCollapse | 
                ImGuiWindowFlags_MenuBar | 
                ImGuiWindowFlags_NoResize};
    // Start first window, with title and menu bar
    ImGui::Begin("Nested Tic-Tac-Toe!", nullptr, flagTitleBarWindow);
    {
        ImGui::PushFont(m_font2);
        // Menu Bar
        if (ImGui::BeginMenuBar()) {
            // File menu
            if (ImGui::BeginMenu("Game")) {
                ImGui::MenuItem("Restart", nullptr);
                if (ImGui::IsItemClicked()) {
                    restart();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        switch(winner) {
            case 0:
                insertTextCenter("X player Win!", 307, 8);
                m_shapes.initializeGL(m_objectsProgram);
                break;
            case 1:
                insertTextCenter("O player Win!", 307, 8);
                m_shapes.initializeGL(m_objectsProgram);
                break;
            case 2:
                insertTextCenter("Draw!", 307, 8);
                m_shapes.initializeGL(m_objectsProgram);
                break;
            default:
                std::string text = players.at(playerTurn) + " turn";
                insertTextCenter(text, 307, 8);
        }   
    }      
    ImGui::PopFont();
    ImGui::End();
       
    auto flagsSmallBoardWindow{ImGuiWindowFlags_NoScrollbar |
                ImGuiWindowFlags_NoResize | 
                ImGuiWindowFlags_NoTitleBar
                };    
    
    // Start a windows for each small board
    for (int k = 0; k < 9; k++) {
        ImGui::SetNextWindowPos(boardPos[k]);   // Set position on screen following index of positions list
        ImGui::SetNextWindowSize(ImVec2(191, 171));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 4.5f);  // Set window border
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f,0.0f,0.0f,0.0f));  // Disable backgroung
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f,255.0f,0.0f,1.0f));  // Set border color to green
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f,255.0f,0.0f,1.0f));    // Set Text color to green (used on buttons text too)
        std::string s = std::to_string(k);
        ImGui::Begin(s.c_str(), nullptr, flagsSmallBoardWindow);  
        
        if (winnerStr[k] != "") {
            ImGui::PushFont(m_font3);
            if (winnerStr[k] == "X") {
                insertText("X", 33, -50);
            } else if (winnerStr[k] == "O") {
                insertText("O", 33, -50);
            } else if (winnerStr[k] == "D") {
                insertText("D", 33, -50);
            }
            ImGui::PopFont();
        } else {
            ImGui::PushFont(m_font);
            ImGui::Columns(3);
            for (int i = 0; i < 9; i++) {

                ImGui::Columns(3);
                ImGui::SetColumnWidth(0, 63.0f);
                ImGui::SetColumnWidth(1, 63.0f);
                ImGui::SetColumnWidth(2, 63.0f);                

                ImGui::Button(board[k][i].c_str(), ImVec2(49, 46));
                if(ImGui::IsItemClicked() && winner == -1) {
                    if (board[k][i] == "") {
                        if (playerTurn == 0) {
                            board[k][i] = "X";
                            playerTurn = 1;
                        } else {
                            board[k][i] = "O";
                            playerTurn = 0;
                        }
                        gameChanged[k] = true;
                    }            
                }    
                ImGui::NextColumn();    
                if ((i+1)%3 == 0 && i < 7 && i > 0) {
                    ImGui::Separator();
                } 
            }
            ImGui::PopFont();
        }
        
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        
        ImGui::End();
    }

    // Verify if game changed to check for winner
    for (int i = 0; i < 9; i++) {
        if(gameChanged[i]) {
            turnCount[i] = turnCount[i] + 1;
            checkEndGame(i);
        }
    }    
}

void OpenGLWindow::resizeGL(int width, int height) {
    m_viewportWidth = width;
    m_viewportHeight = height;

    glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL() {
    glDeleteProgram(m_objectsProgram);
    m_shapes.terminateGL();
}

