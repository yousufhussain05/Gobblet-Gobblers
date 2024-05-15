#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <cctype>

using namespace std;

class Node {
    public:
        // Board Data
        string board[3][3];
        int yellowPieces[3];
        int redPieces[3];
        char turn;

        Node* Next; // Pointer to the next node

        // Constructor to initialize a node with the current game state
        Node(string board[3][3], int yellowPieces[3], int redPieces[3], char turn, Node* Next = nullptr) {
            this->turn = turn;
            this->Next = Next;
            
            for (int i = 0; i < 3; i++) {
                // Copy counts of red and yellow pieces
                this->yellowPieces[i] = yellowPieces[i];
                this->redPieces[i] = redPieces[i];
                for (int j = 0; j < 3; j++) {
                    // Copy the board
                    this->board[i][j] = board[i][j];
                }
            }
        }
};

class Board {
    private:
        string board[3][3];
        int yellowPieces[3] = {2, 2, 2}; // Tracks the number of pieces available for Yellow
        int redPieces[3] = {2, 2, 2}; // Tracks the number of pieces available for Red
        char turn = 'y'; // Tracks whose turn it is, 'y' for Yellow and 'r' for Red
        Node* undoStack;

    public:
        Board() {
            // Initialize board with spaces and position numbers
            int num = 1;
            undoStack = nullptr;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    board[i][j] = " " + to_string(num++); 
                }
                yellowPieces[i] = 2;
                redPieces[i] = 2;
            }
        }

    // Destructor to clear up space
    ~Board() {
        clearUndoStack();
    }

    void clearUndoStack() {
    // Clear all memory allocated for the undo stack to prevent memory leaks
        while (undoStack != nullptr) {
            Node* temp = undoStack; 
            undoStack = undoStack->Next; // Move to the next item in the stack
            delete temp; // Deallocate memory of the current node
        }
    }

    void addToUndoStack() {
        // Push current state onto the stack
        undoStack = new Node(board, yellowPieces, redPieces, turn, undoStack);
    }

    bool undoMove() {
        // Undo the last move
        if (undoStack == nullptr) {
            cout << "Cannot undo." << endl; // Handle case where no moves are left to undo
            return false;
        }

        // Retrieve the most recent game state from the stack
        Node* temp = undoStack;
        undoStack = undoStack->Next; // Move the stack pointer to the next element

        // Restore previous state
        for (int i = 0; i < 3; i++) {
            yellowPieces[i] = temp->yellowPieces[i];
            redPieces[i] = temp->redPieces[i];
            for (int j = 0; j < 3; j++) {
                board[i][j] = temp->board[i][j];
            }
        }
        turn = temp->turn; // Restore the turn
        delete temp; // Free the memory of the node that has just been used

        return true;
    }

    void displayBoard() {
        // Print the current state of the board
        cout << endl;
        const int LEFTSPACES = 10;
        for (int i = 0; i < 3; i++) {
            cout << setw(LEFTSPACES) << ' ';
            for (int j = 0; j < 3; j++) {
                cout << board[i][j];
                if (j != 2) {
                    cout << "|"; // Separate columns with vertical bars
                }
            }

            if (i != 2) {
                cout << endl << setw(18) << "--------" << endl; // Separate rows with horizontal lines
            }
        }
        cout << endl << endl;
    }

    bool updateBoard(string input) {
        // Validate input length and that the second character is a digit
        if ((input.length() != 2) || !isdigit(input[1])) {
            return false; 
        }

        char pieceType = input[0]; // First character represents the piece type
        int piecePosition = stoi(input.substr(1)); // Convert the second character to position

        // Check if the position is within the valid range
        if ((piecePosition < 1) || (piecePosition > 9)) {
            return false;
        }

        int row = (piecePosition - 1) / 3; // Determine row from position
        int col = (piecePosition - 1) % 3; // Determine column from position
        int index = pieceType - 'a'; // Calculate the index for piece arrays based on piece type
        int* pieces = (turn == 'y') ? yellowPieces : redPieces; // Select the correct piece array
        
        if (pieces[index] <= 0) {
            return false; // Check if there are pieces left to place
        }

        string cellContent = board[row][col]; // Current content of the cell
        
        // Check if the cell is not in its initial state
        if (cellContent != " " + to_string(piecePosition)) {
            // Prevent placing large pieces on top of other large pieces
            if ((pieceType == 'a') && (cellContent == "YY" || cellContent == "RR")) {
                return false; 
            }
            // Prevent placing medium pieces on top of other medium or large pieces
            if ((pieceType == 'b') && (cellContent == "YY" || cellContent == "RR" || cellContent[0] == 'Y' || cellContent[0] == 'R')) {
                return false;
            }
            // Prevent placing small pieces on top of other small or medium or large pieces
            if ((pieceType == 'c') && (cellContent == "YY" || cellContent == "RR" || cellContent[0] == 'Y' || cellContent[0] == 'R' || cellContent[0] == 'y' || cellContent[0] == 'r')) {
                return false;
            }
        }

        addToUndoStack();
        
        // Define the symbol to place based on turn and piece type
        string symbol = (turn == 'y' ? "Y" : "R");
        if (pieceType == 'a') {
            symbol += (turn == 'y' ? "Y" : "R"); 
        }
        else if (pieceType == 'b') {
            symbol = (turn == 'y' ? "Y" : "R") + to_string(piecePosition);
        }
        else if (pieceType == 'c') {
            symbol = (turn == 'y' ? "y" : "r") + to_string(piecePosition);
        }

        board[row][col] = symbol; // Update the board with the new symbol
        pieces[index]--; // Decrement the count of the used piece type
        
        return true;
    }
 
    bool checkWin() {
        char target = turn == 'y' ? 'Y' : 'R'; // Determine the current player's symbol to check for a win.
        // Check all rows and columns for a winning line of three matching symbols.
        for (int i = 0; i < 3; i++) {
            // Check horizontal line in row 'i'
            if ((toupper(board[i][0][0]) == target) && (toupper(board[i][1][0]) == target) && (toupper(board[i][2][0]) == target) ||
                // Check vertical line in column 'i'
                (toupper(board[0][i][0]) == target) && (toupper(board[1][i][0]) == target) && (toupper(board[2][i][0]) == target)) {
                return true; // Return true if either condition is met, indicating a win.
            }
        }
        // Check diagonal from top-left to bottom-right
        if ((toupper(board[0][0][0]) == target) && (toupper(board[1][1][0]) == target) && (toupper(board[2][2][0]) == target) ||
            // Check diagonal from top-right to bottom-left
            (toupper(board[0][2][0]) == target) && (toupper(board[1][1][0]) == target) && (toupper(board[2][0][0]) == target)) {
            return true; // Return true if either diagonal condition is met, indicating a win.
        }
        return false; // If no win condition is met, return false.
    }

    bool checkTie() {
        // Check if all pieces from both colors are used
        for (int i = 0; i < 3; i++) {
            if (yellowPieces[i] > 0 || redPieces[i] > 0) {
                return false; // If any pieces are left, it's not a tie
            }
        }
        return true; // Return true if all pieces are used and no one has won
    }

    int piecesRemaining(char turn, int index){
        // Return the number of pieces remaining for the specified type and player
        return (turn == 'y') ? yellowPieces[index] : redPieces[index];
    }
    
    char getTurn() const {
        // Get the current player's turn
        return turn;
    }

    void switchTurns() {
        // Toggle the turn between yellow ('y') and red ('r')
        turn = (turn == 'y' ? 'r' : 'y'); 
    }
};

