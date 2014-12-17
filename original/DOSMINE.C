/*
 ----------------------------------------------------------
    (c) Apr 1994 by DBJ*LTD
 ----------------------------------------------------------
*/

/* #undef _DEBUG */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include "arr.h"
#include "dosmine.h"
#include "console.h"
/*
 ----------------------------------------------------------
	Defines, typedefs, enums
 ----------------------------------------------------------
*/
typedef unsigned short  TxtCoord ;
/*
 ----------------------------------------------------------
	Declarations
 ----------------------------------------------------------
*/
static void info( const char *title, const char *status ) ;
static int      userPlay ( void ) ;
static Boolean  inFrame ( const TxtCoord,const TxtCoord,
						  const TxtCoord,const TxtCoord ) ;
static void     computeMinecount( TxtCoord col, TxtCoord row ) ;
static void     computeLayout(const unsigned int bombNumber ) ;
static void     drawLayout(void) ;
static void     revealLayout(void) ;
static short    userKey ( void );
void            clearAdjacent ( int x, int y );
static void     setNodeApp ( struct layNode *nP );
static void     prtNode ( TxtCoord col, TxtCoord row, struct layNode *nP );
static void     posPlayer( void ) ;
static void     drawBox ( const TxtCoord left, const TxtCoord top,
						  const TxtCoord right, const TxtCoord bottom );
static void     resetLayout() ;

/*
 ----------------------------------------------------------
	Globals
 ----------------------------------------------------------
*/
typedef struct layNode ARRnode ;

unsigned int        bombNumber = 0 , WDT = MAXWID, HGT = MAXHGT ;
/*
struct frameStruct  frameLook = { '', '', '', '', '', '', } ;
*/
ARRnode 	    **layout = NULL ;
Boolean             finished = FALSE ;
TxtCoord            Xorg = 2 ,
					Yorg = 2 ;
