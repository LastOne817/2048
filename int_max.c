#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


/*
   Enum for color sets
   Foreground_Background
   */


#define RED_WHITE 1
#define RED_BLACK 2
#define BLACK_RED 3
#define WHITE_BLUE 4
#define WHITE_RED 5
#define WHITE_BLACK 6
#define RED_BLUE 7
#define RED_YELLOW 8


/*
   Enum for Menu Options
   */


#define START 0
#define LOAD 1
#define SCORE 2
#define SETUP 3
#define EXIT 4


/*
   Additional Menu Options
   Added in Project 2
   */


#define RESUME 5
#define NEWGAME 6
#define SAVEGAME 7



/*
   Menu Window Size
   Arbitrary
   */


#define MENU_WIDTH 30
#define MENU_HEIGHT 1


/*
   Score Window Size
   Arbitrary
   */


#define SCORE_WIDTH 100


/*
   Playboard Tile Size
   Arbitrary
   */


#define TILE_WIDTH 10
#define TILE_HEIGHT 5
#define TILE_SPACE 1


/*
   Ask Window Size
   Arbitrary
   */


#define ASK_WIDTH 60
#define ASK_HEIGHT 5


/*
   Escape Key
   Used for Press Input
   */


#define ESCAPE 27


/*
   Base number for new block
   Arbitrary
   */


#define BASE_NUM 1


/*
   Scoreset Struct
   Username, score, and time are saved
   */


typedef struct _scoreset
{
    char name[15];
    int score;
    char score_time[30];
}
SCORESET;


/*
   Gameset Struct
   Game status are saved
   */


typedef struct _gameset
{
    int height;
    int width;

    int num;
    int score;

    char game_time[30];

    int data[8][8];
}
GAMESET;


/*
   Ask Menu
   Returns user answer as Character(Y/N)
   */
char ask_menu(void);


/*
   Destroys ncurses window safely
   */
void destroy_win(WINDOW* win);


/*
   Initialize ncurses library
   Color initialization
   */
void init_ncurses(void);


/*
   Menu choosing stage
   Returns selected option
   */
int menu_stage(void);


/*
   Gameplay stage
   cur_score : current score (used for save/load)
   */
void game_stage(int cur_score);


/*
   Determines moving is possible for certain direction
   Returns whether possible
   */
int isMoveable(char dir);


/*
   Move tiles on board to given direction
   Returns whether success
   */
int moveTile(char dir);


/*
   Returns length of given number in base 10
   */
int numlen(int n);


/*
   Returns whether game is over
   */
int isGameOver(void);


/*
   Process gameover
   cur_score : current score (used for record save)
   */
void gameOver(int cur_score);


/*
   Gamesave Menu
   cur_score : current score (used for save)
   */
void save_menu(int cur_score);


/*
   Gameload menu
   Returns score
   */
int load_menu(void);


/*
   Score Menu
   Displays scoreboard
   */
void score_menu(void);


/*
   Setup Menu
   Can change tile size, number of tiles generated
   */
void setup_menu(void);


/*
   Swapping pain of scoreset
   Used for sorting scoreboard
   */
void swap_score(SCORESET *a, SCORESET *b);


/*
   Ingame popup submenu
   Returns user answer as Character(Y/N)
   */
int ingame_submenu(void);


/*
   Returns whether board is full with tiles
   */
int isTileFull(void);


/*
   Main Menu Title
   */
const char ascii_art[] = "\n\
 _   __  _   _____          __ __    __   __   __ \n\
| | |  \\| | |_   _|        |  V  |  /  \\  \\ \\_/ / \n\
| | | | ' |   | |    ____  | \\_/ | | /\\ |  > , <  \n\
|_| |_|\\__|   |_|   |____| |_| |_| |_||_| /_/ \\_\\ \n";


/*
   Tile Board
   Maximum 8 * 8
   */

int tile_data[8][8];

int tile_width=5;
int tile_height=4;

int tile_num=1;


SCORESET scoreboard[11];
GAMESET gamelist[11];


int score_cnt;


int
isTileFull(void)
{
    int i, j;


    for( i = 0 ; i < tile_height ; i++ )
    {
        for( j = 0 ; j < tile_width ; j++ )
        {
            if( tile_data[i][j] == 0 )
                return 0;
        }
    }
    return 1;
}


int
numlen(int n)
{
    int len = 0;


    do
    {
        n /= 10;
        len++;
    } while( n > 0 );


    return len;
}


char
ask_menu(void)
{
    WINDOW* menu;
    char msg[] = "Do you want to exit game INT_MAX? ( y / n )      ";
    int key;


    menu = subwin(stdscr, ASK_HEIGHT, ASK_WIDTH,
            (LINES-ASK_HEIGHT)/2, (COLS-ASK_WIDTH)/2);


    wclear(menu);


    box(menu, ACS_VLINE, ACS_HLINE);
    mvwprintw(menu, 2, 4, msg);


    wbkgd(menu, COLOR_PAIR(WHITE_RED));


    touchwin(stdscr);
    wrefresh(menu);
    
    while(1)
    {
        key = getch();

        if( key == 'y' ||
            key == 'Y' )
        {
            destroy_win(menu);
            return 'y';
        }

        else if(key == 'n' ||
                key == 'N')
        {
            destroy_win(menu);
            return 'n';
        }
    }
}


void
destroy_win(WINDOW* win)
{
    wborder(win, ' ',' ',' ',' ',' ',' ',' ',' ');
    wrefresh(win);
    delwin(win);
}


