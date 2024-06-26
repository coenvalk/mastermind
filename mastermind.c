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
#include <sys/time.h>
#include <math.h>

struct Feedback
{
  size_t pegs_in_correct_place;
  size_t pegs_with_correct_color;
};

/// @brief copy code string based on code length and number of unique colors
/// @param length code length
/// @param colors unique colors in code
/// @param index index to get
/// @param code updated
void get_code_at_index(int length, unsigned char colors, int index, unsigned char *out_code)
{
  int i;
  for (i = 0; i < length; i++)
  {
    out_code[length - i - 1] = index % colors;
    index /= colors;
  }
}

/// @brief initializes state of game
/// @param length length of code
/// @param colors number of colors to choose from
/// @return boolean array of true values for each code
bool *initialize_solution_set(int length, unsigned char colors)
{
  size_t n = pow(colors, length);
  bool *solution_set = calloc(sizeof(bool), n);
  for (size_t i = 0; i < n; ++i)
  {
    solution_set[i] = true;
  }

  return solution_set;
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
/// @param solution_set array of booleans - one element for each possible code
/// @param length length of code
/// @param colors unique colors used in game
void print_all_guesses(bool *solution_set, size_t n, int length, unsigned char colors)
{ // Prints all current possibilities
  size_t i;
  unsigned char *code = calloc(sizeof(unsigned char), length);
  for (i = 0; i < n; i++)
  {
    if (solution_set[i])
    {
      get_code_at_index(length, colors, i, code);
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

/// @brief get feedback between solution and guess
/// @param potential_solution possible solution code
/// @param guess code
/// @param length length of code
/// @param colors number of unique colors to choose from
/// @param out_feedback pointer to feedback value to update
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

/// @brief Find the feedback result that is most common for guess
/// @param solution_set set of currently possible solutions
/// @param guess current guess
/// @param length length of code
/// @param colors number of colors to choose from
/// @return size of largest feedback bucket
int max_feedback_result(bool *solution_set, unsigned char *guess, int length, unsigned char colors)
{
  struct Feedback feedback;
  size_t *feedback_buckets = calloc(sizeof(size_t), length * length + 1);
  size_t largest_feedback_bucket_size = 0;
  size_t n = pow(colors, length);
  unsigned char *possible_solution = calloc(sizeof(unsigned char), length);

  for (size_t i = 0; i < n; ++i)
  {
    if (!solution_set[i])
      continue;

    get_code_at_index(length, colors, i, possible_solution);
    get_feedback(possible_solution, guess, length, colors, &feedback);
    feedback_buckets[feedback.pegs_in_correct_place * length + feedback.pegs_with_correct_color]++;
  }

  for (size_t i = 0; i < length * length; ++i)
  {
    if (feedback_buckets[i] > largest_feedback_bucket_size)
    {
      largest_feedback_bucket_size = feedback_buckets[i];
    }
  }

  free(feedback_buckets);
  free(possible_solution);

  return largest_feedback_bucket_size;
}

/// @brief reduces the set of possible results based on the feedback given
/// @param solution_set boolean array of currently possible guesses
/// @param now current guess
/// @param pegs_with_correct_color number of colors in the guess that are in the solution but in the wrong place
/// @param pegs_in_correct_place number of colors in the guess that are in the right place
/// @param length length of the code
/// @param colors number of unique colors
size_t set_reduce(bool *solution_set, unsigned char *guess, int pegs_with_correct_color, int pegs_in_correct_place, int length, unsigned char colors)
{
  struct Feedback feedback;
  size_t n = pow(colors, length);
  unsigned char *code = calloc(sizeof(unsigned char), length);
  size_t new_set_size = 0;
  for (size_t i = 0; i < n; i++)
  {
    if (!solution_set[i])
      continue;

    get_code_at_index(length, colors, i, code);
    get_feedback(code, guess, length, colors, &feedback);
    if (feedback.pegs_with_correct_color != pegs_with_correct_color || feedback.pegs_in_correct_place != pegs_in_correct_place)
    {
      solution_set[i] = false;
    }
    else
    {
      new_set_size++;
    }
  }
  free(code);

  return new_set_size;
}

/// @brief Find best move based on minimax decisionmaking
/// @param solution_set array of booleans representing set of currently still possible results based on previous feedbacck and guesses
/// @param length length of code
/// @param colors number of possible colors to choose from
void get_best_next_move(bool *solution_set, int length, unsigned char colors, unsigned char *out_best_move)
{
  struct timeval start;
  struct timeval end;

  gettimeofday(&start, NULL);

  size_t n = pow(colors, length);
  int best_code_index = 0;
  size_t best_code_reduction = __SIZE_MAX__;
  unsigned char *guess = calloc(sizeof(unsigned char), length);

  for (size_t i = 0; i < n; ++i)
  {
    if (!solution_set[i])
      continue;

    get_code_at_index(length, colors, i, guess);
    size_t guess_largest_feedback_bucket = max_feedback_result(solution_set, guess, length, colors);
    if (guess_largest_feedback_bucket < best_code_reduction)
    {
      best_code_reduction = guess_largest_feedback_bucket;
      best_code_index = i;
    }
  }

  free(guess);
  gettimeofday(&end, NULL);

  long duration_in_milliseconds = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
  printf("finding best move took %ld milliseconds\n", duration_in_milliseconds);

  get_code_at_index(length, colors, best_code_index, out_best_move);
}

int main()
{
  int length = 4;
  unsigned char colors = 6;
  size_t n = pow(colors, length);
  bool *solution_set = initialize_solution_set(length, colors);

  // always start with the same move 0011
  unsigned char *move = calloc(sizeof(unsigned char), length);
  get_best_next_move(solution_set, length, colors, move); // get_code_at_index(length, colors, 7, move);

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

    if (pegs_with_correct_color == 0 && pegs_in_correct_place == length)
    {
      printf("Yay! I win!\n");
      break;
    }
    
    remaining_candidates = set_reduce(solution_set, move, pegs_with_correct_color, pegs_in_correct_place, length, colors);
    if (remaining_candidates == 0)
    {
      printf("I'm stumped...\n");
      break;
    }
    get_best_next_move(solution_set, length, colors, move);
  }
  free(solution_set);
  free(move);
  return 0;
}
