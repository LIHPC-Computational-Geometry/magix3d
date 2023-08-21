/*----------------------------------------------------------------------------*/
/*
 * \file FaceMeshingPropertyDelaunayGMSH.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 10/10/13
 */
/*----------------------------------------------------------------------------*/


#ifndef FACEPROPERTYMESHINGDELAUNAYGMSH_H_
#define FACEPROPERTYMESHINGDELAUNAYGMSH_H_
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
class FaceMeshingPropertyDelaunayGMSH : public CoFaceMeshingProperty {
public:
    /*------------------------------------------------------------------------*/
    /// Constructeur avec paramètres par défaut
    FaceMeshingPropertyDelaunayGMSH()
    : m_min(0.0)
    , m_max(1.0e22)
    , m_is_default(true)
    {
    }
    /*------------------------------------------------------------------------*/
    /// Constructeur avec paramètres spécifiés
    FaceMeshingPropertyDelaunayGMSH(double min, double max)
    : m_min(min)
    , m_max(max)
    , m_is_default(false)
    {
    }

    /*------------------------------------------------------------------------*/
    /// Comparaison de 2 propriétés.
    virtual bool operator == (const CoFaceMeshingProperty& mp) const
    {
    	const FaceMeshingPropertyDelaunayGMSH* fmp = dynamic_cast<const FaceMeshingPropertyDelaunayGMSH*> (&mp);
    	return fmp != 0 &&
    			getMin() == fmp->getMin() &&
				getMax() == fmp->getMax() &&
				m_is_default == fmp->m_is_default &&
				CoFaceMeshingProperty::operator == (*fmp);
    }

    /*------------------------------------------------------------------------*/
    /// Accesseur sur la méthode de maillage
    CoFaceMeshingProperty::meshLaw getMeshLaw() const {return CoFaceMeshingProperty::delaunayGMSH;}

    /*------------------------------------------------------------------------*/
   /// Accesseur sur le nom de la méthode de maillage
    std::string getMeshLawName() const { return "Delaunay (GMSH)"; }

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
    FaceMeshingPropertyDelaunayGMSH* clone() {return new FaceMeshingPropertyDelaunayGMSH(*this);}

    /// Script pour la commande de création Python
    virtual TkUtil::UTF8String getScriptCommand() const
    {
        TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
        if (m_is_default)
            o << getMgx3DAlias() << ".FaceMeshingPropertyDelaunayGMSH()";
        else
            o << getMgx3DAlias() << ".FaceMeshingPropertyDelaunayGMSH("
            << Utils::Math::MgxNumeric::userRepresentation(m_min) << ", "
            << Utils::Math::MgxNumeric::userRepresentation(m_max) <<")";
        return o;
    }

    /// ajoute la description des propriétés spécifiques
    virtual void addProperties(Utils::SerializedRepresentation& ppt) const
    {
    	ppt.addProperty (
    			Utils::SerializedRepresentation::Property ("Utilisation des valeurs par défaut",
    					m_is_default?std::string("vrai"):std::string("faux")));
    	ppt.addProperty (
    			Utils::SerializedRepresentation::Property ("Taille minimum", m_min));
    	ppt.addProperty (
    			Utils::SerializedRepresentation::Property ("Taille maximum", m_max));
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

#endif /* FACEPROPERTYMESHINGDELAUNAYGMSH_H_ */
