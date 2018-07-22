#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
// #include "lab7part2lib.h"  // Testing library

void initialBoard(char board[][26], int n);
void printBoard(char board[][26], int n);
char computerMove(char board[][26], int n, char currentMove, bool *compHasMove);
bool bestMoveComp(char board[][26], int n, char colour, char opcolour, char *bestPosition);
char playerMove(char board[][26], int n, char currentMove, bool *playerHasMove, bool *playerMistake);
bool availableMoves(char board[][26], int n, char colour, char opcolour);
bool checkLegalInDirection(char board[][26], int n, char row, char col,
    char colour, char opcolour, int deltaRow, int deltaCol);
bool positionInBounds(char board[][26], int n, char row, char col);
int scoreInOneDirection(char board[][26], int n, char row, char col,
    char colour, char opcolour, int deltaRow, int deltaCol, int score);
bool flipNextOrNot(char board[][26], int n, char row, char col, char colour, char opcolour);
void doTheMove(char board[][26], int n, char move[3], bool *playerMistake);
void changeColour(char board[][26], int n, char row, char col,
    char colour, char opcolour, int deltaRow, int deltaCol);
void getWinner(char board[][26], int n);


// In part two, computer is designed as the algorism indicated below

int main() {
    char board[26][26];
    int n;
    char sideComputer;
    char currentMove = 'B';
    printf("Enter the board dimension: ");
    scanf("%d", &n);
    printf("Computer plays (B/W) : ");
    scanf(" %c", &sideComputer);

    initialBoard(board, n);
    bool compHasMove = true;
    bool playerHasMove = true;
    bool playerMistake = false;

    while (compHasMove == true || playerHasMove == true) {
        if (currentMove == sideComputer)
            currentMove = computerMove(board, n, currentMove, &compHasMove);
        else {
            currentMove = playerMove(board, n, currentMove, &playerHasMove, &playerMistake);
            if (playerMistake) break;  // In the rule, player making a mistake straightly lose the game
        }
    }
    if (playerMistake)
        printf("%c player wins.\n", sideComputer);
    else
        getWinner(board, n);
    return 0;
}

void initialBoard(char board[][26], int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if ((i == n / 2 - 1 && j == n / 2 - 1) || (i == n / 2 && j == n / 2))
                board[i][j] = 'W';
            else if ((i == n / 2 && j == n / 2 - 1) || (i == n / 2 - 1 && j == n / 2))
                board[i][j] = 'B';
            else
                board[i][j] = 'U';
        }
    }
    printBoard(board, n);
    return;
}

void printBoard(char board[][26], int n) {
    printf("  ");
    for (int i = 0; i < n; i++) {
        printf("%c", 'a' + i);
    }
    printf("\n");
    for (int j = 0; j < n; j++) {
        printf("%c ", 'a' + j);
        for (int k = 0; k < n; k++) {
            printf("%c", board[j][k]);
        }
        printf("\n");
    }
    return;
}

char computerMove(char board[][26], int n, char currentMove, bool *compHasMove) {
    char bestPosition[2] = { 'U', 'U' };
    char opMove;
    bool wholeLegal;
    bool dummy = true;
    if (currentMove == 'B') opMove = 'W';
    else opMove = 'B';
    wholeLegal = bestMoveComp(board, n, currentMove, opMove, bestPosition);
    if (wholeLegal) {
        *compHasMove = true;
        char move[3];
        move[0] = currentMove;
        move[1] = bestPosition[0];
        move[2] = bestPosition[1];
        printf("Computer places %c at %c%c.\n", currentMove, bestPosition[0], bestPosition[1]);
        doTheMove(board, n, move, &dummy);
    }
    else {
        *compHasMove = false;
        bool nextWholeLegal = true;
        nextWholeLegal = availableMoves(board, n, opMove, currentMove);
        if (nextWholeLegal) printf("%c player has no valid move.\n", currentMove);
    }
    if (currentMove == 'B') currentMove = 'W';
    else currentMove = 'B';
    return currentMove;
}

