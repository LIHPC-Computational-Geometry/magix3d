/*----------------------------------------------------------------------------*/
/*
 * \file CommandSetFaceMeshingProperty.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 15/10/13
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDSETFACEMESHINGPROPERTY_H_
#define COMMANDSETFACEMESHINGPROPERTY_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
class CoFaceMeshingProperty;
class CoFace;
/*----------------------------------------------------------------------------*/
/** \class CommandSetFaceMeshingProperty
 *  \brief Commande qui change la discrétisation pour une arête
 */
/*----------------------------------------------------------------------------*/
class CommandSetFaceMeshingProperty: public Topo::CommandEditTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param emp la propriété de discrétisation pour la face
     *  \param fa la face commune dont on va changer la discrétisation
     */
    CommandSetFaceMeshingProperty(Internal::Context& c, CoFaceMeshingProperty& emp, CoFace* fa);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param emp la propriété de discrétisation pour la face
     *  \param cofaces la liste des faces communes dont on va changer la discrétisation
     */
    CommandSetFaceMeshingProperty(Internal::Context& c, CoFaceMeshingProperty& emp,
    		std::vector<CoFace*>& cofaces);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandSetFaceMeshingProperty();

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
    CoFaceMeshingProperty* m_prop;

    /// les faces consernées
    std::vector<CoFace*> m_cofaces;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDSETFACEMESHINGPROPERTY_H_ */
/*----------------------------------------------------------------------------*/
