/*----------------------------------------------------------------------------*/
/*
 * \file BlockMeshingPropertyDelaunayMeshGems.h
 *
 *  \author Nicolas Le Goff
 *
 *  \date 12/02/19
 */
/*----------------------------------------------------------------------------*/

#ifdef USE_MESHGEMS

#ifndef MGX3D_TOPO_BLOCKMESHINGPROPERTYDELAUNAYMESHGEMS_H_
#define MGX3D_TOPO_BLOCKMESHINGPROPERTYDELAUNAYMESHGEMS_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
#include "Topo/BlockMeshingProperty.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/**
   @brief Propriété de la discrétisation d'un bloc suivant une direction
 */
class BlockMeshingPropertyDelaunayMeshGems : public BlockMeshingProperty {
public:

    typedef enum{
        vertices = 0,
		light,
		standard,
		strong
    } MeshGemsOptimizationLevel;

    /*------------------------------------------------------------------------*/
    /// Constructeur pour la méthode de maillage d'un bloc avec le Delaunay de meshgems
    BlockMeshingPropertyDelaunayMeshGems()
    : m_is_default(true),
	  m_optimLvl(MeshGemsOptimizationLevel::standard),
	  m_verbosity(3),
	  m_gradation(1.05),
	  m_min_size(-1.),
	  m_max_size(-1.),
	  m_optimise_worst_elements(false),
	  m_ratio_pyramid_size(1.0)

    {

    }

    /*------------------------------------------------------------------------*/
    /// Constructeur avec paramètres spécifiés
    BlockMeshingPropertyDelaunayMeshGems(MeshGemsOptimizationLevel optimLvl,
    		int verbosity,
			double gradation,
			double min_size,
			double max_size,
			bool optimise_worst_elements,
			double ratio_pyramid_size=1.0) // valeur par défaut pour compatibilité avec anciens scripts

    : m_is_default(false),
	  m_optimLvl(optimLvl),
      m_verbosity(verbosity),
	  m_gradation(gradation),
	  m_min_size(min_size),
	  m_max_size(max_size),
	  m_optimise_worst_elements(optimise_worst_elements),
	  m_ratio_pyramid_size(ratio_pyramid_size)
    {

    }

    /*------------------------------------------------------------------------*/
#ifndef SWIG
    /** Création d'un clone, on copie toutes les informations */
    BlockMeshingPropertyDelaunayMeshGems* clone()  {return new BlockMeshingPropertyDelaunayMeshGems(*this);}
#endif
    /*------------------------------------------------------------------------*/
    /// Accesseur sur la méthode de maillage
    BlockMeshingProperty::meshLaw getMeshLaw() const {return BlockMeshingProperty::delaunayMeshGemsVol;}

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le nom de la méthode de maillage
    std::string getMeshLawName() const { return "Delaunay (MeshGemsVol)"; }

    /*------------------------------------------------------------------------*/
    /** \brief  Indique si la face est structurée
     */
    bool isStructured() const {return false;}

    /*------------------------------------------------------------------------*/
    /** \brief  passe en mode verbose
     */
    void setVerbose() {m_verbosity=10;}
    /*------------------------------------------------------------------------*/
    /** \brief  passe en mode silencieux
     */
    void setQuiet() {m_verbosity=0;}

    /*------------------------------------------------------------------------*/
    /// Accesseur sur la verbosité de l'appel à MeshGems
    int getVerbosity() const { return m_verbosity; }

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le niveau d'optimisation
    MeshGemsOptimizationLevel getOptimLvl() const { return m_optimLvl; }

    /*------------------------------------------------------------------------*/
    /// Accesseur sur la gradation
    double getGradation() const { return m_gradation; }

    /*------------------------------------------------------------------------*/
    /// Accesseur sur la taille min
    double getMinSize() const { return m_min_size; }

    /*------------------------------------------------------------------------*/
    /// Accesseur sur la taille max
    double getMaxSize() const { return m_max_size; }

