/*----------------------------------------------------------------------------*/
/*
 * \file CommandSetEdgeMeshingPropertyToParallelCoEdges.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 20/5/2014
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDSETEDGEMESHINGPROPERTYTOPARALLELCOEDGE_H_
#define COMMANDSETEDGEMESHINGPROPERTYTOPARALLELCOEDGE_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
class CoEdgeMeshingProperty;
class CoEdge;
class CoFace;
/*----------------------------------------------------------------------------*/
/** \class CommandSetEdgeMeshingPropertyToParallelCoEdges
 *  \brief Commande qui change la discrétisation pour des arêtes parallèles à une arête sélectionnée
 */
/*----------------------------------------------------------------------------*/
class CommandSetEdgeMeshingPropertyToParallelCoEdges: public Topo::CommandEditTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param emp la propriété de discrétisation pour l'arête
     *  \param ed l'arête dont on va changer la discrétisation (ainsi que celles parallèles)
     */
    CommandSetEdgeMeshingPropertyToParallelCoEdges(Internal::Context& c, CoEdgeMeshingProperty& emp, CoEdge* ed);


    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandSetEdgeMeshingPropertyToParallelCoEdges();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  appel non prévu
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

private:
    /// la nouvelle propriété de discrétisation
    CoEdgeMeshingProperty* m_prop;

    /// les arêtes concernées
    CoEdge* m_coedge;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDSETEDGEMESHINGPROPERTYTOPARALLELCOEDGE_H_ */
/*----------------------------------------------------------------------------*/
