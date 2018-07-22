# Reversi_Chess_Game
This is a Reversi game written in C for APS105 course project. It can print a chessboard on command line, and has a computer playing with user.

The computer's logic of deciding where to put the next chess: 
1. How many opponent's pieces can be flipped on current move. 
2. Get the corner or side, and avoid position next to corner or side. 
3. Avoid move that can be flipped in the next move of opponent. 
I've also tried "predicting" for a few steps (i.e. let computer play with itself to decide if this is a good move), but the result wasn't ideal.

With these three simple logics, the program is powerful enough to beat assignment's test, both easy and hard level.