void
init_ncurses(void)
{
    FILE* scorefile;
    FILE* gamefile;
    int i,j,k;


    initscr();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();


    // 파일 열기
    scorefile = fopen("score.intmax","a+t");
    gamefile = fopen("gamesave.intmax","a+t");


    // 색상 초기화
    init_pair(RED_WHITE, COLOR_RED, COLOR_WHITE);
    init_pair(RED_BLACK, COLOR_RED, COLOR_BLACK);
    init_pair(BLACK_RED, COLOR_BLACK, COLOR_RED);
    init_pair(WHITE_BLUE, COLOR_WHITE, COLOR_BLUE);
    init_pair(WHITE_RED, COLOR_WHITE, COLOR_RED);
    init_pair(WHITE_BLACK, COLOR_WHITE, COLOR_BLACK);
    init_pair(RED_BLUE, COLOR_RED, COLOR_BLUE);
    init_pair(RED_YELLOW, COLOR_RED, COLOR_YELLOW);


    fscanf(scorefile, "%d", &score_cnt);
    fscanf(scorefile, "\n");

    for( i = 0 ; i < score_cnt ; i++ )
    {
        fscanf(scorefile, "%s %d %[^\n]s",
                scoreboard[i].name,
                &scoreboard[i].score,
                scoreboard[i].score_time);
        fscanf(scorefile, "\n");
    }


    for( i = 0 ; i < 10 ; i++ )
    {
        fscanf(gamefile, "%d %d %d %d %[^\n]s",
                &gamelist[i].height,
                &gamelist[i].width,
                &gamelist[i].num,
                &gamelist[i].score,
                gamelist[i].game_time);
        fscanf(gamefile, "\n");


        for( j = 0 ; j < gamelist[i].height ; j++ )
        {
            for( k = 0 ; k < gamelist[i].width ; k++ )
            {
                fscanf(gamefile, "%d", &(gamelist[i].data[j][k]));
            }
            fscanf(gamefile,"\n");
        }
        fscanf(gamefile,"\n");
    }


    fclose(gamefile);
    fclose(scorefile);


    srand(time(NULL));
}


int
isMoveable(char dir)
{
    int i, j, flg;

    switch(dir)
    {
        case 'U':
            for( i = 0 ; i < tile_width ; i++ )
            {
                flg = 0;


                for( j = 0 ; j < tile_height ; j++ )
                {
                    if( tile_data[j][i] == 0 )
                        flg = 1;


                    if( flg == 1 && tile_data[j][i] != 0 )
                        return 1;


                    if( j > 0 &&
                        tile_data[j][i] == tile_data[j-1][i] &&
                        tile_data[j][i] != 0 )
                        return 1;
                }   
            }
            return 0;
            break;


        case 'D':
            for( i = 0 ; i < tile_width ; i++ )
            {
                flg = 0;


                for( j = tile_height - 1 ; j >= 0 ; j-- )
                {
                    if( tile_data[j][i] == 0 )
                        flg = 1;


                    if( flg == 1 &&
                        tile_data[j][i] != 0 )
                        return 1;


                    if( j > 0 &&
                        tile_data[j][i] == tile_data[j-1][i] &&
                        tile_data[j][i] != 0 )
                        return 1;
                }   
            }
            return 0;
            break;


        case 'L':
            for( i = 0 ; i < tile_height ; i++ )
            {
                flg = 0;


                for( j = 0 ; j < tile_width ; j++ )
                {
                    if( tile_data[i][j] == 0 )
                        flg = 1;


                    if( flg == 1 &&
                        tile_data[i][j] != 0 )
                        return 1;


                    if( j > 0 &&
                        tile_data[i][j] == tile_data[i][j-1] &&
                        tile_data[i][j] != 0 )
                        return 1;
                }   
            }
            return 0;
            break;


        case 'R':
            for( i = 0 ; i < tile_height ; i++ )
            {
                flg = 0;


                for( j = tile_width - 1 ; j >= 0 ; j-- )
                {
                    if( tile_data[i][j] == 0 )
                        flg = 1;


                    if( flg == 1 &&
                        tile_data[i][j] != 0 )
                        return 1;


                    if( j > 0 &&
                        tile_data[i][j] == tile_data[i][j-1] &&
                        tile_data[i][j] != 0 )
                        return 1;
                }   
            }
            return 0;
            break;
    }
}


