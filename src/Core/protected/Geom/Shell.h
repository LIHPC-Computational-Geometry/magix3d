/*----------------------------------------------------------------------------*/
/** \file Shell.h
 *
 *  \author Franck Ledoux
 *
 *  \date 18/10/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef SHELL_H_
#define SHELL_H_
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
#include "Geom/Surface.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/**
 * \class Shell
 * \brief Une entité de type Shell regroupe un ensemble de surfaces formant un
 *        bord d'un volume.
 */
class Shell{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     */
    Shell();

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~Shell();

    /*------------------------------------------------------------------------*/
    /** \brief  Ajoute une surface dans le shell. La méthode teste que surf
     *          n'est pas déjà présente dans le shell
     *
     *  \param surf une nouvelle surface
     */
    void add(Surface* surf);

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit l'accès aux surfaces géométriques composant le shell
     *
     *  \param surfaces les surfaces
     */
    void get(std::vector<Surface*>& surfaces) const;

protected:
    std::vector<Surface*> m_surfaces;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* SHELL_H_ */
/*----------------------------------------------------------------------------*/


