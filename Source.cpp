#include <vector>
#include <limits>
#include <string>
#include <iostream>
#include "sqlite3.h"
#include <cstdint>
#include <conio.h>
#include <chrono>
#include <ctime>
#include <thread>
#include <map>
#include <cstdint>
#include "authentication.h"
#include "c_time.h"
#include "stats.h"

using namespace std;



int GameMode;

class GameBoard {
private:
    int board[3][3]; // Represent the game board
public:
    GameBoard() {
        // Initialize the game board to all zeros
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                board[i][j] = 0;
            }
        }
    }

    // Display the game board
    void display() const {
        cout << "  1 2 3 " << endl;
        cout << " -------" << endl;
        for (int i = 0; i < 3; ++i) {
            cout << i + 1 << "|";
            for (int j = 0; j < 3; ++j) {
                if (board[i][j] == 1) {
                    cout << "X ";
                }
                else if (board[i][j] == -1) {
                    cout << "O ";
                }
                else {
                    cout << "- ";
                }
            }
            cout << endl;
        }
        cout << " -------" << endl;
    }


    // Check if the game has been won by a player
    int checkWin() const {
        // Check rows
        for (int i = 0; i < 3; i++) {
            if (board[i][0] + board[i][1] + board[i][2] == 3) {
                return 1; // Player 1 wins
            }
            else if (board[i][0] + board[i][1] + board[i][2] == -3) {
                return -1; // Player 2 wins
            }
        }
        // Check columns
        for (int j = 0; j < 3; j++) {
            if (board[0][j] + board[1][j] + board[2][j] == 3) {
                return 1; // Player 1 wins
            }
            else if (board[0][j] + board[1][j] + board[2][j] == -3) {
                return -1; // Player 2 wins
            }
        }
        // Check diagonals
        if (board[0][0] + board[1][1] + board[2][2] == 3 || board[0][2] + board[1][1] + board[2][0] == 3) {
            return 1; // Player 1 wins
        }
        else if (board[0][0] + board[1][1] + board[2][2] == -3 || board[0][2] + board[1][1] + board[2][0] == -3) {
            return -1; // Player 2 wins
        }
        // Check for draw
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == 0) {
                    return 0; // Game is not over yet
                }
            }
        }
        return 2; // Game is a draw
    }

    // Accessor for the board
    int getValue(int row, int col) const {
        return board[row][col];
    }

    // Setter for the board
    void setValue(int row, int col, int value) {
        board[row][col] = value;
    }
};

// Abstract class for players
class Player {
public:
    virtual void makeMove(GameBoard& board) const = 0;
    int win_count = 0;
    int lose_count = 0;
    int total_games = 0;

};

// Human player class
class Human1Player : public Player {
public:
    int win_count = 0;
    int lose_count = 0;
    int total_games = 0;
    void makeMove(GameBoard& board) const override {
        int row, col;
        cout << "Enter row and column to place your mark (1-3): ";
        while (true) {
            cin >> row >> col;
            if (row > 3 || col > 3 || row == 0 || col == 0) {
                cout << "false coordinates" << endl;
                cout << "Re-enter moves" << endl;
            }
            else break;
        }
        row--;
        col--;
        while (board.getValue(row, col) != 0) {
            cout << "That spot is already taken. Enter a different spot: ";
            cin >> row >> col;
            row--;
            col--;
        }
        board.setValue(row, col, 1);
    }
};
class Human2Player : public Player {
public:
    int win_count = 0;
    int lose_count = 0;
    int total_games = 0;
    void makeMove(GameBoard& board) const override {
        int row, col;
        cout << "Enter row and column to place your mark (1-3): ";
        while (true) {
            cin >> row >> col;
            if (row > 3 || col > 3 || row == 0 || col == 0) {
                cout << "false coordinates" << endl;
                cout << "Re-enter moves" << endl;
            }
            else break;
        }
        row--;
        col--;
        while (board.getValue(row, col) != 0) {
            cout << "That spot is already taken. Enter a different spot: ";
            cin >> row >> col;
            row--;
            col--;
        }
        board.setValue(row, col, -1);
    }
};


