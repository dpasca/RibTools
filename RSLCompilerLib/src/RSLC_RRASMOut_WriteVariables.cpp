//==================================================================
/// RSLC_RRASMOut_WriteVariables.cpp
///
/// Created by Davide Pasca - 2010/1/7
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include <stdlib.h>
#include "RSLC_Tree.h"
#include "RSLC_Variables.h"
#include "RSLC_RRASMOut.h"

//==================================================================
namespace RSLC
{

//==================================================================
static void writeVariable( FILE *pFile, const Variable &var )
{
	fprintf_s( pFile, "\t" );

	fprintf_s( pFile, "%-12s\t", var.GetUseName().c_str() );

	//--- write storage
	const char *pStorage = "ERROR!!!";

	if ( var.mBaseVal.mUse )	pStorage = "temporary"; else
	if ( var.mIsGlobal )	pStorage = "global"; else
	if ( var.mIsSHParam )	pStorage = "parameter"; else
	{
		DASSERT( 0 );
	}

	fprintf_s( pFile, "%-9s ", pStorage );

	//--- write class (constant, uniform, varying (no "vertex" for now))
	const char *pClass;
	if ( var.mIsVarying )
	{
		DASSTHROW( var.mBaseVal.mUse == false,
				("Internal error ! Variable '%s' is varying but has a base value !",
					var.GetUseName().c_str()) );

		pClass = "varying";
	}
	else
	{
		if ( var.mBaseVal.mUse )
			pClass = "constant";
		else
			pClass = "uniform";
	}
	fprintf_s( pFile, "%-8s ", pClass );

	//--- write the name
	VarType	varType = var.GetVarType();

	fprintf_s( pFile, "%-7s ", VarTypeToString( varType ) );

	//--- write the eventual constant value
	if ( var.mBaseVal.mUse )
	{
		fprintf_s( pFile, "\t" );

		// should be either bool, numbers or a string..
		if ( varType == VT_BOOL )
		{
			fprintf_s( pFile, "%i", var.mBaseVal.mBool ? 1 : 0 );
		}
		else
		if ( varType == VT_STRING )
		{
			if ( var.mBaseVal.mStr.length() )
				fprintf_s( pFile, "\"%s\"", var.mBaseVal.mStr.c_str() );
		}
		else
		{
			for (size_t k=0; k < var.mBaseVal.mNumVec.size(); ++k)
				fprintf_s( pFile, "%f", var.mBaseVal.mNumVec[k] );
		}
	}

	fprintf_s( pFile, "\n" );
}

//==================================================================
// $$$ doesn't really need to be recursive..
static void writeShaderParamVariables( FILE *pFile, TokNode *pNode )
{
	const DVec<Variable> &vars = pNode->GetVars();

	if ( vars.size() )
	{
		//fprintf_s( pFile, "\t; -- vars for block %i\n", pNode->GetBlockID() );

		if ( pNode->GetBlockType() == BLKT_SHPARAMS )
		{
			for (size_t i=0; i < vars.size(); ++i)
			{
				const Variable	&var = vars[i];

				writeVariable( pFile, var );
			}
		}
	}


	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		writeShaderParamVariables( pFile, pNode->mpChilds[i] );
	}
}

//==================================================================
namespace RRASMOut
{

//==================================================================
void WriteVariables( FILE *pFile, TokNode *pNode )
{
	const DVec<Variable> &vars = pNode->GetVars();

	// write all used global vars and constants

	for (size_t i=0; i < vars.size(); ++i)
	{
		const Variable	&var = vars[i];

		//if ( !var.mIsSHParam && ((var.mIsGlobal && var.mIsUsed) || !var.mIsGlobal) )
		if ( !var.mIsSHParam && var.mIsUsed )
			writeVariable( pFile, var );
	}

	fprintf_s( pFile, "\n" );

	writeShaderParamVariables( pFile, pNode );
}

//==================================================================
}

//==================================================================
}
