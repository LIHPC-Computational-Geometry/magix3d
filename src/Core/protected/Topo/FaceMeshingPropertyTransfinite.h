/*----------------------------------------------------------------------------*/
/*
 * \file FaceMeshingPropertyTransfinite.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 10/10/13
 */
/*----------------------------------------------------------------------------*/


#ifndef FACEPROPERTYMESHINGTRANSFINITE_H_
#define FACEPROPERTYMESHINGTRANSFINITE_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
#include "Topo/CoFaceMeshingProperty.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/**
   @brief Propriété de la discrétisation d'une face commune
 */
class FaceMeshingPropertyTransfinite : public CoFaceMeshingProperty {
public:
    /*------------------------------------------------------------------------*/
    /// Constructeur
    FaceMeshingPropertyTransfinite()
    {}

    /*------------------------------------------------------------------------*/
    /// Accesseur sur la méthode de maillage
    CoFaceMeshingProperty::meshLaw getMeshLaw() const {return CoFaceMeshingProperty::transfinite;}

    /*------------------------------------------------------------------------*/
   /// Accesseur sur le nom de la méthode de maillage
    std::string getMeshLawName() const { return "Transfinie"; }

    /*------------------------------------------------------------------------*/
    /** \brief  Indique si la face est structurée
     */
    bool isStructured() const {return true;}

#ifndef SWIG
    /*------------------------------------------------------------------------*/
    /** Création d'un clone, on copie toutes les informations */
    FaceMeshingPropertyTransfinite* clone() {return new FaceMeshingPropertyTransfinite(*this);}

    /*------------------------------------------------------------------------*/
    /// Script pour la commande de création Python
    virtual TkUtil::UTF8String getScriptCommand() const
    {
        TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
        o << getMgx3DAlias() << ".FaceMeshingPropertyTransfinite()";
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

#endif /* FACEPROPERTYMESHINGTRANSFINITE_H_ */
