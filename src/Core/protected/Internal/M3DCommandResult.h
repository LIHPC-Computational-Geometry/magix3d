/*----------------------------------------------------------------------------*/
/** \file		M3DCommandResult.h
 *	\author		Charles
 *	\date		02/04/2013
 */
/*----------------------------------------------------------------------------*/

#ifndef M3D_COMMAND_RESULT_H
#define M3D_COMMAND_RESULT_H

#include "Internal/M3DCommandResultIfc.h"
#include "Internal/CommandInternal.h"
#include "Geom/Volume.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/Vertex.h"
#include "Topo/Block.h"
#include "Topo/CoFace.h"
#include "Topo/CoEdge.h"
#include "Topo/Vertex.h"
#include "Mesh/Volume.h"
#include "Mesh/Surface.h"
#include "Mesh/Line.h"
#include "Mesh/Cloud.h"
#include "Group/Group0D.h"
#include "Group/Group1D.h"
#include "Group/Group2D.h"
#include "Group/Group3D.h"
#include "SysCoord/SysCoord.h"

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/


namespace Geom {
class CommandEditGeom;
}
/*----------------------------------------------------------------------------*/
namespace Internal
{


/** \class M3DCommandResult
 * <P>Fourniseur d'accès aux résultats d'une commande <I>Magix 3D</I>qui a été
 * exécutée.
 * </P>
 */
class M3DCommandResult : public Mgx3D::Internal::M3DCommandResultIfc
{
	public :

	/**
	 * Constructeur.
	 * \param	Commande représentée.
	 */
	M3DCommandResult (Mgx3D::Internal::CommandInternal&);

	/**
	 * Constructeur.
	 * \param	Commande représentée.
	 * \param	Données sur la commande géométrique
	 */
	M3DCommandResult (Mgx3D::Internal::CommandInternal&, Geom::CommandEditGeom* geom_modif);

	/**
	 * Constructeur.
	 * sans commande, donc volontairement en erreur
	 */
	M3DCommandResult ();

	/**
	 * Destructeur.
	 */
	virtual ~M3DCommandResult ( );

#ifndef SWIG

	/**
	 * Les callbacks sur les commandes associées.
	 */
	//@{

	/**
	 * Appelé lorsque la commande est modifiée.
	 */
	virtual void observableModified (
						TkUtil::ReferencedObject* object, unsigned long event);

	/**
	 * Appelé lorsque la commande est détruite. Déréférence cette commande.
	 */
	virtual void observableDeleted (TkUtil::ReferencedObject* object);

	//@}	// Les callbacks sur les commandes associées.

#endif	// SWIG

	/**
	 * Accesseurs aux entités géométriques.
	 */
	//@{

	/**
	 * \return	La liste des volumes créés par la commande.
	 */
	virtual std::vector<Mgx3D::Geom::Volume*> getVolumesObj ( );

	/**
	 * \return	Le i-ème volume créé par la commande.
	 * \except	Lève une exception s'il n'existe pas de tel volume.
	 */
	virtual Mgx3D::Geom::Volume* getVolumeObj (size_t index);

	/**
	 * \return	Le seul volume créé par la commande.
	 * \except	Lève une exception s'il n'existe pas ou s'il y en a plus.
	 */
	virtual Mgx3D::Geom::Volume* getVolumeObj ( );

	/**
	 * \return	La liste des surfaces créées par la commande.
	 */
	virtual std::vector<Mgx3D::Geom::Surface*> getSurfacesObj ( );

	/**
	 * \return	La i-ème surface créée par la commande.
	 * \except	Lève une exception si elle n'existe pas de telle surface.
	 */
	virtual Mgx3D::Geom::Surface* getSurfaceObj (size_t index);

	/**
	 * \return	La seule surface créée par la commande.
	 * \except	Lève une exception si elle n'existe pas ou s'il y en a plus.
	 */
	virtual Mgx3D::Geom::Surface* getSurfaceObj ( );

	/**
	 * \return	La liste des courbe créées par la commande.
	 */
	virtual std::vector<Mgx3D::Geom::Curve*> getCurvesObj ( );

	/**
	 * \return	La i-ème courbe créée par la commande.
	 * \except	Lève une exception si elle n'existe pas de telle courbe.
	 */
	virtual Mgx3D::Geom::Curve* getCurveObj (size_t index);

	/**
	 * \return	La seule courbe créée par la commande.
	 * \except	Lève une exception si elle n'existe pas ou s'il y en a plus.
	 */
	virtual Mgx3D::Geom::Curve* getCurveObj ( );

	/**
	 * \return	La liste des vertex créés par la commande.
	 */
	virtual std::vector<Mgx3D::Geom::Vertex*> getVerticesObj ( );

	/**
	 * \return	Le i-ème vertex créé par la commande.
	 * \except	Lève une exception s'il n'existe pas de tel vertex.
	 */
	virtual Mgx3D::Geom::Vertex* getVertexObj (size_t index);

