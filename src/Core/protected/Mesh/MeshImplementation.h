/*----------------------------------------------------------------------------*/
/*
 * \file MeshImplementation.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 18 nov. 2011
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_MESH_MESHIMPLEMENTATION_H_
#define MGX3D_MESH_MESHIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Mesh/MeshItf.h"
#include "Utils/Point.h"
/*----------------------------------------------------------------------------*/
// GMSH
class GVertex;
class MVertex;
/*----------------------------------------------------------------------------*/
namespace gmds {
class Node;
}
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace QtVtkComponents
{
	class VTKGMDSEntityRepresentation;
}

namespace Topo{
	class CoEdgeMeshingProperty;
	class EdgeMeshingPropertyGlobalInterpolate;
}

namespace Mesh{
    class Cloud;
	class Line;
	class Surface;
	class Volume;
	class SubVolume;
	class Compare2Meshes;
}
namespace Geom{
	class Surface;
	class GeomEntity;
}
namespace Internal{
	class CommandInternal;
}
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
/**
 * \class MeshImplementation
 * Utilisation du maillage GMDS
 *
 */
class MeshImplementation : public MeshItf {
private:

//    /// Masque pour GMDS
//    static const int TMask = DIM3|R|F|E|N|R2N|F2N|E2N|R2F|F2E;

    friend class QtVtkComponents::VTKGMDSEntityRepresentation;
    friend class Mesh::Surface;
    friend class Mesh::Volume;
    friend class Mesh::SubVolume;
    friend class Mesh::Compare2Meshes;
    friend class Topo::EdgeMeshingPropertyGlobalInterpolate;

public:

    MeshImplementation(Internal::Context* c);

    virtual ~MeshImplementation();

    /// Construction du maillage d'une liste de blocs
     virtual void mesh(Mesh::CommandCreateMesh* command,
             std::vector<Topo::Block* >& blocs);

     /// Construction du maillage d'une liste de faces communes
     virtual void mesh(Mesh::CommandCreateMesh* command,
             std::vector<Topo::CoFace* >& faces);

     /// Construction des points du maillage d'un bloc
     virtual void preMesh(Internal::CommandInternal* command, Topo::Block* bloc);

    /// Construction du maillage d'un bloc
    virtual void mesh(Mesh::CommandCreateMesh* command, Topo::Block* bl);

    /// Création du maillage d'une face
    virtual void mesh(Mesh::CommandCreateMesh* command, Topo::Face* fa);

    /// Construction des points du maillage d'une face commune
    virtual void preMesh(Internal::CommandInternal* command, Topo::CoFace* face);

    /// Création du maillage d'une face commune
    virtual void mesh(Mesh::CommandCreateMesh* command, Topo::CoFace* fa);

    /// Construction des points du maillage d'une arête commune
    virtual void preMesh(Internal::CommandInternal* command, Topo::CoEdge* arete);

    /// Création du maillage d'une arête
    virtual void mesh(Mesh::CommandCreateMesh* command, Topo::Edge* ed);

    /// Création du maillage d'une arête commune
    virtual void mesh(Mesh::CommandCreateMesh* command, Topo::CoEdge* ed);

    /// Création du maillage d'un sommet
    virtual void mesh(Mesh::CommandCreateMesh* command, Topo::Vertex* ve);




    /// Retourne le nombre total de noeuds dans le maillage
    virtual int getNbNodes();

    /// Retourne le nombre total de bras dans le maillage
    virtual int getNbEdges();

    /// Retourne le nombre total de polygones dans le maillage
    virtual int getNbFaces();

    /// Retourne le nombre total de polyèdres dans le maillage
    virtual int getNbRegions();

    /** Retourne les coordonnées d'un noeud dont on a le pointeur */
    virtual Utils::Math::Point getCoordNode(gmds::Node nd);

    /// Sauvegarde d'un maillage au format lima (mli)
    virtual void writeMli(std::string nom);

    /// Lecture d'un maillage au format vtk (vtk vtp vtu)
    virtual void readVTK(std::string nom, uint id);

    /// Sauvegarde d'un maillage au format vtk (vtk vtp vtu)
    virtual void writeVTK(std::string nom);

    /// Lecture d'un maillage au format lima (mli) (dans le gmds mesh d'id)
    virtual void readMli(std::string nom, uint id);

    /// Sauvegarde d'un maillage au format CGNS
    virtual void writeCGNS(std::string nom);

