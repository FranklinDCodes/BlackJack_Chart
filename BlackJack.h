/*
    Author: Franklin Doane
    Date created: 29 December 2024
    Purpose: blackjack game classes
*/

// file guards
#ifndef BLACKJACK_H
#define BLACKJACK_H

// imports
#include <vector>
#include <ctime>
#include <random>
#include <algorithm>

#include <iostream>
using std::cout, std::endl;

// namespaces
using std::vector;
using std::time;
using std::mt19937;
using std::shuffle;

// constants
const int CARDS_PER_DECK = 52;
const int CARD_TYPE_COUNT = 13;
const int CARD_SUITS = 4;
const int CARD_TYPES[CARD_TYPE_COUNT] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10};
const int DEALER_STAND = 17;

// scoring struct
struct Scoring {

    // scores
    float blackjack;
    float doubleWin;
    float win;
    float loss;
    float doubleLoss;
    float push;

};

// hand struct
struct Hands {
    // all cards are by numeric value, aces are 1s

    // card showing to dealer
    int dealerShowing;

    // player cards
    vector<int> playerCards;

    // number of aces player has
    int playerAces;

    // sum of player cards with all aces as 1
    int playerSum;

};

// class to handle card dealing
class Dealer {

    private:

        // number of full decks dealer has
        int fullDeckCount;

        // number of decks to go through before shuffling
        int decksBeforeShuffle;

        // deck cards remaining
        vector<int> deck;

        // number of cards delt
        int cardDeltCount;

        // randomizer for shuffle
        mt19937 randomizer;

    public:

        // default constructor
        Dealer() {

            // set decks
            this->fullDeckCount = 1;
            this->decksBeforeShuffle = 1;

            // start cards delt at 0
            this->cardDeltCount = 0;

            // seed random function
            this->randomizer = mt19937(time(0));

            // setup deck
            this->reshuffle();

        }

        // constructor
        Dealer(int deckCount, int beforeShuffle) {

            // set decks
            this->fullDeckCount = deckCount;
            this->decksBeforeShuffle = beforeShuffle;

            // start cards delt at 0
            this->cardDeltCount = 0;

            // seed random function
            this->randomizer = mt19937(time(0));

            // setup deck
            this->reshuffle();

        }

        // regathers and shuffles the deck
        void reshuffle() {

            // empty out deck
            vector<int>().swap(this->deck);
            
            // populate deck
            // iterate through card types 
            for (int i = 0; i < CARD_TYPE_COUNT; i ++) {

                // iterate through individual cards
                // for 4 suits in each deck
                for (int j = 0; j < CARD_SUITS * this->fullDeckCount; j ++) {
                    
                    // add card to deck
                    deck.push_back(CARD_TYPES[i]);

                }

            }

            // run shuffle
            shuffle(this->deck.begin(), this->deck.end(), this->randomizer);

            // start cards delt at 0
            this->cardDeltCount = 0;

        }

        // pops a card
        int deal() {
            
            // grab card
            int card = this->deck.back();
            
            // deal card
            this->deck.pop_back();
            this->cardDeltCount ++;

            // check for reshuffle if cards delt is deck amount
            if (cardDeltCount >= decksBeforeShuffle * CARDS_PER_DECK) {

                // reset deck
                this->reshuffle();

            }

            return card;

        }

};

// class to handle game
class Game {

    private:

        // card dealer
        Dealer* dealer;

        // current game state
        Hands table;
        int dealerSecondCard;

        // player doubled
        bool doubleGame;

        // game is over
        bool gameOver;

        // play was the winner
        bool playerWon;

        // push ending
        bool wasPush;

        // doubled game if true
        float score;

        // scores
        Scoring scoreAmounts;

    public:

        // default constructor
        Game() {
            
            // default construct deck
            this->dealer = nullptr;

            // set hands to empty
            this->table = {0, vector<int>(), 0, 0};
            this->dealerSecondCard = 0;

            // set bet
            this->doubleGame = false;

            // set game conclusion bools to false
            this->gameOver = false;
            this->playerWon = false;
            this->wasPush = false;
            this->score = 0;

            // set scores
            this->scoreAmounts = {0, 0, 0, 0, 0, 0};

        }

        // preferred constructor
        Game(Dealer* dealer, Scoring scoreAmounts) {

            // init dealer with args
            this->dealer = dealer;

            // set hands to empty
            this->table = {0, vector<int>(), 0, 0};
            this->dealerSecondCard = 0;

            // set bet
            this->doubleGame = false;

            // set game conclusion bools to false
            this->gameOver = false;
            this->playerWon = false;
            this->wasPush = false;
            this->score = 0;

            // set scores
            this->scoreAmounts = scoreAmounts;
        }

        // split setup
        void setupSplit(int playerCard, int dealerCard1, int dealerCard2) {

            // set hands to have cards from split game
            this->table = {dealerCard1, vector<int>({playerCard}), (playerCard == 1) ? 1 : 0, playerCard};
            this->dealerSecondCard = dealerCard2;

            // set bet
            this->doubleGame = false;

            // set game conclusion bools to false
            this->gameOver = false;
            this->playerWon = false;
            this->wasPush = false;
            this->score = 0;

        }

