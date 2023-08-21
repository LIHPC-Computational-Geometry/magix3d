/*----------------------------------------------------------------------------*/
/*
 * \file CommandSetBlockMeshingProperty.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 15/10/13
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDSETBLOCKMESHINGPROPERTY_H_
#define COMMANDSETBLOCKMESHINGPROPERTY_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
class BlockMeshingProperty;
class Block;
/*----------------------------------------------------------------------------*/
/** \class CommandSetBlockMeshingProperty
 *  \brief Commande qui change la discrétisation pour une arête
 */
/*----------------------------------------------------------------------------*/
class CommandSetBlockMeshingProperty: public Topo::CommandEditTopo {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param emp la propriété de discrétisation pour le bloc
     *  \param bl le bloc dont on va changer la discrétisation
     */
    CommandSetBlockMeshingProperty(Internal::Context& c, BlockMeshingProperty& emp, Block* bl);

    /*------------------------------------------------------------------------*/
   /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param emp la propriété de discrétisation pour le bloc
     *  \param blocks les blocs dont on va changer la discrétisation
     */
    CommandSetBlockMeshingProperty(Internal::Context& c, BlockMeshingProperty& emp,
    		std::vector<Block*>& blocks);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandSetBlockMeshingProperty();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  appel non prévu
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

private:
    /// recherche le sommet suivant pour une face dans un bloc avec 2 sommets non marqués à 2
    Vertex* getNextVertex(Block* bloc, Vertex* vtx1, Vertex* vtx2, std::map<Topo::Vertex*, uint>& filtre_sommets);

    /// Change la discrétisation pour une ou plusieurs faces en la mettant orthogonale
    void setOrthogonalCoFaces(Face* face, Vertex* vtx1, Vertex* vtx2, int nbLayers, bool reverse);

private:
    /// la nouvelle propriété de discrétisation
    BlockMeshingProperty* m_prop;

    /// les blocs concernés
    std::vector<Block*> m_blocks;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDSETBLOCKMESHINGPROPERTY_H_ */
/*----------------------------------------------------------------------------*/
