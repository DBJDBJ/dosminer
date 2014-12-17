
/*
--------------------------------------------------------------
   Save the game to the file
   Load the game from the file

   Format ( line numbers are not in ) :

   000:     <int>       Field width
   001:     <int>       Field height
   002:     <int>       Last X pos of player
   003:     <int>       Last Y pos of player
   004:     <int>       0 or 1 for mine presence
   005:     <int>       Node appearance character
   006:     <int>       Node mine count

--------------------------------------------------------------
*/
#include "dosmine.h"
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
