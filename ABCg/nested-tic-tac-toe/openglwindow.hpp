#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <imgui.h>

#include "abcg.hpp"
#include "shapes.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
    protected:
        void initializeGL() override;
        void paintGL() override;
        void paintUI() override;
        void restart();
        void resizeGL(int width, int height) override;
        void terminateGL() override;
        void checkEndGame(int game);
        void insertTextCenter(std::string text, int posX, int posY);
        void insertText(std::string text, int posX, int posY);

    private:
        GLuint m_objectsProgram{};

        int m_viewportWidth{};
        int m_viewportHeight{};

        Shapes m_shapes;

        ImFont* m_font{};
        ImFont* m_font2{};
        ImFont* m_font3{};

        std::string board[9][9] = {};  // Array of board 3x3

        int playerTurn = 0;   // Player Turn 0 = player1 and 1 = player2
        std::vector<std::string> players{"X", "O"}; // Game players
        int turnCount[9] = {0}; // Turn Count
        std::array<std::string, 9> winnerStr = {"", "", "", "", "", "", "", "", ""};    // Winner flag per board 0 = player1 win, 1 = player2 win, 2 = draw
        int winner = -1;    // Winner flag 0 = player1 win, 1 = player2 win, 2 = draw
        std::array<ImVec2, 9> boardPos{ImVec2(2, 64),
                                ImVec2(204, 64),
                                ImVec2(407, 64),
                                ImVec2(2, 246),
                                ImVec2(204, 246),
                                ImVec2(407, 246),
                                ImVec2(2, 428),
                                ImVec2(204, 428),
                                ImVec2(407, 428)
                                }; // Array of board windows positions
        };

#endif
