/*----------------------------------------------------------------------------*/
/*
 * \file BlockMeshingPropertyInsertion.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 17/10/13
 */
/*----------------------------------------------------------------------------*/


#ifndef MGX3D_TOPO_BLOCKMESHINGPROPERTYINSERTION_H_
#define MGX3D_TOPO_BLOCKMESHINGPROPERTYINSERTION_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
#include "Topo/BlockMeshingProperty.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/**
   @brief Propriété pour un bloc associé à la méthode insertion
 */
class BlockMeshingPropertyInsertion : public BlockMeshingProperty {
public:

    /*------------------------------------------------------------------------*/
    BlockMeshingPropertyInsertion()
	: m_withRefinement(false)
    {}

    /*------------------------------------------------------------------------*/
    BlockMeshingPropertyInsertion(const bool AWithRefinement)
	: m_withRefinement(AWithRefinement)
    {}

    /*------------------------------------------------------------------------*/
#ifndef SWIG
    /** Création d'un clone, on copie toutes les informations */
    BlockMeshingPropertyInsertion* clone()  {return new BlockMeshingPropertyInsertion(*this);}
#endif
    /*------------------------------------------------------------------------*/
    /// Accesseur sur la méthode de maillage
    BlockMeshingProperty::meshLaw getMeshLaw() const {return BlockMeshingProperty::insertion;}

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le nom de la méthode de maillage
    std::string getMeshLawName() const { return "Insertion"; }

    /*------------------------------------------------------------------------*/
    /** \brief  Indique si la méthode est structurée
     */
    bool isStructured() const {return false;}

    /*------------------------------------------------------------------------*/
    /** \brief  Indique si le maillage doit être rafiné
     */
    bool withRefinement() const { return m_withRefinement; }

    /*------------------------------------------------------------------------*/
#ifndef SWIG
    /// Script pour la commande de création Python
    TkUtil::UTF8String getScriptCommand() const
    {
        TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
        o << getMgx3DAlias() << ".BlockMeshingPropertyInsertion()";
        return o;
    }
#endif

private:

    // Indique s'il y aura rafinement du maillage pour aider l'insertion
    bool m_withRefinement;

};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* MGX3D_TOPO_BLOCKMESHINGPROPERTYINSERTION_H_ */
