/*
    Author: Franklin Doane
    Date created: 29 December 2024
    Purpose: Header file for q learning agent
*/

// file guards
#ifndef AGENT_H
#define AGENT_H

// imports
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <functional>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include "BlackJack.h"

// namespace
using std::string;
using std::ofstream, std::ifstream;
using std::vector, std::pair;
using std::function;
using std::srand, std::time;
using std::max_element;

// hand possibility count
const int PLAYER_HAND_COUNT = 36;
const int DEALER_HAND_COUNT = 10;

// list of all the possible player hands
// labels for the rows on the q table
const string PLAYER_HANDS[PLAYER_HAND_COUNT] = {

    // sums
    "5",
    "6",
    "7",
    "8",
    "9",
    "10",
    "11",
    "12",
    "13",
    "14",
    "15",
    "16",
    "17",
    "18",
    "19",
    "20",
    "21",

    // Ace section
    "A-2",
    "A-3",
    "A-4",
    "A-5",
    "A-6",
    "A-7",
    "A-8",
    "A-9",
    "A-10",

    // Pairs section
    "2-2",
    "3-3",
    "4-4",
    "5-5",
    "6-6",
    "7-7",
    "8-8",
    "9-9",
    "10-10",
    "A-A"

};

// constants for mapping out Q table
const int TABLE_FIRST_ACE_INDEX = 17;
const int TABLE_FIRST_PAIR_INDEX = 26;

// list of all the possible dealer hands (1 card showing)
// labels for columns on the q table
const string DEALER_HANDS[DEALER_HAND_COUNT] = {

    "A",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "10"

};

// action options
const int ACTION_TYPE_COUNT = 4;
enum ActionType {

    // possible actions
    STAND,
    HIT,
    DOUBLE,
    SPLIT

};

// action for history
struct Action {

    // state before action was taken
    Hands state;

    // action type taken
    ActionType type;

};

// converts a state struct to an index pair for q table
// return format is pair<row/player_hand, col/dealer_hand>
pair<int, int> getTableIndex(Hands state) {

    // return index pair
    pair<int, int> indexPair;

    // find player hand

    // check for pair
    if (state.playerCards.size() == 2 && state.playerCards.at(0) == state.playerCards.at(1)) {

        // set row index
        // pair number - 2 (takes lowest from 2 to 0) + first_pair_index
        indexPair.first = state.playerCards.at(0) - 2 + TABLE_FIRST_PAIR_INDEX;
    
    }
    // check for ace that can still be used (sum is 11 or less so changing 1 to 11 wont bust)
    else if (state.playerAces > 0 && state.playerSum <= 11) {

        // set row index
        // player sum - 2 (takes lowest from 2 to 0) - 1 (removes ace) + first_ace_index
        indexPair.first = state.playerSum - 2 - 1 + TABLE_FIRST_ACE_INDEX;

    }
    // else treat it as sum
    else {

        // set row index
        // player sum - 5 (takes lowest from 5 to 0)
        indexPair.first = state.playerSum - 5;

    }

    // find dealer hand

    // dealer sum - 1 (takes lowest from 1 (ace) to 0)
    indexPair.second = state.dealerShowing - 1;

    return indexPair;


}

// returns true if a state can split
bool canSplit(Hands state) {

    return state.playerCards.size() == 2 && state.playerCards.at(0) == state.playerCards.at(1);
}




// figure out splits



// agent class
class BlackJackAgent {

    private:

        // list of actions taken
        vector<Action> gameActions;

        // training example list
        // it goes list[ game[gameActions[action], reward] ]
        vector<pair<vector<Action>, double>> trainingExamples;

        // epsilon and gamma parameters
        function<double(int)> E_FUNC;
        double G;
        double A;

        // Q table
        // table[player hand][dealer hand][action]
        // action dimension indexes correspond to enum int values
        double qTable[PLAYER_HAND_COUNT][DEALER_HAND_COUNT][ACTION_TYPE_COUNT];

        // training examples per q value
        int trainingCounts[PLAYER_HAND_COUNT][DEALER_HAND_COUNT][ACTION_TYPE_COUNT];

        // total number of examples used
        int trainingCountTotal;

    public:

        // default constructor
        // constructor
        BlackJackAgent() {
            
            // set default parameters
            this->E_FUNC = [](int x) { return 0.0; };
            this->G = -1;
            this->A = 0;

            // set count
            this->trainingCountTotal= 0;

            // populate table values and counts
            for (int i = 0; i < PLAYER_HAND_COUNT; i ++) {
                for (int j = 0; j < DEALER_HAND_COUNT; j ++) {
                    for (int k = 0; k < ACTION_TYPE_COUNT; k ++) {

                        // set q table
                        this->qTable[i][j][k] = 0;

                        // set training example counts
                        this->trainingCounts[i][j][k] = 0;

                    }
                }
            }

        }

