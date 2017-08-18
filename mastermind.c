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

/*
  generates indexth code based on length and colors
 */
unsigned char* get_code(int length, unsigned char colors, int index) {
  unsigned char* code = (unsigned char*) malloc(length * sizeof(unsigned char));
  int i;
  for (i = 0; i < length; i++) {
    code[length - i - 1] = index % colors;
    index /= colors;
  }
  return code;
}

/*
  creates array that keeps track of which codes are still possible
 */
bool* create_all(int length, unsigned char colors, int n) {
  bool* S = (bool*) malloc(n * sizeof(bool));
  int i = 0;
  for (i = 0; i < n; i++) {
    S[i] = true;
  }
  return S;
}

void print_one(unsigned char* now, int length) {
  int i;
  for (i = 0; i < length; i++) {
    printf("%d", (int) now[i]);
  }
}

void print_all(bool* S, int n, int length, unsigned char colors) { // Prints all current possibilities
  int i;
  for (i = 0; i < n; i++) {
    if (S[i]) {
      unsigned char* C = get_code(length, colors, i);
      print_one(C, length);
      free(C);
      printf(", ");
    }
  }
  printf("\n");
}

bool isin(unsigned char x, unsigned char* code, int length) {
  int i;
  for (i = 0; i < length; i++) {
    if (code[i] == x) return true;
  }
  return false;
}

int howmany(unsigned char x, unsigned char* code, int length) {
  int r = 0;
  int i;
  for (i = 0; i < length; i++) {
    if (code[i] == x) r++;
  }
  return r;
}

int* analyze(unsigned char* code, unsigned char* guess, int length, unsigned char colors) {
  int i;
  int c = 0;
  int p = 0;
  for (i = 0; i < colors; i++) {
    int gue = howmany(i, guess, length);
    int cod = howmany(i, code, length);
    if (cod > gue)
      c += gue;
    else
      c += cod;
  }
  for (i = 0; i < length; i++) {
    if (isin(guess[i], code, length)) {
      if (guess[i] == code[i]) {
	p++;
      }
    }
  }
  int* r = (int*) malloc(sizeof(int) * 2);
  r[0] = c-p;
  r[1] = p;
  return r;
}

int reduce(bool* S, unsigned char* now, int c, int p, int length, unsigned char colors, int n) {
  int x = 0;
  int i;
  for (i = 0; i < n; i++) {
    if (S[i]) {
      unsigned char* Code = get_code(length, colors, i);
      int* r = analyze(Code, now, length, colors);
      free(Code);
      if (r[0] == c && r[1] == p) x++;
      free(r);
    }
  }
  return x;
}

int fullReduce(bool* S, unsigned char* now, int length, unsigned char colors, int n) {
  int responses[13][2] = {{0, 0}, {1, 0}, {0, 1}, {2, 0}, {1, 1}, {0, 2}, {3, 0}, {2, 1}, {1, 2}, {0, 3}, {4, 0}, {3, 1}, {2, 2}};
  int x = 0;
  int index = 0;
  int i;
  for (i = 0; i < 13; i++) {
    int y = reduce(S, now, responses[i][0], responses[i][1], length, colors, n);
    if (y > x) {
      x = y;
      index = i;
    }
  }
  return x;
}

void SetReduce(bool* S, unsigned char* now, int c, int p, int length, unsigned char colors, int n) {
  // int j = 0;
  int i;
  for (i = 0; i < n; i++) {
    unsigned char* Code = get_code(length, colors, i);
    int* r = analyze(Code, now, length, colors);
    free(Code);
    if (r[0] != c || r[1] != p) {
      S[i] = false;
    }
    free(r);
  }
}

unsigned char* BestMove(bool* S, int length, unsigned char colors, int n) {
  int best = 0;
  unsigned char* Code = get_code(length, colors, 0);
  float bestR = fullReduce(S, Code, length, colors, n);
  free(Code);
  int i;
  for (i = 1; i < n; i++) {
    if (S[i]) {
      Code = get_code(length, colors, i);
      int x = fullReduce(S, Code, length, colors, n);
      free(Code);
      // printf("Code, Value: ");
      // print_one(S[i], length);
      // printf(" %d\n", x);
      if (x < bestR) {
	best = i;
	bestR = x;
      }
    }  
  }
  return get_code(length, colors, best);
}



int main() {
  int length = 4;
  unsigned char colors = 6;
  int n = 1;
  int i;
  for (i = 1; i <= length; i++) {
    n *= colors;
  }
  bool* S = create_all(length, colors, n);
  unsigned char* move = get_code(length, colors, 7); // BestMove(S, length, colors, n);
  int newN = n;
  while (n > 0) {
    printf("%d remaining candidates\n", newN);
    print_one(move, length);
    printf("\n");
    int c;
    int p;
    printf("Colors: ");
    scanf("%d", &c);
    printf("Place: ");
    scanf("%d", &p);
    if (c == 0 && p == 4) {
      printf("Yay! I win!\n");
      break;
    }
    newN = reduce(S, move, c, p, length, colors, n);
    if (newN == 0) {
      printf("I'm Stumped...\n");
      break;
    }
    SetReduce(S, move, c, p, length, colors, n);
    free(move);
    move = BestMove(S, length, colors, n);
  }
  free(S);
  free(move);
  return 0;
}