int
moveTile(char dir)
{
    int i, j, k, cnt;
    int line[8];
    int score=0;


    switch(dir)
    {
        case 'U':
            for( j = 0 ; j < tile_width ; j++ )
            {
                cnt = 0;


                for( k = 0 ; k<tile_height ; k++ )
                {
                    if(tile_data[k][j]!=0)
                        line[cnt++] = tile_data[k][j];
                }


                for(  ; cnt<tile_height ; cnt++ )
                {
                    line[cnt]=0;
                }


                for( k=1 ; k<cnt ; k++ )
                {
                    if( line[k-1] == line[k] &&
                        line[k]!=0)
                    {    
                        line[k-1]++;
                        k++;
                        score += (1<<line[k-2]);
                        line[k-1]=0;
                    }
                }


                cnt = 0;


                for( k=0 ; k<tile_height ; k++ )
                {
                    if(line[k]!=0)
                        tile_data[cnt++][j] = line[k];
                }


                for(  ; cnt<tile_height ; cnt++ )
                {
                    tile_data[cnt][j]=0;
                }
            }
            break;


        case 'R':
            for( j=0 ; j<tile_height ; j++ )
            {
                cnt=0;


                for( k=0 ; k<tile_width ; k++ )
                {
                    if(tile_data[j][tile_width-1-k]!=0)
                        line[cnt++] = tile_data[j][tile_width-1-k];
                }


                for(  ; cnt<tile_width ; cnt++ )
                {
                    line[cnt]=0;
                }


                for( k=1 ; k<cnt ; k++ )
                {
                    if( line[k-1] == line[k] &&
                            line[k]!=0)
                    {
                        line[k-1]++;
                        k++;
                        score += (1<<line[k-2]);
                        line[k-1]=0;
                    }
                }


                cnt=0;


                for( k=0 ; k<tile_width ; k++ )
                {
                    if(line[k]!=0)
                        tile_data[j][tile_width-1-cnt++] = line[k];
                }


                for(  ; cnt<tile_width ; cnt++ )
                {
                    tile_data[j][tile_width-1-cnt]=0;
                }
            }
            break;


        case 'D':
            for( j=0 ; j<tile_width ; j++ )
            {
                cnt=0;


                for( k=0 ; k<tile_height ; k++ )
                {
                    if(tile_data[tile_height-1-k][j]!=0)
                        line[cnt++] = tile_data[tile_height-1-k][j];
                }


                for(  ; cnt<tile_height ; cnt++ )
                {
                    line[cnt]=0;
                }


                for( k=1 ; k<cnt ; k++ )
                {
                    if( line[k-1] == line[k] &&
                            line[k]!=0)
                    {
                        line[k-1]++;
                        k++;
                        score += (1<<line[k-2]);
                        line[k-1]=0;
                    }
                }


                cnt=0;


                for( k=0 ; k<tile_height ; k++ )
                {
                    if(line[k]!=0)
                        tile_data[tile_height-1-cnt++][j] = line[k];
                }


                for(  ; cnt<tile_height ; cnt++ )
                {
                    tile_data[tile_height-1-cnt][j]=0;
                }
            }
            break;


        case 'L':
            for( j=0 ; j<tile_height ; j++ )
            {
                cnt=0;


                for( k=0 ; k<tile_width ; k++ )
                {
                    if(tile_data[j][k]!=0)
                        line[cnt++] = tile_data[j][k];
                }


                for(  ; cnt<tile_width ; cnt++ )
                {
                    line[cnt]=0;
                }


                for( k=1 ; k<cnt ; k++ )
                {
                    if( line[k-1] == line[k] &&
                            line[k]!=0)
                    {
                        line[k-1]++;
                        k++;
                        score += (1<<line[k-2]);
                        line[k-1]=0;
                    }
                }


                cnt=0;


                for( k=0 ; k<tile_width ; k++ )
                {
                    if(line[k]!=0)
                        tile_data[j][cnt++] = line[k];
                }


                for(  ; cnt<tile_width ; cnt++ )
                {
                    tile_data[j][cnt]=0;
                }
            }
            break;
    }


    for( k=0 ; k<tile_num && !isTileFull() ; k++ )
    {
        do
        {
            i = rand()%tile_height;
            j = rand()%tile_width;
        } while(tile_data[i][j]!=0);


        tile_data[i][j] = (rand()%10!=0)?BASE_NUM:BASE_NUM+1;
    }


    return score;
}


int
menu_stage(void)
{
    WINDOW *title;
    WINDOW *menu[6];


    char* menu_name[] = {"", " - START        ", " - LOAD         "," - SCORE         "," - SETUP         "," - EXIT         "};
    int key, cur_op = 0;


    erase();


    bkgd(COLOR_PAIR(WHITE_BLACK));
    title = subwin(stdscr, 10, 60, 0, (COLS-60)/2);
    menu[0] = subwin(stdscr, MENU_HEIGHT*7,MENU_WIDTH,
            (LINES-MENU_HEIGHT*4*5)/2,(COLS-MENU_WIDTH)/2);


    menu[1] = subwin(menu[0],
            MENU_HEIGHT,
            MENU_WIDTH-2,
            (LINES-MENU_HEIGHT*4*5)/2 + MENU_HEIGHT*1,
            (COLS-MENU_WIDTH)/2+1);
    menu[2] = subwin(menu[0],
            MENU_HEIGHT,
            MENU_WIDTH-2,
            (LINES-MENU_HEIGHT*4*5)/2 + MENU_HEIGHT*2,
            (COLS-MENU_WIDTH)/2+1);
    menu[3] = subwin(menu[0],
            MENU_HEIGHT,
            MENU_WIDTH-2,
            (LINES-MENU_HEIGHT*4*5)/2 + MENU_HEIGHT*3,
            (COLS-MENU_WIDTH)/2+1);
    menu[4] = subwin(menu[0],
            MENU_HEIGHT,
            MENU_WIDTH-2,
            (LINES-MENU_HEIGHT*4*5)/2 + MENU_HEIGHT*4,
            (COLS-MENU_WIDTH)/2+1);
    menu[5] = subwin(menu[0],
            MENU_HEIGHT,
            MENU_WIDTH-2,
            (LINES-MENU_HEIGHT*4*5)/2 + MENU_HEIGHT*5,
            (COLS-MENU_WIDTH)/2+1);


    wprintw(title, ascii_art);
    wprintw(menu[1], menu_name[1]);
    wprintw(menu[2], menu_name[2]);
    wprintw(menu[3], menu_name[3]);
    wprintw(menu[4], menu_name[4]);
    wprintw(menu[5], menu_name[5]);


    box(menu[0],ACS_VLINE,ACS_HLINE);


    wbkgd(title,COLOR_PAIR(WHITE_BLACK));
    wbkgd(menu[0],COLOR_PAIR(RED_WHITE));
    wbkgd(menu[1],COLOR_PAIR(RED_WHITE));
    wbkgd(menu[2],COLOR_PAIR(RED_WHITE));
    wbkgd(menu[3],COLOR_PAIR(RED_WHITE));
    wbkgd(menu[4],COLOR_PAIR(RED_WHITE));
    wbkgd(menu[5],COLOR_PAIR(RED_WHITE));


    wbkgd(menu[cur_op+1], COLOR_PAIR(WHITE_RED));


    touchwin(stdscr);
    refresh();


    while(1)
    {
        key = getch();


        wbkgd(menu[1],COLOR_PAIR(RED_WHITE));
        wbkgd(menu[2],COLOR_PAIR(RED_WHITE));
        wbkgd(menu[3],COLOR_PAIR(RED_WHITE));
        wbkgd(menu[4],COLOR_PAIR(RED_WHITE));
        wbkgd(menu[5],COLOR_PAIR(RED_WHITE));


        switch(key) {
            case KEY_UP:
                cur_op--;
                break;
            case KEY_DOWN:
                cur_op++;
                break;
            case '\n':
                destroy_win(menu[0]);
                destroy_win(menu[1]);
                destroy_win(menu[2]);
                destroy_win(menu[3]);
                destroy_win(menu[4]);
                destroy_win(menu[5]);
                erase();
                return cur_op;
                break;
            case 'q':
            case ESCAPE:
                return EXIT;
                break;
        }
        if(cur_op>=5)
            cur_op-=5;
        if(cur_op<0)
            cur_op+=5;


        wbkgd(menu[cur_op+1], COLOR_PAIR(WHITE_RED));


        touchwin(stdscr);
        refresh();
    }
}


