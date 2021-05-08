# Computer Graphics
Computer Graphics programs with C++ and OpenGL.

Three projects made with ABCg library:

# Nested Tic-Tac-Toe
A classic game, with a hint of difficulty, the Nested Tic-Tac-Toe consists of nine Tic-Tac-Toe games that together form a Tic-Tac-Toe, increasing the difficulty, since any of the games can be done simultaneously, with a distraction the player loses easily.

<p align="center">
  <img width="500" src="https://user-images.githubusercontent.com/38412804/117526576-8705df00-af9c-11eb-8660-6eba6df96837.png" alt="Started Nested Tic-Tac-Toe">
</p>

The line created when a player win is created using regular poligon defining point by point of drawing.

<p align="center">
  <img width="500" src="https://user-images.githubusercontent.com/38412804/117526602-b61c5080-af9c-11eb-970d-551122e02609.png" alt="Started Nested Tic-Tac-Toe Wins">
</p>

The code was made using the ABCg library, with OpenGL, the menu, and the buttons on each board were made with ImGui and the lines and columns, in addition to the end-of-game trace, were made with Open graphics.

For each board in the game, an ImGui window is created, which makes it a lot easier, as these windows are generated with just a few for loops.

The description of the positions of each board is made in the file openglwindow.hpp and the description and creation of the graphics are in the files shapes.cpp and shapes.hpp.

The game can be access in web version:

https://erickfunier.github.io/ComputerGraphics/nested-tic-tac-toe/

# Baby Yoda Projection

The projection of Baby Yoda was created from an OBJ of 3D modeling. Some resources for interacting and running the libraries have been added.

For coloring, a uniform variable was defined in the .frag file, which makes it possible to change the color of the fragments at run time. Therefore, through the user interface, it is possible to color with some pre-defined colors, which are stored in vec4 in the code. The separation of fragments is done by selecting the vertices to be drawn, therefore starting from the first element, which in this case is the scarf, the number of vertices necessary for the design was calculated, limiting the application to drawing only those triangles/ fragments with the color specified. This is done for each part of Yoda. The user can still make Yoda's eye "blink", we only do a blink varying the colors from white to black.

<p align="center">
  <img width="500" src="https://user-images.githubusercontent.com/38412804/117527031-3b086980-af9f-11eb-851b-6675a69cd777.png" alt="Baby Yoda Projection 1">
</p>

<p align="center">
  <img width="500" src="https://user-images.githubusercontent.com/38412804/117527051-625f3680-af9f-11eb-8df7-c88ddbe5c76a.png" alt="Baby Yoda Projection 2">
</p>

A bouncing feature was also added to simulate the Yoda's fluctuation movement. (the lightsaber had to be modeled :P)

In addition to the aforementioned features, an exploded view of Yoda was added, separating the elements from the body. For this, uniform variables were added in the .vert, allowing the position to be modified at run time, so before drawing each part of the doll, the axis is displaced with a position obtained in simulations. All added features can be used simultaneously.

The projection can be access in web version:

https://erickfunier.github.io/ComputerGraphics/baby-yoda/

# RUN! from UFABC

The project contains an object made in Autodesk Maya. Using normal map and brick texture.

Two texture maps are loaded Diffuse and Normal, and the mappingMode is defined to use information from the mesh and the object is created using a cube, then exported to Obj. The lighting and shadow are based on the Blinn-Phong model, but we use only a fixed point of light, simulating an open environment.

<p align="center">
  <img width="500" src="https://github.com/erickfunier/abcg/blob/Atividade-3/gif/run.gif" alt="Run from UFABC">
</p>

The user can move with the keys W, A, S, D (Up, Left, Down, Right-Arrow) and Q, E.

The environment follows the idea of a maze, but it looks more like a construction, when you get to the other side the user can enjoy a view with the UFABC (Federal University of ABC) Clock Tower. In addition, there is a ladder that the user can climb and enjoy the roof.

The game can be accessed in web version:

https://erickfunier.github.io/ComputerGraphics/run-ufabc/

# Library
All projects are made with ABCg: https://github.com/hbatagelo/abcg made by _Harlen Batagelo_
