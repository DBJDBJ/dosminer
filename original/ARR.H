/*
------------------------------------------------------------------------
	make ARR structure
------------------------------------------------------------------------
*/
extern void **ARRdim( const int rowNum, const int colNum, int noneSize ) ;
/*
------------------------------------------------------------------------
	free the ARR structure
------------------------------------------------------------------------
*/
extern ARRfree( const int rowNum, const int colNum, void **field ) ;
