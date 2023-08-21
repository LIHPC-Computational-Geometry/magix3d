/*----------------------------------------------------------------------------*/
/*
 * \file CommandModificationTopo.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 21/3/2013
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDMODIFICATIONTOPO_H_
#define COMMANDMODIFICATIONTOPO_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom{
class GeomModificationBaseClass;
class CommandEditGeom;
}
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandModificationTopo
 *  \brief Commande qui modifie la topologie en fonction des modifications
 *  géométriques
 *
 */
/*----------------------------------------------------------------------------*/
class CommandModificationTopo: public Topo::CommandEditTopo {

public:

//    /*------------------------------------------------------------------------*/
//    /** \brief  Constructeur
//     *
//     *  \param c le contexte
//     *  \param geom_modif la classe géométrique avec les modifications
//     */
//    CommandModificationTopo(Internal::Context& c, Geom::GeomModificationBaseClass* geom_modif);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param command_edit_geom la commande d'édition
     */
    CommandModificationTopo(Internal::Context& c, Geom::CommandEditGeom* command_edit_geom);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandModificationTopo();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** Ce qui est fait après la commande suivant le cas en erreur ou non
     */
    virtual void postExecute(bool hasError);

    /*------------------------------------------------------------------------*/
    /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
     *  une représentation des entités créées
     *
     *  La commande doit ensuite être détruite
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

private:

    /** Fait la mise à jour de la topologie pour les sommets */
    void MAJVertices();

    /** Fait la mise à jour de la topologie pour les arêtes */
    void MAJCoEdges();

    /** Fait la mise à jour de la topologie pour les faces */
    void MAJCoFaces();

    /** Fait la mise à jour de la topologie pour les blocs */
    void MAJBlocks();

    /** Traitement des cofaces internes aux volumes, fusion des blocs pour le cas non structuré */
    void MAJInternalsCoFaces();

    /** Met à jour les ratios pour le cas semi-conforme entre les cofaces,
     * la première coface sera remplacée par la deuxième */
    void MAJRatios(CoFace* cf1, CoFace* cf2, uint nbI1, uint nbJ1, uint nbI2, uint nbJ2);

    /** Retourne le sommet topologique associé à un sommet géométrique
     *  et lève une exception s'il n'est pas trouvé
     */
    Topo::Vertex* getVertex(Geom::GeomEntity* ge, bool exceptionIfNotFound);

    /** Retourne les sommets communs topologiques associées à une courbe géométrique
     */
    std::vector<Topo::Vertex*> getVertices(Geom::GeomEntity* ge);

    /** Retourne tous les sommets topologiques associées à une courbe géométrique (y compris ceux aux extrémités)
     *  ou tous les sommets associés à une surface
     */
    std::vector<Topo::Vertex*> getAllVertices(Geom::GeomEntity* ge);

    /** Retourne les arêtes communes topologiques (non détruites) associées à une courbe géométrique
     *  peut se servir de la surface initiale pour certains cas
     */
    std::vector<Topo::CoEdge*> getCoEdges(Geom::GeomEntity* ge,  Geom::GeomEntity* ge_init = 0);

    /** Retourne les faces communes topologiques (non détruites) associées à une surface géométrique
     */
    std::vector<Topo::CoFace*> getCoFaces(Geom::GeomEntity* ge, bool exceptionIfNotFound);

    /** Retourne les blocs topologiques (non détruits) associés à un volume géométrique
     */
    std::vector<Topo::Block*> getBlocks(Geom::GeomEntity* ge);

    /** Recherche les coedges qui possèdent les mêmes sommets que la coedge en entrée
     */
    std::vector<Topo::CoEdge*> getSameCoEdges(Topo::CoEdge* coedge);

    /** Recherche les cofaces qui possèdent les mêmes contours que la coface en entrée
     */
    std::vector<Topo::CoFace*> getSameCoFaces(Topo::CoFace* coface);

    /** Recherche s'il y a des arêtes topologiques internes à cette surface
     *  et fait la fusion avec les sommets internes proches
     */
    void fuseInternalCoEdgesAndVertices(Geom::GeomEntity* ge, std::vector<Geom::GeomEntity*>& newEntities);

    /** Découpe les arêtes de la deuxième liste en fonction des sommets
     *  de la première liste d'arêtes, entre vtx0 et vtx1
     *
     *  Les listes doivent être triés entre vtx0 et vtx1
     *  Retourne la liste coedges2 après modification (découpage des arêtes)
     */
    std::vector<Topo::CoEdge*> splitCoEdges(std::vector<Topo::CoEdge*>& coedges1,
            std::vector<Topo::CoEdge*>& coedges2,
            Topo::Vertex* vtx0, Topo::Vertex* vtx1);

    /** Retourne le sous ensemble des coedges à l'intérieur des vertices
     */
    std::vector<Topo::CoEdge*> getInnerCoEdges(std::vector<Topo::CoEdge*>& coedges,
        		std::vector<Vertex*>& vertices);

    /** Retourne le sous ensemble des cofaces à l'intérieur des coedges
     */
    std::vector<Topo::CoFace*> getInnerCoFaces(std::vector<Topo::CoFace*>& cofaces,
    		std::vector<CoEdge*>& coedges);

    /** Retourne le sous ensemble des blocs à l'intérieur des cofaces
     */
    std::vector<Topo::Block*> getInnerBlocks(std::vector<Topo::Block*>& blocks,
    		std::vector<CoFace*>& cofaces);

    /** Fait le merge de 2 faces commune en privilégiant la structuration */
    void merge(CoFace* cf1, CoFace* cf2);

    /** Recherche les surfaces internes à un ensemble de volumes,
     *  pour celles qui n'ont pas de topologie d'associée, recherche parmis
     *  les cofaces internes à l'ensemble de blocs si un sous ensemble
     *  pourrait lui être associé
     */
    void associateInnerCoFaces(std::vector<Geom::GeomEntity*>& geoms,
    		std::vector<Topo::Block*>& blocks);

    /** Associe les arêtes internes de l'ensemble de cofaces à l'entité géométrique
     */
    void associateInnerCoEdges(std::vector<Topo::CoFace*>& cofaces,
    		Geom::GeomEntity* ge);

    /** Change l'associtaion pour un ensemble de cofaces, les arêtes et sommets
     */
    void updateGeomAssociation(std::vector<CoFace*>& coface, Geom::GeomEntity* oldGE, Geom::GeomEntity* newGE);

private:
    /// la commande d'édition en cours (dans le cas où m_geomModif n'est pas dispo)
    Geom::CommandEditGeom* m_command_edit_geom;

};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDMODIFICATIONTOPO_H_ */
/*----------------------------------------------------------------------------*/
