/*----------------------------------------------------------------------------*/
/*
 * \file CommandTransformTopo.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 2/10/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDTRANSFORMTOPO_H_
#define COMMANDTRANSFORMTOPO_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
#include "Utils/Vector.h"

// OCC
class gp_Trsf;
class gp_GTrsf;
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/

class CommandDuplicateTopo;

/*----------------------------------------------------------------------------*/
/** \class CommandTransformTopo
 *  \brief Commande permettant transformer une topologie
 *
 */
/*----------------------------------------------------------------------------*/
class CommandTransformTopo: public Topo::CommandEditTopo {

protected:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     * \param le contexte
     * \param le nom de la commande
     * \param ve la liste des entités concernées
     */
    CommandTransformTopo(Internal::Context& c,
            std::string name,
            std::vector<TopoEntity*>& ve);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     * \param le contexte
     * \param le nom de la commande
     * \param cmd le constructeur des entités topologiques à transformer
     * \param all_topo vrai si on est dans un cas de copie + transformation de toute la topologie
     */
    CommandTransformTopo(Internal::Context& c,
            std::string name,
			CommandDuplicateTopo* cmd,
			bool all_topo);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     * \param le contexte
     * \param le nom de la commande
     */
    CommandTransformTopo(Internal::Context& c,
            std::string name);

public:
    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandTransformTopo();

protected:
    // initialisation avant toute commande
    virtual void init();

    // applique la transformation à tous les sommets sélectionnés
    void transform(gp_Trsf* transf);
    void transform(gp_GTrsf* transf);

    const std::vector<TopoEntity*>& getEntities() {return m_entities;}

    /** Rotation des sommets à partir d'un bloc */
    void transform(Block* bl, gp_Trsf* transf, std::map<TopoEntity*, bool>& filtre);
    void transform(Block* bl, gp_GTrsf* transf, std::map<TopoEntity*, bool>& filtre);
    /** Rotation des sommets à partir d'un face */
    void transform(CoFace* co, gp_Trsf* transf, std::map<TopoEntity*, bool>& filtre);
    void transform(CoFace* co, gp_GTrsf* transf, std::map<TopoEntity*, bool>& filtre);
    /** Rotation des sommets à partir d'une arête */
    void transform(CoEdge* co, gp_GTrsf* transf, std::map<TopoEntity*, bool>& filtre);
    void transform(CoEdge* co, gp_Trsf* transf, std::map<TopoEntity*, bool>& filtre);
    /** Rotation d'un sommet */
    void transform(Vertex* vtx, gp_Trsf* transf, std::map<TopoEntity*, bool>& filtre);
    void transform(Vertex* vtx, gp_GTrsf* transf, std::map<TopoEntity*, bool>& filtre);
private:

    /// les entités topologiques sélectionnées
    std::vector<TopoEntity*> m_entities;

    /** commande de construction des entités à traiter */
    CommandDuplicateTopo* m_buildEntitiesCmd;

    /// si c'est une transformation globale
    bool m_all_topo;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDTRANSFORMTOPO_H_ */
/*----------------------------------------------------------------------------*/