	/**
	 * \return	Le seul vertex créé par la commande.
	 * \except	Lève une exception s'il n'existe pas ou s'il y en a plus.
	 */
	virtual Mgx3D::Geom::Vertex* getVertexObj ( );


	/**
	 * \return	La liste des volumes créés par la commande.
	 */
	virtual std::vector<std::string> getVolumes ( );

	/**
	 * \return	Le i-ème volume créé par la commande.
	 * \except	Lève une exception s'il n'existe pas de tel volume.
	 */
	virtual std::string getVolume (size_t index);

	/**
	 * \return	Le seul volume créé par la commande.
	 * \except	Lève une exception s'il n'existe pas ou s'il y en a plus.
	 */
	virtual std::string getVolume ( );

	/**
	 * \return	La liste des surfaces créées par la commande.
	 */
	virtual std::vector<std::string> getSurfaces ( );

	/**
	 * \return	La i-ème surface créée par la commande.
	 * \except	Lève une exception si elle n'existe pas de telle surface.
	 */
	virtual std::string getSurface (size_t index);

	/**
	 * \return	La seule surface créée par la commande.
	 * \except	Lève une exception si elle n'existe pas ou s'il y en a plus.
	 */
	virtual std::string getSurface ( );

	/**
	 * \return	La liste des courbe créées par la commande.
	 */
	virtual std::vector<std::string> getCurves ( );

	/**
	 * \return	La i-ème courbe créée par la commande.
	 * \except	Lève une exception si elle n'existe pas de telle courbe.
	 */
	virtual std::string getCurve (size_t index);

	/**
	 * \return	La seule courbe créée par la commande.
	 * \except	Lève une exception si elle n'existe pas ou s'il y en a plus.
	 */
	virtual std::string getCurve ( );

	/**
	 * \return	La liste des point créés par la commande.
	 */
	virtual std::vector<std::string> getVertices ( );

	/**
	 * \return	Le i-ème point créé par la commande.
	 * \except	Lève une exception s'il n'existe pas de tel point.
	 */
	virtual std::string getVertex (size_t index);

	/**
	 * \return	Le seul point créé par la commande.
	 * \except	Lève une exception s'il n'existe pas ou s'il y en a plus.
	 */
	virtual std::string getVertex ( );



	//@}	// Accesseurs aux entités géométriques.

	/**
	 * Accesseurs aux entités topologiques.
	 */
	//@{

	/**
	 * \return	La liste des blocs créés par la commande.
	 */
	virtual std::vector<Mgx3D::Topo::Block*> getBlocksObj ( );

	/**
	 * \return	Le i-ème bloc créé par la commande.
	 * \except	Lève une exception s'il n'existe pas de tel bloc.
	 */
	virtual Mgx3D::Topo::Block* getBlockObj (size_t index);

	/**
	 * \return	Le seul bloc créé par la commande.
	 * \except	Lève une exception s'il n'existe pas ou s'il y en a plus.
	 */
	virtual Mgx3D::Topo::Block* getBlockObj ( );

	/**
	 * \return	La liste des faces créées par la commande.
	 */
	virtual std::vector<Mgx3D::Topo::CoFace*> getFacesObj ( );

	/**
	 * \return	La i-ème face créée par la commande.
	 * \except	Lève une exception si elle n'existe pas de telle face.
	 */
	virtual Mgx3D::Topo::CoFace* getFaceObj (size_t index);

	/**
	 * \return	La seule face créée par la commande.
	 * \except	Lève une exception si elle n'existe pas ou s'il y en a plus.
	 */
	virtual Mgx3D::Topo::CoFace* getFaceObj ( );

	/**
	 * \return	La liste des arête créées par la commande.
	 */
	virtual std::vector<Mgx3D::Topo::CoEdge*> getEdgesObj ( );

	/**
	 * \return	La i-ème arête créée par la commande.
	 * \except	Lève une exception si elle n'existe pas de telle arête.
	 */
	virtual Mgx3D::Topo::CoEdge* getEdgeObj (size_t index);

	/**
	 * \return	La seule arête créée par la commande.
	 * \except	Lève une exception si elle n'existe pas ou s'il y en a plus.
	 */
	virtual Mgx3D::Topo::CoEdge* getEdgeObj ( );

	/**
	 * \return	La liste des sommet créés par la commande.
	 */
	virtual std::vector<Mgx3D::Topo::Vertex*> getTopoVerticesObj ( );

	/**
	 * \return	Le i-ème sommet créé par la commande.
	 * \except	Lève une exception s'il n'existe pas de tel sommet.
	 */
	virtual Mgx3D::Topo::Vertex* getTopoVertexObj (size_t index);

	/**
	 * \return	Le seul sommet créé par la commande.
	 * \except	Lève une exception s'il n'existe pas ou s'il y en a plus.
	 */
	virtual Mgx3D::Topo::Vertex* getTopoVertexObj ( );


	/**
	 * \return	La liste des blocs créés par la commande.
	 */
	virtual std::vector<std::string> getBlocks ( );

	/**
	 * \return	Le i-ème bloc créé par la commande.
	 * \except	Lève une exception s'il n'existe pas de tel bloc.
	 */
	virtual std::string getBlock (size_t index);