class AIPlayer : public Player {
public:
    void makeMove(GameBoard& board) const override {
        cout << "AI Move:" << endl;
        TreeNode* root = new TreeNode;
        root->board = board;
        build_tree(root, -1); // AI is player -1

        int best_score = numeric_limits<int>::min();
        TreeNode* best_move = nullptr;
        for (TreeNode* child : root->children) {
            int score = minimax(child, numeric_limits<int>::min(), numeric_limits<int>::max(), false, 9); // Increase depth of search
            if (score > best_score) {
                best_score = score;
                best_move = child;
            }
        }

        board.setValue(best_move->moveRow, best_move->moveCol, -1); // AI's move

        // Free memory
        for (TreeNode* child : root->children) {
            delete child;
        }
        delete root;
    }

private:
    // Define TreeNode structure
    struct TreeNode {
        GameBoard board;
        vector<TreeNode*> children;
        int score;
        int moveRow;
        int moveCol;
    };

    // Create a recursive function to build the game tree
    void build_tree(TreeNode* node, int player) const {
        int winner = node->board.checkWin();
        if (winner != 0) {
            node->score = winner;
            return;
        }

        // Generate child nodes for possible moves
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (node->board.getValue(i, j) == 0) {
                    TreeNode* child = new TreeNode;
                    child->board = node->board;
                    child->board.setValue(i, j, player);
                    child->moveRow = i;
                    child->moveCol = j;
                    node->children.push_back(child);
                    build_tree(child, -player);
                }
            }
        }
    }

    // Create a function to perform minimax with alpha-beta pruning
    int minimax(TreeNode* node, int alpha, int beta, bool is_max, int depth) const { // Adjust depth parameter
        if (node->children.empty() || depth == 0) {
            return evaluate(node->board); // Evaluate the board state
        }

        if (is_max) {
            int max_score = numeric_limits<int>::min();
            for (TreeNode* child : node->children) {
                int score = minimax(child, alpha, beta, false, depth - 1);
                max_score = max(max_score, score);
                alpha = max(alpha, score);
                if (alpha >= beta) {
                    break;
                }
            }
            return max_score;
        }
        else {
            int min_score = numeric_limits<int>::max();
            for (TreeNode* child : node->children) {
                int score = minimax(child, alpha, beta, true, depth - 1);
                min_score = min(min_score, score);
                beta = min(beta, score);
                if (alpha >= beta) {
                    break;
                }
            }
            return min_score;
        }
    }

    // Evaluate the board state
    int evaluate(const GameBoard& board) const {
        int result = board.checkWin();
        if (result == 1) { // If player wins, return a low score
            return -1000;
        }
        else if (result == -1) { // If AI wins, return a high score
            return 1000;
        }
        else if (result == 2) { // If it's a draw, return 0
            return 0;
        }
        // Otherwise, return a neutral score
        return 0;
    }
};

class Game {
private:
    Player* player1;
    Player* player2;
    sqlite3* db; // Add the SQLite database connection 
    std::string player1Email; // Email of Player 1
    std::string player2Email; // Email of Player 2
    vector<vector<GameBoard>> allGames; // Store all games played
    int gameCounter; // Counter for the number of games played
public:
    Game(Player* p1, Player* p2, sqlite3* dbConn, const std::string& email1, const std::string& email2)
        : player1(p1), player2(p2), db(dbConn), player1Email(email1), player2Email(email2), gameCounter(1) {}

