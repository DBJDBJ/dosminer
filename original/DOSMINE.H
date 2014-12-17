#ifndef DOSMINE_H
#define DOSMINE_H
#undef TURBOC
/*
------------------------------------------------------------------
	(c)Apr 1994 DBJ*LTD(r)
------------------------------------------------------------------
*/
#define Terr( s ){\
 perror(__FILE__);\
 printf("\n\nERROR at line %d, because %s\n",__LINE__,(s));\
 exit(errno); }

/*
------------------------------------------------------------------
	console input/output and other commands for DOS and UNIX
------------------------------------------------------------------
*/
#ifndef __UNIX__
/* DOS console commands  */
#include <conio.h>
/*
------------------------------------------------------------------
	MSDOS Turbo C++ console commands
------------------------------------------------------------------
*/
#ifdef TURBOC

#define TEXTMODE40X24 textmode(C40)
#define TEXTMODE80X24 textmode(C80)
#define GOTOXY( x, y ) gotoxy((x),(y))
#define SET_SOLIDCURSOR _setcursortype(_SOLIDCURSOR)
#define SET_NORMALCURSOR _setcursortype(_NORMALCURSOR);
#define PUTCHAR( c ) putchar((c))
#define PRINT( s ) printf((s))
#define GETCH getch()
#define CLEAR_SCREEN  clrscr()
#define RANDOMIZE  randomize()

#else
/*
------------------------------------------------------------------
	WIN32 Console Fiddler
------------------------------------------------------------------
*/
#include <graph.h>

#define TEXTMODE40X24 (_setvideomode( _TEXTC40 ), _settextrows(25))
#define TEXTMODE80X24 (_setvideomode( _TEXTC80 ), _settextrows(25))
#define GOTOXY(x,y) (void)_settextposition( y, x)
#define SET_SOLIDCURSOR  (void)_settextcursor(0x0007)
#define SET_NORMALCURSOR (void)_settextcursor(0x0007)
#define PUTCHAR(c) _outtext((const char *)(c))
#define PRINT( s ) _outtext((s))
#define GETCH _inchar()
#define CLEAR_SCREEN _clearscreen( _GWINDOW )
#define RANDOMIZE  srand((unsigned)time(NULL))

#endif
/*
------------------------------------------------------------------
		define drawing characters from IBM-PC set
------------------------------------------------------------------
*/
/* appearance characters */
#define MARKEDTILE 0xB0
#define CLEARTILE  ' '
#define TILE       0xB2
#define MINETILE   0x02
/* frame drawing characters */
#define LEFT_TOP  0xDA
#define RIGHT_TOP 0xBF
#define HORIZ     0xC4
#define VERT      0xB3
#define LEFT_BOT  0xC0
#define RIGHT_BOT 0xD9
/*
------------------------------------------------------------------
	EOF PC SETUP PART
------------------------------------------------------------------
*/

#else
/*
------------------------------------------------------------------
	UNIX console commands
------------------------------------------------------------------
*/
#define TEXTMODE40X24
#define TEXTMODE80X24
#define GOTOXY( x, y )
#define SET_SOLIDCURSOR
#define SET_NORMALCURSOR
#define PUTCHAR( c ) putchar((c))
#define GETCH getch()
#define CLEAR_SCREEN  clrscr()
/*
------------------------------------------------------------------
   define some box-drawing characters from UNIX (ANSI?) set
------------------------------------------------------------------
*/
/* appearance characters */
#define MARKEDTILE 0xB0
#define CLEARTILE  ' '
#define TILE       0xB2
#define MINETILE   0x02
/* frame drawing characters */
#define LEFT_TOP  0xDA
#define RIGHT_TOP 0xBF
#define HORIZ     0xC4
#define VERT      0xB3
#define LEFT_BOT  0xC0
#define RIGHT_BOT 0xD9
#endif
/*
------------------------------------------------------------------
	game defaults and constants
------------------------------------------------------------------
*/
enum { MAXHGT = 22 , MAXWID = 37 } ;
const MAXBOMBS = MAXHGT * MAXWID / 5 ;
/*
------------------------------------------------------------------
	commands tokens
------------------------------------------------------------------
*/
typedef
enum tokenTAG {
	LEFT   ,
	RIGHT  ,
	UP     ,
	DOWN   ,
	QUIT   ,
	HELP   ,
	MARK   ,
	UNMARK ,
	CLEAR
} enmCmdToken ;

/*
------------------------------------------------------------------
	global	data structures
------------------------------------------------------------------
*/
typedef enum { FALSE = 0 , TRUE = 1 } Boolean ;

 struct  frameStruct
 {
    unsigned char    ul, ur, ll, lr, hr, vr ;
 } ;
/*
------------------------------------------------------------------
	Game layout is 2D array of following structures
------------------------------------------------------------------
*/
struct  layNode
 {
	Boolean mine ;
	unsigned char  appearance ;
	unsigned short minecounter ;
 } ;
/*
------------------------------------------------------------------
	 keeps last user position
------------------------------------------------------------------
*/
struct userStruct {	unsigned short lastX , lastY ; } ;

#endif /* DOSMINE_H */
