#ifndef NAMEMANAGER_H_
#define NAMEMANAGER_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Entity.h"
#include "Internal/EntitiesHelper.h"
#include "Utils/TypeDedicatedNameManager.h"
#include "Utils/MgxException.h"
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

    /// recherche un objet par son nom (Geom, Topo et SysCoord. Mesh et Group gérés autrement)
    template <typename T, typename = std::enable_if<std::is_base_of<InternalEntity, T>::value>>
    T* findByName(const std::string& name, const std::vector<T*>& entities, const bool exceptionIfNotFound) const
    {
        T* entity = 0;

        if (!T::isA(name)){
            if (exceptionIfNotFound){
                TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
                message << name << " n'est pas un nom correspondant à ce type d'entité";
                throw TkUtil::Exception(message);
            }
            else
                return entity;
        }

        std::string new_name;
        if (isShiftingIdActivated()){
            TkUtil::UTF8String tkutil_name(name);
            Utils::Entity::objectType t = EntitiesHelper::getObjectType(tkutil_name);
            new_name = getTypeDedicatedNameManager(t)->renameWithShiftingId(name);
        } else {
            new_name = name;
        }

        for (T* e : entities)
            if (new_name == e->getName())
                entity = e;

        if (entity != 0 && entity->isDestroyed()){
            TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message << new_name << " existe mais elle est détruite";
            throw Utils::IsDestroyedException(message);
        }

        if (exceptionIfNotFound && entity == 0){
            TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
            message << new_name << " n'a pas été trouvée";
            throw TkUtil::Exception(message);
        }

        return entity;
    }

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
