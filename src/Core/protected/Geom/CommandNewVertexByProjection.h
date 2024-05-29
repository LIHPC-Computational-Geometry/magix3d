/*----------------------------------------------------------------------------*/
/*
 * CommandNewVertexByProjection.h
 *
 *  Created on: 10 juil. 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDNEWVERTEXBYPROJECTION_H_
#define COMMANDNEWVERTEXBYPROJECTION_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateGeom.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/** \class CommandNewVertexByProjection
 *  \brief Commande permettant de créer une sommet par projection d'un sommet
 *         sur une courbe ou une surface
 */
/*----------------------------------------------------------------------------*/
class CommandNewVertexByProjection: public Geom::CommandCreateGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param v le point géométrique a projeter
     *  \param curv la courve sur laquelle v est projetée
     *  \param groupName un nom de groupe (qui peut être vide)
     */
    CommandNewVertexByProjection(Internal::Context& c,
                                 const Geom::Vertex* v,
                                 Geom::Curve* curv,
                                 const std::string& groupName);

    /** \brief  Constructeur
      *
      *  \param c le contexte
      *  \param v le point géométrique a projeter
      *  \param surf la surface sur laquelle v est projetée
      *  \param groupName un nom de groupe (qui peut être vide)
      */
     CommandNewVertexByProjection(Internal::Context& c,
                                  const Geom::Vertex* v,
                                  Geom::Surface* surf,
                                  const std::string& groupName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandNewVertexByProjection();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
     *  une représentation des entités modifiées
     *
     *  La commande doit ensuite être détruite
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

protected:
    /* parametres de l'operation */
    const Geom::Vertex* m_v;
    Geom::Curve* m_c;
    Geom::Surface* m_s;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWVERTEXBYPROJECTION_H_ */
/*----------------------------------------------------------------------------*/