        // constructor
        BlackJackAgent(function<double(int)> epsilon, double gamma, double alpha) {
            
            // set parameters
            this->E_FUNC = epsilon;
            this->G = gamma;
            this->A = alpha;

            // set count
            this->trainingCountTotal= 0;

            // populate table values and counts
            for (int i = 0; i < PLAYER_HAND_COUNT; i ++) {
                for (int j = 0; j < DEALER_HAND_COUNT; j ++) {
                    for (int k = 0; k < ACTION_TYPE_COUNT; k ++) {

                        // set q table
                        this->qTable[i][j][k] = 0;

                        // set training example counts
                        this->trainingCounts[i][j][k] = 0;

                    }
                }
            }

            // seed random function
            srand(time(0));

        }

        // get agent choice
        ActionType makeMove(Hands state) {

            // get q table coordinates
            pair<int, int> coords = getTableIndex(state);

            // check if splitting is an option
            bool splitPossible = canSplit(state);

            // define action ratings array size based on possible options
            // removing split as last option
            int actionsPossible = splitPossible ? ACTION_TYPE_COUNT : ACTION_TYPE_COUNT - 1;
            double* actionRatings = new double[actionsPossible];

            // copy q values to action ratings
            for (int i = 0; i < actionsPossible; i ++) {

                actionRatings[i] = this->qTable[coords.first][coords.second][i];

            }

            // calculate epsilon
            double epsilon = this->E_FUNC(this->trainingCountTotal);

            // get random number between 0-1
            double random = (static_cast<double>(rand()) / RAND_MAX);

            // action details
            ActionType actionChosen;
            double reward;

            // info on max q in action state
            double* maxQPtr = max_element(actionRatings, actionRatings + actionsPossible);
            int maxQIndex = maxQPtr - actionRatings;
            double maxQValue = actionRatings[maxQIndex];

            // explore
            if (random < epsilon) {

                // pick random option 
                actionChosen = static_cast<ActionType>(rand() % actionsPossible);

            }
            // educated guess 
            else {

                actionChosen = static_cast<ActionType>(maxQIndex);

            }

            // delete possible action ratings
            delete [] actionRatings;

            // create new move object
            // set reward to 0 to be replaced later when needed
            Action action = {
                state,
                actionChosen
            };

            // add action to history
            this->gameActions.push_back(action);

            // return action type
            return actionChosen;

        }

        // add game data to training data
        void endGame(double reward) {

            // make sure there are game actions
            if (this->gameActions.size() == 0) {
                return;
            }

            // game value
            pair<vector<Action>, double> gameValue = pair<vector<Action>, double>(this->gameActions, reward);

            // add all examples to training eg
            this->trainingExamples.push_back(gameValue);

            // empties game action vector
            vector<Action>().swap(this->gameActions);

        }

        // train on training examples accumulated
        void train() {

            // list of game actions
            vector<Action> gameActions;

            // game reward 
            double gameReward;

            // set highest q value for next state
            double nextHighestQ;

            // current iteration in actions
            Action action;

            // state coordinates
            pair<int, int> stateIndices;
            
            // highest q value befor they were updated
            double preUpdateHighestQ;

            // current q value for iterating
            double currentQValue;

            // iterate through games
            for (int i = 0; i < this->trainingExamples.size(); i ++) {

                // get list of game actions
                gameActions = this->trainingExamples.at(i).first;

                // get game reward 
                gameReward = this->trainingExamples.at(i).second;

                // iterate through game actions backward
                for (int j = gameActions.size() - 1; j > 0; i --) {
                    
                    // get current action
                    action = gameActions.at(j);

                    // get move coordinates
                    stateIndices = getTableIndex(action.state);

                    // get highest q value before update
                    preUpdateHighestQ = *max_element(this->qTable[stateIndices.first][stateIndices.second], this->qTable[stateIndices.first][stateIndices.second] + ACTION_TYPE_COUNT);

                    // as long as this isn't the last one
                    if (j != this->gameActions.size() - 1) {

                        // get current q value
                        currentQValue = this->qTable[stateIndices.first][stateIndices.second][action.type];

                        // update
                        // Q = Q + A(currentReward + G(nextReward) - Q)
                        this->qTable[stateIndices.first][stateIndices.second][action.type] = currentQValue + this->A * (0 + this->G * nextHighestQ - currentQValue);

                    }
                    // if it's the last one
                    else {

                        // get current q value
                        currentQValue = this->qTable[stateIndices.first][stateIndices.second][action.type];

                        // update
                        // Q = Q + A(gameReward - Q)
                        this->qTable[stateIndices.first][stateIndices.second][action.type] = currentQValue + this->A * (gameReward  - currentQValue);

                    }

                    // update q values update
                    this->trainingCounts[stateIndices.first][stateIndices.second][action.type] ++;
                    this->trainingCountTotal ++;

                    // set next highest q value
                    nextHighestQ = preUpdateHighestQ;

                }

            }

        }

};

#endif
