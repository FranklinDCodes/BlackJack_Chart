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
#include <fstream>
#include <limits>
#include <iomanip>

// namespace
using std::cout, std::endl;
using std::exp;
using std::numeric_limits;
using std::setw, std::fixed;

// CONSTANT TRAINING PARAMETERS
const double E_COEFFICIENT = (1/1.5e5);
const double E_RIGHT_SHIFT = 4;
const auto EPSILON = [](int x) -> double {return (1 / (1 + exp(E_COEFFICIENT*x - E_RIGHT_SHIFT)));};
const float GAMMA = 1.0;
const float ALPHA = 1e-3;
const int GAME_COUNT = 3e6;
const int TRAIN_EVERY = 2000;

// note of what makes this chart unique
const string CHART_NOTE = "3 mil games, larger training interval.";

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

// main
int main(int argc, char* argv[]) {

    // chart names
    const string CHART_ID = argv[1];
    const string CHART_NAME = "Chart" + CHART_ID + ".csv";

    // chart that can be used by computer
    const string USABLE_CHART_NAME = "Chart" + CHART_ID + "_readable.csv";

    // chart saying training examples for each max q action
    const string MAX_TRAINING_CHART_NAME = "Chart" + CHART_ID + "_backing.csv";

    // chart saying all q values
    const string Q_CHART_NAME = "Chart" + CHART_ID + "_Q.csv";

    // chart saying all training examples
    const string TRAINING_CHART_NAME = "Chart" + CHART_ID + "_wholeBacking.csv";

    // text file containing all the parameters of creation
    const string PARAM_FILE_NAME = CHART_ID + "_parameters.txt";

    // blackjack dealer
    Dealer* dealer = new Dealer(DECK_COUNT, SHUFFLE_EVERY_N_DECKS);

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
    cout << "Beginning training..." << endl;
    for (int gameNum = 0; gameNum < GAME_COUNT; gameNum ++) {

        // deal game
        gameOver = game->dealHands();

        // set default agent move for iteration
        agentMove = HIT;

        // while the game isn't over and player isn't done making moves
        while (!gameOver && (agentMove == HIT || agentMove == SPLIT)) {

            //cout << "In game" << endl;

            // get player moves
            agentMove = agent->makeMove(game->getState());

            //cout << "agent move made" << endl;

            // carry out agent move
            switch (agentMove) {
                
                // agent hit
                case HIT:
                    //cout << "hit" << endl;

                    // hit in game
                    gameOver = game->hit();
                    break;

                case SPLIT:
                    //cout << "split" << endl;

                    // add split info to split list to play other side of game
                    split = {
                        game->getState().playerCards.at(0),
                        game->getState().dealerShowing,
                        game->getDealerSecondCard(),
                        agent->getGameActions()
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
                    //cout << "stand" << endl;

                    break;

            }

        }

        //cout << "player played" << endl;

        // player dealer turn if game isn't over
        if (!gameOver) {

            game->playDealer();

        }
        //cout << "dealer played" << endl;

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

            // deal player second card
            game->hit();

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

            // set reward for game results
            agent->endGame(game->getScore());

            // reset game
            game->reset();

        }
        //cout << "splits played" << endl;

        // check if time to train
        if (gameNum % TRAIN_EVERY == 0) {

            // update q tables
            agent->train();
            cout << "Trained through games " << gameNum << endl << endl;


        }

    }

    cout << "Training complete." << endl << endl;

    // release dealer and game
    delete dealer;
    delete game;


    // build chart out of agent

    // open chart
    ofstream outfile(CHART_NAME);

    // write top left corner
    outfile << ",";

    // write header
    for (int i = 0; i < DEALER_HAND_COUNT; i ++) {

        outfile << DEALER_HANDS[i] << ",";

    }
    outfile << endl;

    // max q index finders
    int maxQ;

    // iterate through possible hand combos
    for (int i = 0; i < PLAYER_HAND_COUNT; i ++) {

        // print player hand
        outfile << PLAYER_HANDS[i] << ",";

        for (int j = 0; j < DEALER_HAND_COUNT; j ++) {

            // check if can split
            if (!handIdxCanSplit(i)) {

                // set split value to min if not possible
                agent->getQTable()[i][j][SPLIT] = numeric_limits<double>::min();

            }

            // get highest q value index
            maxQ = max_element(agent->getQTable()[i][j], agent->getQTable()[i][j] + ACTION_TYPE_COUNT) - agent->getQTable()[i][j];

            // check for max double
            if (maxQ == DOUBLE) {

                // if stand is a higher second option than hit...
                if (agent->getQTable()[i][j][STAND] > agent->getQTable()[i][j][HIT]) {

                    outfile << ACTION_NAMES[DOUBLE_STAND] << ",";

                }
                else {

                    outfile << ACTION_NAMES[DOUBLE_HIT] << ",";

                }

            }
            // else add to chart
            else {

                // print action
                outfile << ACTION_NAMES[maxQ] << ",";

            }

        }

        outfile << endl;
    }
    outfile.close();


    // open chart that is usable
    outfile.open(USABLE_CHART_NAME);

    // write top left corner
    outfile << ",";

    // write header
    for (int i = 0; i < DEALER_HAND_COUNT; i ++) {

        outfile << DEALER_HANDS[i] << ",";

    }
    outfile << endl;

    // iterate through possible hand combos
    for (int i = 0; i < PLAYER_HAND_COUNT; i ++) {

        // print player hand
        outfile << PLAYER_HANDS[i] << ",";

        for (int j = 0; j < DEALER_HAND_COUNT; j ++) {

            
            // check if can split
            if (!handIdxCanSplit(i)) {

                // set split value to min if not possible
                agent->getQTable()[i][j][SPLIT] = numeric_limits<int>::min();

            }

            // get highest q value index
            maxQ = max_element(agent->getQTable()[i][j], agent->getQTable()[i][j] + ACTION_TYPE_COUNT) - agent->getQTable()[i][j];

            // check for max double
            if (maxQ == DOUBLE) {

                // if stand is a higher second option than hit...
                if (agent->getQTable()[i][j][STAND] > agent->getQTable()[i][j][HIT]) {

                    outfile << DOUBLE_STAND << ",";

                }
                else {

                    outfile << DOUBLE_HIT << ",";

                }

            }
            // else add to chart
            else {

                // print action
                outfile << maxQ << ",";

            }

        }

        outfile << endl;
    }
    outfile.close();


    // print training counts
    outfile.open(MAX_TRAINING_CHART_NAME);

    // write top left corner
    outfile << ",";

    // write header
    for (int i = 0; i < DEALER_HAND_COUNT; i ++) {

        outfile << DEALER_HANDS[i] << ",";

    }
    outfile << endl;

    // iterate through possible hand combos
    for (int i = 0; i < PLAYER_HAND_COUNT; i ++) {

        // print player hand
        outfile << PLAYER_HANDS[i] << ",";

        for (int j = 0; j < DEALER_HAND_COUNT; j ++) {

            // get highest q value index
            maxQ = max_element(agent->getQTable()[i][j], agent->getQTable()[i][j] + ACTION_TYPE_COUNT) - agent->getQTable()[i][j];

            // print action
            outfile << agent->getQTableCounts()[i][j][maxQ] << ",";

        }

        outfile << endl;
    }
    outfile.close();


    // print q values
    outfile.open(Q_CHART_NAME);

    // write top left corner
    outfile << ",";

    // write header
    for (int i = 0; i < DEALER_HAND_COUNT; i ++) {

        outfile << DEALER_HANDS[i] << ",";

    }
    outfile << endl;

    // iterate through possible hand combos
    for (int i = 0; i < PLAYER_HAND_COUNT; i ++) {

        // print player hand
        outfile << PLAYER_HANDS[i] << ",";

        for (int j = 0; j < DEALER_HAND_COUNT; j ++) {

            // print q values
            outfile << "[" << setw(4) << fixed;

            for (int k = 0; k < ACTION_TYPE_COUNT - 1; k ++) {

                // print action if not numerical min
                if (agent->getQTable()[i][j][k] == numeric_limits<int>::min()) {

                    outfile << " _  ";

                }
                else {

                    outfile << agent->getQTable()[i][j][k] << " ";

                }

            }

            // print action if not numerical min
            if (agent->getQTable()[i][j][ACTION_TYPE_COUNT - 1] == numeric_limits<int>::min()) {

                outfile << " _ ";

            }
            else {

                outfile << agent->getQTable()[i][j][ACTION_TYPE_COUNT - 1];

            }

            outfile << "],";

        }

        outfile << endl;
    }
    outfile.close();


    // print all training counts
    outfile.open(TRAINING_CHART_NAME);

    // write top left corner
    outfile << ",";

    // write header
    for (int i = 0; i < DEALER_HAND_COUNT; i ++) {

        outfile << DEALER_HANDS[i] << ",";

    }
    outfile << endl;

    // iterate through possible hand combos
    for (int i = 0; i < PLAYER_HAND_COUNT; i ++) {

        // print player hand
        outfile << PLAYER_HANDS[i] << ",";

        for (int j = 0; j < DEALER_HAND_COUNT; j ++) {

            // print q values
            outfile << "[";

            for (int k = 0; k < ACTION_TYPE_COUNT - 1; k ++) {

                // print action
                outfile << agent->getQTableCounts()[i][j][k] << " ";

            }

            outfile << agent->getQTableCounts()[i][j][ACTION_TYPE_COUNT - 1];

            outfile << "],";

        }

        outfile << endl;
    }
    outfile.close();


    // print all training counts
    outfile.open(PARAM_FILE_NAME);

    // write epsilon function parameter
    outfile << "Training parameters for chart #" << CHART_ID << endl;
    outfile << "\tChart note: " << CHART_NOTE << endl << endl;
    outfile << "Epsilon Function:" << endl;
    outfile << "\ttype: sigmoid" << endl;
    outfile << "\tx coefficient: " << E_COEFFICIENT << endl;
    outfile << "\tx right shift: " << E_RIGHT_SHIFT << endl;
    outfile << "Gamma: " << GAMMA << endl;
    outfile << "Alpha: " << ALPHA << endl;
    outfile << "Game count: " << GAME_COUNT << endl;
    outfile << "Training interval: " << TRAIN_EVERY << " games" << endl;
    outfile << "Deck count: " << DECK_COUNT << endl;
    outfile << "Reshuffle interval: " << SHUFFLE_EVERY_N_DECKS << " decks" << endl;
    outfile << "Rewards:" << endl;
    outfile << "\tWin: " << SCORES.win << endl;
    outfile << "\tBlackjack: " << SCORES.blackjack << endl;
    outfile << "\tDouble: " << SCORES.doubleWin << endl;
    outfile << "\tLoss: " << SCORES.loss << endl;
    outfile << "\tDouble loss: " << SCORES.doubleLoss << endl;
    outfile << "\tPush: " << SCORES.push << endl;
    
    outfile.close();


    // cleanup
    delete agent;

    return 0;
}
