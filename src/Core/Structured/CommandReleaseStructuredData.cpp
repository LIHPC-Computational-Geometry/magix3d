/*
 * \file	CommandReleaseStructuredData.cpp
 * \author	Charles PIGNEROL
 * \date	05/12/2018
 */

#include "Structured/CommandReleaseStructuredData.h"
#include "Utils/Common.h"

#include <TkUtil/Exception.h>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Utils;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace Structured
{


CommandReleaseStructuredData::CommandReleaseStructuredData (Internal::Context& c, const string& name)
	: CommandInternal (c, name)
{
}	// CommandReleaseStructuredData::CommandReleaseStructuredData


CommandReleaseStructuredData::CommandReleaseStructuredData (const CommandReleaseStructuredData& c)
	: CommandInternal (c)
{
	MGX_FORBIDDEN ("CommandReleaseStructuredData copy constructor is not allowed.")
}	// CommandReleaseStructuredData::CommandReleaseStructuredData


CommandReleaseStructuredData& CommandReleaseStructuredData::operator = (const CommandReleaseStructuredData& mm)
{
	MGX_FORBIDDEN ("CommandReleaseStructuredData assignment operator is not allowed.")
	return *this;
}	// CommandReleaseStructuredData::operator =


CommandReleaseStructuredData::~CommandReleaseStructuredData ( )
{
}	// CommandReleaseStructuredData::~CommandReleaseStructuredData


void CommandReleaseStructuredData::internalExecute ( )
{
	getContext ( ).getStructuredMeshManager ( ).releaseMeshEntities ( );
}	// CommandReleaseStructuredData::internalExecute


void CommandReleaseStructuredData::internalUndo ( )
{
	throw Exception ("CommandReleaseStructuredData::internalUndo n'est pas implémentée.");
}	// CommandReleaseStructuredData::internalUndo


void CommandReleaseStructuredData::internalRedo ( )
{
    internalExecute ( );
}	// CommandReleaseStructuredData::internalRedo


}	// namespace Structured

}	// namespace Mgx3D

