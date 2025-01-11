/*
    Author: Franklin
    Date created: 10 January 2024
    Purpose: To open chart and play games with it to test effectiveness
*/

// imports
#include <iostream>
#include <fstream>
#include <string>
#include "BlackJack.h"
#include "BlackJackAgent.h"

// namespace
using std::cout, std::endl;
using std::string;
using std::getline;
using std::stoi;
using std::ifstream, std::ofstream;

// constants

// number of rounds to play with resetting balance each
const int ROUND_COUNT = 5000;

// number of games per round
const int GAME_COUNT = 1000;

// starting balance for each round
const double STARTING_BAL = 1000.0;

// amount to be for each game
const double BET = 0.50;

// Game parameters
const int DECK_COUNT = 4;
const int SHUFFLE_EVERY_N_DECKS = 2;

// how the payouts are calculated
// user pays bet, and game returns bet + bet * PAYOUT
const Scoring PAYOUTS = {

    1.5,  // blackjack;
    2,    // doubleWin;
    1,    // win;
    -1,   // loss;
    -2,   // doubleLoss;
    0     // push;

};

// main
int main(int argc, char* argv[]) {

    // chart name
    const string CHART_ID = argv[1];
    const string CHART_PATH = "Chart" + CHART_ID + "/Chart" + CHART_ID + "_readable.csv";

    // save info to this filename
    const string SAVE_PATH = "Chart" + CHART_ID + "/Eval" + CHART_ID + ".txt";

    // announce
    cout << "Evaluating chart..." << endl;

    // create dealer
    Dealer* dealer = new Dealer(DECK_COUNT, SHUFFLE_EVERY_N_DECKS);

    // create game
    Game* game = new Game(dealer, PAYOUTS);

    // chart
    int chart[PLAYER_HAND_COUNT][DEALER_HAND_COUNT];

    // results
    double sum = 0;

    // open file stream to populate chart
    ifstream infile(CHART_PATH);
    string readIn;

    // game round variables
    double bal;

    // read in header
    getline(infile, readIn);

    // iterate through player hands
    for (int i = 0; i < PLAYER_HAND_COUNT; i ++) {

        // read in row label
        getline(infile, readIn, ',');

        // read in all dealer hands except last
        for (int j = 0; j < DEALER_HAND_COUNT - 1; j ++) {

            // put action in chart
            getline(infile, readIn, ',');
            chart[i][j] = stoi(readIn);

        }

        // read in last \n delimited item
        getline(infile, readIn);
        chart[i][DEALER_HAND_COUNT - 1] = stoi(readIn);

    }

    // game variables
    bool gameOver;
    ActionType agentMove;
    pair<int, int> stateCoords;
    SplitInfo split;
    vector<SplitInfo> splits;

    // iter through rounds
    for (int round = 0; round < ROUND_COUNT; round ++) {

        // set new balance
        bal = STARTING_BAL;

        // reshuffle deck
        dealer->reshuffle();

        // start games
        for (int gameNum = 0; gameNum < GAME_COUNT; gameNum ++) {

            // take bet
            bal -= BET;

            // deal game
            gameOver = game->dealHands();

            // set default agent move for iteration
            agentMove = HIT;

            // while the game isn't over and player isn't done making moves
            while (!gameOver && (agentMove == HIT || agentMove == SPLIT)) {

                // get player moves
                stateCoords = getTableIndex(game->getState());
                agentMove = static_cast<ActionType>(chart[stateCoords.first][stateCoords.second]);

                // carry out agent move
                switch (agentMove) {
                    
                    // agent hit
                    case HIT:

                        // hit in game
                        gameOver = game->hit();
                        break;

                    case SPLIT:

                        // add split info to split list to play other side of game
                        split = {
                            game->getState().playerCards.at(0),
                            game->getState().dealerShowing,
                            game->getDealerSecondCard()
                        };
                        splits.push_back(split);

                        // setup this half of the game
                        game->runSplit();

                        break;

                    // agent double 
                    case DOUBLE:
                        //cout << "double" << endl;

                        // double bet in game
                        game->doubleBet();

                        // take hit
                        gameOver = game->hit();
                        break;

                    // agent stand
                    case STAND:

                        break;

                }

            }

            // player dealer turn if game isn't over
            if (!gameOver) {

                game->playDealer();

            }

            // update balance
            bal += BET + game->getScore() * BET;

            // reset game
            game->reset();
            
            // run other branches of split game
            while (splits.size() > 0) {

                // play split game

                // fetch split data
                split = splits.back();
                splits.pop_back();

                // deal game
                game->setupSplit(split.playerCard, split.dealerCard1, split.dealerCard2);

                // deal player second card
                game->hit();

                // set default agent move for iteration
                agentMove = HIT;

                // while the game isn't over and player isn't done making moves
                while (!gameOver && (agentMove == HIT || agentMove == SPLIT)) {

                    // get player moves
                    stateCoords = getTableIndex(game->getState());
                    agentMove = static_cast<ActionType>(chart[stateCoords.first][stateCoords.second]);

                    // carry out agent move
                    switch (agentMove) {
                        
                        // agent hit
                        case HIT:
                            
                            // hit in game
                            gameOver = game->hit();
                            break;

                        case SPLIT:

                            // add split info to split list
                            split = {
                                game->getState().playerCards.at(0),
                                game->getState().dealerShowing,
                                game->getDealerSecondCard()
                            };
                            splits.push_back(split);

                            // setup first half of game
                            game->runSplit();

                            break;

                        // agent double 
                        case DOUBLE:

                            // double bet in game
                            game->doubleBet();

                            // take hit
                            gameOver = game->hit();
                            break;

                        // agent stand
                        case STAND:
                            break;

                    }

                }

                // player dealer turn if game isn't over
                if (!gameOver) {

                    game->playDealer();

                }

                // update balance
                bal += BET + game->getScore() * BET;

                // reset game
                game->reset();

            }

        }

        // round information
        sum += bal;

    }

    // save eval info to file
    ofstream outfile(SAVE_PATH);

    // write out all eval params
    outfile << "Evaluation for Chart" << CHART_ID << endl << endl;
    outfile << "Gambling rounds: " << ROUND_COUNT << endl;
    outfile << "Games per round: " <<  GAME_COUNT << endl;
    outfile << "Round starting balance: $" << STARTING_BAL << endl;
    outfile << "Bet per game: $" << BET << endl;
    outfile << "Deck count: " << DECK_COUNT << endl;
    outfile << "Decks dealt before reshuffle: " << SHUFFLE_EVERY_N_DECKS << endl;
    outfile << "Results:" << endl;
    outfile << "\tAverage final balance: $" << sum / ROUND_COUNT << endl;
    outfile << "\tAverage balance increase: $" << ((sum / ROUND_COUNT) - STARTING_BAL) << " | " << ((sum / ROUND_COUNT) - STARTING_BAL) / STARTING_BAL * 100 << "%" << endl;

    // release memory
    delete dealer;
    delete game;

    cout << "Evaluation complete." << endl;

    return 0;
}