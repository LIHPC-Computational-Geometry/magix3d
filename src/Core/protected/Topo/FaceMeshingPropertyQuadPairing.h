/*----------------------------------------------------------------------------*/
 /*
 * FaceMeshingPropertyQuadPairing.h
 *
 *  Created on: 8 juil. 2014
 *      Author: ledouxf
 */

/*----------------------------------------------------------------------------*/
#ifndef FACEMESHINGPROPERTYQUADPAIRING_H_
#define FACEMESHINGPROPERTYQUADPAIRING_H_
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
class FaceMeshingPropertyQuadPairing : public CoFaceMeshingProperty {
public:
    /*------------------------------------------------------------------------*/
    /// Constructeur avec paramètres par défaut
    FaceMeshingPropertyQuadPairing()
    : m_min(0.0)
    , m_max(1.0e22)
    , m_is_default(true)
    {
    }
    /*------------------------------------------------------------------------*/
    /// Constructeur avec paramètres spécifiés
    FaceMeshingPropertyQuadPairing(double min, double max)
    : m_min(min)
    , m_max(max)
    , m_is_default(false)
    {
    }

    /*------------------------------------------------------------------------*/
    /// Comparaison de 2 propriétés.
    virtual bool operator == (const CoFaceMeshingProperty& mp) const
    {
    	const FaceMeshingPropertyQuadPairing* fmp = dynamic_cast<const FaceMeshingPropertyQuadPairing*> (&mp);
    	return fmp != 0 &&
    			getMin() == fmp->getMin() &&
				getMax() == fmp->getMax() &&
				m_is_default == fmp->m_is_default &&
				CoFaceMeshingProperty::operator == (*fmp);
    }

    /*------------------------------------------------------------------------*/
    /// Accesseur sur la méthode de maillage
    CoFaceMeshingProperty::meshLaw getMeshLaw() const {return CoFaceMeshingProperty::quadPairing;}

    /*------------------------------------------------------------------------*/
   /// Accesseur sur le nom de la méthode de maillage
    std::string getMeshLawName() const { return "Quad Pairing"; }

    /*------------------------------------------------------------------------*/
    /** \brief  Indique si la face est structurée
     */
    bool isStructured() const {return false;}

    /*------------------------------------------------------------------------*/
    ///  Accesseur sur paramètre de taille minimum
    double getMin() const {return m_min;}

    ///  Accesseur sur paramètre de taille maximum
    double getMax() const {return m_max;}

    /*------------------------------------------------------------------------*/
#ifndef SWIG
    /** Création d'un clone, on copie toutes les informations */
    FaceMeshingPropertyQuadPairing* clone() {return new FaceMeshingPropertyQuadPairing(*this);}

    /// Script pour la commande de création Python
    virtual TkUtil::UTF8String getScriptCommand() const
    {
        TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
        if (m_is_default)
            o << getMgx3DAlias() << ".FaceMeshingPropertyQuadPairing()";
        else
            o << getMgx3DAlias() << ".FaceMeshingPropertyQuadPairing("
            << Utils::Math::MgxNumeric::userRepresentation(m_min) << ", "
            << Utils::Math::MgxNumeric::userRepresentation(m_max) <<")";
        return o;
    }
#endif

private:

    /// taille minimum pour une arête
    double m_min;

    /// taille maximale pour une arête
    double m_max;

    /// est-ce que l'on utilise les valeurs par défaut
    bool m_is_default;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* FACEMESHINGPROPERTYQUADPAIRING_H_ */
