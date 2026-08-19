#ifdef _WIN32
    #include <curses.h>   
#else
    #include <ncurses.h>  
#endif

#ifdef _WIN32
    #include <windows.h>   // Sleep()
#else
    #include <unistd.h>    // usleep()
#endif

#include "game.h"
#include "render.h"

//--------------------------------game loop----------------------
void game_loop(const char *difficulty)
{
    int running = 1;

    
    int sentence_x = 2;
    int sentence_y = 2;
    
    
    // get screen limits
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    int min_line = max_y - 3;

    Sentence s = { "HELLO WORLD", 2, 2 };
    
    
    
// calculate fall x difficulty
int fall_time_ms = 0;

if (strcmp(difficulty, "00") == 0) {
    fall_time_ms = 20000; // 20s
}
else if (strcmp(difficulty, "01") == 0) {
    fall_time_ms = (rand() % 20 + 10) * 1000; // unkw
}
else if (strcmp(difficulty, "02") == 0) {
    int letters = strlen(s.text);
    int words = 1;
    for (int i = 0; s.text[i]; i++)
        if (s.text[i] == ' ') words++;

    fall_time_ms = 20000 - (letters * 1000) - (words * 2000);
}
else if (strcmp(difficulty, "03") == 0) {
    int letters = strlen(s.text);
    int words = 1;
    for (int i = 0; s.text[i]; i++)
        if (s.text[i] == ' ') words++;

    fall_time_ms = 100000 - (words * 4000) - (letters * 2000);
}

// convert time to frame
int total_frames = fall_time_ms / 16; // 16ms por frame
float fall_speed = (float)(min_line - 2) / total_frames;

    while (running)
    {

        render_clear();


        render_sentence(s.text, s.x, s.y);
        
        
        render_input_bar();
        

        //  input sin bloquear
        int ch = getch();
        if (ch == 'q') {
            running = 0; 
        }

        
        render_present();
        

    
        // fall
        s.y += fall_speed * 5.0f;



        // restart
        if (s.y > min_line) {
            s.y = 2; 
            s.text = "[TEXTO BLOCK]";
        }



        // FPSS (60 FPS aprox)
       #ifdef _WIN32
           Sleep(16);
       #else
           usleep(16000);
       #endif

    }
}
//--------------------------------------------------------------------------------



