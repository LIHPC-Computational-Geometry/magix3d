/*----------------------------------------------------------------------------*/
/*
 * \file NameManager.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 17 janv. 2011
 */
/*----------------------------------------------------------------------------*/


#ifndef NAMEMANAGER_H_
#define NAMEMANAGER_H_

#include "Utils/Entity.h"

#include <vector>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {

namespace Utils {
class TypeDedicatedNameManager;
}
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
/** \class NameManager
 * \brief Gestionnaire global pour les noms des entités
 */
class NameManager {

public:

    /// Constructeur
    NameManager();

    virtual ~NameManager();

#ifndef SWIG
    /** Réinitialisation     */
    virtual void clear();
#endif
    /// Accesseur au manager de nom suivant un type donné
    Utils::TypeDedicatedNameManager* getTypeDedicatedNameManager(const Utils::Entity::objectType& ot);
    const Utils::TypeDedicatedNameManager* getTypeDedicatedNameManager(const Utils::Entity::objectType& ot) const;

    /// retourne un id unique quelque soit le type d'entités
    unsigned long getUniqueId() {return ++m_global_id;}

    /// retourne un id unique quelque soit le type d'entités
    unsigned long getNextUniqueId() {return m_global_id+1;}

    /// retourne un id pour un explorateur
    unsigned long getExplorerId() {return ++m_explorer_id;}

    /// renseigne un vecteur avec les id max des entités
    void getInternalStats (std::vector<unsigned long> & vid);

    /// affecte les id max des entités
    void setInternalStats (std::vector<unsigned long> & vid);

    /// active le décalage des id pour renommer les entités
    void activateShiftingId();

    /// désactive le décalage des id pour renommer les entités
    void unactivateShiftingId();

    /// retourne vrai si le décalage d'id (renommage) est actif
    bool isShiftingIdActivated() const {return m_is_name_shifting;}

private:
    /// id global pour les entités
    unsigned long m_global_id;

    /// id pour les extractions du type exploration
    unsigned long m_explorer_id;

    std::vector<Utils::TypeDedicatedNameManager*> m_all_type_name_managers;

    /// vrai si l'on doit modifier les noms des entités (cas d'une importation d'un script)
    bool m_is_name_shifting;
};
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* NAMEMANAGER_H_ */
