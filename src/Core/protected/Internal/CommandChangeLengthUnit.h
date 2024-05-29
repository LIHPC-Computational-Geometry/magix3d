/*----------------------------------------------------------------------------*/
/*
 * \file CommandChangeLengthUnit.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9 juil. 2014
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDCHANGELENGTHUNIT_H_
#define COMMANDCHANGELENGTHUNIT_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
/** \class CommandChangeLengthUnit
 *  \brief Commande permettant de changer l'unité de longueur
 */
/*----------------------------------------------------------------------------*/
class CommandChangeLengthUnit: public Internal::CommandInternal{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param context  le contexte
     *  \param lu la nouvelle unité de longueur
     */
    CommandChangeLengthUnit(Internal::Context& context, const Utils::Unit::lengthUnit& lu);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandChangeLengthUnit();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalExecute();

    /** Ce qui est fait de propre à la commande pour annuler une commande */
    virtual void internalUndo();

    /** Ce qui est fait de propre à la commande pour rejouer une commande */
    virtual void internalRedo();


protected:

    /* nouvelle unité */
    Utils::Unit::lengthUnit m_new_length_unit;

    /* ancienne unité */
    Utils::Unit::lengthUnit m_old_length_unit;
};
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDCHANGELENGTHUNIT_H_ */
/*----------------------------------------------------------------------------*/
