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
int main () {

    // blackjack dealer
    Dealer* dealer = new Dealer;

    // blackjack game
    Game game(dealer, SCORES);

    game.dealHands();

    // print hands
    printTable(game.getState(), game.getDealerSecondCard());
    cout << endl;

    // game stats
    cout << "Game is over: " << game.getGameOver() << endl;
    cout << "Player won: " << game.getPlayerWon() << endl;
    cout << "Player score: " << game.getScore() << endl;
    cout << "Game pushed: " << game.getWasPush() << endl;
    cout << endl;

    // hit once
    game.hit();

    // print hands
    printTable(game.getState(), game.getDealerSecondCard());
    cout << endl;

    // game stats
    cout << "Game is over: " << game.getGameOver() << endl;
    cout << "Player won: " << game.getPlayerWon() << endl;
    cout << "Player score: " << game.getScore() << endl;
    cout << "Game pushed: " << game.getWasPush() << endl;
    cout << endl;

    // play out dealer hand
    game.playDealer();

    // game stats
    cout << "Game is over: " << game.getGameOver() << endl;
    cout << "Player won: " << game.getPlayerWon() << endl;
    cout << "Player score: " << game.getScore() << endl;
    cout << "Game pushed: " << game.getWasPush() << endl;

    // release dealer
    delete dealer;

    return 0;
}


// make sure not to let player hit on 21



