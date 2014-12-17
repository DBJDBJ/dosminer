#include <stdio.h>
#include <stdlib.h>
/*
------------------------------------------------------------------------
	make ARR structure
------------------------------------------------------------------------
*/
void **ARRdim( const int rowNum, const int colNum, int nodeSize )
{
	int		row ;
	void **field = (void **)malloc( rowNum * sizeof(void *) ) ;

	if ( field == NULL ) return NULL ;

	for ( row=0 ; row < rowNum ; row++ )
	if (( field[row] = (void *)malloc( colNum * nodeSize )) == NULL )
	      return NULL ;

	return field ;
}
/*
------------------------------------------------------------------------
	free the ARR structure
------------------------------------------------------------------------
*/
#define NUSED(x) (void *)&(x)
ARRfree( const int rowNum, const int colNum, void **field )
{
	int		row ;
	NUSED( colNum )     ; /* no use */
	for ( row=0 ; row < rowNum ; row++ ) free(field[row]) ;

	free(field) ;

	return (1) ;


}
