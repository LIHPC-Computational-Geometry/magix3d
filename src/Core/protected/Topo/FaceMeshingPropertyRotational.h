/*----------------------------------------------------------------------------*/
/*
 * \file FaceMeshingPropertyRotational.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 10/10/13
 */
/*----------------------------------------------------------------------------*/


#ifndef FACEPROPERTYMESHINGROTATIONAL_H_
#define FACEPROPERTYMESHINGROTATIONAL_H_
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
class FaceMeshingPropertyRotational : public CoFaceMeshingProperty {
public:

    /*------------------------------------------------------------------------*/
    /** Constructeur avec rotation suivant 2 sommets,
        autour d'un axe défini par deux points
    */
    FaceMeshingPropertyRotational(Utils::Math::Point v1, Utils::Math::Point v2,
            Utils::Math::Point axis1, Utils::Math::Point axis2)
    : m_dir(dir_undef), m_v1(v1), m_v2(v2), m_axis1(axis1), m_axis2(axis2)
    {}

    /** Constructeur avec rotation déterminée,
         autour d'un axe défini par deux points
     */
     FaceMeshingPropertyRotational(meshDirLaw md,
             Utils::Math::Point axis1, Utils::Math::Point axis2)
     : m_dir(md), m_v1(), m_v2(), m_axis1(axis1), m_axis2(axis2)
     {}

     /*------------------------------------------------------------------------*/
     /// Comparaison de 2 propriétés.
     virtual bool operator == (const CoFaceMeshingProperty& mp) const
     {
     	const FaceMeshingPropertyRotational* fmp = dynamic_cast<const FaceMeshingPropertyRotational*> (&mp);
     	return fmp != 0 &&
     			getDir() == fmp->getDir() &&
				m_v1 == fmp->m_v1 &&
				m_v2 == fmp->m_v2 &&
				m_axis1 == fmp->m_axis1 &&
				m_axis2 == fmp->m_axis2 &&
 				CoFaceMeshingProperty::operator == (*fmp);
     }
     /*------------------------------------------------------------------------*/
    /// Accesseur sur la méthode de maillage
    CoFaceMeshingProperty::meshLaw getMeshLaw() const {return CoFaceMeshingProperty::rotational;}

    /*------------------------------------------------------------------------*/
   /// Accesseur sur le nom de la méthode de maillage
    std::string getMeshLawName() const
    {
       if (m_dir == dir_i)
            return "RotationI";
        else if (m_dir == dir_j)
            return "RotationJ";
        else
            return "Rotation à préciser";
    }

    /*------------------------------------------------------------------------*/
    /// Accesseur sur la direction associée à la CoFace
    uint getDir() const
    {
        if (m_dir == dir_i)
            return 0;
        else if (m_dir == dir_j)
            return 1;
        else
            throw TkUtil::Exception(TkUtil::UTF8String ("FaceMeshingPropertyRotational::getDir() ne peut se faire sans initialiser la direction", TkUtil::Charset::UTF_8));
    }

    /*------------------------------------------------------------------------*/
    /** \brief  Indique si la face est structurée
     */
    bool isStructured() const {return true;}

    /*------------------------------------------------------------------------*/
    /**  Retourne l'axe de rotation */
    void getAxis(Utils::Math::Point & axis1, Utils::Math::Point & axis2)
    {
        axis1 = m_axis1;
        axis2 = m_axis2;
    }
    /*------------------------------------------------------------------------*/
    /**  Change l'axe de rotation */
    void setAxis(Utils::Math::Point & axis1, Utils::Math::Point & axis2)
    {
        m_axis1 = axis1;
        m_axis2 = axis2;
    }

    /*------------------------------------------------------------------------*/
#ifndef SWIG
    void initDir(CoFace* coface)
    {
        m_dir = _computeDir(coface, m_v1, m_v2);
    }

    /*------------------------------------------------------------------------*/
    /** Création d'un clone, on copie toutes les informations */
    FaceMeshingPropertyRotational* clone() {return new FaceMeshingPropertyRotational(*this);}

    /*------------------------------------------------------------------------*/
   /// Script pour la commande de création Python
   virtual TkUtil::UTF8String getScriptCommand() const
    {
        TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
        o << getMgx3DAlias() << ".FaceMeshingPropertyRotational("
                << m_v1.getScriptCommand() << ", "
                << m_v2.getScriptCommand() << ", "
                << m_axis1.getScriptCommand() << ", "
                << m_axis2.getScriptCommand() << ")";
        return o;
    }
#endif

private:

    /*------------------------------------------------------------------------*/
    /// Direction suivant laquelle est effectuée la discrétisation (relative à la CoFace)
    meshDirLaw m_dir;

    /// premier point pour déterminer la direction de la rotation
    Utils::Math::Point m_v1;

    /// deuxième point pour déterminer la direction de la rotation
    Utils::Math::Point m_v2;

    /// premier point pour l'axe de rotation de la méthode "rotation"
    Utils::Math::Point m_axis1;

    /// deuxième point pour l'axe de rotation de la méthode "rotation"
    Utils::Math::Point m_axis2;

};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* FACEPROPERTYMESHINGROTATIONAL_H_ */