    void play() {
        cout << "Welcome to Tic Tac Toe!" << endl;

        // Initialize game settings based on the mode
        if (GameMode == 2) {
            cout << "You will be playing against an unbeatable AI." << endl;
            cout << "You will be X and the AI will be O. Let's begin!" << endl;
        }
        else if (GameMode == 1) {
            cout << "You will be playing against each other." << endl;
            cout << "Player 1 will be X and Player 2 will be O." << endl;
        }

        bool playAgain = true; // To determine whether to keep playing
        while (playAgain) { // Outer game loop
            GameBoard board; // Create a new board for each game
            vector<GameBoard> moves; // Store moves for the current game
            bool player1Turn = true;

            while (true) { // Inner game loop
                board.display(); // Show current board state

                if (player1Turn) {
                    player1->makeMove(board); // Player 1's move
                }
                else {
                    player2->makeMove(board); // Player 2's (or AI's) move
                }

                moves.push_back(board); // Store the current board state

                int winner = board.checkWin(); // Check for win/draw
                if (winner != 0) {
                    board.display(); // Show the final board state

                    // Handle the winner announcement and stat updates
                    if (winner == 1) { // Player 1 wins
                        cout << "Player 1 wins!" << endl;
                        player1->win_count++;
                        player1->total_games++;
                        player2->lose_count++;
                        player2->total_games++;

                        updatePlayerStats(db, player1Email, player1->win_count, player1->lose_count, player1->total_games);
                        updatePlayerStats(db, player2Email, player2->win_count, player2->lose_count, player2->total_games);
                    }
                    else if (winner == -1) { // Player 2 wins
                        cout << "Player 2 wins!" << endl;
                        player2->win_count++;
                        player2->total_games++;
                        player1->lose_count++;
                        player1->total_games++;

                        updatePlayerStats(db, player2Email, player2->win_count, player2->lose_count, player2->total_games);
                        updatePlayerStats(db, player1Email, player1->win_count, player1->lose_count, player1->total_games);
                    }
                    else if (winner == 2) { // Draw/tie
                        cout << "It's a tie!" << endl;
                        player1->total_games++;
                        player2->total_games++;

                        updatePlayerStats(db, player1Email, player1->win_count, player1->lose_count, player1->total_games);
                        updatePlayerStats(db, player2Email, player2->win_count, player2->lose_count, player2->total_games);
                    }

                    break; // Exit the inner loop once the game is over
                }

                player1Turn = !player1Turn; // Toggle turns
            }

            allGames.push_back(moves); // Store the moves for this game


            // Ask if the players want to play again
            cout << "Do you want to play again? (1: Yes, 0: No): ";
            int playAgainChoice;
            cin >> playAgainChoice;
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer

            playAgain = (playAgainChoice == 1); // Continue if 'Yes', otherwise stop
        };

        // Game ended, optionally show statistics or other information    };




        cout << "1) player1 stats" << endl << "2)Player 2 stats" << endl << "3) Show Moves" << endl;
        int choose_1;
        cin >> choose_1;
        if (choose_1 == 1) {
            //code1
            cout << "Games won: " << player1->win_count << endl;
            cout << "Games lost: " << player1->lose_count << endl;
            cout << "Games Played: " << player1->total_games << endl;

        }
        else if (choose_1 == 2) {
            //code 2
            cout << "Games won: " << player1->win_count << endl;
            cout << "Games lost: " << player1->lose_count << endl;
            cout << "Games Played: " << player1->total_games << endl;
        }
        else if (choose_1 == 3) {
            while (true) {
                int choose_2;
                int chosenGame;
                cout << "1) show final board" << endl << "2) show move by move" << endl;
                cin >> choose_2;
                cout << "Enter the number of the game you want to display: ";
                cin >> chosenGame;
                if (choose_2 == 1) {
                    //final board
                    cout << "Press any key to show the Final Form of game " << chosenGame << ":" << endl;
                    cin.get();
                    allGames[chosenGame - 1].back().display();
                }
                else if (choose_2 == 2) {
                    //move by move
                    if (chosenGame >= 1 && chosenGame <= allGames.size()) {
                        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear input buffer
                        cout << "Press any key to start showing moves of Game " << chosenGame << ":" << endl;
                        cin.get();
                        for (size_t j = 0; j < allGames[chosenGame - 1].size(); ++j) {
                            if (j > 0) {
                                cout << "Press any key to show the next move:" << endl;
                                cin.get(); // Wait for user to press a key before displaying the next move
                            }
                            allGames[chosenGame - 1][j].display();
                            cout << endl;
                        }
                        // Prompt the user to display the final move separately
                        cout << "Press any key to show the Final Form of game " << chosenGame << ":" << endl;
                        cin.get();
                        allGames[chosenGame - 1].back().display(); // Display the final move
                    }
                    else {
                        cout << "Invalid game number." << endl;
                        break;
                    }
                }
                else {
                    cout << "invalid option" << endl;
                    break;
                }
                cout << "Do you want to display moves of another game? (1: Yes, 0: No): ";
                int displayAnotherGameChoice;
                cin >> displayAnotherGameChoice;
                if (displayAnotherGameChoice != 1) {
                    break;
                }

            }
        }

    }
    // Function to save game moves to the database
    void saveMovesToDatabase(int gameID, const std::vector<GameBoard>& moves) {
        for (size_t i = 0; i < moves.size(); ++i) {
            const GameBoard& board = moves[i];
            // Example SQL statement to insert a move into the database
            std::string insertSQL = "INSERT INTO game_moves (game_id, move_number, player, row, column) "
                "VALUES (" + std::to_string(gameID) + ", " + std::to_string(i + 1) + ", 'Player', ?, ?)";

            // Prepare the SQL statement
            sqlite3_stmt* stmt;
            if (sqlite3_prepare_v2(db, insertSQL.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
                std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << std::endl;
                return;
            }

            // Bind parameters and execute the statement
            for (int row = 0; row < 3; ++row) {
                for (int col = 0; col < 3; ++col) {
                    if (board.getValue(row, col) != 0) {
                        sqlite3_bind_int(stmt, 1, row);
                        sqlite3_bind_int(stmt, 2, col);
                        if (sqlite3_step(stmt) != SQLITE_DONE) {
                            std::cerr << "Error inserting move into database: " << sqlite3_errmsg(db) << std::endl;
                        }
                        sqlite3_reset(stmt); // Reset the statement for reuse
                    }
                }
            }

            // Finalize the statement
            sqlite3_finalize(stmt);
        }
    }

    // Function to retrieve game moves from the database
    std::vector<GameBoard> retrieveMovesFromDatabase(int gameID) {
        std::vector<GameBoard> moves;
        // Example SQL query to retrieve moves of a game from the database
        std::string querySQL = "SELECT row, column FROM game_moves WHERE game_id = " + std::to_string(gameID) + " ORDER BY move_number";

        // Prepare the SQL query
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, querySQL.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Error preparing SQL query: " << sqlite3_errmsg(db) << std::endl;
            return moves;
        }

        // Execute the query and process results
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int row = sqlite3_column_int(stmt, 0);
            int col = sqlite3_column_int(stmt, 1);
            // Create a new game board and set the move
            GameBoard board;
            board.setValue(row, col, 1); // Assuming the move is by Player 1
            moves.push_back(board);
        }