int
isGameOver(void)
{
    int i,j;
    for( i=0 ; i<tile_height ; i++ )
    {
        for( j=0 ; j<tile_width ; j++ )
        {
            if(tile_data[i][j]==0)
                return 0;
        }
    }
    if(isMoveable('U'))
        return 0;


    if(isMoveable('D'))
        return 0;


    if(isMoveable('L'))
        return 0;


    if(isMoveable('R'))
        return 0;


    return 1;
}


void
swap_score(SCORESET *a, SCORESET *b)
{
    int tmp;
    char buf[40];


    tmp = (*a).score;
    (*a).score = (*b).score;
    (*b).score = tmp;


    strcpy(buf, (*a).name);
    strcpy((*a).name, (*b).name);
    strcpy((*b).name, buf);


    strcpy(buf, (*a).score_time);
    strcpy((*a).score_time, (*b).score_time);
    strcpy((*b).score_time, buf);
}


void
gameOver(int cur_score)
{
    char buf[30];
    WINDOW* menu;
    char* msg[] = {"   Game Over... Press Enter to return to menu      ","   Game Over... Enter your name : "};


    int key, i, j, max;


    time_t rawtime;


    FILE* scorefile;


    // 점수 파일 열기
    scorefile = fopen("score.intmax","wt");


    if( score_cnt<10 ||
            scoreboard[score_cnt-1].score < cur_score )
    {
        echo();
        menu = subwin(stdscr, ASK_HEIGHT, ASK_WIDTH,
                (LINES-ASK_HEIGHT)/2, (COLS-ASK_WIDTH)/2);


        wclear(menu);


        box(menu, ACS_VLINE, ACS_HLINE);


        while(1)
        {
            mvwprintw(menu,2,1,"%*s",ASK_WIDTH-2,"");
            mvwprintw(menu,2,4,msg[1]);


            wbkgd(menu, COLOR_PAIR(WHITE_RED));


            touchwin(stdscr);
            wrefresh(menu);


            mvwscanw(menu, 2, 4+strlen(msg[1]), "%[^\n]s",buf);
            if(strlen(buf)<=10 &&
                    strlen(buf)>0)
                break;
        }


        strcpy(scoreboard[score_cnt].name, buf);
        scoreboard[score_cnt].score = cur_score;
        time(&rawtime);


        strcpy(scoreboard[score_cnt].score_time, ctime(&rawtime));
        (scoreboard[score_cnt].score_time)[strlen(scoreboard[score_cnt].score_time)-1]='\0';


        noecho();


        score_cnt++;


        for( i=0 ; i<score_cnt ; i++ )
        {
            max = i;


            for( j=i+1 ; j<score_cnt ; j++ )
            {
                if(scoreboard[j].score > scoreboard[max].score)
                    max = j;
            }
            swap_score(&scoreboard[max], &scoreboard[i]);
        }


        if(score_cnt>10)
            score_cnt = 10;


        fprintf(scorefile,"%d\n",score_cnt);
        for( i=0 ; i<score_cnt ; i++ )
        {
            fprintf(scorefile,"%s %d %s\n",
                    scoreboard[i].name,
                    scoreboard[i].score,
                    scoreboard[i].score_time
                    );
        }


        fclose(scorefile);


        return;
    }
    else
    {
        menu = subwin(stdscr,
                ASK_HEIGHT,
                ASK_WIDTH,
                (LINES-ASK_HEIGHT)/2,
                (COLS-ASK_WIDTH)/2);


        wclear(menu);


        box(menu, ACS_VLINE, ACS_HLINE);
        mvwprintw(menu,2,4,msg[0]);


        wbkgd(menu, COLOR_PAIR(WHITE_RED));


        touchwin(stdscr);
        wrefresh(menu);


        while(1)
        {
            key = getch();


            switch(key)
            {
                case '\n':
                    return;
                    break;
            }
        }
    }
}


