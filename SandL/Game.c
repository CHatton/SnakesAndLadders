#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<conio.h>

typedef struct player_t {
	int pos; // current position player is at
	int row; // row the player is currently on
	int col; // col player is currently on
	bool isTurn; // this bool will be used when saving the game to know which player's turn it is
} player_t;

typedef struct ladder_t {
	int headRow; // row of head
	int headCol; // col of head
	int tailRow; // row of tail
	int tailCol; // col of tail
} ladder_t;

typedef struct snake_t {
	int headRow; // row of head
	int headCol; // col of head
	int tailRow; // row of tail
	int tailCol; // col of tail
} snake_t;

void printBoard(); // displays the current game board state
void randomizeBoard(char); // generates a random game board
void initBoard(); // initializes board
void showSnakesLaddersPos(); // prints out the positions of all snakes and ladders in the game
int rollDice(); // generates number from 1 - 6
void save(); // allows the user to save to one of three files
bool load(); // allows the user to load a previously saved game
void startNewGame(char); // allows the user to start a new game without restarting the program
void roll(); // move player along the board

char** gameBoard; // pointer to game board
player_t* players; // store the players
snake_t* snakes; // store the snakes
ladder_t* ladders; // store the ladders
FILE* file; // need a file pointer for saving and loading the game

int boardRows; // number of rows in the board
int boardCols; // number of cols in the board
int activePlayer; // the current player that is active
int numSnakesAndLadders; // the amount of snakes and ladders in the game
int numPlayers; // number of players in the game


void main() {
	bool loaded = false; // assume no load has taken place
	int i; // counter
	char choice; // user choice for main switch
	int fromPos, toPos; // keep track of where players were and where they moved to
	int rolled; // hold onto the value rolled
	char newGame; // store the user's choice to start a new game

	do {
		printf("Would you like to load a previously saved game? (Y/N)");
		scanf(" %c", &choice); // prompt to load game

	} while (choice != 'n'
		&& choice != 'N'
		&& choice != 'y'
		&& choice != 'Y');
	// continuously prompt the user until they enter either n or y - case insensitively

	switch (choice) {
	case 'y':
	case 'Y':loaded = load(); // determine if they've loaded a game or not
		break;
	case 'n':
	case 'N':printf("Creating a new game!\n"); // create a new game if not loading a game
		break;
	}

	if (loaded == 0) // no game was loaded, start a new game
		startNewGame('y'); // when a game is initially started, force a "yes" choice

	activePlayer = 0; // start of with player at index[0] being the active player
	while (choice != 'x' && choice != 'X') {

		for (i = 0; i < numPlayers; i++) {
			if (players[i].isTurn == true) {
				activePlayer = i; // remember the position of the active player for load feature
			}
		}

		printf("\nIt's player %d's turn. \n", (activePlayer + 1));
		printf("Select your option: \n");
		printf("Show (B)oard\n");
		printf("(S)ave game\n");
		printf("(L)oad game\n");
		printf("(R)oll Dice\n");
		printf("(P)osition of Snakes and Ladders\n");
		printf("(N)ew game\n");
		printf("E(X)it - will lose all progess since last save!\n");

		scanf(" %c", &choice);
		players[activePlayer].isTurn = true;
		/*
		want to save which player's turn it is
		so that when you load the game it can resume
		from the same player's turn
		*/
		switch (choice) {
		case 'b':
		case 'B':printBoard(); // show the current board state
			break;
		case 's':
		case 'S':save(); // save the game
			break;
		case 'l':
		case 'L':load(); // load the game
			break;
		case 'n':
		case 'N':
			printf("Are you sure you want to start a new game? (Y/N): ");
			// only want to prompt if they make a new game after starting a new game for the first time
			scanf(" %c", &newGame);
			startNewGame(newGame); // start a new game with their choice
			break;
		case 'r':
		case 'R': roll(); // advances player turn
			break;
		case 'p':
		case 'P':showSnakesLaddersPos(); // print out all the positions of the snakes and ladders
			break;
		case 'x':
		case 'X': // game ends here
			break;
		} // switch

	} // main while loop

	getch();
}