    /// Lissage du maillage
    virtual void smooth();

    /// Supression des entités créées par une commande CommandCreateMesh
    virtual void undoCreatedMesh(Mesh::CommandCreateMesh* command);

    /// Suppression de tous le maillage
    virtual void deleteMesh();

    /// Retourne une UTF8String avec les infos sur le noeud
    virtual TkUtil::UTF8String getInfo(gmds::Node nd);

	/// Accès au maillage.
    virtual const gmds::Mesh& getGMDSMesh ( ) const;
    virtual gmds::Mesh& getGMDSMesh ( );

    /// Retourne l'indice d'un nouveau maillage
    virtual uint createNewGMDSMesh ( );

    /// Détruit le dernier maillage
    virtual void deleteLastGMDSMesh ( );

    /// Retourne le ième maillage
    virtual gmds::Mesh& getGMDSMesh (uint id);

    /// Ajoute les groupes de mailles de gmds
    bool createGMDSGroups();

    /// Retire les groupes de mailles de gmds
    void deleteGMDSGroups();

    /** Déplace les sommets à partir de l'indice nbLayers pour que les point aillent vers le point ptN
     *
     */
    static void courbeDiscretisation(Utils::Math::Point& ptN,
    		std::vector<Utils::Math::Point>& ptInternes,
			uint nbLayers);

    /// Réinitialise la structure GMDS pour le maillage en sortie, avec nouvelle dimension
    virtual void updateMeshDim();

private:
    /** Création des nuages et y ajoute les noeuds de l'arête */
    void _addNodesInClouds(Mesh::CommandCreateMesh* command, Topo::CoEdge* ed);
    /** Création des nuages et y ajoute le noeud du sommet */
    void _addNodesInClouds(Mesh::CommandCreateMesh* command, Topo::Vertex* ve);
//    /** Ajoute les noeuds de l'arête à un nuage */
//    void _addNodesInCloud(std::vector<gmds::Node*>& nodes, gmds::Mesh<TMask>::cloud& cl);

    /** Création des lignes et y ajoute les bras de l'arête */
    void _addEdgesInLines(Mesh::CommandCreateMesh* command, Topo::CoEdge* ed);

    /** Création des polygones, des surfaces de maillage et y ajoute les polygones (pour CoFace structurée) */
    void _addFacesInSurfaces(Mesh::CommandCreateMesh* command, Topo::CoFace* fa);
    /** Création des polygones, des surfaces de maillage et y ajoute les polygones
     *  F. LEDOUX pour prototype maillage hybride TODO [FL] a modifier par la suite */
    void _addFacesInSurfacesForDelaunay(Mesh::CommandCreateMesh* command, Topo::CoFace* fa);
//    /** Ajoute les polygones à une surface */
//    void _addFacesInSurface(std::vector<gmds::Face*>& elem, gmds::Mesh<TMask>::surface& sf);

    /** Création des polyèdres, des volumes de maillage et y ajoute les polyèdres */
    void _addRegionsInVolumes(Mesh::CommandCreateMesh* command, Topo::Block* bl,
            uint nbBrasI, uint nbBrasJ, uint nbBrasK);
//    /** Ajoute les polyèdres à un volume */
//    void _addRegionsInVolume(std::vector<gmds::Region*>& elem, gmds::Mesh<TMask>::volume& vo);
//    /** Retire les polyèdres du volume */
//    void _removeRegionsFromVolume(std::set<gmds::Region*>& elem, gmds::Mesh<TMask>::volume& vo);

    /// Création des points d'un maillage structuré pour un bloc (thread-safe)
    virtual void preMeshStrutured(Topo::Block* bl);

    /// Création d'un maillage structuré pour un bloc
    virtual void meshStrutured(Mesh::CommandCreateMesh* command, Topo::Block* b);

    /// Création d'un maillage tétraédrique de Delaunay pour un bloc (version Tetgen)
    virtual void meshDelaunayTetgen(Mesh::CommandCreateMesh* command, Topo::Block* b);

#ifdef USE_MESHGEMS
    /// Création d'un maillage tétraédrique de Delaunay pour un bloc (version MeshGems)
    virtual void meshDelaunayMeshGemsVol(Mesh::CommandCreateMesh* command, Topo::Block* b);
#endif	// USE_MESHGEMS
    /// Création d'un maillage triangulaire de Delaunay pour un bloc par insertion
    virtual void meshInsertion(Mesh::CommandCreateMesh* command, Topo::Block* b);