    /*------------------------------------------------------------------------*/
    /// Accesseur sur l'optimisation des éléments les plus mauvais
    bool getOptimWorstElem() const { return m_optimise_worst_elements; }

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le ratio pour la hauteur des pyramides
    double getRatioPyramidSize() const { return m_ratio_pyramid_size; }

    /*------------------------------------------------------------------------*/
#ifndef SWIG
    /// Script pour la commande de création Python
    TkUtil::UTF8String getScriptCommand() const
    {
        TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
        if (m_is_default) {
            o << getMgx3DAlias() << ".BlockMeshingPropertyDelaunayMeshGems()";
        } else {
        	o << getMgx3DAlias() << ".BlockMeshingPropertyDelaunayMeshGems("
        			<< Utils::Math::MgxNumeric::userRepresentation(m_optimLvl) << ", "
					<< Utils::Math::MgxNumeric::userRepresentation(m_verbosity) << ", "
					<< Utils::Math::MgxNumeric::userRepresentation(m_gradation) << ", "
					<< Utils::Math::MgxNumeric::userRepresentation(m_min_size) << ", "
					<< Utils::Math::MgxNumeric::userRepresentation(m_max_size) << ", "
					<< Utils::Math::MgxNumeric::userRepresentation(m_optimise_worst_elements) << ", "
                    << (m_optimise_worst_elements?"True":"False") << ", "
        			<< Utils::Math::MgxNumeric::userRepresentation(m_ratio_pyramid_size) << ")";
        }

        return o;
    }

    TkUtil::UTF8String serializeParameters() const{
        TkUtil::UTF8String params ("Y", TkUtil::Charset::UTF_8);
        params<<Utils::Math::MgxNumeric::userRepresentation(m_verbosity);

        return params;
    }

    /// ajoute la description des propriétés spécifiques
    virtual void addProperties(Utils::SerializedRepresentation& ppt) const
    {
    	ppt.addProperty (
    			Utils::SerializedRepresentation::Property ("Utilisation des valeurs par défaut",
    					m_is_default?std::string("vrai"):std::string("faux")));
    	ppt.addProperty (
    			Utils::SerializedRepresentation::Property ("Niveau d'optimisation", (long)m_optimLvl));
    	ppt.addProperty (
    			Utils::SerializedRepresentation::Property ("Niveau de verbosité", (long)m_verbosity));
    	ppt.addProperty (
    			Utils::SerializedRepresentation::Property ("Ratio max entre 2 adjacents", m_gradation));
    	ppt.addProperty (
    			Utils::SerializedRepresentation::Property ("Taille minimum", m_min_size));
    	ppt.addProperty (
    			Utils::SerializedRepresentation::Property ("Taille maximum", m_max_size));
    	ppt.addProperty (
    			Utils::SerializedRepresentation::Property ("Optimisation des mauvais éléments",
    					m_optimise_worst_elements?std::string("vrai"):std::string("faux")));
    	ppt.addProperty (
    			Utils::SerializedRepresentation::Property ("Ratio pour la hauteur des pyramides", m_ratio_pyramid_size));
    }

#endif

private:

    // est-ce que l'on utilise les valeurs par défaut
    bool m_is_default;

    // niveau d'optimisation
    MeshGemsOptimizationLevel m_optimLvl;

    // niveau de verbosité
    int m_verbosity;

    // desired maximum ratio between 2 adjacent tetrahedra edges
    double m_gradation;

    // desired minimum edge size in the generated mesh. -1 means no min length
    double m_min_size;

    // desired maximum edge size in the generated mesh. -1 means no max length
    double m_max_size;

    // applies an optimization processing to improve the worst quality elements
    // whenever possible
    bool m_optimise_worst_elements;

    /// paramètre de contrôle de la hauteur des pyramides insérées sur les faces lorsqu'elles sont structurées (1 par défaut)
    double m_ratio_pyramid_size;

    };
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* MGX3D_TOPO_BLOCKMESHINGPROPERTYDELAUNAYMESHGEMS_H_ */

#endif	// USE_MESHGEMS