void showSnakesLaddersPos() {
	int i;
	int headPos, tailPos;

	for (i = 0; i < numSnakesAndLadders; i++) {
		headPos = (ladders[i].headRow * boardCols) + ladders[i].headCol;
		tailPos = (ladders[i].tailRow * boardCols) + ladders[i].tailCol;
		printf("Ladder #%d: Head at pos: %d, bottom at pos: %d\n", (i + 1), headPos + 1, tailPos + 1);
	}// show the locatin of all the ladders on the board

	printf("\n");

	for (i = 0; i < numSnakesAndLadders; i++) {
		headPos = (snakes[i].headRow * boardCols) + snakes[i].headCol;
		tailPos = (snakes[i].tailRow * boardCols) + snakes[i].tailCol;
		printf("Snake #%d: Head at pos: %d, tail at pos: %d\n", (i + 1), headPos + 1, tailPos + 1);
	} // show the location of all snakes on the board

	printf("\n");

	for (i = 0; i < numPlayers; i++) {
		if (players[i].pos == 0) {
			printf("Player #%d hasn't started yet!\n", (i + 1));
		}
		else {
			printf("Player #%d at pos: %d\n", (i + 1), players[i].pos);
		}
	}
	printf("\n");
}

void initBoard() {
	int i, j;
	for (i = 0; i < boardRows; i++) {
		for (j = 0; j < boardCols; j++) {
			gameBoard[i][j] = ' '; // empty every position on the board
		}
	}
}
/*
clear the contents of the board and
empty every slot. Used in the load
function.
*/

void randomizeBoard(char type) {
	int i; // counter
	int hrow, hcol, trow, tcol; // head row , col and tail row, col

	srand(time(NULL));
	// make the seed for the random numbers the current time
	switch (type) {
	case 'L':
	case 'l': numSnakesAndLadders = (boardCols * boardRows) / 20; // 5% of total
		break;
	case 'M':
	case 'm': numSnakesAndLadders = (boardCols * boardRows) /14; // 7% of total
		break;
	case 'H':
	case 'h': numSnakesAndLadders = (boardCols * boardRows)/10; // 10% of total
		break;
	default:
		numSnakesAndLadders = (boardCols * boardRows) / 14; // 7% default
		break;
	}

	// create an array of snakes and ladders using dynamic memory allocation
	snakes = (snake_t*)malloc(numSnakesAndLadders*sizeof(snake_t));
	ladders = (ladder_t*)malloc(numSnakesAndLadders*sizeof(ladder_t));

	for (i = 0; i < numSnakesAndLadders; i++) {

		/*
		CONDITIONS FOR A LADDER
		1. the head must be above the tail
		2. must not be in last position
		*/

		do {
			hrow = rand() % boardRows;
			hcol = rand() % boardCols; // randomly assign row and col for head

			trow = rand() % boardRows;
			tcol = rand() % boardCols; // randomly assign row and col for tail
		} while ((hrow < trow) // cannot be in a lower row
			|| (hrow == trow) // cannot be on the same row
			|| (gameBoard[hrow][hcol] != ' ') // not an empty space
			|| (gameBoard[trow][tcol] != ' ')  // not an empty space
			|| (hrow == (boardRows - 1) && hcol == (boardCols-1))); // must not be the final square
			

		ladders[i].headCol = hcol;
		ladders[i].headRow = hrow;
		ladders[i].tailCol = tcol;
		ladders[i].tailRow = trow;
		// fill in the details for that ladder

		gameBoard[ladders[i].headRow][ladders[i].headCol] = 'L';
		gameBoard[ladders[i].tailRow][ladders[i].tailCol] = 'l';
		// fill up the gameboard with the positions of the ladders

		/*
		CONDITIONS FOR SNAKE
		1. the head must be below the tail
		2. must not be in last position
		*/

		do {
			hrow = rand() % boardRows;
			hcol = rand() % boardCols; // randomly assign row and col for head

			trow = rand() % boardRows;
			tcol = rand() % boardCols; // randomly assign row and col for tail
		} while ((hrow < trow) // must not be in a higher row
			|| (hrow == trow) // must not be on the same row
			|| gameBoard[hrow][hcol] != ' ' // must be an empty space
			|| gameBoard[trow][tcol] != ' '  // must be an empty space
			|| (hrow == (boardRows - 1) && hcol == (boardCols - 1))); // must not be on the last square
			// conditions for being invalid

		snakes[i].headCol = hcol;
		snakes[i].headRow = hrow;
		snakes[i].tailCol = tcol;
		snakes[i].tailRow = trow;

		gameBoard[snakes[i].headRow][snakes[i].headCol] = 'S';
		gameBoard[snakes[i].tailRow][snakes[i].tailCol] = 's';
		// fill up the gameboard with the positions of the snakes

	} // for each snake and ladder


} // randomizeBoard

