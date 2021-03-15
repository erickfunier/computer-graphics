#include <fmt/core.h>

#include "openglwindow.hpp"

#include <imgui.h>

void OpenGLWindow::initializeGL() {
  auto windowSettings{getWindowSettings()};
  fmt::print("Initial window size: {}x{}\n", windowSettings.width,
             windowSettings.height);
}

void OpenGLWindow::paintGL() {
  // Set the clear color
  glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2],
               m_clearColor[3]);
  // Clear the color buffer
  glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::resetGame() {
    for (int i = 0; i < 9; i++) {
        board[i] = "";
    }
    playerTurn = 0;
    winner = -1;
    turnCount = 0;
}

void OpenGLWindow::checkEndGame() {
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
                combination = board[0] + board[1] + board[2]; 
                break; 
            case 1: 
                combination = board[3] + board[4] + board[5]; 
                break; 
            case 2: 
                combination = board[6] + board[7] + board[8]; 
                break; 
            case 3: 
                combination = board[0] + board[3] + board[6]; 
                break; 
            case 4: 
                combination = board[1] + board[4] + board[7]; 
                break; 
            case 5: 
                combination = board[2] + board[5] + board[8]; 
                break; 
            case 6: 
                combination = board[0] + board[4] + board[8]; 
                break; 
            case 7: 
                combination = board[2] + board[4] + board[6]; 
                break; 
        }
        
        if (combination == "XXX") { 
            winner = 0; 
            return;
        } else if (combination == "OOO") { 
            winner = 1; 
            return;
        } 
    }
    if (turnCount > 8) {
        winner = 2;
    }
}

void OpenGLWindow::insertTextCenter(std::string text) {
    float font_size = ImGui::GetFontSize() * text.size() / 2;
    ImGui::SetCursorPosX(307 - font_size + (font_size / 2));
    ImGui::Text(text.c_str());
}

void OpenGLWindow::paintUI() {
  // Parent class will show fullscreen button and FPS meter
  //abcg::OpenGLWindow::paintUI();
    
    ImGui::SetNextWindowSize(ImVec2(600, 600));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    auto flags{ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize};
    // Window begin
    ImGui::Begin("Tic-Tac-Toe!", nullptr, flags);
    {        
        bool gameChanged = false;
        // Menu Bar
        if (ImGui::BeginMenuBar()) {
            // File menu
            if (ImGui::BeginMenu("Game")) {
                ImGui::MenuItem("Restart", nullptr);
                if (ImGui::IsItemClicked()) {
                    resetGame();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        
        if (winner != -1) {
            switch (winner) {
                case 0:
                    insertTextCenter("X player Win!");
                    break;
                case 1:
                    insertTextCenter("O player Win!");
                    break;
                case 2:
                    insertTextCenter("Draw!");
                    break;
            }
        } else {
            std::string text = players.at(playerTurn) + " turn";
            insertTextCenter(text);
        }

        ImGui::Columns(3);
        ImGui::Button(board[0].c_str(), ImVec2(185, 150));
        if(ImGui::IsItemClicked() && winner == -1) {
            if (board[0] == "") {
                if (playerTurn == 0) {
                    board[0] = "X";
                    playerTurn = 1;
                } else {
                    board[0] = "O";
                    playerTurn = 0;
                }
                gameChanged = true;
            }            
        }
        ImGui::NextColumn();
        ImGui::Button(board[1].c_str(), ImVec2(185, 150));
        if(ImGui::IsItemClicked() && winner == -1) {
            if (board[1] == "") {
                if (playerTurn == 0) {
                    board[1] = "X";
                    playerTurn = 1;
                } else {
                    board[1] = "O";
                    playerTurn = 0;
                }
            }    
            gameChanged = true;        
        }
        ImGui::NextColumn();
        ImGui::Button(board[2].c_str(), ImVec2(185, 150));
        if(ImGui::IsItemClicked() && winner == -1) {
            if (board[2] == "") {
                if (playerTurn == 0) {
                    board[2] = "X";
                    playerTurn = 1;
                } else {
                    board[2] = "O";
                    playerTurn = 0;
                }
            }   
            gameChanged = true;         
        }
        ImGui::NextColumn();
        ImGui::Separator();
        ImGui::Button(board[3].c_str(), ImVec2(185, 150));
        if(ImGui::IsItemClicked() && winner == -1) {
            if (board[3] == "") {
                if (playerTurn == 0) {
                    board[3] = "X";
                    playerTurn = 1;
                } else {
                    board[3] = "O";
                    playerTurn = 0;
                }
            }     
            gameChanged = true;       
        }
        ImGui::NextColumn();
        ImGui::Button(board[4].c_str(), ImVec2(185, 150));
        if(ImGui::IsItemClicked() && winner == -1) {
            if (board[4] == "") {
                if (playerTurn == 0) {
                    board[4] = "X";
                    playerTurn = 1;
                } else {
                    board[4] = "O";
                    playerTurn = 0;
                }
            }     
            gameChanged = true;       
        }
        ImGui::NextColumn();
        ImGui::Button(board[5].c_str(), ImVec2(185, 150));
        if(ImGui::IsItemClicked() && winner == -1) {
            if (board[5] == "") {
                if (playerTurn == 0) {
                    board[5] = "X";
                    playerTurn = 1;
                } else {
                    board[5] = "O";
                    playerTurn = 0;
                }
            }        
            gameChanged = true;    
        }
        ImGui::NextColumn();
        ImGui::Separator();
        ImGui::Button(board[6].c_str(), ImVec2(185, 150));
        if(ImGui::IsItemClicked() && winner == -1) {
            if (board[6] == "") {
                if (playerTurn == 0) {
                    board[6] = "X";
                    playerTurn = 1;
                } else {
                    board[6] = "O";
                    playerTurn = 0;
                }
            }          
            gameChanged = true;  
        }
        ImGui::NextColumn();
        ImGui::Button(board[7].c_str(), ImVec2(185, 150));
        if(ImGui::IsItemClicked() && winner == -1) {
            if (board[7] == "") {
                if (playerTurn == 0) {
                    board[7] = "X";
                    playerTurn = 1;
                } else {
                    board[7] = "O";
                    playerTurn = 0;
                }
            }          
            gameChanged = true;  
        }
        ImGui::NextColumn();
        ImGui::Button(board[8].c_str(), ImVec2(185, 150));
        if(ImGui::IsItemClicked() && winner == -1) {
            if (board[8] == "") {
                if (playerTurn == 0) {
                    board[8] = "X";
                    playerTurn = 1;
                } else {
                    board[8] = "O";
                    playerTurn = 0;
                }
            }         
            gameChanged = true;   
        }
        ImGui::Columns();
        ImGui::SetCursorPosY(550);
        ImGui::Button("Restart Game", ImVec2(582, 40));
        if(ImGui::IsItemClicked()) {
            resetGame();
        }

        if(gameChanged) {
            turnCount++;
            checkEndGame();
        }        
    }
    // Window end
    ImGui::End();
  
}