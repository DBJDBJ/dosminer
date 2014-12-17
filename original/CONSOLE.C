#include <assert.h>
#include <stdlib.h>
#include <conio.h>
#include "console.h"
/* */
typedef struct text_info    Tinfo ;
static Tinfo                * tinfo = 0 ;
/* */
static void zapTextInfo ()
{
    if ( tinfo ) free( tinfo ) ;
}
/* */
static void getTextInfo ()
{
    if ( ! tinfo ) {
        assert( 0 == atexit( zapTextInfo )) ;
        tinfo = (Tinfo*)malloc(sizeof(Tinfo)) ;
        gettextinfo(tinfo);
    }
}
/* */
static void showTextInfo ()
{
    getTextInfo() ;
cprintf("window left      %2d\r\n",tinfo->winleft);
cprintf("window top       %2d\r\n",tinfo->wintop);
cprintf("window right     %2d\r\n",tinfo->winright);
cprintf("window bottom    %2d\r\n",tinfo->winbottom);
cprintf("attribute        %2d\r\n",tinfo->attribute);
cprintf("normal attribute %2d\r\n",tinfo->normattr);
cprintf("current mode     %2d\r\n",tinfo->currmode);
cprintf("screen height    %2d\r\n",tinfo->screenheight);
cprintf("screen width     %2d\r\n",tinfo->screenwidth);
cprintf("current x        %2d\r\n",tinfo->curx);
cprintf("current y        %2d\r\n",tinfo->cury);
}
/*
  switch to 40 column mode taking core of mono or color
  issues
  dbj 080996
*/
void switchTo40 ()
{
    getTextInfo() ;
    /**/
    if ( tinfo->currmode == C40  ) return ;
    if ( tinfo->currmode == BW40 ) return ;
    /**/
    if ( tinfo->currmode == BW80 )
                         textmode( BW40 ) ;
    else
    if ( tinfo->currmode == MONO )
                         textmode( BW40 ) ;
    else
    if ( tinfo->currmode == C80 )
                         textmode( C40 ) ;
    else
    assert(("Can not switch to 40 column screen ?",0)) ;
    /*
    */
}
