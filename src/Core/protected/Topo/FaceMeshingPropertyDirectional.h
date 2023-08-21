/*----------------------------------------------------------------------------*/
/*
 * \file FaceMeshingPropertyDirectional.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 10/10/13
 */
/*----------------------------------------------------------------------------*/


#ifndef FACEPROPERTYMESHINGDIRECTIONAL_H_
#define FACEPROPERTYMESHINGDIRECTIONAL_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
#include "Topo/CoFaceMeshingProperty.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/**
   @brief Propriété de la discrétisation d'une face commune suivant une direction
 */
class FaceMeshingPropertyDirectional : public CoFaceMeshingProperty {
public:

    /*------------------------------------------------------------------------*/
    /// Constructeur avec direction suivant 2 sommets
    FaceMeshingPropertyDirectional(Utils::Math::Point v1, Utils::Math::Point v2)
    : m_dir(dir_undef), m_v1(v1), m_v2(v2)
    {}

    /// Constructeur dont on connait la direction
    FaceMeshingPropertyDirectional(meshDirLaw md)
    : m_dir(md), m_v1(), m_v2()
    {}

    /*------------------------------------------------------------------------*/
    /// Comparaison de 2 propriétés.
    virtual bool operator == (const CoFaceMeshingProperty& mp) const
    {
    	const FaceMeshingPropertyDirectional* fmp = dynamic_cast<const FaceMeshingPropertyDirectional*> (&mp);
    	return fmp != 0 &&
    			m_dir == fmp->m_dir &&
    			m_v1 == fmp->m_v1 &&
    			m_v2 == fmp->m_v2 &&
				CoFaceMeshingProperty::operator == (*fmp);
    }

    /*------------------------------------------------------------------------*/
    /// Accesseur sur la méthode de maillage
    virtual CoFaceMeshingProperty::meshLaw getMeshLaw() const {return CoFaceMeshingProperty::directional;}

    /*------------------------------------------------------------------------*/
   /// Accesseur sur le nom de la méthode de maillage
    virtual std::string getMeshLawName() const
    {
       if (m_dir == dir_i)
            return "DirectionI";
        else if (m_dir == dir_j)
            return "DirectionJ";
        else
            return "Direction à préciser";
    }

    /*------------------------------------------------------------------------*/
    /// Accesseur sur la direction associée à la CoFace
    virtual uint getDir() const
    {
        if (m_dir == dir_i)
            return 0;
        else if (m_dir == dir_j)
            return 1;
        else
            throw TkUtil::Exception(TkUtil::UTF8String ("FaceMeshingPropertyDirectional::getDir() ne peut se faire sans initialiser la direction", TkUtil::Charset::UTF_8));
    }

    /*------------------------------------------------------------------------*/
    /// inversion de la direction
    virtual void permDir()
    {
        if (m_dir == dir_i)
            m_dir = dir_j;
        else if (m_dir == dir_j)
            m_dir = dir_i;
        else
            throw TkUtil::Exception(TkUtil::UTF8String ("FaceMeshingPropertyDirectional::permDir() ne peut se faire sans initialiser la direction", TkUtil::Charset::UTF_8));
    }

    /*------------------------------------------------------------------------*/
    /** \brief  Indique si la face est structurée
     */
    virtual bool isStructured() const {return true;}

#ifndef SWIG
    /*------------------------------------------------------------------------*/
    /// initialisation de la direction en fonction de ce qui a été donné à la construction
    virtual void initDir(CoFace* coface)
    {
        m_dir = _computeDir(coface, m_v1, m_v2);
    }

   /*------------------------------------------------------------------------*/
    /** Création d'un clone, on copie toutes les informations */
   virtual FaceMeshingPropertyDirectional* clone() {return new FaceMeshingPropertyDirectional(*this);}

    /*------------------------------------------------------------------------*/
    /// Script pour la commande de création Python
   virtual TkUtil::UTF8String getScriptCommand() const
    {
        TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
        o << getMgx3DAlias() << ".FaceMeshingPropertyDirectional("
                << m_v1.getScriptCommand() << ", "
                << m_v2.getScriptCommand() << ")";
        return o;
    }
#endif

protected:

    /*------------------------------------------------------------------------*/
    /// Direction suivant laquelle est effectuée la discrétisation (relative à la CoFace)
    meshDirLaw m_dir;

    /// premier point pour déterminer la direction
    Utils::Math::Point m_v1;

    /// deuxième point pour déterminer la direction
    Utils::Math::Point m_v2;

};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* FACEPROPERTYMESHINGDIRECTIONAL_H_ */
