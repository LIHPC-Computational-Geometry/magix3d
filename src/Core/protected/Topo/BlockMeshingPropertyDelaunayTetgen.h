/*----------------------------------------------------------------------------*/
/*
 * \file BlockMeshingPropertyDelaunayTetgen.h
 *
 *  \author Nicolas Le Goff
 *
 *  \date 22/10/13
 */
/*----------------------------------------------------------------------------*/


#ifndef MGX3D_TOPO_BLOCKMESHINGPROPERTYDELAUNAYTETGEN_H_
#define MGX3D_TOPO_BLOCKMESHINGPROPERTYDELAUNAYTETGEN_H_
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
class BlockMeshingPropertyDelaunayTetgen : public BlockMeshingProperty {
public:

    typedef enum{
        Min,//CDT,
        QCRadius,
        QCMaxVol,
        QC,
    } DelaunayLaw;

    /*------------------------------------------------------------------------*/
    /// Constructeur pour la méthode de maillage d'un bloc avec le Delaunay de Tetgen
    BlockMeshingPropertyDelaunayTetgen(const DelaunayLaw delLaw = QC,
                                       const double val=2.0, const double val2 = 2.0, double ratio_pyramid_size=1.0)
    : m_law(delLaw), m_verbose(false), m_is_default(true), m_ratio_pyramid_size(ratio_pyramid_size)
    {
        if(m_law==QC){
            m_radius_edge_ratio=val;
            m_max_volume=val2;
        }
        else if(m_law==QCRadius){
            m_radius_edge_ratio=val;
            m_max_volume=0;
        }
        else if(m_law==QCMaxVol){
            m_radius_edge_ratio=0;
            m_max_volume=val;
        }
        else{
            m_radius_edge_ratio=0;
            m_max_volume=0;
        }
    }

    /*------------------------------------------------------------------------*/
    /// Constructeur avec paramètres spécifiés
    BlockMeshingPropertyDelaunayTetgen(double radius_edge_ratio, double max_volume, double ratio_pyramid_size=1.0)
    : m_law(QC),
      m_radius_edge_ratio(radius_edge_ratio),
      m_max_volume(max_volume),
      m_verbose(false),
      m_is_default(true),
	  m_ratio_pyramid_size(ratio_pyramid_size)
    {}

    /*------------------------------------------------------------------------*/
#ifndef SWIG
    /** Création d'un clone, on copie toutes les informations */
    BlockMeshingPropertyDelaunayTetgen* clone()  {return new BlockMeshingPropertyDelaunayTetgen(*this);}
#endif
    /*------------------------------------------------------------------------*/
    /// Accesseur sur la méthode de maillage
    BlockMeshingProperty::meshLaw getMeshLaw() const {return BlockMeshingProperty::delaunayTetgen;}

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le nom de la méthode de maillage
    std::string getMeshLawName() const { return "Delaunay (Tetgen)"; }

    /*------------------------------------------------------------------------*/
    /** \brief  Indique si la face est structurée
     */
    bool isStructured() const {return false;}

    /*------------------------------------------------------------------------*/
    /** \brief  passe en mode verbose
     */
    void setVerbose() {m_verbose=true;}
    /*------------------------------------------------------------------------*/
    /** \brief  passe en mode silencieux
     */
    void setQuiet() {m_verbose=false;}

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le facteur de qualité
    double getRadiusEdgeRatio() const { return m_radius_edge_ratio; }

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le volume maximum d'une maille
    double getVolumeMax() const { return m_max_volume; }

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le ratio pour la hauteur des pyramides
    double getRatioPyramidSize() const { return m_ratio_pyramid_size; }

