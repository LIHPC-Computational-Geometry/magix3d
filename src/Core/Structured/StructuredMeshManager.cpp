/*
 * \file StructuredMeshManager.cpp
 * \author	Charles PIGNEROL
 * \date	03/12/2018
 */

#include "Structured/StructuredMeshManager.h"
#include "Structured/CommandReleaseStructuredData.h"
#include "Utils/Common.h"
#include "Utils/CommandManagerIfc.h"
#include "Internal/M3DCommandResult.h"

#include <TkUtil/Exception.h>
#include <TkUtil/TraceLog.h>


using namespace std;
using namespace TkUtil;
using namespace Mgx3D;
using namespace Mgx3D::Internal;


namespace Mgx3D
{

namespace Structured
{


StructuredMeshManager::StructuredMeshManager (const string& name, Mgx3D::Internal::ContextIfc* c)
	: StructuredMeshManagerIfc (name, c), _meshes ( )
{
}	// StructuredMeshManagerIfc::StructuredMeshManager


StructuredMeshManager::StructuredMeshManager (const StructuredMeshManager& mm)
	: StructuredMeshManagerIfc ("Invalid StructuredMeshManager", 0), _meshes ( )
{
	MGX_FORBIDDEN ("StructuredMeshManager copy constructor is not allowed.")
}	// StructuredMeshManager::StructuredMeshManager


StructuredMeshManager& StructuredMeshManager::operator = (const StructuredMeshManager& mm)
{
	MGX_FORBIDDEN ("StructuredMeshManager assignment operator is not allowed.")
	return *this;
}	// StructuredMeshManager::operator =


StructuredMeshManager::~StructuredMeshManager ( )
{
	try
	{
		for (map<int, StructuredMeshEntity*>::iterator itm = _meshes.begin ( ); _meshes.end ( ) != itm; itm++)
			delete itm->second;
		_meshes.clear ( );
	}
	catch (...)
	{
	}
}	// StructuredMeshManager::~StructuredMeshManager


M3DCommandResultIfc* StructuredMeshManager::releaseMesh ( )
{
	UTF8String message (Charset::UTF_8);
	message << "StructuredMeshManager::releaseMesh ( )";
	log (TraceLog (message, TkUtil::Log::TRACE_5));

	CommandReleaseStructuredData*	cmd	= new CommandReleaseStructuredData (dynamic_cast<Context&>(getContext ( )), "Déchargement des données structurées");
	UTF8String script (Charset::UTF_8);
	script << getContextAlias ( ) << "." << "getStructuredMeshManager ( ).releaseMesh ( )";
	cmd->setScriptCommand (script);
	getCommandManager ( ).addCommand (cmd, Utils::Command::DO);
	M3DCommandResultIfc*	result	= new M3DCommandResult (*cmd);

	return result;
}	// StructuredMeshManager::releaseMesh


void StructuredMeshManager::setMeshEntity (size_t procNum, StructuredMeshEntity*& mesh)
{
	bool	alreadySet	= false;

	try
	{
		getMesh (procNum);
		alreadySet	= true;
	}
	catch (...)
	{
	}

	if (true == alreadySet)
		throw Exception (UTF8String ("(Partition du) maillage déjà chargé(e)."));	

	_meshes.insert (pair<int, StructuredMeshEntity*>(procNum, mesh));
}	// StructuredMeshManager::setMeshEntity


void StructuredMeshManager::releaseMeshEntities ( )
{
	for (map<int, StructuredMeshEntity*>::iterator itm = _meshes.begin ( ); _meshes.end ( ) != itm; itm++)
	{
		try
		{
			delete itm->second;
		}
		catch (...)
		{
		}	
	}	// for (map<int, StructuredMeshEntity*>::iterator itm = _meshes.begin ( ); _meshes.end ( ) != itm; itm++)

	_meshes.clear ( );
}	// StructuredMeshManager::releaseMeshEntities


const StructuredMeshEntity& StructuredMeshManager::getMeshEntity (size_t procNum) const
{
	map<int, StructuredMeshEntity*>::const_iterator itm	= _meshes.find (procNum);
	if (_meshes.end ( ) == itm)
		throw Exception (UTF8String ("(Partition du) maillage non chargé(e)."));

	return *(itm->second);
}	// StructuredMeshManager::getMeshEntity


StructuredMeshEntity& StructuredMeshManager::getMeshEntity (size_t procNum)
{
	map<int, StructuredMeshEntity*>::const_iterator itm	= _meshes.find (procNum);
	if (_meshes.end ( ) == itm)
		throw Exception (UTF8String ("(Partition du) maillage non chargé(e)."));

	return *(itm->second);
}	// StructuredMeshManager::getMeshEntity


const StructuredMesh& StructuredMeshManager::getMesh (size_t procNum) const
{
	return getMeshEntity (procNum).getMesh ( );
}	// StructuredMeshManager::getMesh


StructuredMesh& StructuredMeshManager::getMesh (size_t procNum)
{
	return getMeshEntity (procNum).getMesh ( );
}	// StructuredMeshManager::getMesh


vector<StructuredMeshEntity*> StructuredMeshManager::getMeshEntities ( ) const
{
	vector<StructuredMeshEntity*>	entities;
	for (map<int, StructuredMeshEntity*>::const_iterator itm = _meshes.begin ( ); _meshes.end ( ) != itm; itm++)
		entities.push_back (itm->second);

	return entities;
}	// StructuredMeshManager::getMeshEntities


}	// namespace Structured

}	// namespace Mgx3D

