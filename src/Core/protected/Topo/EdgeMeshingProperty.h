/*----------------------------------------------------------------------------*/
/*
 * \file EdgeMeshingProperty.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 25/4/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef EDGEPROPERTYMESHING_H_
#define EDGEPROPERTYMESHING_H_
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
   @brief Propriété (ratio) de la discrétisation d'une arête par rapport aux discrétisations
   des arêtes communes
 */
class EdgeMeshingProperty {
public:

    /*------------------------------------------------------------------------*/
    /// Constructeur avec aucun ratio de spécifié (donc 1 par défaut)
    EdgeMeshingProperty()
    {}

    /*------------------------------------------------------------------------*/
    /** Création d'un clone, on copie toutes les informations */
    virtual EdgeMeshingProperty* clone()
    {
        // on copie simplement la map
        EdgeMeshingProperty* emp = new EdgeMeshingProperty();
        for (std::map<CoEdge*, uint>::iterator iter = m_ratio.begin();
                iter != m_ratio.end(); ++iter)
            emp->setRatio((*iter).first, (*iter).second);
        return emp;
    }

    /*------------------------------------------------------------------------*/
    /// Destructeur
    virtual ~EdgeMeshingProperty()
    {}

    /*------------------------------------------------------------------------*/
    /// Affecte un ratio pour une arête commune
    void setRatio(CoEdge* ce, uint ratio)
    {
        m_ratio[ce] = ratio;
    }

    /*------------------------------------------------------------------------*/
    /// Retourne le ratio avec une arête commune, 1 s'il n'est pas défini
    uint getRatio(CoEdge* ce) const
    {
    	std::map<CoEdge*, uint>::const_iterator it = m_ratio.find(ce);
    	if (it == m_ratio.end() || it->second==0)
    		return 1;
    	else
    		return it->second;
    }

protected:
    /*------------------------------------------------------------------------*/
    /// Constructeur par copie, il faut faire un clone pour l'utiliser
    EdgeMeshingProperty(const EdgeMeshingProperty& pm)
    {
        MGX_FORBIDDEN("Constructeur de copie EdgeMeshingProperty()");
    }

    /// on protège la copie, il faut faire un clone
    virtual EdgeMeshingProperty& operator=(const EdgeMeshingProperty&)
    {
        MGX_FORBIDDEN("copie EdgeMeshingProperty()");
        return (*this);
    }

protected:
    /// Ratio du nombre de bras pris en comptes dans une CoEdge (utile pour la semi-conformité)
    std::map<CoEdge*, uint> m_ratio;

};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* EDGEPROPERTYMESHING_H_ */