void save() {
	int save, i, j;

	printf("Save over which file? - 1, 2, 3: ");
	scanf("%d", &save);

	switch (save) {
	case 1:file = fopen("save1.dat", "w");
		break;
	case 2:file = fopen("save2.dat", "w");
		break;
	case 3:file = fopen("save3.dat", "w");
		break;
	default:
		printf("Sorry, please enter either 1,2 or 3 for saving the game!\n");
		file = NULL;
	} // open up one of three files, or give error message

	if (file != NULL) { // save over the specified file

		fprintf(file, "%d %d %d %d\n", numPlayers, numSnakesAndLadders,boardRows,boardCols);
		// save all the non snakes/ladders/players info needed

		for (i = 0; i < numSnakesAndLadders; i++) {
			fprintf(file, "%d %d %d %d\n"
				, snakes[i].headCol, snakes[i].headRow, snakes[i].tailCol,snakes[i].tailRow);
		} // print all the snake information

		for (i = 0; i < numSnakesAndLadders; i++) {
			fprintf(file, "%d %d %d %d\n"
				, ladders[i].headCol, ladders[i].headRow, ladders[i].tailCol, ladders[i].tailRow);
		}// print all the ladder information

		for (i = 0; i < numPlayers; i++) {
			fprintf(file, "%d %d %d %d\n", players[i].col, players[i].isTurn, players[i].pos, players[i].row);
		} // print all the player information
		fclose(file); // close the file

	    //should be able to recreate any given board state with this information
		printf("Successfully Saved to the file!\n\n");

	}
	else {
		printf("There was an error saving to the file!\n\n");
	}
}

void printBoard() {
	int row, col;
	int playerPos;
	int playerRow;
	int playerCol;
	int i;
	
	char** copyBoard;
	copyBoard = (char**)malloc(boardRows*sizeof(char*));

	for (i = 0; i < boardRows; i++) {
		copyBoard[i] = (char*)malloc(boardCols * sizeof(char));
	}

	for (row = 0; row < boardRows; row++) {
		for (col = 0; col < boardCols; col++) {
			copyBoard[row][col] = gameBoard[row][col];
		}
	}

	/*
	Dynamically create a 2D array which is a copy
	of the main game board.
	*/

	copyBoard[boardRows - 1][boardCols - 1] = '*';
	// * to indicate the last position on the board
	/*
	The reason I created a copy of the board instead
	of using the board itself is that I want to
	display the current location of the players, in doing
	so I update the board and then print it. The alternative
	is a messy conditional printf in the nested for loop.
	This way, I can just overwrite the contents of the board
	to show the player is there, but not lose the information
	I need from the board.
	*/

	for (i = 0; i < numPlayers; i++) {
		playerPos = players[i].pos - 1; // position doesn't exactly allign with display
		playerRow = playerPos / boardCols; // get the current row
		playerCol = playerPos % boardCols; // get the current col
		copyBoard[playerRow][playerCol] = 'P'; // fill up that position with a 'P' to show player
	} // show the players on the board but don't overwrite the main board

	printf("\n "); // formatting

	for (i = 0; i < boardCols; i++) {
		printf("==="); // formatting
	}
	printf("=\n "); // formatting

	for (row = boardRows-1; row >= 0; row--) {

		if (row % 2 == 0) {
			for (col = 0; col < boardCols; col++) {
				printf("|%c ", copyBoard[row][col]);
			}
			printf("|\n "); // formatting

			for (i = 0; i < boardCols; i++) {
				printf("==="); // formatting
			}
			printf("=\n "); // formatting
		}
		else { // flip the odd rows to show it like the actual board game
			for (col = boardCols-1; col >= 0; col--) {
				printf("|%c ", copyBoard[row][col]);
			}
			printf("|\n "); // formatting

			for (i = 0; i < boardCols; i++) {
				printf("==="); // formatting
			}
			printf("=\n "); // formatting
		}

		/*
		in this function, because of how the positions
		on a snakes and ladders board are arranged, every
		other row needs to be printed out backwards. The
		contents of the array don't need to be changed, it's
		just the printing to the screen for the user to see.
		The formatting is a bit messy, but because I chose 
		to let the user pick the size of the board, the formatting
		needs to change based on the given board size.
		*/
	}
} // printBoard
  /*
  The purpose of print board is to simply print out
  the board state at any given time
  */

int rollDice() {
	int rnd; // will store our random number
	srand(time(NULL)); // use the current time of day as a seed for the random number
	rnd = rand() % 6; // % 6 so the number will be in the correct range
	return (rnd + 1); // returns 1 - 6 rather than 0 - 5
}