int main() {
    // Initialize the game board 
    Board gameBoard;
    string input;
    bool displayBoard = true;  // Check if the board should be displayed

    while (true) {
        if (displayBoard) {
            gameBoard.displayBoard(); // Display the current state of the board
            // Display remaining pieces for both players
            cout << "a. " << (gameBoard.getTurn() == 'y' ? "YY  " : "RR  ") << gameBoard.piecesRemaining(gameBoard.getTurn(), 0) << " remain." << endl;
            cout << "b. " << (gameBoard.getTurn() == 'y' ? "Y   " : "R   ") << gameBoard.piecesRemaining(gameBoard.getTurn(), 1) << " remain." << endl;
            cout << "c. " << (gameBoard.getTurn() == 'y' ? "y   " : "r   ") << gameBoard.piecesRemaining(gameBoard.getTurn(), 2) << " remain." << endl;
            cout << "q to exit." << endl << endl;
        }

        cout << "It is " << (gameBoard.getTurn() == 'y' ? "yellow's" : "red's") << " turn." << endl;
        cout << "Choose action and location, for example a2: ";
        cin >> input;

        displayBoard = true; // Board needs to be displayed

        if (input == "q") {
            break; // Exit loop if 'q' is entered
        }
        else if (input == "u") {
            displayBoard = gameBoard.undoMove() ? true : false; // Control display based on undo success
        }
        else {
            if (!gameBoard.updateBoard(input)) {
                cout << "Invalid move. Try again." << endl;
                displayBoard = false; // Do not display board after an invalid move
            } 
            else {
                if (gameBoard.checkWin()) {
                    gameBoard.displayBoard(); // Display board if a win condition is detected
                    cout << (gameBoard.getTurn() == 'y' ? "Yellow wins!" : "Red wins!") << endl;
                    break; // Exit the game on a win
                }
                gameBoard.switchTurns(); // Switch turns after a valid move
                if (gameBoard.checkTie()) {
                    gameBoard.displayBoard(); // Display board if a tie condition is detected
                    cout << "Tie game." << endl;
                    break; // Exit the game on a tie
                }
            }
        }
    }
    return 0;
}