struct userStruct   player = { 4 , 2 } ; /* initial position */
/*
 ----------------------------------------------------------
	Main
 ----------------------------------------------------------
*/
int main ( int argc, char *argv[] )
{
	if ( ! strcmp( "/?", argv[1])) /* little help */
	   {
	printf("\nUSAGE: %s <width> <height> [<bomb number>]",argv[0]);
	printf("\n       2 < width < %d, 2 < height < %d",MAXWID,MAXHGT);
	printf("\n       Default bomb number = width * height / 5.0\n");
		exit(0);
	   }                        
	
	if ( argc >= 2 ) 
	   WDT = atoi(argv[1]) % MAXWID + 1;
	else
	   WDT = MAXWID ;
	   
	if ( argc >= 3 )
	   HGT = atoi(argv[2]) % MAXHGT +1 ;
	else
	   HGT = MAXHGT ;

    if ( argc >= 4 )
		bombNumber = atoi(argv[3]) % MAXBOMBS + 1 ;
	else                                         
		bombNumber = (int)(WDT * HGT / 5.0);
	    
		if ( WDT < 2 ) WDT = 3  ;
		if ( HGT < 2 ) HGT = 3  ;
		if ( bombNumber < 3 ) bombNumber = 3 ;

	   layout =(ARRnode **)ARRdim(WDT,HGT,sizeof(ARRnode));

	/*
		Draw initial layout 
	*/
	switchTo40() ;
	SET_SOLIDCURSOR ;        /* Switch to a solid cursor */
	CLEAR_SCREEN ;
	drawBox( Xorg-1, Yorg -1 , Xorg+MAXWID, Yorg+MAXHGT);
	drawBox( Xorg-1, Yorg -1 , Xorg+WDT, Yorg+HGT);
again:
	#ifndef _DEBUG
		info( NULL, NULL ); /* default messages */
	#endif

	resetLayout(); /* no mines, no numbers all tiles appearance */

	drawLayout() ; /* draw the initial tiles layout, all covered */

	computeLayout( bombNumber ) ; /* calculate the layout for this run */

	posPlayer() ; /* initial cursor position */

	finished = FALSE ; /* global */
	/*
		main game loop
	*/
	while ( ! finished )
	{
	  (void)userPlay() ;
	}
	revealLayout() ; /* show true layout, all un-covered */

	posPlayer() ;  /* keep cursor on deadly position */

	info( "GAME OVER","[Q]uit [A]gain") ;
	switch(GETCH) /* again maybe ? */
	{
		case 'a':
		case 'A' : goto again ;
		default:  break;
	}

	SET_NORMALCURSOR ;
	TEXTMODE80X24 ;
	CLEAR_SCREEN ;
	return ARRfree(WDT,HGT,(void **)layout);
}
/*
------------------------------------------------------------------
 player commands interpreter
------------------------------------------------------------------
*/
static int userPlay ( void )
{
	switch ( userKey())
	{
	  case LEFT :
	player.lastX = player.lastX == 0 ? WDT-1 : --player.lastX ;
			posPlayer() ;
			return TRUE ;
	  case RIGHT :
		player.lastX = player.lastX == WDT-1 ? 0 : ++player.lastX ;
			posPlayer() ;
			return TRUE ;
	  case UP:
	player.lastY = player.lastY == 0 ? HGT-1 : --player.lastY ;
			posPlayer() ;
			return TRUE ;
	  case DOWN :
		player.lastY = player.lastY == HGT-1 ? 0 : ++player.lastY ;
			posPlayer() ;
				return TRUE ;
	  case QUIT :
		   finished = TRUE ;
			return TRUE ;
	  case HELP :
		   break ;

	 case MARK :

	 if ( layout[player.lastX][player.lastY].appearance == TILE )
		  layout[player.lastX][player.lastY].appearance = MARKEDTILE ;
	 else
	 if ( layout[player.lastX][player.lastY].appearance == MARKEDTILE )
		  layout[player.lastX][player.lastY].appearance = TILE ;

			 prtNode( player.lastX,player.lastY,
			         &layout[player.lastX][player.lastY]);

		 return TRUE ;

	case CLEAR :
		   if ( layout[player.lastX][player.lastY].mine )
		   /* player stepped on mine */
		   {
			 finished = TRUE ;
		   }
		   else
		   if ( layout[player.lastX][player.lastY].minecounter == 0 )
			clearAdjacent( player.lastX, player.lastY ) ;

			 setNodeApp(&layout[player.lastX][player.lastY]) ; 
			 prtNode( player.lastX,player.lastY,
			         &layout[player.lastX][player.lastY]);
		return TRUE ;
	default :
				 /* unknown command ? */
			 exit(1);
		 return FALSE ;
		}


 return TRUE ;
}
/*
--------------------------------------------------------------------------
	If tile with no mine, or no number revealed, clear all
	adjacent clean tiles ending with ones with numbers
--------------------------------------------------------------------------
*/
void clearAdjacent ( const int x, const int y )
{
	 if ( inFrame( x,y,WDT,HGT))    /* in the frame      */
	 if ( ! layout[x][y].mine )     /* and no mine under */
	 if ( layout[x][y].appearance == TILE ) /* is it visited yet ? */
	  {
		 setNodeApp( &layout[x][y] ) ; /* real appearance */

		 prtNode( x,y, &layout[x][y] ) ;

	 if ( layout[x][y].minecounter == 0 )   /* clear tile        */
		{
				clearAdjacent( x-1,y  );
				clearAdjacent( x+1,y  );
				clearAdjacent( x+1,y-1);
				clearAdjacent( x  ,y-1);
				clearAdjacent( x-1,y-1);
				clearAdjacent( x-1,y+1);
				clearAdjacent( x  ,y+1);
				clearAdjacent( x+1,y+1);
		 } /* if */
          }
}
/*
--------------------------------------------------------------------------
	Plant the bombs , mark each field with number of
	adjacent bombs and make them all appear as solid
	tiles
	Called at the game beginning
	Game field is a 2D array of structures...
--------------------------------------------------------------------------
*/
static void computeLayout(const unsigned int bombNumber )
{
 TxtCoord    i = 0, col = 0, row = 0 ;

	 /*
	 first plant the bombs
	 */
	 RANDOMIZE;
	 for ( i = 1 ; i < bombNumber ; i++ )
	 {
	  do {  col = rand() % WDT ;
			row = rand() % HGT ;
	  } while (layout[col][row].mine == TRUE ) ;
		  
	  layout[col][row].mine = TRUE ;
		  #ifdef _DEBUG
				setNodeApp(&layout[col][row]) ;
				prtNode(col,row, &layout[col][row]) ;
		  #endif

	 }
	 /*
	 then place the number markers
	 */
	 for ( col = 0 ; col < WDT ; col++)
	   for ( row = 0 ; row < HGT ; row++)
	   {
		if ( layout[col][row].mine == TRUE )
		{
		   #ifdef _DEBUG
				GOTOXY( col, row ) ;
				setNodeApp(&layout[col][row]) ;
				prtNode(col,row, &layout[col][row]) ;
				while( GETCH != ' ' );
		  #endif
			computeMinecount( col , row ) ;

		  }
	   }
} /* end of computeLayout */
/*
--------------------------------------------------------------
	For-mined tile, increment all surrounding non-mined tiles
	mine counter
--------------------------------------------------------------
*/
static void computeMinecount( TxtCoord col, TxtCoord row )
{
   short    x, y ;

	for ( x = -1 ; x < 2 ; x++ )

	for ( y = -1 ; y < 2 ; y++ )
	 
	if ( x || y )
	
	if ( inFrame( col+x, row+y, WDT, HGT))
	{
	if ( layout[col+x][row+y].mine != TRUE )
		 layout[col+x][row+y].minecounter += 1 ;

	#ifdef _DEBUG
		   GOTOXY( col+x, row+y ) ;
		   setNodeApp(&layout[col+x][row+y]) ;
		   prtNode(col+x,row+y, &layout[col+x][row+y]) ;
		   while( GETCH != ' ' );
	#endif
	}
}
/*
--------------------------------------------------------------
	 called at game end 
--------------------------------------------------------------
*/
static void revealLayout()
{
	TxtCoord col, row ;

	 for ( col = 0 ; col < WDT ; col++)
	 for ( row = 0 ; row < HGT ; row++)
	 {
	   GOTOXY( Xorg + col, Yorg + row ) ;

		if ( layout[col][row].mine == TRUE )
			 printf ("%c", MINETILE ) ;
		else if ( layout[col][row].minecounter == 0 )
			 printf ("%c", CLEARTILE ) ;
		else
			 printf ("%d",layout[col][row].minecounter ) ;
	 }
}
/*
--------------------------------------------------------------------------
	 Move cursor where user last played
--------------------------------------------------------------------------
*/
static void posPlayer( void )
{
	GOTOXY( Xorg + player.lastX, Yorg + player.lastY );
}
/*
--------------------------------------------------------------------------
	setup appearance of the layout node, by its 
	mine variable value  
--------------------------------------------------------------------------
*/
static void setNodeApp ( struct layNode *nP )
{
	if ( nP->mine )
		 nP->appearance = MINETILE ;
	else if ( nP->minecounter == 0 )
		 nP->appearance = CLEARTILE ;
	else
		 nP->appearance = (unsigned char)('0' + nP->minecounter);
}

