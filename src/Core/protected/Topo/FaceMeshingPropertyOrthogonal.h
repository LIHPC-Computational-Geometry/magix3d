/*----------------------------------------------------------------------------*/
/*
 * \file FaceMeshingPropertyOrthogonal.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9/7/15
 */
/*----------------------------------------------------------------------------*/


#ifndef FACEPROPERTYMESHINGORTHOGONAL_H_
#define FACEPROPERTYMESHINGORTHOGONAL_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
#include "Topo/FaceMeshingPropertyDirectional.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/**
   @brief Propriété de la discrétisation d'une face commune suivant une direction
 */
class FaceMeshingPropertyOrthogonal : public FaceMeshingPropertyDirectional {
public:

    /*------------------------------------------------------------------------*/
    /// Constructeur avec direction suivant 2 sommets
    FaceMeshingPropertyOrthogonal(Utils::Math::Point v1, Utils::Math::Point v2, int nbLayers)
    : FaceMeshingPropertyDirectional(v1, v2)
	, m_side(side_undef)
    , m_nbLayers(nbLayers)
    {}

    /// Constructeur dont on connait la direction
    FaceMeshingPropertyOrthogonal(meshDirLaw md, int nbLayers)
    : FaceMeshingPropertyDirectional(md)
    , m_side(side_undef)
    , m_nbLayers(nbLayers)
    {}

    /*------------------------------------------------------------------------*/
    /// Comparaison de 2 propriétés.
    virtual bool operator == (const CoFaceMeshingProperty& mp) const
    {
    	const FaceMeshingPropertyOrthogonal* fmp = dynamic_cast<const FaceMeshingPropertyOrthogonal*> (&mp);
    	return fmp != 0 &&
    			m_nbLayers == fmp->m_nbLayers &&
    			m_side == fmp->m_side &&
				FaceMeshingPropertyDirectional::operator == (*fmp);
    }

/*------------------------------------------------------------------------*/
    /// Accesseur sur la méthode de maillage
    virtual CoFaceMeshingProperty::meshLaw getMeshLaw() const
    {return CoFaceMeshingProperty::orthogonal;}

    /*------------------------------------------------------------------------*/
   /// Accesseur sur le nom de la méthode de maillage
    virtual std::string getMeshLawName() const
    {
       if (m_dir == dir_i)
            return "OrthogonalI";
        else if (m_dir == dir_j)
            return "OrthogonalJ";
        else
            return "Direction à préciser";
    }

#ifndef SWIG
    /*------------------------------------------------------------------------*/
    /// initialisation du côté en plus
    virtual void initDir(CoFace* coface)
    {
    	FaceMeshingPropertyDirectional::initDir(coface);

    	m_side = _computeSide(coface, m_v1, m_v2, getDir());
    }

    /*------------------------------------------------------------------------*/
    virtual uint getSide() const
    {
    	if (m_side == side_min)
    		return 0;
    	else if (m_side == side_max)
    		return 1;
    	else
    		throw TkUtil::Exception(TkUtil::UTF8String ("FaceMeshingPropertyOrthogonal::getSide() ne peut se faire sans initialiser le côté", TkUtil::Charset::UTF_8));
    }

   /*------------------------------------------------------------------------*/
    /** Création d'un clone, on copie toutes les informations */
   virtual FaceMeshingPropertyOrthogonal* clone() {return new FaceMeshingPropertyOrthogonal(*this);}

    /*------------------------------------------------------------------------*/
    /// Script pour la commande de création Python
   virtual TkUtil::UTF8String getScriptCommand() const
    {
        TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
        o << getMgx3DAlias() << ".FaceMeshingPropertyOrthogonal("
                << m_v1.getScriptCommand() << ", "
                << m_v2.getScriptCommand() << ", "
				<< (long)m_nbLayers << ")";
        return o;
    }

   /// ajoute la description des propriétés spécifiques
   virtual void addProperties(Utils::SerializedRepresentation& ppt) const
   {
   	ppt.addProperty (
   			Utils::SerializedRepresentation::Property ("Nombre de couches orthogonales", (long)m_nbLayers));
   	ppt.addProperty (
   			Utils::SerializedRepresentation::Property ("Discrétisation orthogonale coté",
   					m_side == side_min?std::string("départ"):std::string("arrivée")));
   }
#endif
   /*------------------------------------------------------------------------*/
   virtual int getNbLayers() const
   { return m_nbLayers; }
   /*------------------------------------------------------------------------*/

private:

   /*------------------------------------------------------------------------*/
   /// coté où on se contraint à être orthogonal
   meshSideLaw m_side;

   /// nombre de couches pour orthogonalité
   int m_nbLayers;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* FACEPROPERTYMESHINGORTHOGONAL_H_ */
