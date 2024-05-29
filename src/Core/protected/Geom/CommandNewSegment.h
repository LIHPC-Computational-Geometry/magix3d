/*----------------------------------------------------------------------------*/
/** \file CommandNewSegment.h
 *
 *  \author Franck Ledoux
 *
 *  \date 14/01/2011
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDNEWSEGMENT_H_
#define COMMANDNEWSEGMENT_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandCreateWithOtherGeomEntities.h"
#include "Geom/Vertex.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
/** \class CommandNewSegment
 *  \brief Commande permettant de créer une segment
 */
/*----------------------------------------------------------------------------*/
class CommandNewSegment: public Geom::CommandCreateWithOtherGeomEntities{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param v1 le premier sommet
     *  \param v2 le second sommet
     *  \param groupName un nom de groupe (qui peut être vide)
     */
    CommandNewSegment(  Internal::Context& c,
            Geom::Vertex* v1,
            Geom::Vertex* v2,
            const std::string& groupName);

    /** \brief  Constructeur
      *
      *  \param c le contexte
      *  \param cmd1 la commande de création du premier sommet
      *  \param cmd2 la commande de création du second sommet
      *  \param groupName un nom de groupe (qui peut être vide)
      */
     CommandNewSegment(  Internal::Context& c,
             Geom::CommandCreateGeom* cmd1,
             Geom::CommandCreateGeom* cmd2,
             const std::string& groupName);
    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandNewSegment();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();


protected:
    /* parametres de l'operation */
    Geom::Vertex* m_v1;
    Geom::Vertex* m_v2;

    /* commandes de création des objets qui servent à l'opération*/
    Geom::CommandCreateGeom* m_cmd_v1;
    Geom::CommandCreateGeom* m_cmd_v2;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWSEGMENT_H_ */
/*----------------------------------------------------------------------------*/

