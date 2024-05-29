/*----------------------------------------------------------------------------*/
/*
 * CommandNewBSpline.h
 *
 *  Created on: 2 avr. 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDNEWBSPLINE_H_
#define COMMANDNEWBSPLINE_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateGeom.h"
#include "Geom/Vertex.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/** \class CommandNewBSpline
 *  \brief Commande permettant de créer une courbe bspline à partir d'un
           ensemble ordonnées de sommets
 */
/*----------------------------------------------------------------------------*/
class CommandNewBSpline: public Geom::CommandCreateGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param points un vecteur de sommets définissant la B-Spline
     *  \param deg_min degré minimum pour le polynome de la B-Spline
     *  \param deg_max degré maximum pour le polynome de la B-Spline
     *  \param groupName un nom de groupe (qui peut être vide)
     */
    CommandNewBSpline(Internal::Context& c,std::vector<Geom::Vertex*>& points,
    		int deg_min,
    		int deg_max,
            const std::string& groupName);

    /** \brief  Constructeur
     *
     *  \param cmds les commandes de création des points définissant la B-Spline
     *  \param deg_min degré minimum pour le polynome de la B-Spline
     *  \param deg_max degré maximum pour le polynome de la B-Spline
     *  \param groupName un nom de groupe (qui peut être vide)
     */
    CommandNewBSpline(Internal::Context& c,
    		const std::vector<Geom::CommandCreateGeom*>& cmds,
    		int deg_min,
    		int deg_max,
            const std::string& groupName);

    /** \brief  Constructeur
     *
     *  \param vtx1 le sommet de départ de la courbe
     *  \param vp un vecteur de points définissant la B-Spline
     *  \param vtx2 le sommet d'arrivée de la courbe
     *  \param deg_min degré minimum pour le polynome de la B-Spline
     *  \param deg_max degré maximum pour le polynome de la B-Spline
     *  \param groupName un nom de groupe (qui peut être vide)
     */
    CommandNewBSpline(Internal::Context& c,
    		Vertex* vtx1,
			std::vector<Point>& vp,
			Vertex* vtx2,
    		int deg_min,
    		int deg_max,
			const std::string& groupName);
    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandNewBSpline();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();


protected:
    /** parametres de l'operation pour le cas avec des sommets tout le long */
    std::vector<Geom::Vertex*> m_control_points;

    /** parametres de l'operation pour le cas avec des sommets aux extrémités et des points entre */
    Vertex* m_vtx1;
    std::vector<Point> m_vp;
    Vertex* m_vtx2;

    /// degré min pour la b-spline
    int m_deg_min;
    /// degré max pour la b-spline
    int m_deg_max;

    /* commandes de création des objets qui servent à l'opération*/
    std::vector<Geom::CommandCreateGeom*> m_cmds_vertices;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWBSPLINE_H_ */
/*----------------------------------------------------------------------------*/
