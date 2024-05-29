/*----------------------------------------------------------------------------*/
/*
 * \file FaceMeshingPropertyMeshGems.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 10/10/13
 */
/*----------------------------------------------------------------------------*/

#ifdef USE_MESHGEMS

#ifndef FACEPROPERTYMESHINGMeshGems_H_
#define FACEPROPERTYMESHINGMeshGems_H_
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
class FaceMeshingPropertyMeshGems : public CoFaceMeshingProperty {
public:
    /*------------------------------------------------------------------------*/
    /// Constructeur avec paramètres par défaut
    FaceMeshingPropertyMeshGems()
    : m_size(0.1),
	  m_gradation(1.3),
	  m_is_default(true)
    {
    }
    /*------------------------------------------------------------------------*/
    /// Constructeur avec paramètres spécifiés
    FaceMeshingPropertyMeshGems(double size, double grad)
    : m_size(size),
	  m_gradation(grad),
	  m_is_default(false)
    {
    }

    /*------------------------------------------------------------------------*/
    /// Comparaison de 2 propriétés.
    virtual bool operator == (const CoFaceMeshingProperty& mp) const
    {
    	const FaceMeshingPropertyMeshGems* fmp = dynamic_cast<const FaceMeshingPropertyMeshGems*> (&mp);
    	return fmp != 0 &&
    			getSize() == fmp->getSize() &&
				getGradation() == fmp->getGradation() &&
				m_is_default == fmp->m_is_default &&
				CoFaceMeshingProperty::operator == (*fmp);
    }
    /*------------------------------------------------------------------------*/
    /// Accesseur sur la méthode de maillage
    CoFaceMeshingProperty::meshLaw getMeshLaw() const {return CoFaceMeshingProperty::MeshGems;}

    /*------------------------------------------------------------------------*/
   /// Accesseur sur le nom de la méthode de maillage
    std::string getMeshLawName() const { return "MeshGems (Distene)"; }

    /*------------------------------------------------------------------------*/
    /** \brief  Indique si la face est structurée
     */
    bool isStructured() const {return false;}

    /*------------------------------------------------------------------------*/
    ///  Accesseur sur paramètre de taille
    double getSize() const {return m_size;}

    /*------------------------------------------------------------------------*/
    ///  Accesseur sur paramètre de gradation
    double getGradation() const {return m_gradation;}

    /*------------------------------------------------------------------------*/
#ifndef SWIG
    /** Création d'un clone, on copie toutes les informations */
    FaceMeshingPropertyMeshGems* clone() {return new FaceMeshingPropertyMeshGems(*this);}

    /// Script pour la commande de création Python
    virtual TkUtil::UTF8String getScriptCommand() const
    {
        TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
        if (m_is_default)
            o << getMgx3DAlias() << ".FaceMeshingPropertyMeshGems()";
        else
            o << getMgx3DAlias() << ".FaceMeshingPropertyMeshGems("
        	<< Utils::Math::MgxNumeric::userRepresentation(m_size) << ", "
            << Utils::Math::MgxNumeric::userRepresentation(m_gradation) <<")";
        return o;
    }

    /// ajoute la description des propriétés spécifiques
    virtual void addProperties(Utils::SerializedRepresentation& ppt) const
    {
    	ppt.addProperty (
    			Utils::SerializedRepresentation::Property ("Utilisation des valeurs par défaut",
    					m_is_default?std::string("vrai"):std::string("faux")));
    	ppt.addProperty (
    			Utils::SerializedRepresentation::Property ("Taille à respecter", m_size));
    	ppt.addProperty (
    			Utils::SerializedRepresentation::Property ("Ratio max entre 2 adjacents", m_gradation));
    }
#endif

private:

    /// taille à respecter sur la face
    double m_size;

    /// Gradation sur la face
    double m_gradation;

    /// est-ce que l'on utilise les valeurs par défaut
    bool m_is_default;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* FACEPROPERTYMESHINGMeshGems_H_ */

#endif	// USE_MESHGEMS