int
ingame_submenu(void)
{
    WINDOW* menu[5];
    char *menu_name[] = {"EXIT         ","RESUME         ","NEW GAME           ", "SAVE              ",""};
    int key, cur_op=RESUME;


    menu[4] = subwin(stdscr, MENU_HEIGHT*6, MENU_WIDTH,
            (LINES-MENU_HEIGHT*4*4)/2, (COLS-MENU_WIDTH)/2);


    menu[1] = subwin(menu[4],
            MENU_HEIGHT,
            MENU_WIDTH-2,
            (LINES-MENU_HEIGHT*4*4)/2 + MENU_HEIGHT*1,
            (COLS-MENU_WIDTH)/2+1);
    menu[2] = subwin(menu[4],
            MENU_HEIGHT,
            MENU_WIDTH-2,
            (LINES-MENU_HEIGHT*4*4)/2 + MENU_HEIGHT*2,
            (COLS-MENU_WIDTH)/2+1);
    menu[3] = subwin(menu[4],
            MENU_HEIGHT,
            MENU_WIDTH-2,
            (LINES-MENU_HEIGHT*4*4)/2 + MENU_HEIGHT*3,
            (COLS-MENU_WIDTH)/2+1);
    menu[0] = subwin(menu[4],
            MENU_HEIGHT,
            MENU_WIDTH-2,
            (LINES-MENU_HEIGHT*4*4)/2 + MENU_HEIGHT*4,
            (COLS-MENU_WIDTH)/2+1);


    wclear(menu[0]);
    wclear(menu[1]);
    wclear(menu[2]);
    wclear(menu[3]);
    wclear(menu[4]);


    wprintw(menu[1], menu_name[1]);
    wprintw(menu[2], menu_name[2]);
    wprintw(menu[3], menu_name[3]);
    wprintw(menu[0], menu_name[0]);


    box(menu[4], ACS_VLINE, ACS_HLINE);


    wbkgd(menu[0],COLOR_PAIR(RED_WHITE));
    wbkgd(menu[1],COLOR_PAIR(RED_WHITE));
    wbkgd(menu[2],COLOR_PAIR(RED_WHITE));
    wbkgd(menu[3],COLOR_PAIR(RED_WHITE));
    wbkgd(menu[4],COLOR_PAIR(RED_WHITE));


    wbkgd(menu[cur_op-4], COLOR_PAIR(WHITE_RED));
    touchwin(stdscr);
    refresh();
    while(1)
    {
        key = getch();


        wbkgd(menu[0],COLOR_PAIR(RED_WHITE));
        wbkgd(menu[1],COLOR_PAIR(RED_WHITE));
        wbkgd(menu[2],COLOR_PAIR(RED_WHITE));
        wbkgd(menu[3],COLOR_PAIR(RED_WHITE));


        switch(key)
        {
            case KEY_UP:
                cur_op--;
                break;
            case KEY_DOWN:
                cur_op++;
                break;
            case '\n':
                return cur_op;
                break;
        }
        if(cur_op>=8)
            cur_op-=4;
        if(cur_op<4)
            cur_op+=4;


        wbkgd(menu[cur_op-4], COLOR_PAIR(WHITE_RED));


        touchwin(stdscr);
        refresh();
    }
}


void
game_stage(int cur_score)
{
    WINDOW *frame, *tile[8][8], *title, *score[2];
    int i, j, k, key, best_score=0;

    int eggCount = 0;


    if(cur_score<0)
        return;


    if(score_cnt)
        best_score = scoreboard[0].score;


    while(1)
    {
        erase();


        bkgd(COLOR_PAIR(WHITE_BLACK));
        frame = subwin(stdscr,
                TILE_HEIGHT*tile_height+TILE_SPACE*(tile_height+1),
                TILE_WIDTH*tile_width+TILE_SPACE*(tile_width+1)*2,
                (LINES-(TILE_HEIGHT*tile_height+TILE_SPACE*(tile_height+1)))/2,
                (COLS-(TILE_WIDTH*tile_width+TILE_SPACE*(tile_width+1)*2))/2
                );
        title = subwin(stdscr, 10, 60, 0, (COLS-60)/2);


        score[0] = subwin(stdscr,
                MENU_HEIGHT*4,
                MENU_WIDTH,
                12,
                10);
        score[1] = subwin(stdscr,
                MENU_HEIGHT*4,
                MENU_WIDTH,
                12,
                COLS-MENU_WIDTH-10);


        wprintw(title,"%s",ascii_art);
        wclear(frame);
        mvwprintw(score[0],1,1,"Score: %d",cur_score);
        mvwprintw(score[1],1,1,"Best: %d",best_score);


        box(frame, ACS_VLINE, ACS_HLINE);
        box(score[0], ACS_VLINE, ACS_HLINE);
        box(score[1], ACS_VLINE, ACS_HLINE);


        wbkgd(title, COLOR_PAIR(WHITE_BLACK));
        wbkgd(frame, COLOR_PAIR(RED_BLUE));
        wbkgd(score[0], COLOR_PAIR(WHITE_BLACK));
        wbkgd(score[1], COLOR_PAIR(WHITE_BLACK));


        for( i=0 ; i<tile_height ; i++ )
        {
            for( j=0 ; j<tile_width ; j++ )
            {
                tile[i][j] = subwin(frame, TILE_HEIGHT, TILE_WIDTH,
                        (LINES-(TILE_HEIGHT*tile_height+TILE_SPACE*(tile_height+1)))/2 + TILE_SPACE*(i+1) + TILE_HEIGHT*i,
                        (COLS-(TILE_WIDTH*tile_width+TILE_SPACE*(tile_width+1)*2))/2 + TILE_SPACE*(j+1)*2 + TILE_WIDTH*j
                        );
                wbkgd(tile[i][j], COLOR_PAIR(RED_WHITE));
                if(tile_data[i][j]!=0)
                {
                    wbkgd(tile[i][j], COLOR_PAIR(RED_YELLOW));
                    mvwprintw(tile[i][j],2,(TILE_WIDTH-numlen(1<<tile_data[i][j]))/2,"%d",1<<tile_data[i][j]);
                }
            }
        }


        touchwin(stdscr);
        refresh();


        if(isGameOver())
        {
            gameOver(cur_score);
            return;
        }


        key = getch();


        switch(key)
        {
            case 'q':
                switch(ask_menu())
                {
                    case 'y':
                        return;


                    case 'n':
                        break;
                }
                break;


            case 'm':
                switch(ingame_submenu())
                {
                    case RESUME:
                        break;


                    case NEWGAME:
                        for( i=0 ; i<tile_height ; i++ )
                        {
                            for( j=0 ; j<tile_width ; j++ )
                            {
                                tile_data[i][j] = 0;
                            }
                        }
                        tile_data[rand()%tile_height][rand()%tile_width] = (rand()%10!=0)?BASE_NUM:BASE_NUM+1;
                        do
                        {
                            i = rand()%tile_height;
                            j = rand()%tile_width;
                        } while(tile_data[i][j]!=0);


                        tile_data[i][j] = (rand()%10!=0)?BASE_NUM:BASE_NUM+1;
                        cur_score = 0;
                        best_score = scoreboard[0].score;
                        break;


                    case SAVEGAME:
                        save_menu(cur_score);
                        break;


                    case EXIT:
                        return;
                        break;
                }
                break;


            case KEY_UP:
                if(isMoveable('U'))
                    cur_score += moveTile('U');
                else
                    if(eggCount >= 100)
                        cur_score *= 1.1;
                    eggCount++;
                break;


            case KEY_DOWN:
                if(isMoveable('D'))
                    cur_score += moveTile('D');
                else
                    if(eggCount >= 100)
                        cur_score *= 1.1;
                    eggCount++;
                break;


            case KEY_LEFT:
                if(isMoveable('L'))
                    cur_score += moveTile('L');
                else
                    if(eggCount >= 100)
                        cur_score *= 1.1;
                    eggCount++;
                break;


            case KEY_RIGHT:
                if(isMoveable('R'))
                    cur_score += moveTile('R');
                else
                    if(eggCount >= 100)
                        cur_score *= 1.1;
                    eggCount++;
                break;
        }
        if(cur_score > best_score)
            best_score = cur_score;
        touchwin(stdscr);
        refresh();
    }
}


