# Spelling Bee Game

## About 📋
The following program is inspired by the New York Times Spelling Bee minigame. 

The program has two primary functionalities being a game (just like the one in NYT) and a hive solver, which are flag-toggled modes.

This particular implementation deviates from the standard minigame of only finding words from a given list, with a specified letter, by permitting the user to customize the game through various flags (more on these below).

The gameplay itself introduces a new way to play with the addition of user-selected, concrete point goals, a time-based point system, and a local leaderboard.

Before execution end, the program displays several statistics about the hive and all its valid solutions, including a complete list of such.

## How To Use 🛠️

### General 👨‍💻

For general use, please download the folder in this repository. At a minimum main.c, local_highscore.txt, and some dictionary are needed for the program to function properly. I have provided 'dictionary.txt' (which the program uses by default if no other dictionary is chosen), which is representative of the official Scrabble, valid words dictionary, but feel free to use your own.

Further instructions are available as you run the program, but please read the following section on flags to fully grasp the program's functionality.

### Flags ⛳️

By default, the program enters its solver mode when run without flags. The user will be prompted to enter a string of unique characters and the mandatory character from that string and provide a list of all valid solutions... so if you're having trouble with today's spelling bee, give the default settings a try!

**The following flags are available to customize your experience:**

(-r num) specifies whether the user wishes to enter random mode, meaning a random string of 'num' characters is generated to serve as a word hive. *The user must specify the hive size*.

(-d filename) specifies the dictionary the user wants to use for their run. By default, 'dictionary.txt' is used. *The user must specify the text file they wish to use for their dictionary*.

(-s num) specifies the seed the user wishes to use for the randomizer. By default, the current time is used. Mainly for debugging. *The user must specify the number to use for the seed*.

(-p) specifies whether the user wants to enter 'play mode', where the solver doesn't run immediately, and the user can actually go through the regular gameplay features.

(-o) enables the optimized solution solver using an efficient algorithm to find all solutions to a specific hive instead of using a linear, brute force search.


### Game 🎮

### Solver 🧩


## Screenshots and Gameplay 🐝


## Questions? 📧
