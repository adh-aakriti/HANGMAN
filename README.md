# Hangman Race
Multiplayer client–server Hangman game in C.

# How it works
- Each player races through 3 word levels (4 → 3 → 2 letters).
- Timers reset if you fail a level and you get a new word.
- First person to solve level 3 wins.

# Build & Run on Linux (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install -y gcc cmake libsdl2-dev libsdl2-ttf-dev pthread
git clone <repo>
cd HANGMAN
mkdir build
cd build
cmake ..
make
./server_app
./client_app