        // deal out both hands
        // returns game over flag
        bool dealHands() {

            // deal all cards
            // add to sums
            // check for aces

            // check for 1 card (from split)
            if (this->table.playerCards.size() == 0) {

                this->table.playerCards.push_back(this->dealer->deal());
                this->table.playerSum += this->table.playerCards.at(0);

                if (this->table.playerCards.at(0) == 1) {
                    this->table.playerAces ++;
                }
            }

            this->table.playerCards.push_back(this->dealer->deal());
            this->table.playerSum += this->table.playerCards.at(1);
            if (this->table.playerCards.at(1) == 1) {
                this->table.playerAces ++;
            }

            this->table.dealerShowing = this->dealer->deal();
            
            this->dealerSecondCard = this->dealer->deal();

            // check for blackjacks
            bool player21 = this->table.playerSum == 11;
            bool dealer21 = (this->table.dealerShowing == 10 && this->dealerSecondCard == 1) || (this->table.dealerShowing == 1 && this->dealerSecondCard == 10);

            // check for player blackjack
            if (player21 && !dealer21) {

                // set win
                this->gameOver = true;
                this->playerWon = true;
                this->score = this->scoreAmounts.blackjack;
                this->wasPush = false;

            }
            // check for dealer blackjack
            else if (!player21 && dealer21) {

                // set win
                this->gameOver = true;
                this->playerWon = true;
                this->score = this->scoreAmounts.loss;
                this->wasPush = false;

            }
            // check for double blackjack
            else if (player21 && dealer21) {

                // set win
                this->gameOver = true;
                this->playerWon = false;
                this->score = this->scoreAmounts.push;
                this->wasPush = true;

            }

            return this->gameOver;

        }

        // take hit for player
        bool hit() {

            // update state
            this->table.playerCards.push_back(this->dealer->deal());

            // check for ace
            if (this->table.playerCards.back() == 1) {

                this->table.playerAces ++;

            }

            // add to sum
            this->table.playerSum += this->table.playerCards.back();

            // check for bust
            if (this->table.playerSum > 21) {

                // set loss
                this->gameOver = true;
                this->playerWon = false;
                this->score = this->scoreAmounts.loss;
                this->wasPush = false;

            }

            return this->gameOver;

        }

        // play out dealer hits
        void playDealer() {

            // make sure game isn't over
            if (this->gameOver) {
                return;
            }

            // dealer attributes
            int dealerSum = this->table.dealerShowing + this->dealerSecondCard;
            bool dealerHasAce = (this->table.dealerShowing == 1 || this->dealerSecondCard == 1);
            int dealerHit;

            // check if dealer will stand (check to use ace as 11)
            bool dealerWillStand = (dealerSum >= DEALER_STAND || ((dealerSum + 10) >= DEALER_STAND && dealerHasAce));

            // dealer went over 21
            bool dealerBusted = (dealerSum > 21);

            // keep hitting while too low to stand and didn't bust
            while (!dealerWillStand && !dealerBusted) {

                // get hit for dealer
                dealerHit = this->dealer->deal();
                cout << "dealer hit: " << dealerHit << endl << endl;

                // update sum and ace
                dealerSum += dealerHit;
                dealerHasAce = (dealerHasAce || dealerHit == 1);

                // reset bools
                // stand if over stand num, or adding 10 will put over stand number, wont bust, and is doable (has ace)
                dealerWillStand = (dealerSum >= DEALER_STAND || ((dealerSum + 10) >= DEALER_STAND && (dealerSum + 10) <= 21 && dealerHasAce));
                dealerBusted = (dealerSum > 21);

            }

            // checking for wins
            // if the player busted, the game shouldn't have made it this far

            // check to switch ace for dealer if has ace and wouldn't bust
            dealerSum = ((dealerSum + 10) <= 21 && dealerHasAce) ? (dealerSum + 10) : dealerSum;

            // check to switch ace for player if has ace and wouldn't bust
            this->table.playerSum = ((this->table.playerSum + 10) <= 21 && this->table.playerAces) ? (this->table.playerSum + 10) : this->table.playerSum;

            // check for player win 
            if (dealerBusted || dealerSum < this->table.playerSum) {

                // set win
                this->gameOver = true;
                this->playerWon = true;
                this->score = this->doubleGame ? this->scoreAmounts.doubleWin : this->scoreAmounts.win;
                this->wasPush = false;

            }
            // check if dealer is higher than player
            else if (dealerSum > this->table.playerSum) {

                // set loss
                this->gameOver = true;
                this->playerWon = false;
                this->score = this->doubleGame ? this->scoreAmounts.doubleLoss : this->scoreAmounts.loss;
                this->wasPush = false;

            }
            // else if push
            else {
                
                // set loss
                this->gameOver = true;
                this->playerWon = false;
                this->score = this->scoreAmounts.push;
                this->wasPush = true;

            }

        }

        // reset game so that it is ready for a new one
        void reset() {

            // set hands to empty
            this->table = {0, vector<int>(), 0, 0};
            this->dealerSecondCard = 0;

            // set bet
            this->doubleGame = false;

            // set game conclusion bools to false
            this->gameOver = false;
            this->playerWon = false;
            this->wasPush = false;
            this->score = 0;

        }

        // state accessor
        Hands getState() const {

            return this->table;
        }

        // access dealers hidden card
        int getDealerSecondCard() const {
            return this->dealerSecondCard;
        }

        // game is over
        bool getGameOver() const {
            return this->gameOver;
        }

        // play was the winner
        bool getPlayerWon() const {
            return this->playerWon;
        }

        // push ending
        bool getWasPush() const {
            return this->wasPush;
        }

        // doubled game if true
        float getScore() const {
            return this->score;
        }

        // double player bet
        void doubleBet() {

            this->doubleGame = true;
        }


};

#endif

