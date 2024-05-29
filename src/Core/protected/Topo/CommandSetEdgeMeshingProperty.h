/*----------------------------------------------------------------------------*/
/*
 * \file CommandSetEdgeMeshingProperty.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 31/1/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDSETEDGEMESHINGPROPERTY_H_
#define COMMANDSETEDGEMESHINGPROPERTY_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
class CoEdgeMeshingProperty;
class CoEdge;
/*----------------------------------------------------------------------------*/
/** \class CommandSetEdgeMeshingProperty
 *  \brief Commande qui change la discrétisation pour des arêtes
 */
/*----------------------------------------------------------------------------*/
class CommandSetEdgeMeshingProperty: public Topo::CommandEditTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param emp la propriété de discrétisation pour l'arête
     *  \param ed l'arête dont on va changer la discrétisation
     */
    CommandSetEdgeMeshingProperty(Internal::Context& c, CoEdgeMeshingProperty& emp, CoEdge* ed);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param emp la propriété de discrétisation pour l'arête
     *  \param coedges les arêtes dont on va changer la discrétisation
     */
    CommandSetEdgeMeshingProperty(Internal::Context& c, CoEdgeMeshingProperty& emp,
    		std::vector<CoEdge*>& coedges);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandSetEdgeMeshingProperty();

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
    std::vector<CoEdge*> m_coedges;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDSETEDGEMESHINGPROPERTY_H_ */
/*----------------------------------------------------------------------------*/
