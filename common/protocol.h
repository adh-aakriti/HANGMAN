#ifndef PROTOCOL_H //if protocol was already defined it will skip to below
#define PROTOCOL_H 
#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_LIVES 6  //we actually use 7 so not really enforced 
#define NUM_LEVELS 3
#define CMD_READY "READY"
#define CMD_GUESS "GUESS"
#define MSG_WELCOME "WELCOME" 
#define MSG_LEVEL "LEVEL"    
#define MSG_WORD "WORD"    
#define MSG_TIMER "TIMER"   
#define MSG_UPDATE "UPDATE"   
#define MSG_TIMEUP "TIME_UP"  
#define MSG_NEWWORD "NEW_WORD" 
#define MSG_WIN "WINNER"    
#define MSG_GAMEOVER "GAME_OVER"
#endif // Here <<<<<------