    /// Création des points d'un maillage transfini pour une face commune
    virtual void preMeshStrutured(Topo::CoFace* fa);

    /// Création d'un maillage transfini pour une face commune
    virtual void meshStrutured(Mesh::CommandCreateMesh* command, Topo::CoFace* fa);

#ifdef USE_MESHGEMS
    /// Création d'un maillage triangulaire  pour une face commune (version MeshGems)
    virtual void meshTriangularMeshGems(Mesh::CommandCreateMesh* command, Topo::CoFace* fa);
#endif	// USE_MESHGEMS

    /// Création d'un maillage triangulaire de Delaunay pour une face commune (version GMSH)
    virtual void meshDelaunayGMSH(Mesh::CommandCreateMesh* command, Topo::CoFace* fa);
    /// Création d'un maillage quadrangulaire non structuré pour une face commune
    virtual void meshQuadPairing(Mesh::CommandCreateMesh* command, Topo::CoFace* fa);



    /** Création des coordonnées internes d'un quadrangle déformé, par méthode transfinie
        @param nbBrasI nombre de bras dans la première direction pour ce contour
        @param nbBrasJ nombrede bras  dans la deuxième direction pour ce contour
        @param l_points tableau de (nbBrasI+1)*(nbBrasJ+1) points,
            les extrémités (i=0, j=0, i=nbBrasI, j=nbBrasJ) sont données en entrée,
            tout le tableau est mis à jour en sortie
    */
    void discretiseTransfinie (uint nbBrasI, uint nbBrasJ, Utils::Math::Point* l_points);

    /** Création des coordonnées internes d'un bloc structuré, par méthode transfinie
        @param nbBrasI nombre de bras dans la première direction pour ce contour
        @param nbBrasJ nombrede bras  dans la deuxième direction pour ce contour
        @param nbBrasK nombrede bras  dans la troisième direction pour ce contour
        @param l_points tableau de (nbBrasI+1)*(nbBrasJ+1)*(nbBrasK+1) points,
            tout le tableau est mis à jour en sortie
    */
    void discretiseTransfinie (uint nbBrasI, uint nbBrasJ, uint nbBrasK, Utils::Math::Point* l_points);

    /** Création des coordonnées internes d'un bloc structuré, par méthode unidirectionnelle
        @param empI la discrétisation dans la première direction pour ce contour
        @param empJ la discrétisation dans la deuxième direction pour ce contour
        @param empK la discrétisation dans la troisième direction pour ce contour
        @param l_points tableau de (nbBrasI+1)*(nbBrasJ+1)*(nbBrasK+1) points,
            tout le tableau est mis à jour en sortie
        @param dir la direction suivant laquelle est faite la discrétisation
    */
    void discretiseDirection (
            Topo::CoEdgeMeshingProperty* empI,
            Topo::CoEdgeMeshingProperty* empJ,
            Topo::CoEdgeMeshingProperty* empK,
            Utils::Math::Point* l_points,
            uint dir);

    /** Création des coordonnées internes d'un bloc structuré, par méthode orthogonale
     * suivant une direction, puis courbe pour terminer
        @param empI la discrétisation dans la première direction pour ce contour
        @param empJ la discrétisation dans la deuxième direction pour ce contour
        @param empK la discrétisation dans la troisième direction pour ce contour
        @param l_points tableau de (nbBrasI+1)*(nbBrasJ+1)*(nbBrasK+1) points,
            tout le tableau est mis à jour en sortie
        @param dir la direction suivant laquelle est faite la discrétisation
        @param side le côté où la discrétisation est orthogonale au départ
        @param nbLayers nombre de couches pour lesquelles on souhaite avoir l'orthogonalité
        @param surface pour laquelle on maille orthogonallement (peut être nul)
    */
    void discretiseOrthogonalPuisCourbe (
            Topo::CoEdgeMeshingProperty* empI,
            Topo::CoEdgeMeshingProperty* empJ,
            Topo::CoEdgeMeshingProperty* empK,
            Utils::Math::Point* l_points,
            uint dir,
			uint side,
			uint nbLayers,
			Geom::Surface* surface);

