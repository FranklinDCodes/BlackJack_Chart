/*
    Author: Franklin Doane
    Date Created: 30 December 2024
    Purpose: generate a blackjack chard using the q learning agent
*/

// imports
#include "BlackJack.h"
#include "BlackJackAgent.h"
#include <iostream>
#include <cmath>

// namespace
using std::cout, std::endl;
using std::exp;

// CONSTANT TRAINING PARAMETERS
const auto EPSILON = [](int x) {return 1 / (1 + exp((1/1.2e4)*x - 3));};
const float GAMMA = 1.0;
const float ALPHA = 1e-3;
const int GAME_COUNT = 150000;
const int TRAIN_EVERY = 100;

// CONSTANT BLACKJACK GAME PARAMETERS
const int DECK_COUNT = 4;
const int SHUFFLE_EVERY_N_DECKS = 2;
const Scoring SCORES = {

    1.5,  // blackjack;
    2,    // doubleWin;
    1,    // win;
    -1,   // loss;
    -2,   // doubleLoss;
    0     // push;

};

// print state function
void printTable(Hands table, int dealer2nd) {

    cout << "Dealer cards: " << endl;
    cout << "\t" << table.dealerShowing << endl;
    cout << "\t" << dealer2nd << endl;
    cout << "Player cards: " << endl;
    for (unsigned int i = 0; i < table.playerCards.size(); i ++) {

        cout << "\t" << table.playerCards.at(i) << endl;

    }

}

// struct to hold info on splits
struct SplitInfo {

    // cards for game
    int playerCard;
    int dealerCard1;
    int dealerCard2;

    // action history for agent
    vector<Action> actionHistory;

};

// main
int main () {

    // blackjack dealer
    Dealer* dealer = new Dealer;

    // blackjack game
    Game* game = new Game(dealer, SCORES);

    // initialize q learning agent
    BlackJackAgent* agent = new BlackJackAgent(EPSILON, GAMMA, ALPHA);

    // game status
    bool gameOver;

    // player move
    ActionType agentMove;

    // alternate game paths as held for split
    vector<SplitInfo> splits;
    SplitInfo split;

    // iterate through games
    for (int gameNum = 0; gameNum < GAME_COUNT; gameNum ++) {

        // deal game
        gameOver = game->dealHands();

        // set default agent move for iteration
        agentMove = HIT;

        // while the game isn't over and player isn't done making moves
        while (!gameOver && (agentMove == HIT || agentMove == SPLIT)) {

            // get player moves
            agentMove = agent->makeMove(game->getState());

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
                        game->getDealerSecondCard(),
                        agent->getGameActions()
                    };
                    splits.push_back(split);

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

        // set reward for game results
        agent->endGame(game->getScore());

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

            // set game history
            agent->setGameActions(split.actionHistory);

            // set default agent move for iteration
            agentMove = HIT;

            // while the game isn't over and player isn't done making moves
            while (!gameOver && (agentMove == HIT || agentMove == SPLIT)) {

                // get player moves
                agentMove = agent->makeMove(game->getState());

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
                            game->getDealerSecondCard(),
                            agent->getGameActions()
                        };
                        splits.push_back(split);

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

            // set reward for game results
            agent->endGame(game->getScore());

            // reset game
            game->reset();

        }

        // check if time to train
        if (gameNum % TRAIN_EVERY == 0) {

            // update q tables
            agent->train();
        }

    }

    // release dealer and agent
    delete dealer;
    delete game;
    delete agent;

    return 0;
}



