# Spelling Bee Game

## About 📋
The following program is inspired by the New York Times Spelling Bee minigame. 

The program has two primary functionalities being a game (just like the one in NYT) and a hive solver, which are flag-toggled modes.

This particular implementation deviates from the standard minigame of only finding words from a given list, with a specified letter, by permitting the user to customize the game through various flags (more on these below).

The gameplay itself introduces a new way to play with the addition of user-selected, concrete point goals, a time-based point system, and a local leaderboard.

Before execution end, the program displays several statistics about the hive and all its valid solutions, including a complete list of such.

*Note: A valid answer to the hive is at least 4 characters long.*

## How To Use 🛠️

### General 👨‍💻

For general use, please download the folder in this repository. At a minimum main.c, local_highscore.txt, and some dictionary are needed for the program to function properly. I have provided 'dictionary.txt' (which the program uses by default if no other dictionary is chosen), which is representative of the official Scrabble, valid words dictionary, but feel free to use your own. 

Further instructions are available as you run the program, but please read the following section on flags to fully grasp the program's functionality.

Please ensure to compile using your compiler of choice or run 'make build' to compile (if using the attached makefile).

### Flags ⛳️

By default, the program enters its solver mode when run without flags. The user will be prompted to enter a string of unique characters and the mandatory character from that string and provide a list of all valid solutions... so if you're having trouble with today's spelling bee, give the default settings a try!

**The following flags are available to customize your experience:**

(-r num) specifies whether the user wishes to enter random mode, meaning a random string of 'num' characters is generated to serve as a word hive. *The user must specify the hive size*.

(-d filename) specifies the dictionary the user wants to use for their run. By default, 'dictionary.txt' is used. *The user must specify the text file they wish to use for their dictionary*.

(-s num) specifies the seed the user wishes to use for the randomizer. By default, the current time is used. Mainly for debugging. *The user must specify the number to use for the seed*.

(-p) specifies whether the user wants to enter 'play mode', where the solver doesn't run immediately, and the user can actually go through the regular gameplay features.

(-o) enables the optimized solution solver using an efficient algorithm to find all solutions to a specific hive instead of using a linear, brute force search.

A makefile has been included containing some premade settings, including:

make run (runs with no flags, i.e., runs solver)

make run_play (executes in play mode with a random hive of length 7)

### Game 🎮

In general, the game functions fundamentally the same as the NYT's spelling bee game, in which a user has to form as many words as possible from a given list of letters, with the twist being that the words they form must contain some specific letter out of that group. As previously mentioned, this version allows the user to select a point goal they would like to attempt to reach and is scored based not only on how many points they reach but also the speed at which they reach said score. In other words, it is important not only to score high but also to reach that score fast.

At the end, the user's score is compared to the current saved local high score and said local highscore is updated accordingly.

To enable game mode, run your code with -p. F

For a random game, follow the procedure for enabling random mode described in the 'Flags' section above.

You may use 'make run_play' to execute in play mode with a random hive of length 7

### Solver 🧩

The solver takes a hive and the character that must be included, and finds every word in the provided dictionary that is a valid solution to the provided hive. The solutions are then printed out alongside their respective scores based on the number of characters and whether they are a pangram/perfect pangram (pangrams being those that use all the characters in the hive at least once).

The programs runs in its solver mode by default (no -p flag, although the solver still runs after the gameplay loop terminates). 

For the optimized solver, which more efficiently finds these solutions, please use the -o flag.

## Screenshots and Gameplay 🐝

**Typical Gameplay Loop with a Win**

<img width="839" height="593" alt="Screenshot 2026-02-22 at 6 17 59 PM" src="https://github.com/user-attachments/assets/6b4bf39f-0d66-4898-927d-6c5160c19303" />

**List of solutions for hive 'adhlruy' with mandatory 'u'**

<img width="260" height="645" alt="Screenshot 2026-02-22 at 6 18 21 PM" src="https://github.com/user-attachments/assets/bc27925e-fde4-47ac-85d4-9ac119380218" />



## Questions? 📧
Feel free to contact me over at jorge.martin0308@gmail.com with any questions about gameplay, how to run, or even if you find any bugs! 🐛

