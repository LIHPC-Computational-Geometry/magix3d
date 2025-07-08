/*----------------------------------------------------------------------------*/
/*
 * \file Container.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date  Mars 2013
 *
 *  Comme pour Container, mais sans les Handles
 *  */
/*----------------------------------------------------------------------------*/
#ifndef UTILS_CONTAINER_H_
#define UTILS_CONTAINER_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Entity.h"
/*----------------------------------------------------------------------------*/
#include <vector>
#include <list>
#include <algorithm>
#include <TkUtil/Exception.h>
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/
/// retourne l'indice d'une entité, une exception si l'entité n'est pas trouvée
template <typename T> static uint
getIndexOf(const std::vector<T*>& container, const T* entity)
{
    for (uint i = 0; i<container.size(); ++i)
        if (entity == container[i])
            return i;

    throw TkUtil::Exception("Erreur interne avec Utils::getIndexOf, entité non trouvée");
}
/*----------------------------------------------------------------------------*/
/// recherche et enlève l'entité
template <typename T> static void
remove(std::vector<T*>& container, const T* entity, const bool exceptionIfNotFound)
{
    auto iter = find(container.begin(), container.end(), entity);
    if (iter != container.end())
        container.erase(iter);
    else if (exceptionIfNotFound)
        throw TkUtil::Exception("Erreur interne (pas d'entité), avec Utils::remove");
}
/*----------------------------------------------------------------------------*/
/// recherche si une entité est présente ou non
template <typename T> static bool
find(const std::vector<T*>& container, const T* entity)
{
    return (std::find(container.begin(), container.end(), entity) != container.end());
}
/*----------------------------------------------------------------------------*/
/// lance une exception si l'une des entités est marquée détruite
template <typename T> static void
checkIfDestroyed(const std::vector<T*>& container)
{
    for (T* entity : container) {
        if (entity->isDestroyed()){
            TkUtil::UTF8String   message;
            message << entity->getName() << " marquée à détruire." ;
            throw TkUtil::Exception (message);
        }
    }
}
/*----------------------------------------------------------------------------*/
/// détruit les entités du conteneur et le vide
template <typename T> static void
deleteAndClear(std::vector<T*>& container)
{
    for (uint i = 0; i<container.size(); ++i)
        delete container[i];

    container.clear();
}
/*----------------------------------------------------------------------------*/
/// \return  Le nombre d'entités non détruites
template <typename T> static uint
getVisibleNb(const std::vector<T*>& container)
{
    uint nb = 0;
    for (T* entity : container)
        if (!entity->isDestroyed())
            nb++;
    return nb;
}
/*----------------------------------------------------------------------------*/
template <typename T> static std::vector<T*>
getEntities(const std::vector<T*>& container)
{
    std::vector<T*> ret;
    for (T* entity : container)
        if (!entity->isDestroyed())
            ret.push_back(entity);

    std::sort(ret.begin(), ret.end(), Utils::Entity::compareEntity);
    return ret;
}
/*----------------------------------------------------------------------------*/
template <typename T> static std::vector<std::string>
getEntityNames(const std::vector<T*>& container)
{
    std::vector<std::string> ret;
    for (T* entity : container)
        if (!entity->isDestroyed())
            ret.push_back(entity->getName());

    std::sort(ret.begin(), ret.end());
    return ret;
}
/*----------------------------------------------------------------------------*/
template <typename T> static std::vector<T*>
toVector(const Utils::EntitySet<T*>& container)
{
    std::vector<T*> vect(container.begin(), container.end());
    return vect;
}
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* UTILS_CONTAINER_H_ */
