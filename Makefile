# MIT License

# Copyright (c) 2017 Coen Valk

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.


mastermind: mastermind.c
	clang mastermind.c -O3 -Wall -Werror -lm -o mastermind.out
test:
	./mastermind.out < tests/test0.txt > test.out
	diff test.out tests/test0-output.txt
	./mastermind.out < tests/test1.txt > test.out
	diff test.out tests/test1-output.txt
	./mastermind.out < tests/test2.txt > test.out
	diff test.out tests/test2-output.txt
	./mastermind.out < tests/test3.txt > test.out
	diff test.out tests/test3-output.txt
	rm test.out