bool bestMoveComp(char board[][26], int n, char colour, char opcolour, char *bestPosition) {
    bool wholeLegal = false;
    int highestScore = -10;
    for (char row = 'a'; row < 'a' + n; row++) {
        for (char col = 'a'; col < 'a' + n; col++) {
            if (board[(int)(row - 'a')][(int)(col - 'a')] == 'U') {
                int score = -10;
                bool legal = false;
                bool positionLegal = false;
                for (int deltaRow = -1; deltaRow < 2; deltaRow++) {
                    for (int deltaCol = -1; deltaCol < 2; deltaCol++) {
                        legal = checkLegalInDirection(board, n, row, col, colour, opcolour, deltaRow, deltaCol);
                        if (legal) {
                            wholeLegal = true;
                            positionLegal = true;
                            // Similar to part 1, calculate how many to flip in each direction and add them
                            score = scoreInOneDirection(board, n, row, col, colour, opcolour, deltaRow, deltaCol, score);
                        }
                    }
                }
                if (positionLegal) {
                    // Second part of changing score, try to get side and corner 
                    if (row == 'a' || row == 'a' + n - 1 || col == 'a' || col == 'a' + n - 1) score = score + 2;
                    if ((row == 'a' && col == 'a') || (row == 'a' + n - 1 && col == 'a') || (row == 'a' && col == 'a' + n - 1) || (row == 'a' + n - 1 && col == 'a' + n - 1))
                        score = score + 3;
                    // Third part of changing score, try to avoid the row and column next to corner 
                    if (n >= 6) {
                        if ((row == 'a' + 1 && col == 'a' + 1) || (row == 'a' + n - 2 && col == 'a' + 1) || (row == 'a' + 1 && col == 'a' + n - 2) || (row == 'a' + n - 2 && col == 'a' + n - 2))
                            score = score - 2;
                    }
                    // If this position can be immediately flipped by player next, it will have a lower mark
                    bool canBeFlipedNext = false;
                    canBeFlipedNext = flipNextOrNot(board, n, row, col, colour, opcolour);
                    if (canBeFlipedNext) {
                        if (n>4) score = score - 3;
                        else score = score - 2;
                    }
                }
                // printf("Score of %c%c is %d\n", row, col, score);
                // If this position has a higher score, change best to this one
                if (score > highestScore) {
                    highestScore = score;
                    *bestPosition = row;
                    *(bestPosition + 1) = col;
                }
            }
        }
    }
    return wholeLegal;
}

char playerMove(char board[][26], int n, char currentMove, bool *playerHasMove, bool *playerMistake) {
    char opMove;
    bool wholeLegal;
    if (currentMove == 'B') opMove = 'W';
    else opMove = 'B';
    wholeLegal = availableMoves(board, n, currentMove, opMove);
    if (wholeLegal) {
        *playerHasMove = true;
        // For testing
        /*
        * int row;
        * int col;
        * findSmartestMove(board, n, currentMove, &row, &col);
        * printf("Testing AI move (row, col): %c%c\n", row + 'a', col + 'a');
        */
        printf("Enter move for colour %c (RowCol): ", currentMove);
        char move[3];
        move[0] = currentMove;
        for (int i = 1; i <= 2; i++) {
            scanf(" %c", &move[i]);
        }
        /*
        * char move[3];
        * move[0] = currentMove;
        * move[1] = row+'a';
        * move[2] = col+'a';
        */
        doTheMove(board, n, move, playerMistake);
    }
    else {
        *playerHasMove = false;
        bool nextWholeLegal = true;
        nextWholeLegal = availableMoves(board, n, opMove, currentMove);
        if (nextWholeLegal) printf("%c player has no valid move.\n", currentMove);
    }
    if (currentMove == 'B') currentMove = 'W';
    else currentMove = 'B';
    return currentMove;
}

bool availableMoves(char board[][26], int n, char colour, char opcolour) {
    bool wholeLegal = false;
    for (char row = 'a'; row < 'a' + n; row++) {
        for (char col = 'a'; col < 'a' + n; col++) {
            if (board[(int)(row - 'a')][(int)(col - 'a')] == 'U') {
                bool legal = false;
                for (int deltaRow = -1; deltaRow < 2 && !legal; deltaRow++) {
                    for (int deltaCol = -1; deltaCol < 2 && !legal; deltaCol++) {
                        legal = checkLegalInDirection(board, n, row, col, colour, opcolour, deltaRow, deltaCol);
                        if (legal) {
                            wholeLegal = true;
                        }
                    }
                }
            }
        }
    }
    return wholeLegal;
}