    /** Création des coordonnées internes d'une face structurée, par méthode unidirectionnelle
        @param empI la discrétisation dans la première direction pour ce contour
        @param empJ la discrétisation dans la deuxième direction pour ce contour
        @param l_points tableau de (nbBrasI+1)*(nbBrasJ+1) points,
            tout le tableau est mis à jour en sortie
        @param dir la direction suivant laquelle est faite la discrétisation
    */
    void discretiseDirection (
            Topo::CoEdgeMeshingProperty* empI,
            Topo::CoEdgeMeshingProperty* empJ,
            Utils::Math::Point* l_points,
            uint dir);

    /** Création des coordonnées internes d'une face structurée, par méthode orthogonale
     * suivant une direction, puis courbe pour terminer
        @param empI la discrétisation dans la première direction pour ce contour
        @param empJ la discrétisation dans la deuxième direction pour ce contour
        @param l_points tableau de (nbBrasI+1)*(nbBrasJ+1) points,
            tout le tableau est mis à jour en sortie
        @param dir la direction suivant laquelle est faite la discrétisation
        @param side le côté où la discrétisation est orthogonale au départ
        @param nbLayers nombre de couches pour lesquelles on souhaite avoir l'orthogonalité
        @param surface pour laquelle on maille orthogonallement (peut être nul)
    */
    void discretiseOrthogonalPuisCourbe (
            Topo::CoEdgeMeshingProperty* empI,
            Topo::CoEdgeMeshingProperty* empJ,
            Utils::Math::Point* l_points,
            uint dir,
			uint side,
			uint nbLayers,
			Geom::Surface* surface);

    /** Création des coordonnées internes d'un bloc structuré, par méthode unidirectionnelle
        @param nbBrasI nombre de bras dans la première direction pour ce contour
        @param nbBrasJ nombrede bras  dans la deuxième direction pour ce contour
        @param nbBrasK nombrede bras  dans la troisième direction pour ce contour
        @param l_points tableau de (nbBrasI+1)*(nbBrasJ+1)*(nbBrasK+1) points,
            tout le tableau est mis à jour en sortie
        @param axis1 le premier point qui défini l'axe de rotation
        @param axis2 le deuxième point qui défini l'axe de rotation
        @param dir la direction suivant laquelle est faite la discrétisation
    */
    void discretiseRotation (
            uint nbBrasI, uint nbBrasJ, uint nbBrasK,
            Utils::Math::Point* l_points,
            Utils::Math::Point axis1,
            Utils::Math::Point axis2,
            uint dir);



    /// calcul la distance normalisée entre un des points et le premier
    void _calculDistances(Utils::Math::Point* l_points, uint indDep, uint increment, uint nbBras, double* ui);

    /// à partir des distances entre les points, calcul la distance normalisée / premier point à I fixé
    void _calculDistancesIfixe(Utils::Math::Point* uij, uint indCoord, uint nbNoeudsI, uint nbNoeudsJ);
    /// à partir des distances entre les points, calcul la distance normalisée / premier point à J fixé
    void _calculDistancesJfixe(Utils::Math::Point* uij, uint indCoord, uint nbNoeudsI, uint nbNoeudsJ);
    /// met à val une des coordonnées
    void _setToVal(Utils::Math::Point* uij, uint indCoord, uint nbNoeudsI, uint nbNoeudsJ, double val);

    /// ajoute un noeud gmds au sommet gmsh en parcourant les sommets d'une Edge, retourne l'indice du sommet dans l'arête
    uint _addGMDSVertex2GVertex(Topo::Edge* edge,
            GVertex* gv1,
            std::map<gmds::TCellID, MVertex*>& cor_gmdsNode_gmshVertex,
            std::map<MVertex*, gmds::TCellID>& cor_gmshVertex_gmdsNode);

    /** recherche des indices (idxI, idxJ, idxK) d'un noeud parmi ceux d'un bloc structuré
    */
    void _getIndexNode(gmds::TCellID node, std::vector<gmds::TCellID>& nodes,
    		uint ni, uint nj, uint nk,
    		uint &idxI, uint &idxJ, uint &idxK);

    /** recherche indice dans tableau de 2 valeurs identiques et non marquée dans filtre
     */
    void _getIndicesIdAndVal(uint* ipnts1, uint* ipnts2, bool* filtre,
    		uint &ind, uint &val);

    /** recherche d'un triplet d'entier
     */
    uint _getIndiceIJK(std::vector<std::vector<uint> >& idxIJK_vertices,
    		uint* idxIJK);

private:

    /** Lien sur les structures de maillage GMDS */
    std::vector<gmds::Mesh*> m_gmds_mesh;
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* MGX3D_MESH_MESHIMPLEMENTATION_H_ */
