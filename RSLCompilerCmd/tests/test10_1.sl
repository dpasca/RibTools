/*==================================================================
/// test10_1.sl
///
/// Created by Davide Pasca - 2010/02/07
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//================================================================*/

// This file is to test the preprocessor
// (verify output by using -prepro option in the shader compiler)

// test include
#include "test10_2.sl"

// test line splicing and macros that contract in size
#define CONTR	\
		1

// test macros that expand in size
#define EXPA	123456

// test macros that have the same size
#define SAME	1234

// test empty macros
#define PLAIN_SYMBOL1	   

//==================================================================
surface test10_1()
{
	Ci = test10_2_getColor();
	
	Oi = EXPA;		// should become 123456
	
	Oi = CONTR;		// should become 1

	Oi = SAME;		// should become 1234
	
	PLAIN_SYMBOL1;	// should become just ;
}
