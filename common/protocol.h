#ifndef PROTOCOL_H
#define PROTOCOL_H

#define PORT 8080
#define BUFFER_SIZE 1024

// Game Constants
#define MAX_LIVES 6
#define NUM_LEVELS 3

// Protocol Headers
#define CMD_READY "READY"
#define CMD_GUESS "GUESS"

#define MSG_WELCOME "WELCOME" // CONNECTION ACK
#define MSG_LEVEL "LEVEL"     // LEVEL <n>
#define MSG_WORD "WORD"       // WORD <masked>
#define MSG_TIMER "TIMER"     // TIMER <seconds>
#define MSG_UPDATE "UPDATE"   // UPDATE <masked> <mistakes>
#define MSG_TIMEUP "TIME_UP"  
#define MSG_NEWWORD "NEW_WORD" // NEW_WORD <masked>
#define MSG_WIN "WINNER"      // WINNER <name>
#define MSG_GAMEOVER "GAME_OVER"

#endif