    /*------------------------------------------------------------------------*/
#ifndef SWIG
    /// Script pour la commande de création Python
    TkUtil::UTF8String getScriptCommand() const
    {
        TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
        if (m_is_default)
            o << getMgx3DAlias() << ".BlockMeshingPropertyDelaunayTetgen()";
        else{
            if(m_law==QC)
                o << getMgx3DAlias() << ".BlockMeshingPropertyDelaunayTetgen("
                  << getMgx3DAlias()<<".BlockMeshingPropertyDelaunayTetgen.QC, "
                  << Utils::Math::MgxNumeric::userRepresentation(m_radius_edge_ratio) << ", "
                  << Utils::Math::MgxNumeric::userRepresentation(m_max_volume) <<")";
            else if(m_law==QCRadius)
                o << getMgx3DAlias() << ".BlockMeshingPropertyDelaunayTetgen("
                  << getMgx3DAlias()<<".BlockMeshingPropertyDelaunayTetgen.QCRadius, "
                  << Utils::Math::MgxNumeric::userRepresentation(m_radius_edge_ratio) << ")";
            else if(m_law==QCMaxVol)
                o << getMgx3DAlias() << ".BlockMeshingPropertyDelaunayTetgen("
                  << getMgx3DAlias()<<".BlockMeshingPropertyDelaunayTetgen.QCMaxVol, "
                  << Utils::Math::MgxNumeric::userRepresentation(m_max_volume) << ")";
            else
                o << getMgx3DAlias() << ".BlockMeshingPropertyDelaunayTetgen("
                  << getMgx3DAlias()<<".BlockMeshingPropertyDelaunayTetgen.Min)";
        }
        return o;
    }

    TkUtil::UTF8String serializeParameters() const{
        TkUtil::UTF8String params ("Y", TkUtil::Charset::UTF_8);
        params<<(m_verbose?"V":"Q");

        if(m_law==QC){
            params<<"q"<<Utils::Math::MgxNumeric::userRepresentation(m_radius_edge_ratio);
            params<<"a"<<Utils::Math::MgxNumeric::userRepresentation(m_max_volume);
        }
        else if(m_law==QCRadius){
            params<<"q"<<Utils::Math::MgxNumeric::userRepresentation(m_radius_edge_ratio);
        }
        else if(m_law==QCMaxVol){
            params<<"a"<<Utils::Math::MgxNumeric::userRepresentation(m_max_volume);
        }

        return params;
    }

    /*------------------------------------------------------------------------*/
    /// ajoute la description des propriétés spécifiques
    virtual void addProperties(Utils::SerializedRepresentation& ppt) const
    {
    	ppt.addProperty (
    			Utils::SerializedRepresentation::Property ("Utilisation des valeurs par défaut",
    					m_is_default?std::string("vrai"):std::string("faux")));
    	ppt.addProperty (
    			Utils::SerializedRepresentation::Property ("Utilisation du mode verbeux",
    					m_verbose?std::string("vrai"):std::string("faux")));

    	ppt.addProperty (
    			Utils::SerializedRepresentation::Property ("Indice loi de maillage", (long)m_law));
    	ppt.addProperty (
    			Utils::SerializedRepresentation::Property ("Facteur de qualité", m_radius_edge_ratio));
    	ppt.addProperty (
    			Utils::SerializedRepresentation::Property ("Taille max", m_max_volume));
    	ppt.addProperty (
    			Utils::SerializedRepresentation::Property ("Ratio pour la hauteur des pyramides", m_ratio_pyramid_size));
   }
#endif

private:

    DelaunayLaw m_law;

    // indique de passer en mode verbose pour remonter des informations de
    // debug - defaut -silence = false
    bool m_verbose;

    // est-ce que l'on utilise les valeurs par défaut
    bool m_is_default;

    // facteur de qualite - defaut : 2.0, contrainte >0
    double m_radius_edge_ratio;

    // taille max des elements - defaut : 1
    double m_max_volume;

    /// paramètre de contrôle de la hauteur des pyramides insérées sur les faces lorsqu'elles sont structurées (1 par défaut)
    double m_ratio_pyramid_size;

    };
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* MGX3D_TOPO_BLOCKMESHINGPROPERTYDELAUNAYTETGEN_H_ */
