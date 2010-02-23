//==================================================================
/// RSLC_Expressions.h
///
/// Created by Davide Pasca - 2009/11/1
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RSLC_EXPRESSIONS_H
#define RSLC_EXPRESSIONS_H

//==================================================================
namespace RSLC
{

//==================================================================
class DynConst
{
public:
	enum Type
	{
		TYPE_UNKNOWN,
		TYPE_NUM,
		TYPE_STRING,
		TYPE_BOOL,
		TYPE_N
	};

	Type	mType;
	double	mNum;
	DStr	mStr;
	bool	mBool;

	DynConst() :
		mType(TYPE_UNKNOWN),
		mNum(0.0),
		mBool(false)
	{
	}

	void SetNum( double val )		{ mType = TYPE_NUM; mNum = val; }
	void SetBool( bool val )		{ mType = TYPE_BOOL; mBool = val; }
	void SetStr( const DStr &val )	{ mType = TYPE_STRING; mStr = val; }
	void SetStr( const char *val )	{ mType = TYPE_STRING; mStr = val; }
};

void SolveExpressions( class TokNode *pNode, bool processShaderOnly );
void SolveGlobalConstants( TokNode *pRoot );
void SolveVariablesDetail( TokNode *pRoot );

void SolveConstantExpression( TokNode *pNode, DynConst &out_dynConst );

//==================================================================
}

#endif
