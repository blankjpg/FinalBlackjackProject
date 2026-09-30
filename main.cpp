 #include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cctype> //tolower() used
#include <limits>
#include <algorithm>
#include <string>
#include <sstream> //gives string stream properties

using namespace std;

// CARD STRUCTURE
struct Card {
    string name;
    int value;
//practice change
};

// DECK
vector<Card> deck;

void createDeck() {
    deck.clear();
    vector<string> names = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
    vector<int> values  = {11, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10};

    for (int i = 0; i < 4; i++) { // 4 suits
        for (size_t j = 0; j < names.size(); j++) {
            deck.push_back({names[j], values[j]});
        }
    }

    random_shuffle(deck.begin(), deck.end());
}

Card drawCard() {
    if (deck.empty()) {
        createDeck();
    }
    Card card = deck.back();
    deck.pop_back();
    return card;
}

// INTRODUCTION
void printIntroduction() {
    cout<<"\n=================== WELCOME TO BLACKJACK ===================\n";
    cout<<"Rules:\n";
    cout<<"You start with £1000.\n";
    cout<<"You Place a bet each round.\n";
    cout<<"Cards: A, 2-10, J, Q, K.\n";
    cout<<"Aces are 11 or 1 automatically, the game decides your best odds\n";
    cout<<"Face cards (J, Q, K) have the values of 10.\n";
    cout<<"Dealer hits until reaching 17. (dealer stands on soft 17)\n";
    cout<<"you Hit (h) to take another card, Stand (s) to keep your hand.\n";
    cout<<"or you can double Down (d) doubles your bet, draws 1 card, then automatically stands.\n";
    cout<<"if you double down with not enough money, you loose the original money you bet! be careful and watch your cash! \n";
    cout<<"If you exceed 21, you bust and lose your bet.\n";
    cout<<"The Game continues until you run out of money.\n";
    cout<<"===========================================================\n\n";
}

// HAND VALUE
int calculateHandValue(const vector<Card>& hand) {
    int total = 0;
    int aces = 0;

    for (const Card& card : hand) {
        total += card.value;
        if (card.name == "A") aces++;
    }

    while (total > 21 && aces > 0) {
        total -= 10;
        aces--;
    }

    return total;
}

// PRINT THE HAND
void printHand(const vector<Card>& hand, bool hideFirst = false) {
    for (size_t i = 0; i < hand.size(); i++) {
        if (i == 0 && hideFirst)
            cout << "[?] ";
        else
            cout << "[" << hand[i].name << "] ";
    }
    cout << endl;
}

// DOUBLE DOWN
bool doubleDown(vector<Card>& playerHand, int &bet, int money) {
    if (bet * 2 > money) {
        cout << "You're too poor to double down!\n";
        return false;
    }

    bet *= 2;
    cout << "You doubled your bet to $" << bet << "!\n";

    playerHand.push_back(drawCard());
    cout << "Your hand after double down: ";
    printHand(playerHand);
    cout << "Your value: " << calculateHandValue(playerHand) << endl;

    return true; // automatically stands after one card
}

// PLAYER TURN
bool playerTurn(vector<Card>& playerHand, int &bet, int money) {
    while (true) {
        int total = calculateHandValue(playerHand);

        if (total > 21) {
            cout << "Bust! Your total is " << total << endl;
            return false;
        }

        string input;
        cout << "Hit, Stand, or Double Down? (h/s/d): ";
        getline(cin, input);

        if (input.length() != 1) {
            cout << "Invalid input. Please enter 'h', 's', or 'd'.\n";
            continue;
        }

        char choice = tolower(input[0]);

        if (choice == 'h') {
            playerHand.push_back(drawCard());
            cout << "Your hand: ";
            printHand(playerHand);
            cout << "Value: " << calculateHandValue(playerHand) << endl;
        }
        else if (choice == 's') {
            return true;
        }
        else if (choice == 'd') {
            return doubleDown(playerHand, bet, money);
        }
        else {
            cout << "Invalid input. Please enter 'h', 's', or 'd'.\n";
        }
    }
}

// DEALER TURN
void dealerTurn(vector<Card>& dealerHand) {
    while (calculateHandValue(dealerHand) < 17) {
        dealerHand.push_back(drawCard());
    }
}

// SAFE/BACKUP BET
int getValidBet(int money) {
    string input;
    int bet;

    while (true) {
        cout << "\nMoney: $" << money << endl;
        cout << "Place your bet: $";
        getline(cin, input);

        stringstream ss(input);
        if (!(ss >> bet) || bet <= 0 || bet > money) {
            cout << "You can't bet that! Enter a whole number between 1 and " << money << ".\n";
            continue;
        }

        return bet; // valid bet
    }
}

// PLAY ROUND
int playRound(int money) {
    createDeck(); // fresh shuffled deck per round

    int bet = getValidBet(money);

    vector<Card> playerHand = { drawCard(), drawCard() };
    vector<Card> dealerHand = { drawCard(), drawCard() };

    cout << "\nDealer's hand: ";
    printHand(dealerHand, true);

    cout << "Your hand: ";
    printHand(playerHand);
    cout << "Value: " << calculateHandValue(playerHand) << endl;

    if (!playerTurn(playerHand, bet, money)) {
        cout << "You lose $" << bet << endl;
        return money - bet;
    }

    cout << "\ndealer reveals hand: ";
    printHand(dealerHand);
    dealerTurn(dealerHand);

    int playerTotal = calculateHandValue(playerHand);
    int dealerTotal = calculateHandValue(dealerHand);

    cout << "Dealer's final hand: ";
    printHand(dealerHand);
    cout << "Dealers value: " << dealerTotal << endl;

    if (dealerTotal > 21 || playerTotal > dealerTotal) {
        cout << "You win $: " << bet << "!\n";
        return money + bet;
    }
    else if (playerTotal < dealerTotal) {
        cout << "You lose $: " << bet << ".\n";
        return money - bet;
    }

    cout << "Push! It's a tie, same value as the dealer\n";
    return money;
}

// MAIN FUNCTION
int main() {
    srand(static_cast<unsigned>(time(nullptr)));
    char playAgain = 'y';

    // Print intro at start
    printIntroduction();

    while (playAgain == 'y' || playAgain == 'Y') {
        int money = 1000;

        while (money > 0) {
            money = playRound(money);
        }

        cout << "\n GAME OVERRRR!!! You ran out of money :( \n";
        cout << "Play again? Y to play again, any other key to exit: ";
        cin >> playAgain;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (playAgain == 'y' || playAgain == 'Y') {
            printIntroduction();
        }
    }

    cout << "Thank you for playing Toni's Blackjack!\n";
    return 0;
}
