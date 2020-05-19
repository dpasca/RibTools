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
namespace RRASMOut
{

//==================================================================
static void writeVariable( FILE *pFile, const Variable &var )
{
	fprintf_s( pFile, "\t" );

	fprintf_s( pFile, "%-12s\t", var.GetUseName().c_str() );

	//--- write storage
	const char *pStorage = "ERROR!!!";

	if ( var.HasBaseVal() || var.mIsArray )	pStorage = "temporary"; else
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
        if ( var.HasBaseVal() )
            DEX_RUNTIME_ERROR(
                    "Internal error ! Variable '%s' is varying but has a base value !",
					var.GetUseName().c_str() );

		pClass = "varying";
	}
	else
	{
		if ( var.HasBaseVal() )
			pClass = "constant";
		else
			pClass = "uniform";
	}
	fprintf_s( pFile, "%-8s ", pClass );

	//--- eventual array definition
	if ( var.IsArray() )
	{
		fprintf_s( pFile, " array %u ", var.mArraySize );
	}

	//--- write the name
	VarType	varType = var.GetVarType();

	fprintf_s( pFile, "%-7s ", VarTypeToString( varType ) );

	//--- write the eventual constant value
	if ( var.HasBaseVal() )
	{
		fprintf_s( pFile, "\t" );

		// should be either bool, numbers or a string..
		if ( varType == VT_BOOL )
		{
			fprintf_s( pFile, "%i", var.GetBaseValBool() ? 1 : 0 );
		}
		else
		if ( varType == VT_STRING )
		{
			fprintf_s( pFile, "\"%s\"", var.GetBaseValString() );
		}
		else
		{
			const DVec<float>	&vec = var.GetBaseValFloatVec();

			for (size_t k=0; k < vec.size(); ++k)
				fprintf_s( pFile, "%f", vec[k] );
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

		if ( pNode->GetBlockType() == BLKT_DECL_PARAMS_SH )
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
static void writeVarsRec( FILE *pFile, TokNode *pNode )
{
	// write all used global vars and constants
	switch ( pNode->GetBlockType() )
	{
	case BLKT_ROOT:
	case BLKT_CODEBLOCK:
		{
		const DVec<Variable> &vars = pNode->GetVars();
		for (size_t i=0; i < vars.size(); ++i)
		{
			const Variable	&var = vars[i];

			// only write variables that are not shader params
			// that are used and that are not registers
			if (!var.mIsSHParam &&
				var.mIsUsed &&
				!var.IsRegisterAssigned() )
				writeVariable( pFile, var );
		}
		}
		break;

    default:
        break;
	}

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		writeVarsRec( pFile, pNode->mpChilds[i] );
	}
}

//==================================================================
void WriteVariables( FILE *pFile, TokNode *pNode )
{
	writeVarsRec( pFile, pNode );

	fprintf_s( pFile, "\n" );

	writeShaderParamVariables( pFile, pNode );
}

//==================================================================
}

//==================================================================
}
