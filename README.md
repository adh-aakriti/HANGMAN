# Hangman Race
Multiplayer client–server Hangman game in C.

# How it works
- Each player races through 3 word levels (4 → 3 → 2 letters).
- Timers reset if you fail a level and you get a new word.
- First person to solve level 3 wins.

# Build & Run
```bash
git clone <repo>
cd HANGMAN
conda env update
conda activate hangman
cmake -B build
cmake --build build
./server_app &
./client_app
```
