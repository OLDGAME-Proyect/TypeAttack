#ifdef _WIN32
    #include <curses.h>   
#else
    #include <ncurses.h>  
#endif

#include <stdlib.h>
#include <time.h>
#include "game.h"

int main(void)
{
    // init ncurses
    initscr();             
    cbreak();              
    noecho();               
    curs_set(0);            
    nodelay(stdscr, TRUE);  
    keypad(stdscr, TRUE);   

    srand(time(NULL)); 
    
    
    //menu
    
    int choice = menu();
    if (choice == 1) {
       endwin();
        return 0;
    }
   //
    
    
    //============================config========================
    // load config from fikle
   char difficulty[8] = "00";  // default

     FILE *cfg = fopen("config", "r");
     if (cfg) {
       fscanf(cfg, "type=%7s", difficulty);
        fclose(cfg);
    } else {
        cfg = fopen("config", "w");
        fprintf(cfg, "type=00");
        fclose(cfg);
    }
    //===============================================================     

    // go to main loop
    game_loop(difficulty);

    // end ncurses
    endwin();

    return 0;
}
