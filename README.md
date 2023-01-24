# Differentiator

The program is free-to-use for all kinds of developers.

***
## What does the project do
The program calculates the derivative of given expression, simplifies the result and outputs it to LaTeX, showing simplification steps.

***
## Why is the project useful
Taking derivative of math expressions, especially of significant ones, is always a pain in the neck, so this program called up to
help with this problem.

***
## Input
Input requires to be written using full parentheses notation, when each operation or number requires to be surrounded by parentheses.

***
## Structure
Firstly, the program builds tree of an expression. Then it takes derivative, successively changing the tree. 
After that the result is being simplified using __constant convolution technique__, when some nodes with constants are simplified,
and __trivialization technique__, when some trivial cases like adding/subtracting zero, exponentation one are processed.
All steps of simplification are shown in LaTeX file accompanied with some entertaining phrases.

***
## How can I help
I explicitly welcome any outside contributors. Use my mail: _ilichev.kv@phystech.edu_.

__A strong request__:
* Do not break the codestyle
* Do not destroy previous functionality with additions

***
## Where to ask questions
Use the mail mentioned earlier.
