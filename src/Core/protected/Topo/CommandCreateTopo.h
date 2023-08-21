/*----------------------------------------------------------------------------*/
/** \file CommandCreateTopo.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 17/11/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDCREATETOPO_H_
#define COMMANDCREATETOPO_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
#include "Topo/Block.h"
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {

namespace Geom {
class GeomEntity;
class CommandCreateGeom;
}
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
class TopoManager;
class TopoEntity;
/*----------------------------------------------------------------------------*/
/** \class CommandCreateTopo
 *  \brief Partie commune à toutes les commandes de créations de topologies
 */
/*----------------------------------------------------------------------------*/
class CommandCreateTopo: public CommandEditTopo {

protected:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     * \param le contexte
     * \param le nom de la commande
     */
    CommandCreateTopo(Internal::Context& c, std::string name);

public:
    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur, destruction de la topologie
     */
    virtual ~CommandCreateTopo();

//    /*------------------------------------------------------------------------*/
//    /** \brief  annule la commande
//     */
//    void internalUndo();
//
//    /*------------------------------------------------------------------------*/
//    /** \brief  rejoue la commande
//     */
//    void internalRedo();

    /*------------------------------------------------------------------------*/
    /** \brief  stockage dans le manageur topologique des blocs et autres
     *  entités de niveau inférieur
     *
     *  à faire avant les modifications (merge et autres ...)
     */
    virtual void split();

    /*------------------------------------------------------------------------*/
    /** \brief  ajoute le bloc à la liste des blocs créés
     */
    virtual void addCreatedBlock(Block* b);

    /** \brief  retourne la liste des blocs créés (copie de la liste)
     */
    void getBlocks(std::vector<Block* >& blocks) const;

    /*------------------------------------------------------------------------*/
    /** \brief  ajoute la face commune à la liste des cofaces créées
     */
    virtual void addCreatedCoFace(CoFace* c);

    /** \brief  retourne la liste des cofaces créées (copie de la liste)
     */
    void getCoFaces(std::vector<CoFace* >& cofaces) const;

    /*------------------------------------------------------------------------*/
    /** \brief  permet de spécifier une entité géométrique pour la construction de la topologie
     */
    void setGeomEntity(Geom::GeomEntity* ge) {m_geom_entity = ge;}

    /** \brief  retourne l'entité géométrique pour la construction de la topologie
     */
    Geom::GeomEntity* getGeomEntity() const {return m_geom_entity;}

    /*------------------------------------------------------------------------*/
    /** \brief  permet de spécifier une commande de construction géométrique
     */
    void setCommandCreateGeom (Geom::CommandCreateGeom* ccg) {m_command_create_geom = ccg;}

    /*------------------------------------------------------------------------*/
    /** \brief  permet de spécifier si l'on souhaite des blocs (ou cofaces) structurés ou non
     */
    void setStructured(bool st) {m_structured = st;}

    /** \brief  permet de savoir si l'on souhaite des blocs structurés (ou cofaces) ou non
     */
    bool isStructured() const {return m_structured;}

protected:
    /*------------------------------------------------------------------------*/
    /** Méthode de vérification et validation de l'entité présente */
    virtual void validGeomEntity();

private:

    /** resultat de l'opération (on ne stocke que les nouveaux blocs) */
    std::vector<Block* > m_blocks;

    /** resultat de l'opération (on ne stocke que les nouvelles CoFaces) */
    std::vector<CoFace* > m_cofaces;

    /** parametre optionnel de l'opération */
    Geom::GeomEntity* m_geom_entity;

    /** commande de création d'une entité géométrique */
    Geom::CommandCreateGeom* m_command_create_geom;

    /// vrai si l'on souhaite obtenir un bloc structure autant que possible
    bool m_structured;

};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDCREATETOPO_H_ */
/*----------------------------------------------------------------------------*/

