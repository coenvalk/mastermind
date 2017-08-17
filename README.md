# Mastermind Solver [![Build Status](https://travis-ci.org/coenvalk/mastermind.svg?branch=updates)](https://travis-ci.org/coenvalk/mastermind)

This program plays using Donald Knuth's algorithm to solve a game of mastermind in 6 moves or less.

I originally wrote this in highschool for my math internal assessment for completing the International Baccalaureate diploma program. The paper associated with this program is [here](https://drive.google.com/file/d/0B8CNl_hZHtFzTm5ESzZ0QVRLV0k/view?usp=sharing)

## Rules and Gameplay of Mastermind

from [wikipedia](https://en.wikipedia.org/wiki/Mastermind_(board_game)):
> The game is played using:
> * a decoding board, with a shield at one end covering a row of four large holes, and twelve (or ten, or eight, or six) additional rows containing four large holes next to a set of four small holes;
> * code pegs of six (or more; see Variations below) different colors, with round heads, which will be placed in the large holes on the board; and
> * key pegs, some colored black, some white, which are flat-headed and smaller than the code pegs; they will be placed in the small holes on the board.
>
> The two players decide in advance how many games they will play, which must be an even number. One player becomes the codemaker, the other the codebreaker. The codemaker chooses a pattern of four code pegs. Duplicates and blanks are allowed depending on player choice, so the player could even choose four code pegs of the same color or four blanks. In the instance that blanks are not elected to be a part of the game, the codebreaker may not use blanks in order to establish the final code. The chosen pattern is placed in the four holes covered by the shield, visible to the codemaker but not to the codebreaker.
>
> The codebreaker tries to guess the pattern, in both order and color, within twelve (or ten, or eight) turns. Each guess is made by placing a row of code pegs on the decoding board. Once placed, the codemaker provides feedback by placing from zero to four key pegs in the small holes of the row with the guess. A colored or black key peg is placed for each code peg from the guess which is correct in both color and position. A white key peg indicates the existence of a correct color code peg placed in the wrong position.
>
> If there are duplicate colours in the guess, they cannot all be awarded a key peg unless they correspond to the same number of duplicate colours in the hidden code. For example, if the hidden code is white-white-black-black and the player guesses white-white-white-black, the codemaker will award two colored key pegs for the two correct whites, nothing for the third white as there is not a third white in the code, and a colored key peg for the black. No indication is given of the fact that the code also includes a second black.
>
> Once feedback is provided, another guess is made; guesses and feedback continue to alternate until either the codebreaker guesses correctly, or twelve (or ten, or eight) incorrect guesses are made.
>
> The codemaker gets one point for each guess a codebreaker makes. An extra point is earned by the codemaker if the codebreaker doesn't guess the pattern exactly in the last guess. (An alternative is to score based on the number of colored key pegs placed.) The winner is the one who has the most points after the agreed-upon number of games are played.
>
> Other rules may be specified.

### Specific Usage
This program plays with numbers (0 - 5) instead of colors. For each turn the program returns a guess and the number of possible codes are left. Then the program prompts:

    Colors:

Where the user inputs the amount of pegs that are the right color, but in the wrong place. Then the program prompts for:

    Place:
    
Where the user inputs the amount of pegs that are both the right color and in the right position.

The program should always find the code you are thinking of in 6 moves or less. If there is an error in your feedback, it might come to the conclusion that there are no solutions and respond with

    I'm stumped...

And the program ends. Once the program finds the answer, the user inputs 0 for the colors prompt and 4 four the place prompt which also ends the program.

## Variations
The standard version of this game is played with 4 pegs of 6 colors. This can be changed, by changing the values `length` and `colors` on line 188 and 189, respectively.

## Installation
There is a Makefile for added simplicity.

    $ make
    $ ./mastermind.out
    
## License
This program is licensed under the MIT license.