bool checkLegalInDirection(char board[][26], int n, char row, char col,
    char colour, char opcolour, int deltaRow, int deltaCol) {
    bool legal = false;
    while (!legal) {
        row = row + deltaRow;
        col = col + deltaCol;
        bool inBounds = positionInBounds(board, n, row, col);
        if (board[(int)(row - 'a')][(int)(col - 'a')] == colour && board[(int)(row - 'a') - deltaRow][(int)(col - 'a') - deltaCol] == opcolour)
            legal = true;
        else if ((board[(int)(row - 'a')][(int)(col - 'a')] == colour && board[(int)(row - 'a') - deltaRow][(int)(col - 'a') - deltaCol] != opcolour)
            || (board[(int)(row - 'a')][(int)(col - 'a')] == 'U' || !inBounds))
                break;
    }
    return legal;
}

bool positionInBounds(char board[][26], int n, char row, char col) {
    bool inBounds;
    if (row >= 'a' && row <= 'a' + n - 1 && col >= 'a' && col <= 'a' + n - 1)
        inBounds = true;
    else inBounds = false;
    return inBounds;
}

int scoreInOneDirection(char board[][26], int n, char row, char col,
    char colour, char opcolour, int deltaRow, int deltaCol, int score) {
    score = 0;
    row = row + deltaRow;
    col = col + deltaCol;
    while (board[(int)(row - 'a')][(int)(col - 'a')] == opcolour) {
        score++;
        row = row + deltaRow;
        col = col + deltaCol;
    }
    return score;
}

// Function to decide whether this position will be flipped or not in next move by player
bool flipNextOrNot(char board[][26], int n, char row, char col, char colour, char opcolour) {
    bool canBeFlipped = false;
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            if (board[(int)(row - 'a') + i][(int)(col - 'a') + j] == 'U') {
                char nextRow = row - i;
                char nextCol = col - j;
                while (board[(int)(nextRow - 'a')][(int)(nextCol - 'a')] != 'A') {
                    if (board[(int)(nextRow - 'a')][(int)(nextCol - 'a')] == opcolour) {
                        canBeFlipped = true;
                        break;
                    }
                    else if (board[(int)(nextRow - 'a')][(int)(nextCol - 'a')] == colour) {
                        nextRow = nextRow - i;
                        nextCol = nextCol - j;
                    }
                    else break;
                }
            }
        }
    }
    return canBeFlipped;
}

void doTheMove(char board[][26], int n, char move[3], bool *playerMistake) {
    char row = move[1];
    char col = move[2];
    char colour = move[0];
    char opcolour;
    if (colour == 'B') opcolour = 'W';
    else opcolour = 'B';
    bool legal = false;
    bool wholeLegal = false;
    for (int deltaRow = -1; deltaRow < 2; deltaRow++) {
        for (int deltaCol = -1; deltaCol < 2; deltaCol++) {
            legal = checkLegalInDirection(board, n, row, col, colour, opcolour, deltaRow, deltaCol);
            if (legal) {
                wholeLegal = true;
                board[(int)(row - 'a')][(int)(col - 'a')] = colour;
                changeColour(board, n, row, col, colour, opcolour, deltaRow, deltaCol);
            }
        }
    }
    if (!wholeLegal) {
        *playerMistake = true;
        printf("Invalid move.\n");
    }
    else printBoard(board, n);
    return;
}

void changeColour(char board[][26], int n, char row, char col,
    char colour, char opcolour, int deltaRow, int deltaCol) {
    row = row + deltaRow;
    col = col + deltaCol;
    while (board[(int)(row - 'a')][(int)(col - 'a')] != colour) {
        board[(int)(row - 'a')][(int)(col - 'a')] = colour;
        row = row + deltaRow;
        col = col + deltaCol;
    }
    return;
}

void getWinner(char board[][26], int n) {
    int blackNum = 0;
    int whiteNum = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (board[i][j] == 'B') blackNum++;
            else if (board[i][j] == 'W') whiteNum++;
        }
    }
    // printf("%d white\n", whiteNum);
    // printf("%d black\n", blackNum);
    if (blackNum > whiteNum) printf("B player wins.\n");
    else if (whiteNum > blackNum) printf("W player wins.\n");
    else printf("Draw!\n");
    return;
}