bool load() { // load a previously saved game
	int choice, i;

	printf("Load which save file? - 1,2 or 3?: ");
	scanf("%d", &choice);

	switch (choice) {
	case 1: file = fopen("save1.dat", "r");
		break;
	case 2: file = fopen("save2.dat", "r");
		break;
	case 3: file = fopen("save3.dat", "r");
		break;
	default:
		file = NULL;
		printf("Sorry, that file doesn't exist!\n");
	} // open up the corresponfing file in read mode


	if (file != NULL) { // as long as the file exists
						// read in all the values from the file
		
		fscanf(file, "%d %d %d %d\n", &numPlayers, &numSnakesAndLadders,&boardRows, &boardCols);
		// read in the non snakes/players/ladders info

		players = (player_t*)malloc(sizeof(player_t)*numPlayers); // create array to store players
		snakes = (snake_t*)malloc(sizeof(snake_t)*(numSnakesAndLadders)); // create array to store snakes
		ladders = (ladder_t*)malloc(sizeof(ladder_t)*(numSnakesAndLadders)); // create array to store ladders
		// re-create the arrays to store the players, snakes and ladders


		gameBoard = (char**)malloc(boardRows*sizeof(char*));

		for (i = 0; i < boardRows; i++) {
			gameBoard[i] = (char*)malloc(boardCols * sizeof(char));
		}
		// re-create the game board

		initBoard(); // reset the contents of the board

		/*
		I need to use initBoard AFTER I've created the board and know
		the board rows and board cols.
		*/

		for (i = 0; i < numSnakesAndLadders; i++) {
			fscanf(file, "%d %d %d %d\n"
				, &snakes[i].headCol,  &snakes[i].headRow, &snakes[i].tailCol, &snakes[i].tailRow);

			gameBoard[snakes[i].headRow][snakes[i].headCol] = 'S';
			gameBoard[snakes[i].tailRow][snakes[i].tailCol] = 's';
			// update the board with the position of the snakes

		} // load all the snake information

		for (i = 0; i < numSnakesAndLadders; i++) {
			fscanf(file, "%d %d %d %d\n"
				, &ladders[i].headCol, &ladders[i].headRow, &ladders[i].tailCol, &ladders[i].tailRow);

			gameBoard[ladders[i].headRow][ladders[i].headCol] = 'L';
			gameBoard[ladders[i].tailRow][ladders[i].tailCol] = 'l';
			// update the board with the position of the ladders

		}// load all the ladder information

		for (i = 0; i < numPlayers; i++) {
			fscanf(file, "%d %d %d %d\n", &players[i].col, &players[i].isTurn, &players[i].pos, &players[i].row);
		} // load from file
		fclose(file); // close the file

		/*
			when the boolean isTurn is saved as an int, it will
			be 1 for true, and 0 for false.
		*/

		for (i = 0; i < numPlayers; i++) {
			switch (players[i].isTurn) {
			case 1: players[i].isTurn = true;
				break;
			case 0: players[i].isTurn = false;
				break;
			default:
				break;
			} // assign each player their current "isTurn" value
		}

		printf("Successfully loaded the game! - Have fun!\n");
		return true; // 1 indicates that a load has occurred
	}
	else { // file doesn't exist
		printf("Sorry, there was an error opening the file.\n");
		return false; // 0 if there was not a load
	}
} // load

void startNewGame(newGame) {
	char choice;
	int i;

	switch (newGame) {
	case 'y':
	case 'Y':
		do { // how many players
			printf("Start a game with how many players? (2-6): ");
			scanf("%d", &numPlayers);
		} while (numPlayers > 6 || numPlayers < 2);
		// ensure they enter number between 2 and 6 with do while validation

		players = (player_t*)malloc(numPlayers*sizeof(player_t));
		// dynamically create storage for number of players


		do {
			printf("Choose board rows between 5 and 30 (reccomended 10): ");
			scanf("%d", &boardRows);
		} while (boardRows > 30 || boardRows < 5);
	

		do{
			printf("Choose board cols between 5 and 30 (reccomended 10): ");
			scanf("%d", &boardCols);
		} while (boardCols > 30 || boardCols < 5);
			
		

		// Dynamically allocate board space
		gameBoard = (char**)malloc(boardRows*sizeof(char*));

		for (i = 0; i < boardRows; i++) {
			gameBoard[i] = (char*)malloc(boardCols * sizeof(char));
		} 

		initBoard(); // initialize the game board		

		// create the players
		for (i = 0; i < numPlayers; i++) {
			players[i].col = 0;
			players[i].row = 0;
			players[i].isTurn = false;
			players[i].pos = 0;
		} // initialize all the players in the game

		players[0].isTurn = true; // ensure player one is always the first player
		/*
		density of snakes and ladders
		l/L - 5% of board size snakes and ladders
		m/M - 7% of board size snakes and ladders
		h/H - 10% of board size snakes and ladders
		*/
		printf("Choose the density of snakes and ladders: (L)ow, (M)edium or (H)igh (Reccomended is Medium): ");
		scanf(" %c", &choice); // read in the game density

		printf("\n\n"); // new line for formatting
		randomizeBoard(choice); // creates the game based on the user's choice

		printf("Game has started with the following settings: \nPlayers: %d\nNo Snakes/Ladders %d\nBoard dimensions %d X %d\n\nHave Fun!\n", numPlayers, numSnakesAndLadders,boardRows,boardCols);
		// print out the starting conditions for the game
		break;
	case 'n':
	case 'N': // a no choice will just continue without starting a new game
		break;
	default:
		break;
	} // switch
} // startNewGame

