/*----------------------------------------------------------------------------*/
/*
 * \file FaceMeshingProperty.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 25/4/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef FACEPROPERTYMESHING_H_
#define FACEPROPERTYMESHING_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Common.h"
#include <sys/types.h>
#include <map>
#include "TkUtil/Exception.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/**
   @brief Propriété (2 ratios) de la discrétisation d'une face par rapport aux discrétisations
   des faces communes, et propriété de structuration ou non
 */
class FaceMeshingProperty {
public:

    /*------------------------------------------------------------------------*/
    /// Constructeur avec aucun ratio de spécifié (donc 1 par défaut)
    FaceMeshingProperty(bool isStr)
    : m_isMeshStructured(isStr)
    {}

    /*------------------------------------------------------------------------*/
    /** Création d'un clone, on copie toutes les informations */
    virtual FaceMeshingProperty* clone()
    {
        // on copie simplement la map
        FaceMeshingProperty* emp = new FaceMeshingProperty(m_isMeshStructured);
        for (std::map<CoFace*, uint>::iterator iter = m_ratio_i.begin();
                iter != m_ratio_i.end(); ++iter)
            emp->setRatio((*iter).first, (*iter).second, 0);
        for (std::map<CoFace*, uint>::iterator iter = m_ratio_j.begin();
                iter != m_ratio_j.end(); ++iter)
            emp->setRatio((*iter).first, (*iter).second, 1);
        return emp;
    }

    /*------------------------------------------------------------------------*/
    /// Destructeur
    virtual ~FaceMeshingProperty()
    {}

    /*------------------------------------------------------------------------*/
    /** \brief  Indique si la face est structurée
     */
    bool isStructured() const {return m_isMeshStructured;}

    /// change l'indication sur la structuration
    void setStructured(bool st) {m_isMeshStructured = st;}

    /*------------------------------------------------------------------------*/
    /// Affecte un ratio pour une arête commune
    void setRatio(CoFace* ce, uint ratio, uint dir)
    {
        if (dir == 0){
            m_ratio_i[ce] = ratio;
        }
        else if (dir == 1){
            m_ratio_j[ce] = ratio;
        }
        else
            throw TkUtil::Exception(TkUtil::UTF8String ("FaceMeshingProperty::setRatio n'a que 2 directions possibles", TkUtil::Charset::UTF_8));

    }

    /*------------------------------------------------------------------------*/
    /// Retourne le ratio avec une arête commune, 1 s'il n'est pas défini
    uint getRatio(CoFace* ce, uint dir) const
    {
    	if (dir == 0){
    		std::map<CoFace*, uint>::const_iterator it = m_ratio_i.find(ce);
    		if (it == m_ratio_i.end() || it->second == 0)
    			return 1;
    		else
    			return it->second;
    	}
    	else if (dir == 1){
    		std::map<CoFace*, uint>::const_iterator it = m_ratio_j.find(ce);
    		if (it == m_ratio_j.end() || it->second == 0)
    			return 1;
    		else
    			return it->second;
    	}
        else
            throw TkUtil::Exception(TkUtil::UTF8String ("FaceMeshingProperty::getRatio n'a que 2 directions possibles", TkUtil::Charset::UTF_8));
    }

protected:
    /*------------------------------------------------------------------------*/
    /// Constructeur par copie, il faut faire un clone pour l'utiliser
    FaceMeshingProperty(const FaceMeshingProperty& pm)
    : m_isMeshStructured(false)
    {
        MGX_FORBIDDEN("Constructeur de copie FaceMeshingProperty()");
    }

    /// on protège la copie, il faut faire un clone
    virtual FaceMeshingProperty& operator=(const FaceMeshingProperty&)
    {
        MGX_FORBIDDEN("copie FaceMeshingProperty()");
        return (*this);
    }

protected:
    /// maillage structuré ou non
    bool m_isMeshStructured;

    /// Ratio du nombre de bras pris en comptes dans une CoFace (utile pour la semi-conformité) pour la direction i
    std::map<CoFace*, uint> m_ratio_i;

    /// idem pour la direction j (direction dans la Face)
    std::map<CoFace*, uint> m_ratio_j;

};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* FACEPROPERTYMESHING_H_ */
