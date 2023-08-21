/*----------------------------------------------------------------------------*/
/*
 * \file MeshItf.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 7 nov. 2011
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_MESH_MESHITF_H_
#define MGX3D_MESH_MESHITF_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
#include "Utils/Common.h"
#include "Topo/Block.h"
#include <vector>
/*----------------------------------------------------------------------------*/
#include <GMDS/IG/IGMesh.h>
//#include <ConstQualif.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
class Vertex;
class Edge;
class Face;
class Block;
}
namespace Internal {
class Context;
class CommandInternal;
}
namespace Mesh{
class Surface;
class Volume;
}
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
class CommandCreateMesh;
/*----------------------------------------------------------------------------*/
/**
 * \class MeshItf
 * \brief Interface vers le maillage (GMDS pour le moment)
 *
 */
class MeshItf {
public:

	/// Masque pour GMDS en 3D
	static const int TMask3D = gmds::DIM3|gmds::R|gmds::F|gmds::E|gmds::N|gmds::R2N|gmds::F2N|gmds::E2N|gmds::R2F|gmds::F2E;
	/// Masque pour GMDS en 2D
	static const int TMask2D = gmds::DIM2|gmds::F|gmds::E|gmds::N|gmds::F2N|gmds::E2N|gmds::F2E;

    MeshItf(Internal::Context* c) : m_context(c) {}

    virtual ~MeshItf() {}

    /// Construction du maillage d'une liste de blocs
    virtual void mesh(Mesh::CommandCreateMesh* command,
            std::vector<Topo::Block* >& blocs) =0;

    /// Construction des points du maillage d'un bloc
    virtual void preMesh(Internal::CommandInternal* command,
    		Topo::Block* bloc) =0;

    /// Construction du maillage d'un bloc
    virtual void mesh(Mesh::CommandCreateMesh* command,
            Topo::Block* bloc) =0;

    /// Construction du maillage d'une liste de faces communes
    virtual void mesh(Mesh::CommandCreateMesh* command,
            std::vector<Topo::CoFace* >& faces) =0;

    /// Construction des points du maillage d'une face commune
    virtual void preMesh(Internal::CommandInternal* command,
            Topo::CoFace* face) =0;

    /// Construction du maillage d'une face commune
    virtual void mesh(Mesh::CommandCreateMesh* command,
            Topo::CoFace* face) =0;

    /// Construction des points du maillage d'une arête commune
    virtual void preMesh(Internal::CommandInternal* command,
    		Topo::CoEdge* arete) =0;

    /// Construction du maillage d'une arête commune
    virtual void mesh(Mesh::CommandCreateMesh* command,
    		Topo::CoEdge* arete) =0;

    /// Construction du maillage d'un sommet
    virtual void mesh(Mesh::CommandCreateMesh* command,
    		Topo::Vertex* sommet) =0;

    /// Retourne le nombre total de noeuds dans le maillage
    virtual int getNbNodes() =0;

    /// Retourne le nombre total de polygones dans le maillage
    virtual int getNbFaces() =0;

    /// Retourne le nombre total de polyèdres dans le maillage
    virtual int getNbRegions() =0;

    /// Sauvegarde d'un maillage au format lima (mli)
    virtual void writeMli(std::string nom) =0;

    /// Lecture d'un maillage au format vtk (vtk vtp vtu)
    virtual void readVTK(std::string nom, uint id) =0;

    /// Sauvegarde d'un maillage au format vtk (vtk vtp vtu)
    virtual void writeVTK(std::string nom) =0;

    /// Lecture d'un maillage au format lima (mli)
    virtual void readMli(std::string nom, uint id) =0;

    /// Sauvegarde d'un maillage au format CGNS
    virtual void writeCGNS(std::string nom) =0;

   /// Lissage du maillage
    virtual void smooth() =0;

    /** retourne le contexte */
    virtual Internal::Context& getContext() {return *(m_context);}

    /// Supression des entités créées par une commande CommandCreateMesh
    virtual void undoCreatedMesh(Mesh::CommandCreateMesh* command) =0;

    /// Suppression de tous le maillage
    virtual void deleteMesh() =0;

    /// Retourne une UTF8String avec les infos sur le noeud
    virtual TkUtil::UTF8String getInfo(gmds::Node nd) =0;

	/// Accès au maillage (lecture/écriture car possibilité de création de surfaces/volumes).
	virtual const gmds::IGMesh& getGMDSMesh ( ) const =0;
	virtual gmds::IGMesh& getGMDSMesh ( ) = 0;

	/// Réinitialise la structure GMDS pour le maillage en sortie, avec nouvelle dimension
	virtual void updateMeshDim() =0;

protected:
    MeshItf(const MeshItf&): m_context(0)
    {
        MGX_FORBIDDEN("MeshItf::MeshItf is not allowed.");
    }

    MeshItf& operator = (const MeshItf&)
    {
        MGX_FORBIDDEN("MeshItf::operator = is not allowed.");
        return *this;
    }

private:

    /** le contexte */
    Internal::Context* m_context;
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* MGX3D_MESH_MESHITF_H_ */