void
setup_menu(void)
{
    WINDOW *title;
    WINDOW *menu[5];
    char* menu_name[] = {"", "  NUMBER OF ROWS : ", "  NUMBER OF COLS : ","  NUMBER OF RANDOM TILES : ","  OK"};
    int key, cur_op = 0;


    erase();


    bkgd(COLOR_PAIR(WHITE_BLACK));
    title = subwin(stdscr, 10, 60, 0, (COLS-60)/2);
    menu[0] = subwin(stdscr, MENU_HEIGHT*6,MENU_WIDTH*2,
            (LINES-MENU_HEIGHT*4*5)/2,(COLS-MENU_WIDTH*2)/2);


    menu[1] = subwin(menu[0], MENU_HEIGHT, MENU_WIDTH*2-2,
            (LINES-MENU_HEIGHT*4*5)/2 + MENU_HEIGHT*1, (COLS-MENU_WIDTH*2)/2+1);
    menu[2] = subwin(menu[0], MENU_HEIGHT, MENU_WIDTH*2-2,
            (LINES-MENU_HEIGHT*4*5)/2 + MENU_HEIGHT*2, (COLS-MENU_WIDTH*2)/2+1);
    menu[3] = subwin(menu[0], MENU_HEIGHT, MENU_WIDTH*2-2,
            (LINES-MENU_HEIGHT*4*5)/2 + MENU_HEIGHT*3, (COLS-MENU_WIDTH*2)/2+1);
    menu[4] = subwin(menu[0], MENU_HEIGHT, MENU_WIDTH*2-2,
            (LINES-MENU_HEIGHT*4*5)/2 + MENU_HEIGHT*4, (COLS-MENU_WIDTH*2)/2+1);


    wprintw(title, ascii_art);
    mvwprintw(menu[1],0,0, "%s%d",menu_name[1],tile_height);
    mvwprintw(menu[2],0,0, "%s%d",menu_name[2],tile_width);
    mvwprintw(menu[3],0,0, "%s%d",menu_name[3],tile_num);
    mvwprintw(menu[4],0,0, "%s",menu_name[4]);


    box(menu[0],ACS_VLINE,ACS_HLINE);


    wbkgd(title,COLOR_PAIR(WHITE_BLACK));
    wbkgd(menu[0],COLOR_PAIR(RED_WHITE));
    wbkgd(menu[1],COLOR_PAIR(RED_WHITE));
    wbkgd(menu[2],COLOR_PAIR(RED_WHITE));
    wbkgd(menu[3],COLOR_PAIR(RED_WHITE));
    wbkgd(menu[4],COLOR_PAIR(RED_WHITE));


    wbkgd(menu[cur_op+1], COLOR_PAIR(WHITE_RED));


    touchwin(stdscr);
    refresh();


    while(1)
    {
        key = getch();


        switch(key)
        {
            case KEY_UP:
                cur_op--;
                break;


            case KEY_DOWN:
                cur_op++;
                break;


            case KEY_LEFT:
                if(cur_op==0)
                {
                    if(tile_height>4)
                        tile_height--;
                }
                else if(cur_op==1)
                {
                    if(tile_width>4)
                        tile_width--;
                }
                else if(cur_op==2)
                {
                    if(tile_num>1)
                        tile_num--;
                }
                break;


            case KEY_RIGHT:
                if(cur_op==0)
                {
                    if(tile_height<8)
                        tile_height++;
                }
                else if(cur_op==1)
                {
                    if(tile_width<8)
                        tile_width++;
                }
                else if(cur_op==2)
                {
                    if(tile_num<4)
                        tile_num++;
                }
                break;


            case '\n':
                if(cur_op==3)
                {
                    destroy_win(menu[0]);
                    destroy_win(menu[1]);
                    destroy_win(menu[2]);
                    destroy_win(menu[3]);
                    destroy_win(menu[4]);


                    erase();
                    return;
                    break;
                }
        }
        if(cur_op>=4)
            cur_op-=4;
        if(cur_op<0)
            cur_op+=4;


        mvwprintw(menu[1],0,0, "%s%d",menu_name[1],tile_height);
        mvwprintw(menu[2],0,0, "%s%d",menu_name[2],tile_width);
        mvwprintw(menu[3],0,0, "%s%d",menu_name[3],tile_num);
        mvwprintw(menu[4],0,0, "%s",menu_name[4]);


        wbkgd(menu[1],COLOR_PAIR(RED_WHITE));
        wbkgd(menu[2],COLOR_PAIR(RED_WHITE));
        wbkgd(menu[3],COLOR_PAIR(RED_WHITE));
        wbkgd(menu[4],COLOR_PAIR(RED_WHITE));


        wbkgd(menu[cur_op+1], COLOR_PAIR(WHITE_RED));


        touchwin(stdscr);
        refresh();
    }
}


