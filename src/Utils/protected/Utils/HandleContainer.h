/*----------------------------------------------------------------------------*/
/*
 * \file HandleContainer.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 23 févr. 2012
 */
/*----------------------------------------------------------------------------*/
#ifndef UTILS_HANDLECONTAINER_H_
#define UTILS_HANDLECONTAINER_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Handle.h"
#include <vector>
#include <list>
#include <algorithm>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/** \class HandleContainer
 *  \brief Conteneur (du type vector) sur des Handles
 */
/*----------------------------------------------------------------------------*/
template<class T>
class HandleContainer{

public:
    /// ajoute une entité (un handle sur cette entité)
    void add(T* entity)
    {
        Utils::Handle<T> he(entity);
        m_entities.push_back(he);
    }

    /*----------------------------------------------------------------------------*/
    /// ajoute un vecteur d'entités
    void add(const std::vector<Utils::Handle<T> >& entities)
    {
        m_entities.insert(m_entities.end(), entities.begin(), entities.end());
    }

    /// ajoute une liste d'entités
    void add(const std::list<Utils::Handle<T> >& entities)
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
        else if(exceptionIfNotFound)
        	throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne (pas d'entité), avec HandleContainer::remove", TkUtil::Charset::UTF_8));

        // la même chose mais le compilo n'en veut pas !!!
        //        std::vector<Utils::Handle<T> >::iterator iter;
        //        Utils::Handle<T> he(entity);
        //        iter = find(m_entities.begin(), m_entities.end(), he);
        //        if (iter != m_entities.end())
        //            m_entities.erase(iter);
        //        else if(exceptionIfNotFound)
        //            throw TkUtil::Exception("Erreur interne (pas d'entité), avec HandleContainer::remove");
    }

    /*----------------------------------------------------------------------------*/
    /// copie le contenu du conteneur
    void get(std::vector<Utils::Handle<T> >& entities) const
    {
        entities.clear();
        entities.insert(entities.end(), m_entities.begin(), m_entities.end());
    }

    /*----------------------------------------------------------------------------*/
    /// copie le contenu du conteneur
    void get(std::list<Utils::Handle<T> >& entities) const
    {
        entities.clear();
        entities.insert(entities.end(), m_entities.begin(), m_entities.end());
    }

    /*----------------------------------------------------------------------------*/
    /// accès directe au contenu du conteneur
    const std::vector<Utils::Handle<T> >& get() const
    {
        return m_entities;
    }

    /*----------------------------------------------------------------------------*/
    /// copie le contenu du conteneur
    void clone(HandleContainer<T>& cont)
    {
        m_entities.insert(m_entities.end(), cont.m_entities.begin(), cont.m_entities.end());
    }

    /*----------------------------------------------------------------------------*/
    /// retourne la ième entité
    Utils::Handle<T> get(uint ind) const
            {
#ifdef _DEBUG2
        if (ind>=m_entities.size())
            throw TkUtil::Exception("Erreur interne (indice trop élevé), avec HandleContainer::get");
#endif

        return m_entities[ind];
            }

    /*----------------------------------------------------------------------------*/
    /// modifie la ième entité
    void set(uint ind, T* entity)
    {
#ifdef _DEBUG2
        if (ind>=m_entities.size())
            throw TkUtil::Exception("Erreur interne (indice trop élevé), avec HandleContainer::set");
#endif

        m_entities[ind] = entity;
    }

    /*----------------------------------------------------------------------------*/
    /// \return  Le nombre d'entités
    uint getNb() const
    {
        return m_entities.size();
    }

    /*----------------------------------------------------------------------------*/
    /// vide le conteneur
    void clear()
    {
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
    bool find(T* entity)
    {
        uint i = 0;
        for (; i<m_entities.size() && entity != m_entities[i]; ++i)
            ;

        return (i!=m_entities.size());
    }

    /*----------------------------------------------------------------------------*/
    /// retourne l'indice d'une entité, une exception si l'entité n'est pas trouvée
    uint getIndex(T* entity)
    {
        for (uint i = 0; i<m_entities.size(); ++i)
            if (entity == m_entities[i])
                return i;

    	throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne avec HandleContainer::getIndex, entité non trouvée", TkUtil::Charset::UTF_8));
    }

private:
    std::vector<Utils::Handle<T> > m_entities;
};
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* UTILS_HANDLECONTAINER_H_ */
