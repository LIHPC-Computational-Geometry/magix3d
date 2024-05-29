/*----------------------------------------------------------------------------*/
/*
 * \file CommandFacetedSurfaceOffset.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 11 Décembre 2020
 *
 *  Commande qui ne permet pas cette implémentation de faire un undo
 *  Il faudrait pour cela mémoriser la position des noeuds
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Utils/Command.h"
#include "Internal/Context.h"
#include "Internal/InfoCommand.h"
#include "Geom/CommandFacetedSurfaceOffset.h"
#include "Geom/Surface.h"
#include "Geom/FacetedSurface.h"
#include "Geom/FacetedHelper.h"
#include "Mesh/MeshItf.h"
#include "Mesh/MeshManager.h"
#include "Mesh/MeshImplementation.h"

// TkUtil
#include <TkUtil/Exception.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/MemoryError.h>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
CommandFacetedSurfaceOffset::
CommandFacetedSurfaceOffset(Internal::Context& c,
                           Surface* surface,
                           const double& offset)
: Internal::CommandInternal (c, "Offset pour surface facétisée")
, m_surface(surface)
, m_offset(offset)
{
    if (surface == 0)
        throw TkUtil::Exception(TkUtil::UTF8String ("La commande Offset ne peut se faire sans une surface", TkUtil::Charset::UTF_8));
    if (surface->needLowerDimensionalEntityModification())
        throw TkUtil::Exception(TkUtil::UTF8String ("La commande Offset ne peut se faire que sur une surface facétisée", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
CommandFacetedSurfaceOffset::~CommandFacetedSurfaceOffset()
{
}
/*----------------------------------------------------------------------------*/
void CommandFacetedSurfaceOffset::
internalExecute()
{
    FacetedSurface* fs = dynamic_cast<FacetedSurface*>(m_surface->getComputationalProperty());
    CHECK_NULL_PTR_ERROR(fs)

    Mesh::MeshImplementation* mesh = dynamic_cast<Mesh::MeshImplementation*>(getContext().getLocalMeshManager().getMesh());
    if (mesh == 0)
        throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne dans CommandFacetedSurfaceOffset::internalExecute, mesh == 0", TkUtil::Charset::UTF_8));

    FacetedHelper::transformOffset(mesh, fs, m_offset);
}
/*----------------------------------------------------------------------------*/
void CommandFacetedSurfaceOffset::
internalUndo()
{
	// on ne fait rien
}
/*----------------------------------------------------------------------------*/
void CommandFacetedSurfaceOffset::
internalRedo()
{
    // on ne fait rien
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