void
score_menu(void)
{
    WINDOW *title;
    WINDOW *menu[12];
    int key, cur_op, i;


    cur_op = 0;


    erase();


    bkgd(COLOR_PAIR(WHITE_BLACK));
    title = subwin(stdscr, 10, 60, 0, (COLS-60)/2);


    menu[0] = subwin(stdscr, MENU_HEIGHT*13,SCORE_WIDTH,
            (LINES-MENU_HEIGHT*13)/2,(COLS-SCORE_WIDTH)/2);
    for( i=1 ; i<=10 ; i++ )
    {
        menu[i] = subwin(menu[0], MENU_HEIGHT, SCORE_WIDTH-2,
                (LINES-MENU_HEIGHT*13)/2 + MENU_HEIGHT*i, (COLS-SCORE_WIDTH)/2+1);
    }
    menu[11] = subwin(stdscr, MENU_HEIGHT,SCORE_WIDTH-2,
            (LINES-MENU_HEIGHT*13)/2 + MENU_HEIGHT*11, (COLS-SCORE_WIDTH)/2+1);


    wprintw(title, ascii_art);


    for( i=0 ; i<score_cnt ; i++ )
    {
        wprintw(menu[i+1], "%d. Name : %15s / Score : %15d @ %s",i+1,scoreboard[i].name,scoreboard[i].score,scoreboard[i].score_time);
    }
    for( ; i<10 ; i++ )
    {
        wprintw(menu[i+1], "----------");
    }
    wprintw(menu[11],"OK");


    box(menu[0],ACS_VLINE,ACS_HLINE);


    wbkgd(title,COLOR_PAIR(WHITE_BLACK));
    for( i=0 ; i<=11 ; i++ )
    {
        wbkgd(menu[i],COLOR_PAIR(RED_WHITE));
    }
    wbkgd(menu[cur_op+1], COLOR_PAIR(WHITE_RED));


    touchwin(stdscr);
    refresh();


    while(1)
    {
        key = getch();


        for( i=1 ; i<=11 ; i++ )
        {
            wbkgd(menu[i],COLOR_PAIR(RED_WHITE));
        }


        switch(key)
        {
            case KEY_UP:
                cur_op--;
                break;


            case KEY_DOWN:
                cur_op++;
                break;


            case '\n':
                if(cur_op==10)
                {
                    for( i=0 ; i<=11 ; i++ )
                    {
                        destroy_win(menu[i]);
                    }
                    erase();
                    return;
                }
                break;
        }


        if(cur_op>=11)
            cur_op-=11;
        if(cur_op<0)
            cur_op+=11;


        wbkgd(menu[cur_op+1], COLOR_PAIR(WHITE_RED));


        touchwin(stdscr);
        refresh();
    }
}


void
save_menu(int cur_score)
{
    FILE* gamefile;
    WINDOW *title;
    WINDOW *menu[12];
    int key, cur_op, i,j,k;
    time_t rawtime;


    cur_op = 0;


    erase();


    bkgd(COLOR_PAIR(WHITE_BLACK));
    title = subwin(stdscr, 10, 60, 0, (COLS-60)/2);


    menu[0] = subwin(stdscr, MENU_HEIGHT*13,SCORE_WIDTH,
            (LINES-MENU_HEIGHT*13)/2,(COLS-SCORE_WIDTH)/2);
    for( i=1 ; i<=10 ; i++ )
    {
        menu[i] = subwin(menu[0], MENU_HEIGHT, SCORE_WIDTH-2,
                (LINES-MENU_HEIGHT*13)/2 + MENU_HEIGHT*i, (COLS-SCORE_WIDTH)/2+1);
    }
    menu[11] = subwin(stdscr, MENU_HEIGHT,SCORE_WIDTH-2,
            (LINES-MENU_HEIGHT*13)/2 + MENU_HEIGHT*11, (COLS-SCORE_WIDTH)/2+1);


    wprintw(title, ascii_art);
    for( i=0 ; i<10 ; i++ )
    {
        if(gamelist[i].height==0 &&
                gamelist[i].width==0)
            wprintw(menu[i+1], " (Empty Slot)");
        else
            wprintw(menu[i+1], "Grid : %2d x %2d / Random : %d / %15d Points @ %s\n",
                    gamelist[i].height,
                    gamelist[i].width,
                    gamelist[i].num,
                    gamelist[i].score,
                    gamelist[i].game_time
                    );
    }
    wprintw(menu[11],"EXIT");


    box(menu[0],ACS_VLINE,ACS_HLINE);


    wbkgd(title,COLOR_PAIR(WHITE_BLACK));
    for( i=0 ; i<=11 ; i++ )
    {
        wbkgd(menu[i],COLOR_PAIR(RED_WHITE));
    }
    wbkgd(menu[cur_op+1], COLOR_PAIR(WHITE_RED));


    touchwin(stdscr);
    refresh();


    while(1)
    {
        key = getch();


        for( i=1 ; i<=11 ; i++ )
        {
            wbkgd(menu[i],COLOR_PAIR(RED_WHITE));
        }


        switch(key)
        {
            case KEY_UP:
                cur_op--;
                break;


            case KEY_DOWN:
                cur_op++;
                break;


            case '\n':
                if(cur_op==10)
                {
                    for( i=0 ; i<=11 ; i++ )
                    {
                        destroy_win(menu[i]);
                    }
                    erase();
                    return;
                }
                else
                {
                    gamelist[cur_op].height = tile_height;
                    gamelist[cur_op].width = tile_width;
                    gamelist[cur_op].num = tile_num;
                    for( i=0 ; i<tile_height ; i++ )
                    {
                        for( j=0 ; j<tile_width ; j++ )
                        {
                            gamelist[cur_op].data[i][j] = tile_data[i][j];
                        }   
                    }
                    gamelist[cur_op].score = cur_score;
                    time(&rawtime);
                    strcpy(gamelist[cur_op].game_time, ctime(&rawtime));
                    gamelist[cur_op].game_time[strlen(gamelist[cur_op].game_time)-1] = '\0';
                    gamefile = fopen("gamesave.intmax","wt");


                    for( i=0 ; i<10 ; i++ )
                    {
                        if(gamelist[i].height==0 &&
                            gamelist[i].width==0)
                        {
                            fprintf(gamefile,"0 0 0 0\n");
                        }
                        else {
                            fprintf(gamefile,"%d %d %d %d %s\n",
                                    gamelist[i].height,
                                    gamelist[i].width,
                                    gamelist[i].num,
                                    gamelist[i].score,
                                    gamelist[i].game_time
                                    );
                            for( j=0 ; j<gamelist[i].height ; j++ )
                            {
                                for( k=0 ; k<gamelist[i].width ; k++ )
                                {
                                    fprintf(gamefile,"%d ",gamelist[i].data[j][k]);
                                }
                                fprintf(gamefile,"\n");
                            }   
                        }
                    }
                    
                    fclose(gamefile);


                    return;
                }
                break;
        }
        if(cur_op>=11)
            cur_op-=11;
        if(cur_op<0)
            cur_op+=11;


        wbkgd(menu[cur_op+1], COLOR_PAIR(WHITE_RED));


        touchwin(stdscr);
        refresh();
    }
}


