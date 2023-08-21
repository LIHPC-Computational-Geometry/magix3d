/*----------------------------------------------------------------------------*/
/** \file Handle.h
 *
 *  \author Franck Ledoux
 *
 *  \date 19/10/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef UTILS_HANDLE_H_
#define UTILS_HANDLE_H_
/*----------------------------------------------------------------------------*/
#include "TkUtil/Exception.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/
/** \class RefCountObject
 *  \brief Fournit le service de comptage de référence pour les objets des
 *         classes héritant de RefCountObject
 */
/*----------------------------------------------------------------------------*/
class RefCountObject{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     */
    RefCountObject();

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~RefCountObject();


    /*------------------------------------------------------------------------*/
    /** \brief  méthode à implémenter par les classes filles de RefCountObject.
     *          Cette méthode est appelée lors de l'appel au destructeur. En
     *          particulier, quel choix faire si il reste des références sur
     *          this. Par défaut, une exception est levée
     */
    virtual void killBehaviour();

    /*------------------------------------------------------------------------*/
    /** \brief  méthode à implémenter par les classes filles de RefCountObject.
     *          Cette méthode est appelée lors du décrément du compteur de
     *          référence si celui-ci vaut 0. Chaque classe fille doit donc
     *          implémenter son comportement par défaut quand plus aucun objet
     *          de type "Handle" ne le référence.
     */
    virtual void noMoreHandle() =0;

    /*------------------------------------------------------------------------*/
    /** \brief  augmente de 1 le nombre de références sur l'objet
     */
    void increaseNbRefs();

    /*------------------------------------------------------------------------*/
    /** \brief  diminue de 1 le nombre de références sur l'objet
     */
    void decreaseNbRefs();

    /*------------------------------------------------------------------------*/
    /** \brief  Retourne le nombre de références sur l'objet en question.
     */
    int getNbRefs() const;

    /*------------------------------------------------------------------------*/
    /** \brief  surcharge de l'opérateur d'égalité
     */
    const RefCountObject& operator = (const RefCountObject&);


private:

    /** compteur de référence sur l'objet */
    int m_nbRefs;
};
/*----------------------------------------------------------------------------*/
/** \class Handle
 *  \brief Fournit un service pour la gestion de pointeurs où l'objet pointé
 *         héritant de RefCountObject maintient un compteur de référence des
 *         entités ayant une poignée sur lui. L'objet pointé est de type T.
 *
 *  \param T le type de l'objet pointé, qui est forcément une classe et pas un
 *         type de base
 */
/*----------------------------------------------------------------------------*/
template<class T>
class Handle{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur par défaut.
     */
    Handle():m_ref(0){}

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur sur un objet de type T qui hérite forcément de
     *          RefCountObject sinon une erreur de compilation survient.
     */
    Handle(T* ptr):m_ref(ptr){
        if(m_ref)
            m_ref->increaseNbRefs();
    }

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur par copie.
     */
    Handle(const Handle<T>& h):m_ref(h.m_ref){
        if(m_ref)
            m_ref->increaseNbRefs();
    }

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~Handle(){
        if(m_ref)
            m_ref->decreaseNbRefs();
    }

    /*------------------------------------------------------------------------*/
    /** \brief  surcharge de l'opérateur = à partir d'un pointeur T*
     */
    Handle<T>& operator=(T* ptr){
        // on fait attention à ne pas créer une poignet sur le même objet
        if(m_ref==ptr)
            return *this;
        /* sinon on se déréférence de l'objet sur lequel on pointait et on
         * se référence sur ptr
         */
        if(m_ref)
            m_ref->decreaseNbRefs();
        m_ref = ptr;
        if(m_ref)
            m_ref->increaseNbRefs();
        return *this;
    }

    /*------------------------------------------------------------------------*/
    /** \brief  surcharge de l'opérateur = à partir d'une autre poignée
     */
    Handle<T>& operator=(const Handle<T>& h){
        // on fait attention à ne pas créer une poignet sur le même objet
        if(m_ref==h.m_ref)
            return *this;
        /* sinon on se déréférence de l'objet sur lequel on pointait et on
         * se référence sur ptr
         */
        if(m_ref)
            m_ref->decreaseNbRefs();
        m_ref = h.m_ref;
        if(m_ref)
            m_ref->increaseNbRefs();
        return *this;
    }

    /*------------------------------------------------------------------------*/
    /** \brief  Accès au pointeur encapsulé
     */
    T& operator*() const { return *m_ref;}
    /*------------------------------------------------------------------------*/
    /** \brief  Accès au pointeur encapsulé
     */
    T* operator->() const { return m_ref;}

    /*------------------------------------------------------------------------*/
    /** \brief  Accès au pointeur encapsulé
     */
    operator T*() const { return m_ref;}

    /*------------------------------------------------------------------------*/
    /** \brief  Surcharge de l'opérateur < pour les conteneurs de la STL
     */
    bool operator< (const Handle<T>& h) const {
        return m_ref<h.m_ref;
    }

    /*------------------------------------------------------------------------*/
    /** \brief  Surcharge de l'opérateur ==
     */
    bool operator==(const Handle<T>& h) const {
        return m_ref==h.m_ref;
    }
    /*------------------------------------------------------------------------*/
    /** \brief  Surcharge de l'opérateur !=
     */
    bool operator!=(const Handle<T>& h) const {
        return m_ref!=h.m_ref;
    }

private:

    /** l'objet pointé*/
    T* m_ref;
};
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* UTILS_HANDLE_H_ */
/*----------------------------------------------------------------------------*/