void roll() {
	char choice;
	int i;
	int fromPos, toPos;
	int rolled;

	players[activePlayer].isTurn = false;
	/*
	it should only be one player active at
	a time. As soon as the player takes their
	turn, isTurn is set to false.
	*/

	rolled = rollDice(); // random value from 1- 6
	printf("\nPlayer %d Rolled a %d!\n", (activePlayer + 1), rolled);
	// shows which player rolled the dice and what they got

	fromPos = players[activePlayer].pos; // get the position they're at right now
	players[activePlayer].pos += rolled; // add whatever they rolled
	toPos = players[activePlayer].pos; // get the position they're now at

	players[activePlayer].row = players[activePlayer].pos / boardCols; // gets the row the player is on
	players[activePlayer].col = players[activePlayer].pos % boardCols; // gets the col the player is on


	if (toPos >= boardRows * boardCols) { // player has won
		players[activePlayer].pos = boardRows * boardCols; // make sure you don't print a number greater than 100
		players[activePlayer].row = boardRows - 1; // make it so there's no index out of bounds exception if player won
		players[activePlayer].col = boardCols - 1;
		toPos = boardRows * boardCols;
	}

	if (fromPos == 0) {
		printf("Player %d moved to position %d\n", (activePlayer + 1), toPos);
	} // so it doesn't say from 0 to X
	else
	{

		printf("Player %d moved from position %d to position %d\n", (activePlayer + 1), fromPos, toPos);
		// lets the user know where they have moved from and to
	}
	if (gameBoard[players[activePlayer].row][players[activePlayer].col] == 'l') {
		// bottom of ladder
		for (i = 0; i < numSnakesAndLadders; i++) { // search through ladders
			if ((ladders[i].tailRow * boardRows + ladders[i].tailCol) == toPos) { // locate the one that links to the one landed on
				players[activePlayer].pos = (ladders[i].headRow * boardCols + ladders[i].headCol); // move the player up to that position
				players[activePlayer].row = ladders[i].headRow;
				players[activePlayer].col = ladders[i].headCol;
				printf("Player %d landed at the base of a ladder! They moved to position %d\n", (activePlayer + 1), players[activePlayer].pos);
				break; // don't need to look anymore
			} // look through ladders
		} // for
	} // if ladder

	else if (gameBoard[players[activePlayer].row][players[activePlayer].col] == 'S') {
		// head of snake
		for (i = 0; i < numSnakesAndLadders; i++) {
			if ((snakes[i].headRow * boardRows + snakes[i].headCol) == toPos) { // if player landed on head of a snake
				players[activePlayer].pos = (snakes[i].tailRow * boardCols + snakes[i].tailCol); // move player to the tail position on the snake
				players[activePlayer].row = snakes[i].tailRow;
				players[activePlayer].col = snakes[i].tailCol;

				printf("Player %d landed on the head of a snake! They moved to position %d\n", (activePlayer + 1), players[activePlayer].pos);
				break; // no need to search further


			} // if landed on snake head
		} // search through snakes
	}
	else {
		// normal square, nothing special! - check for victory
		if (players[activePlayer].pos >= boardRows * boardCols) {

			printf("Player %d has won the game!\n", (activePlayer + 1));

			do {
				printf("Would you like to start a new game? (Y/N): ");
				scanf(" %c", &choice);
			} while (choice != 'n'
				&& choice != 'N'
				&& choice != 'y'
				&& choice != 'Y');

			switch (choice) {
			case 'n':
			case 'N': printf("Thank you for playing!\n");
				choice = 'x';
				break;
			case 'y':
			case 'Y':startNewGame(choice);
				break;
			default:
				break;
			}
		}
	}

	// other wise assign the next player
	if (activePlayer == (numPlayers - 1)) { // if it's the last player
		activePlayer = 0; // back to player 1
	}
	else {
		activePlayer++; // next player's turn
	}

}