int
load_menu(void)
{
    WINDOW *title;
    WINDOW *menu[12];
    int key, cur_op = 0, i,j;


    erase();


    bkgd(COLOR_PAIR(WHITE_BLACK));
    title = subwin(stdscr, 10, 60, 0, (COLS-60)/2);


    menu[0] = subwin(stdscr, MENU_HEIGHT*13,SCORE_WIDTH,
            (LINES-MENU_HEIGHT*13)/2,(COLS-SCORE_WIDTH)/2);
    for( i=1 ; i<=10 ; i++ ) {
        menu[i] = subwin(menu[0], MENU_HEIGHT, SCORE_WIDTH-2,
                (LINES-MENU_HEIGHT*13)/2 + MENU_HEIGHT*i, (COLS-SCORE_WIDTH)/2+1);
    }
    menu[11] = subwin(stdscr, MENU_HEIGHT,SCORE_WIDTH-2,
            (LINES-MENU_HEIGHT*13)/2 + MENU_HEIGHT*11, (COLS-SCORE_WIDTH)/2+1);


    wprintw(title, ascii_art);
    for( i=0 ; i<10 ; i++ )
    {
        if(gamelist[i].height==0 &&
                gamelist[i].width==0)
            wprintw(menu[i+1], " (Empty Slot)");
        else
            wprintw(menu[i+1], "Grid : %2d x %2d / Random : %d / %15d Points @ %s\n",
                    gamelist[i].height,
                    gamelist[i].width,
                    gamelist[i].num,
                    gamelist[i].score,
                    gamelist[i].game_time
                    );
    }
    wprintw(menu[11],"EXIT");


    box(menu[0],ACS_VLINE,ACS_HLINE);


    wbkgd(title,COLOR_PAIR(WHITE_BLACK));
    for( i=0 ; i<=11 ; i++ )
    {
        wbkgd(menu[i],COLOR_PAIR(RED_WHITE));
    }
    wbkgd(menu[cur_op+1], COLOR_PAIR(WHITE_RED));


    touchwin(stdscr);
    refresh();


    while(1)
    {
        key = getch();


        for( i=1 ; i<=11 ; i++ )
        {
            wbkgd(menu[i],COLOR_PAIR(RED_WHITE));
        }


        switch(key)
        {
            case KEY_UP:
                cur_op--;
                break;


            case KEY_DOWN:
                cur_op++;
                break;


            case '\n':
                if(cur_op==10)
                {
                    for( i=0 ; i<=11 ; i++ )
                    {
                        destroy_win(menu[i]);
                    }


                    erase();
                    return -1;
                }
                else if(gamelist[cur_op].height!=0 &&
                        gamelist[cur_op].width!=0)
                {
                    tile_height = gamelist[cur_op].height;
                    tile_width = gamelist[cur_op].width;
                    tile_num = gamelist[cur_op].num;


                    for( i=0 ; i<tile_height ; i++ )
                    {
                        for( j=0 ; j<tile_width ; j++ )
                        {
                            tile_data[i][j] = gamelist[cur_op].data[i][j];
                        }   
                    }
                    return gamelist[i].score;
                }
                break;
        }
        if(cur_op>=11)
            cur_op-=11;
        if(cur_op<0)
            cur_op+=11;


        wbkgd(menu[cur_op+1], COLOR_PAIR(WHITE_RED));


        touchwin(stdscr);
        refresh();
    }
}

int
main(void){
    int i,j;


    init_ncurses();


    while(1)
    {
        switch(menu_stage())
        {
            case START:
                for( i=0 ; i<tile_height ; i++ )
                {
                    for( j=0 ; j<tile_width ; j++ )
                    {
                        tile_data[i][j] = 0;
                    }
                }
                tile_data[rand()%tile_height][rand()%tile_width] = (rand()%10!=0) ? BASE_NUM:BASE_NUM+1;
                do
                {
                    i = rand()%tile_height;
                    j = rand()%tile_width;
                }while(tile_data[i][j]!=0);
                tile_data[i][j] = (rand()%10!=0)?BASE_NUM:BASE_NUM+1;


                game_stage(0);
                break;


            case LOAD:
                game_stage(load_menu());
                break;


            case SCORE:
                score_menu();
                break;


            case SETUP:
                setup_menu();
                break;


            case EXIT:
                endwin();
                return 0;
        }
    }
}
