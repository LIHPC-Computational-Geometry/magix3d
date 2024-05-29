/*----------------------------------------------------------------------------*/
/*
 * \file CommandReplaceTransfiniteMethod.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 8/1/15
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDREPLACETRANSFINITEMETHOD_H_
#define COMMANDREPLACETRANSFINITEMETHOD_H_
/*----------------------------------------------------------------------------*/
#include "Mesh/CommandCreateMesh.h"
//#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
class CoEdge;
/*----------------------------------------------------------------------------*/
/** \class CommandReplaceTransfiniteMethod
 *  \brief Commande qui change la discrétisation pour les cofaces et blocks
 *  dans le cas transfinie pour mettre autant que possible une méthode directionnelle
 *  car celles-ci sont plus rapides pour la création d'un maillage
 */
/*----------------------------------------------------------------------------*/
class CommandReplaceTransfiniteMethod: public Mesh::CommandCreateMesh {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     */
    CommandReplaceTransfiniteMethod(Internal::Context& c);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandReplaceTransfiniteMethod();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /**
     * la commande a vocation à être lancée dans un autre <I>thread</I>.
     */
    virtual bool threadable ( ) const
        { return true; }

    /*------------------------------------------------------------------------*/
    /** \brief  appel non prévu
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

    /*------------------------------------------------------------------------*/
    /// estimation du temps prévu pour mailler les blocs
    virtual unsigned long getEstimatedDuration (PLAY_TYPE playType = DO);

private:
    /// retourne vrai si tous les points des discrétisations sont alignés
    virtual bool isRectiligne(Edge* edge);
    /// retourne vrai si tous les points des discrétisations sont alignés
    virtual bool isRectiligne(std::vector<CoEdge*>& coedges);
    /// retourne vrai si tous les points de la discrétisations sont alignés
    virtual bool isRectiligne(CoEdge* coedge);

    /// stockage temporaire à 1 si une discrétisation d'une arête est jugée rectiligne, 2 sinon, 0 quand on ne sait pas encore
    std::map<CoEdge*,uint> m_coedge_isRectiligne;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDREPLACETRANSFINITEMETHOD_H_ */
/*----------------------------------------------------------------------------*/
