/*----------------------------------------------------------------------------*/
/*
 * \file BlockMeshingPropertyTransfinite.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 17/10/13
 */
/*----------------------------------------------------------------------------*/


#ifndef MGX3D_TOPO_BLOCKMESHINGPROPERTYTRANSFINITE_H_
#define MGX3D_TOPO_BLOCKMESHINGPROPERTYTRANSFINITE_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
#include "Topo/BlockMeshingProperty.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/**
   @brief Propriété de la discrétisation d'un bloc
 */
class BlockMeshingPropertyTransfinite : public BlockMeshingProperty {
public:

    /*------------------------------------------------------------------------*/
    BlockMeshingPropertyTransfinite()
    {}

    /*------------------------------------------------------------------------*/
#ifndef SWIG
   /** Création d'un clone, on copie toutes les informations */
    BlockMeshingPropertyTransfinite* clone()  {return new BlockMeshingPropertyTransfinite(*this);}
#endif
    /*------------------------------------------------------------------------*/
    /// Accesseur sur la méthode de maillage
    BlockMeshingProperty::meshLaw getMeshLaw() const {return BlockMeshingProperty::transfinite;}

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le nom de la méthode de maillage
    std::string getMeshLawName() const { return "Transfinie"; }

    /*------------------------------------------------------------------------*/
    /** \brief  Indique si la face est structurée
     */
    bool isStructured() const {return true;}

    /*------------------------------------------------------------------------*/
#ifndef SWIG
    /// Script pour la commande de création Python
    TkUtil::UTF8String getScriptCommand() const
    {
        TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
        o << getMgx3DAlias() << ".BlockMeshingPropertyTransfinite()";
        return o;
    }
#endif

private:

};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* MGX3D_TOPO_BLOCKMESHINGPROPERTYTRANSFINITE_H_ */
