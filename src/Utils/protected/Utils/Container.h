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
#include <vector>
#include <list>
#include <algorithm>
#include "TkUtil/Exception.h"
#include "TkUtil/UTF8String.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/** \class Container
 *  \brief Conteneur (du type vector) sur des pointeurs
 */
/*----------------------------------------------------------------------------*/
template<class T>
class Container{

public:
    /// ajoute une entité
    void add(T* entity)
    {
        m_entities.push_back(entity);
    }

    /*----------------------------------------------------------------------------*/
    /// ajoute un vecteur d'entités
    void add(const std::vector<T* >& entities)
    {
        m_entities.insert(m_entities.end(), entities.begin(), entities.end());
    }

    /// ajoute une liste d'entités
    void add(const std::list<T* >& entities)
    {
        m_entities.insert(m_entities.end(), entities.begin(), entities.end());
    }

    /*----------------------------------------------------------------------------*/
    /// recherche et enlève l'entité
    void remove(T* entity, const bool exceptionIfNotFound)
    {
        uint i = 0;
        for (; i<m_entities.size() && entity != m_entities[i]; ++i)
            ;

        if (i!=m_entities.size())
            m_entities.erase(m_entities.begin()+i);
        else if(exceptionIfNotFound){
            TkUtil::UTF8String   message;
            message << "Erreur interne (pas d'entité), avec Container::remove";
            //message << ", pour " << entity->getName();
            throw TkUtil::Exception (message);
        }
        // la même chose mais le compilo n'en veut pas !!!
        //        std::vector<T* >::iterator iter;
        //        iter = find(m_entities.begin(), m_entities.end(), entity);
        //        if (iter != m_entities.end())
        //            m_entities.erase(iter);
        //        else if(exceptionIfNotFound)
        //            throw TkUtil::Exception("Erreur interne (pas d'entité), avec Container::remove");
    }

    /*----------------------------------------------------------------------------*/
    /// copie le contenu du conteneur
    void get(std::vector<T* >& entities) const
    {
        entities.clear();
        entities.insert(entities.end(), m_entities.begin(), m_entities.end());
    }

    /*----------------------------------------------------------------------------*/
    /// copie le contenu du conteneur
    void get(std::list<T* >& entities) const
    {
        entities.clear();
        entities.insert(entities.end(), m_entities.begin(), m_entities.end());
    }

    /*----------------------------------------------------------------------------*/
    /// accès directe au contenu du conteneur
    const std::vector<T* >& get() const
    {
        return m_entities;
    }

    /*----------------------------------------------------------------------------*/
    /// copie le contenu du conteneur
    void clone(Container<T>& cont)
    {
        m_entities.insert(m_entities.end(), cont.m_entities.begin(), cont.m_entities.end());
    }

    /*----------------------------------------------------------------------------*/
    /// retourne la ième entité
    T* get(uint ind) const
    {
#ifdef _DEBUG2
        if (ind>=m_entities.size())
            throw TkUtil::Exception(UTF8String ("Erreur interne (indice trop élevé), avec Container::get", TkUtil::Charset::UTF_8));
#endif

        return m_entities[ind];
    }

    /*----------------------------------------------------------------------------*/
    /// modifie la ième entité
    void set(uint ind, T* entity)
    {
#ifdef _DEBUG2
        if (ind>=m_entities.size())
            throw TkUtil::Exception(UTF8String ("Erreur interne (indice trop élevé), avec Container::set", TkUtil::Charset::UTF_8));
#endif

        m_entities[ind] = entity;
    }

    /*----------------------------------------------------------------------------*/
    /// \return  Le nombre d'entités
    uint getNb() const
    {
        return m_entities.size();
    }
    uint size() const
    {
        return m_entities.size();
    }

    /// \return  Le nombre d'entités non détruites
    uint getVisibleNb() const
    {
        uint nb = 0;
        for (uint i = 0; i<m_entities.size(); ++i)
            if (!m_entities[i]->isDestroyed())
                nb++;

        return nb;
    }

    /*----------------------------------------------------------------------------*/
    /// \return  vrai s'il est vide
    bool empty() const
    {
        return m_entities.empty();
    }

    /*----------------------------------------------------------------------------*/
    /// vide le conteneur
    void clear()
    {
        m_entities.clear();
    }

    /*----------------------------------------------------------------------------*/
    /// détruit les entités du conteneur et le vide
    void deleteAndClear()
    {
        for (uint i = 0; i<m_entities.size(); ++i)
            delete m_entities[i];

        m_entities.clear();
    }

    /*----------------------------------------------------------------------------*/
    /// change la taille du conteneur
    void resize(uint nb)
    {
        m_entities.resize(nb);
    }

    /*----------------------------------------------------------------------------*/
    /// recherche si une entité est présente ou non
    bool find(const T* entity)
    {
        uint i = 0;
        for (; i<m_entities.size() && entity != m_entities[i]; ++i)
            ;

        return (i!=m_entities.size());
    }

    /*----------------------------------------------------------------------------*/
    /// retourne l'indice d'une entité, une exception si l'entité n'est pas trouvée
    uint getIndex(const T* entity)
    {
        for (uint i = 0; i<m_entities.size(); ++i)
            if (entity == m_entities[i])
                return i;

        throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne avec Container::getIndex, entité non trouvée", TkUtil::Charset::UTF_8));
    }


    /*----------------------------------------------------------------------------*/
    /// lance une exception si l'une des entités est marquée détruite
    void checkIfDestroyed() const
    {
        for (uint i = 0; i<m_entities.size(); ++i)
            if (m_entities[i]->isDestroyed()){
                TkUtil::UTF8String   message;
                message << "Une des entités est marquée à détruite: "
                        << m_entities[i]->getName();
                throw TkUtil::Exception (message);
            }
    }


private:
    std::vector<T* > m_entities;
};
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* UTILS_CONTAINER_H_ */