        // Finalize the statement
        sqlite3_finalize(stmt);

        return moves;
    }

    void statistics() {
        while (true) {
            cout << "1)Player1 stats" << endl << "2)Player 2 stats" << endl << "3) Show Moves" << endl;
            int choose_1;
            cin >> choose_1;
            if (choose_1 == 1) {
                //code1
                cout << "Games won: " << player1->win_count << endl;
                cout << "Games lost: " << player1->lose_count << endl;
                cout << "Games Played: " << player1->total_games << endl;

            }
            else if (choose_1 == 2) {
                //code 2
                cout << "Games won: " << player1->win_count << endl;
                cout << "Games lost: " << player1->lose_count << endl;
                cout << "Games Played: " << player1->total_games << endl;
            }
            else if (choose_1 == 3) {
                while (true) {
                    int choose_2;
                    int chosenGame;
                    cout << "1) show final board" << endl << "2) show move by move" << endl;
                    cin >> choose_2;
                    cout << "Enter the number of the game you want to display: ";
                    cin >> chosenGame;
                    if (choose_2 == 1) {
                        //final board
                        cout << "Press any key to show the Final Form of game " << chosenGame << ":" << endl;
                        cin.get();
                        allGames[chosenGame - 1].back().display();
                    }
                    else if (choose_2 == 2) {
                        //move by move
                        if (chosenGame >= 1 && chosenGame <= allGames.size()) {
                            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear input buffer
                            cout << "Press any key to start showing moves of Game " << chosenGame << ":" << endl;
                            cin.get();
                            for (size_t j = 0; j < allGames[chosenGame - 1].size(); ++j) {
                                if (j > 0) {
                                    cout << "Press any key to show the next move:" << endl;
                                    cin.get(); // Wait for user to press a key before displaying the next move
                                }
                                allGames[chosenGame - 1][j].display();
                                cout << endl;
                            }
                            // Prompt the user to display the final move separately
                            cout << "Press any key to show the Final Form of game " << chosenGame << ":" << endl;
                            cin.get();
                            allGames[chosenGame - 1].back().display(); // Display the final move
                        }
                        else {
                            cout << "Invalid game number." << endl;
                            break;
                        }
                    }
                    else {
                        cout << "invalid option" << endl;
                        break;
                    }
                    cout << "Do you want to display moves of another game? (1: Yes, 0: No): ";
                    int displayAnotherGameChoice;
                    cin >> displayAnotherGameChoice;
                    if (displayAnotherGameChoice != 1) {
                        break;
                    }

                }

            }
            cout << "do you want to quit of continue reviewing stats" << endl;
            cout << "1)Quit" << endl << "2)Continue" << endl;
            int choose_3;
            cin >> choose_3;
            if (choose_3 == 1) { break; }
        }

    }


};



