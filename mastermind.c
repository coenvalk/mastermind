/*
  MIT License

  Copyright (c) 2017 Coen Valk

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/
// Guess Mastermind code

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

/// @brief get code string based on code length and number of unique colors
/// @param length code length
/// @param colors unique colors in code
/// @param index index to get
/// @return code string at index with given color characters
unsigned char *get_code(int length, unsigned char colors, int index)
{
  unsigned char *code = (unsigned char *)malloc(length * sizeof(unsigned char));
  int i;
  for (i = 0; i < length; i++)
  {
    code[length - i - 1] = index % colors;
    index /= colors;
  }
  return code;
}

/// @brief initializes state of game
/// @param length 
/// @param colors 
/// @param n 
/// @return boolean array of true values for each code
bool *initialize_array(int length, unsigned char colors, int n)
{
  bool *S = (bool *)malloc(n * sizeof(bool));
  int i = 0;
  for (i = 0; i < n; i++)
  {
    S[i] = true;
  }
  return S;
}

/// @brief prints a single code to the console
/// @param now current code to be printed
/// @param length length of code
void print_guess(unsigned char *now, int length)
{
  int i;
  for (i = 0; i < length; i++)
  {
    printf("%d", (int)now[i]);
  }
}

/// @brief prints all codes for which corresponding index is true
/// @param S array of booleans - one element for each value
/// @param n length of boolean array S
/// @param length length of code
/// @param colors unique colors used in game
void print_all_guesses(bool *S, int n, int length, unsigned char colors)
{ // Prints all current possibilities
  int i;
  for (i = 0; i < n; i++)
  {
    if (S[i])
    {
      unsigned char *C = get_code(length, colors, i);
      print_guess(C, length);
      free(C);
      printf(", ");
    }
  }
  printf("\n");
}

/// @brief Checks whether a particular character is in code
/// @param x character being searched for
/// @param code code being searched through
/// @param length length of code
/// @return true if code contains x, false otherwise
bool contains(unsigned char x, unsigned char *code, int length)
{
  int i;
  for (i = 0; i < length; i++)
  {
    if (code[i] == x)
      return true;
  }
  return false;
}

/// @brief counts how many instances of x are in code
/// @param x character being searched for
/// @param code code being searched for
/// @param length length of code
/// @return how many instances of character `x` are in code
int instance_count(unsigned char x, unsigned char *code, int length)
{
  int r = 0;
  int i;
  for (i = 0; i < length; i++)
  {
    if (code[i] == x)
      r++;
  }
  return r;
}

int *analyze(unsigned char *code, unsigned char *guess, int length, unsigned char colors)
{
  int i;
  int c = 0;
  int p = 0;
  for (i = 0; i < colors; i++)
  {
    int gue = instance_count(i, guess, length);
    int cod = instance_count(i, code, length);
    if (cod > gue)
      c += gue;
    else
      c += cod;
  }
  for (i = 0; i < length; i++)
  {
    if (contains(guess[i], code, length))
    {
      if (guess[i] == code[i])
      {
        p++;
      }
    }
  }
  int *r = (int *)malloc(sizeof(int) * 2);
  r[0] = c - p;
  r[1] = p;
  return r;
}

/// @brief get number of possible still possible based on given guess and feedback
/// @param S array of booleans for each code
/// @param now current guess
/// @param c number of colors that are in the solution but in the wrong place
/// @param p number of colors that are in the correct place
/// @param length length of the code
/// @param colors number of unique colors in game
/// @param n length of array S
/// @return number of possible solutions still possible
int reduce(bool *S, unsigned char *now, int c, int p, int length, unsigned char colors, int n)
{
  int x = 0;
  int i;
  for (i = 0; i < n; i++)
  {
    if (S[i])
    {
      unsigned char *Code = get_code(length, colors, i);
      int *r = analyze(Code, now, length, colors);
      free(Code);
      if (r[0] == c && r[1] == p)
        x++;
      free(r);
    }
  }
  return x;
}

/// @brief get highest number of possibly remaining results for a guess based on all possible feedback options
/// @param S current list of possible codes
/// @param now current guess
/// @param length length of code
/// @param colors number of unique codes
/// @param n length of array S
/// @return highest number of possibly remaining results for a guess
int full_reduce(bool *S, unsigned char *now, int length, unsigned char colors, int n)
{
  int responses[13][2] = {{0, 0}, {1, 0}, {0, 1}, {2, 0}, {1, 1}, {0, 2}, {3, 0}, {2, 1}, {1, 2}, {0, 3}, {4, 0}, {3, 1}, {2, 2}};
  int x = 0;
  int i;
  for (i = 0; i < 13; i++)
  {
    int y = reduce(S, now, responses[i][0], responses[i][1], length, colors, n);
    if (y > x)
    {
      x = y;
    }
  }
  return x;
}

/// @brief reduces the set of possible results based on the feedback given
/// @param S boolean array of currently possible guesses
/// @param now current guess
/// @param c number of colors in the guess that are in the solution but in the wrong place
/// @param p number of colors in the guess that are in the right place
/// @param length length of the code
/// @param colors number of unique colors
/// @param n length of S
void set_reduce(bool *S, unsigned char *now, int c, int p, int length, unsigned char colors, int n)
{
  // int j = 0;
  int i;
  for (i = 0; i < n; i++)
  {
    unsigned char *Code = get_code(length, colors, i);
    int *r = analyze(Code, now, length, colors);
    free(Code);
    if (r[0] != c || r[1] != p)
    {
      S[i] = false;
    }
    free(r);
  }
}

/// @brief Find best move based on minimax decisionmaking
/// @param S array of booleans representing set of currently still possible results based on previous feedbacck and guesses
/// @param length length of code
/// @param colors number of possible colors to choose from
/// @param n length of array S
/// @return 
unsigned char *get_best_move(bool *S, int length, unsigned char colors, int n)
{
  int best = 0;
  unsigned char *Code = get_code(length, colors, 0);
  float bestR = full_reduce(S, Code, length, colors, n);
  free(Code);
  int i;
  for (i = 1; i < n; i++)
  {
    if (S[i])
    {
      Code = get_code(length, colors, i);
      int x = full_reduce(S, Code, length, colors, n);
      free(Code);
      // printf("Code, Value: ");
      // print_guess(S[i], length);
      // printf(" %d\n", x);
      if (x < bestR)
      {
        best = i;
        bestR = x;
      }
    }
  }
  return get_code(length, colors, best);
}

int main()
{
  int length = 4;
  unsigned char colors = 6;
  int n = 1;
  int i;
  for (i = 1; i <= length; i++)
  {
    n *= colors;
  }
  bool *S = initialize_array(length, colors, n);

  // always start with the same move 0011
  unsigned char *move = get_code(length, colors, 7); // get_best_move(S, length, colors, n);
  int newN = n;
  while (n > 0)
  {
    printf("%d remaining candidates\n", newN);
    print_guess(move, length);
    printf("\n");
    int c;
    int p;
    printf("Colors: ");
    scanf("%d", &c);
    printf("Place: ");
    scanf("%d", &p);
    if (c == 0 && p == 4)
    {
      printf("Yay! I win!\n");
      break;
    }
    newN = reduce(S, move, c, p, length, colors, n);
    if (newN == 0)
    {
      printf("I'm Stumped...\n");
      break;
    }
    set_reduce(S, move, c, p, length, colors, n);
    free(move);
    move = get_best_move(S, length, colors, n);
  }
  free(S);
  free(move);
  return 0;
}
