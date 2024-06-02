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
#include <math.h>

struct Feedback
{
  unsigned int pegs_in_correct_place;
  unsigned int pegs_with_correct_color;
};

/// @brief copy code string based on code length and number of unique colors
/// @param length code length
/// @param colors unique colors in code
/// @param index index to get
/// @param code updated
void get_code_inplace(int length, unsigned char colors, int index, unsigned char *out_code)
{
  int i;
  for (i = 0; i < length; i++)
  {
    out_code[length - i - 1] = index % colors;
    index /= colors;
  }
}

/// @brief initializes state of game
/// @param length
/// @param colors
/// @param n
/// @return boolean array of true values for each code
bool *initialize_array(int length, unsigned char colors, int n)
{
  bool *code_set = (bool *)malloc(n * sizeof(bool));
  int i = 0;
  for (i = 0; i < n; i++)
  {
    code_set[i] = true;
  }
  return code_set;
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
/// @param code_set array of booleans - one element for each value
/// @param n length of boolean array code_set
/// @param length length of code
/// @param colors unique colors used in game
void print_all_guesses(bool *code_set, int n, int length, unsigned char colors)
{ // Prints all current possibilities
  int i;
  unsigned char *code = calloc(sizeof(unsigned char), length);
  for (i = 0; i < n; i++)
  {
    if (code_set[i])
    {
      get_code_inplace(length, colors, i, code);
      print_guess(code, length);
      printf(", ");
    }
  }
  printf("\n");
  free(code);
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
int count_all_instances(unsigned char x, unsigned char *code, int length)
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

void get_feedback(unsigned char *potential_solution, unsigned char *guess, int length, unsigned char colors, struct Feedback *out_feedback)
{
  int pegs_in_correct_place = 0;
  int pegs_with_correct_color = 0;
  for (int color = 0; color < colors; color++)
  {
    int color_pegs_in_guess = count_all_instances(color, guess, length);
    int color_pegs_in_solution = count_all_instances(color, potential_solution, length);

    if (color_pegs_in_solution > color_pegs_in_guess)
      pegs_with_correct_color += color_pegs_in_guess;
    else
      pegs_with_correct_color += color_pegs_in_solution;
  }

  for (int idx = 0; idx < length; idx++)
  {
    if (guess[idx] == potential_solution[idx])
    {
      pegs_in_correct_place++;
    }
  }

  // subtract pegs in correct place from pegs with matching color to remove duplicates
  pegs_with_correct_color -= pegs_in_correct_place;

  out_feedback->pegs_in_correct_place = pegs_in_correct_place;
  out_feedback->pegs_with_correct_color = pegs_with_correct_color;
}

/// @brief get number of possible still possible based on given guess and feedback
/// @param code_set array of booleans for each code
/// @param now current guess
/// @param pegs_with_correct_color number of colors that are in the solution but in the wrong place
/// @param pegs_in_correct_place number of colors that are in the correct place
/// @param length length of the code
/// @param colors number of unique colors in game
/// @param n length of array code_set
/// @return number of possible solutions still possible
int reduce(bool *code_set, unsigned char *guess, int pegs_with_correct_color, int pegs_in_correct_place, int length, unsigned char colors, int n)
{
  int x = 0;
  int i;
  unsigned char *potential_solution = calloc(sizeof(unsigned char), length);
  struct Feedback feedback;
  for (i = 0; i < n; i++)
  {
    if (code_set[i])
    {
      get_code_inplace(length, colors, i, potential_solution);
      get_feedback(potential_solution, guess, length, colors, &feedback);
      if (feedback.pegs_with_correct_color == pegs_with_correct_color && feedback.pegs_in_correct_place == pegs_in_correct_place)
        x++;
    }
  }
  free(potential_solution);
  return x;
}

/// @brief get highest number of possibly remaining results for a guess based on all possible feedback options
/// @param code_set current list of possible codes
/// @param now current guess
/// @param length length of code
/// @param colors number of unique codes
/// @param n length of array code_set
/// @return highest number of possibly remaining results for a guess
int full_reduce(bool *code_set, unsigned char *now, int length, unsigned char colors, int n)
{
  int responses[13][2] = {{0, 0}, {1, 0}, {0, 1}, {2, 0}, {1, 1}, {0, 2}, {3, 0}, {2, 1}, {1, 2}, {0, 3}, {4, 0}, {3, 1}, {2, 2}};
  int x = 0;
  int i;
  for (i = 0; i < 13; i++)
  {
    int y = reduce(code_set, now, responses[i][0], responses[i][1], length, colors, n);
    if (y > x)
    {
      x = y;
    }
  }
  return x;
}

/// @brief reduces the set of possible results based on the feedback given
/// @param code_set boolean array of currently possible guesses
/// @param now current guess
/// @param pegs_with_correct_color number of colors in the guess that are in the solution but in the wrong place
/// @param pegs_in_correct_place number of colors in the guess that are in the right place
/// @param length length of the code
/// @param colors number of unique colors
/// @param n length of code_set
void set_reduce(bool *code_set, unsigned char *guess, int pegs_with_correct_color, int pegs_in_correct_place, int length, unsigned char colors, int n)
{
  // int j = 0;
  int i;
  unsigned char *code = calloc(sizeof(unsigned char), length);
  struct Feedback feedback;
  for (i = 0; i < n; i++)
  {
    if (code_set[i])
    {
      get_code_inplace(length, colors, i, code);
      get_feedback(code, guess, length, colors, &feedback);
      if (feedback.pegs_with_correct_color != pegs_with_correct_color || feedback.pegs_in_correct_place != pegs_in_correct_place)
      {
        code_set[i] = false;
      }
    }
  }
  free(code);
}

/// @brief Find best move based on minimax decisionmaking
/// @param code_set array of booleans representing set of currently still possible results based on previous feedbacck and guesses
/// @param length length of code
/// @param colors number of possible colors to choose from
/// @param n length of array code_set
void get_best_next_move(bool *code_set, int length, unsigned char colors, int n, unsigned char* out_best_move)
{
  int best_code_index = 0;
  unsigned char *guess = calloc(sizeof(unsigned char), length);
  get_code_inplace(length, colors, 0, guess);
  float current_best_reduction = full_reduce(code_set, guess, length, colors, n);
  int i;
  for (i = 1; i < n; i++)
  {
    if (code_set[i])
    {
      get_code_inplace(length, colors, i, guess);
      int x = full_reduce(code_set, guess, length, colors, n);
      // printf("code, Value: ");
      // print_guess(code_set[i], length);
      // printf(" %d\n", x);
      if (x < current_best_reduction)
      {
        best_code_index = i;
        current_best_reduction = x;
      }
    }
  }

  get_code_inplace(length, colors, best_code_index, out_best_move);
  free(guess);
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
  bool *code_set = initialize_array(length, colors, n);

  // always start with the same move 0011
  unsigned char *move = calloc(sizeof(unsigned char), length);
  get_best_next_move(code_set, length, colors, n, move); // get_code_inplace(length, colors, 7, move);
  int remaining_candidates = n;
  while (n > 0)
  {
    printf("%d remaining candidates\n", remaining_candidates);
    print_guess(move, length);
    printf("\n");
    int pegs_with_correct_color;
    int pegs_in_correct_place;
    printf("Pegs in the right place: ");
    scanf("%d", &pegs_in_correct_place);
    printf("Pegs in the wrong place, but present in the code: ");
    scanf("%d", &pegs_with_correct_color);
    if (pegs_with_correct_color == 0 && pegs_in_correct_place == 4)
    {
      printf("Yay! I win!\n");
      break;
    }
    remaining_candidates = reduce(code_set, move, pegs_with_correct_color, pegs_in_correct_place, length, colors, n);
    if (remaining_candidates == 0)
    {
      printf("I'm Stumped...\n");
      break;
    }
    set_reduce(code_set, move, pegs_with_correct_color, pegs_in_correct_place, length, colors, n);
    get_best_next_move(code_set, length, colors, n, move);
  }
  free(code_set);
  free(move);
  return 0;
}