int main() {
    sqlite3* db; // SQLite database connection
    const char* dbFilename = "tictactoe3.db"; // Database file name

    // Open the database connection
    if (sqlite3_open(dbFilename, &db) != SQLITE_OK) {
        cerr << "Cannot open database: " << sqlite3_errmsg(db) << endl;
        return 1; // Exit on error
    }

    int gameMode;
    cout << "Choose game mode: 1. Player vs. Player, 2. Player vs. AI" << endl;
    cin >> gameMode;

    if (gameMode == 1) { // Player vs. Player
        std::string player1Email, player2Email;

        // Ask if players have accounts
        int hasAccount1;
        cout << "Player 1, do you have an account? (1: Yes, 0: No): ";
        cin >> hasAccount1;
        if (hasAccount1 == 1) {
            player1Email = login(db); // Log in Player 1
        }
        else {
            player1Email = signup(db); // Sign up Player 1
        }

        int hasAccount2;
        cout << "Player 2, do you have an account? (1: Yes, 0: No): ";
        cin >> hasAccount2;
        if (hasAccount2 == 1) {
            player2Email = login(db); // Log in Player 2
        }
        else {
            player2Email = signup(db); // Sign up Player 2
        }

        // Set up the game for Player vs. Player
        Human1Player human1;
        Human2Player human2;
        Game game(&human1, &human2, db, player1Email, player2Email); // Pass both emails
        game.play(); // Start the game
    }
    else if (gameMode == 2) { // Player vs. AI
        std::string player1Email; // Only one email for the human player
        std::string player2Email;
        player2Email = 123;
        int hasAccount;
        cout << "Do you have an account? (1: Yes, 0: No): ";
        cin >> hasAccount;
        if (hasAccount == 1) {
            player1Email = login(db); // Log in
        }
        else {
            player1Email = signup(db); // Sign up
        }

        Human1Player human;
        AIPlayer ai; // Set up AI
        Game game(&human, &ai, db, player1Email, player2Email); // Pass email to Game
        game.play(); // Start the game
    }
    else {
        cout << "Invalid choice." << endl;
    }

    // Close the database connection
    sqlite3_close(db);
    return 0;
}