	/**
	 * \return	Le seul bloc créé par la commande.
	 * \except	Lève une exception s'il n'existe pas ou s'il y en a plus.
	 */
	virtual std::string getBlock ( );

	/**
	 * \return	La liste des faces créées par la commande.
	 */
	virtual std::vector<std::string> getFaces ( );

	/**
	 * \return	La i-ème face créée par la commande.
	 * \except	Lève une exception si elle n'existe pas de telle face.
	 */
	virtual std::string getFace (size_t index);

	/**
	 * \return	La seule face créée par la commande.
	 * \except	Lève une exception si elle n'existe pas ou s'il y en a plus.
	 */
	virtual std::string getFace ( );

	/**
	 * \return	La liste des arête créées par la commande.
	 */
	virtual std::vector<std::string> getEdges ( );

	/**
	 * \return	La i-ème arête créée par la commande.
	 * \except	Lève une exception si elle n'existe pas de telle arête.
	 */
	virtual std::string getEdge (size_t index);

	/**
	 * \return	La seule arête créée par la commande.
	 * \except	Lève une exception si elle n'existe pas ou s'il y en a plus.
	 */
	virtual std::string getEdge ( );

	/**
	 * \return	La liste des sommet créés par la commande.
	 */
	virtual std::vector<std::string> getTopoVertices ( );

	/**
	 * \return	Le i-ème sommet créé par la commande.
	 * \except	Lève une exception s'il n'existe pas de tel sommet.
	 */
	virtual std::string getTopoVertex (size_t index);

	/**
	 * \return	Le seul sommet créé par la commande.
	 * \except	Lève une exception s'il n'existe pas ou s'il y en a plus.
	 */
	virtual std::string getTopoVertex ( );


	//@}	// Accesseurs aux entités topologiques.

	/**
	 * Accesseurs aux entités de maillage.
	 */
	//@{

	/**
	 * \return	La liste des volumes de mailles créés par la commande.
	 */
	virtual std::vector<Mgx3D::Mesh::Volume*> getMeshVolumesObj ( );

	/**
	 * \return	La liste des surfaces de mailles créées par la commande.
	 */
	virtual std::vector<Mgx3D::Mesh::Surface*> getMeshSurfacesObj ( );

	/**
	 * \return	La liste des lignes de bras créées par la commande.
	 */
	virtual std::vector<Mgx3D::Mesh::Line*> getMeshLinesObj ( );

	/**
	 * \return	La liste des nuages de noeuds créés par la commande.
	 */
	virtual std::vector<Mgx3D::Mesh::Cloud*> getMeshCloudsObj ( );

	//@}	// Accesseurs aux entités de maillage.



	/**
	 * Accesseurs aux entités groupes.
	 */
	//@{

	/**
	 * \return	La liste des groupes 0D créés par la commande.
	 */
	virtual std::vector<Mgx3D::Group::Group0D*> getGroups0DObj ( );
	virtual std::vector<std::string> getGroups0D ( );

	/**
	 * \return	La liste des groupes 1D créés par la commande.
	 */
	virtual std::vector<Mgx3D::Group::Group1D*> getGroups1DObj ( );
	virtual std::vector<std::string> getGroups1D ( );

	/**
	 * \return	La liste des groupes 2D créés par la commande.
	 */
	virtual std::vector<Mgx3D::Group::Group2D*> getGroups2DObj ( );
	virtual std::vector<std::string> getGroups2D ( );

	/**
	 * \return	La liste des groupes 3D créés par la commande.
	 */
	virtual std::vector<Mgx3D::Group::Group3D*> getGroups3DObj ( );
	virtual std::vector<std::string> getGroups3D ( );

	//@}	// Accesseurs aux entités groupes.

	/// accesseur sur un repère local
	virtual std::string getSysCoord ( );
	virtual Mgx3D::CoordinateSystem::SysCoord* getSysCoordObj ( );


	protected :

	/**
	 * \return		La commande associée.
	 * \except		Une exception est levée si il n'y a pas de commande
	 *				associée (par exemple si elle est détruite).
	 */
	virtual Mgx3D::Internal::CommandInternal& getCommandInternal( );

    /**
     * \return      L'InfoCommand associé.
     * \except      Une exception si la commande n'est pas Done.
     */
	virtual Mgx3D::Internal::InfoCommand& getInfoCommand();


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	M3DCommandResult (const M3DCommandResult&);
	M3DCommandResult& operator = (const M3DCommandResult&);

	/** La commande associée. */
	Mgx3D::Internal::CommandInternal*		_commandInternal;

	/** La commande de modification géométrique */
	Geom::CommandEditGeom* m_geom_modif;

};	// class M3DCommandResult


/*----------------------------------------------------------------------------*/
}	// namespace Internal
/*----------------------------------------------------------------------------*/
}	// namespace Mgx3D
/*----------------------------------------------------------------------------*/



#endif	// M3D_COMMAND_RESULT_H
/*----------------------------------------------------------------------------*/