/*
--------------------------------------------------------------
	print out node appearance, on given position by 
	getting appearance from the node itself
--------------------------------------------------------------
*/
static void prtNode ( const TxtCoord col, const TxtCoord row,
			  struct layNode *nP )
{   GOTOXY( Xorg+col, Yorg+row ) ;
	PUTCHAR(nP->appearance ) ;        /* moves cursor right */
	GOTOXY( Xorg+col, Yorg+row ) ;    /* cursor back */
}
/*
--------------------------------------------------------------
	Show each node of the layout, as it is
--------------------------------------------------------------
*/
static void drawLayout()
{
	TxtCoord col, row ;

	 for ( col = 0 ; col < WDT ; col++)
	 for ( row = 0 ; row < HGT ; row++)
	   prtNode(col,row, &layout[col][row]) ;
}
/*
--------------------------------------------------------------
	No mines, no counters , all tiles
--------------------------------------------------------------
*/
static void resetLayout()
{
	TxtCoord col, row ;

	 for ( col = 0 ; col < WDT ; col++)
	 for ( row = 0 ; row < HGT ; row++)
	 {
	   layout[col][row].mine = FALSE ;
	   layout[col][row].minecounter = 0 ;
	   layout[col][row].appearance = TILE ;
	 }
}
/*
------------------------------------------------------------------
	is text coordinate inside given frame ?
	Frames UL is always 0,0 so :
------------------------------------------------------------------
*/
static Boolean inFrame( const TxtCoord x,
			const TxtCoord y,
			const TxtCoord W,
			const TxtCoord H )
{
  /*
	return  ( (x >=0) && (x < W) && (y >= 0) && (y < H)) ;
	because TxtCoord is unsigned type following is ok :
  */
    return  ( (x < W) && (y < H)) ;
}
/*
------------------------------------------------------------------
	 user key press translated into the command
------------------------------------------------------------------
*/
static short userKey ( void )
{
  while ( TRUE )
  {
  switch ( GETCH )
  {
	case 'q':
	case 'Q': return QUIT ;
	case 'h':
	case 'H': return HELP ;

	case '#': return MARK ;

	case '/': return CLEAR;

	default :
		switch ( GETCH)
		{
		   case 'K' : return LEFT ;
		   case 'M' : return RIGHT ;
		   case 'P' : return DOWN ;
		   case 'H' : return UP ;
		   default  : break ;
		} /* switch */
		break ;
   } /* switch */
   } /* while */
}    /* end of userKey */
/*
------------------------------------------------------------------
	 Frame arround the field ...
------------------------------------------------------------------
*/
static void drawBox (   const TxtCoord left, 
			const TxtCoord top,
			const TxtCoord right,
			const TxtCoord bottom )
{
  TxtCoord i ;

	for (i = left ; i < right ; i++)
	{ /* draw the top and bottom of the box */
		GOTOXY( i, top     ) ; PUTCHAR(HORIZ);
		GOTOXY( i, bottom  ) ; PUTCHAR(HORIZ);
	}

	for (i = top ; i < bottom ; i++)
	{/* draw the left and right of the box */
		GOTOXY( left,  i  ) ; PUTCHAR(VERT);
		GOTOXY( right, i  ) ; PUTCHAR(VERT);
	}

	/* corners */
	GOTOXY( left, top  ); PUTCHAR(LEFT_TOP);
	GOTOXY(right, top  ); PUTCHAR(RIGHT_TOP);
	GOTOXY(left ,bottom); PUTCHAR(LEFT_BOT);
	GOTOXY(right,bottom); PUTCHAR(RIGHT_BOT);

}
/*
--------------------------------------------------------------------
	Messages on frame title and status line
--------------------------------------------------------------------
*/
static void info( const char *title, const char *status )
{
  TxtCoord 	i ;
  char		msg[255] = "" ;

	for (i = Xorg ; i < Xorg + MAXWID ; i++)
	{ /* draw the top and bottom of the box */
		GOTOXY( i, Yorg - 1) ; PUTCHAR(HORIZ);
		GOTOXY( i, Yorg +MAXHGT) ; PUTCHAR(HORIZ);
	}

 GOTOXY( Xorg + 0   + 1, Yorg - 1);

	 if ( title )
		 sprintf (msg, "%s", title );
	 else
		 sprintf( msg, "%d/%d Bombs %d",WDT,HGT,bombNumber);
		 
		 PRINT( msg ) ;

 GOTOXY( Xorg + 0   + 1, Yorg + MAXHGT );

	 if ( status )
		 sprintf (msg, "%s", status );
	 else
		 sprintf(msg, "[/] clear [#] (un)mark [Q]uit");
                      
         PRINT( msg ) ;
         
	 posPlayer(); /* cursor back where player is */
}
