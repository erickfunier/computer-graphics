#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <array>

#include "abcg.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resetGame();
  void checkEndGame();
  void insertTextCenter(std::string text);

 private:
  std::array<float, 4> m_clearColor{0.906f, 0.910f, 0.918f, 1.0f};  
  std::array<std::string, 9> board = {"", "", "", "", "", "", "", "", ""};  // Array of board 3x3
  int playerTurn = 0;                                                       // Player Turn 0 = player1 and 1 = player2
  std::vector<std::string> players{"X", "O"};                               // Game players
  int turnCount = 0;                                                        // Turn Count
  int winner = -1;                                                          // Winner flag 0 = player1 win, 1 = player2 win, 2 = draw
};

#endif