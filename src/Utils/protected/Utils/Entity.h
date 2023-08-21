/*----------------------------------------------------------------------------*/
/** \file Entity.h
 *
 *  \author Franck Ledoux, Eric Brière de l'Isle
 *
 *  \date 30/11/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef UTIL_ENTITY_H_
#define UTIL_ENTITY_H_
/*----------------------------------------------------------------------------*/
#include "Utils/DisplayRepresentation.h"
#include "Utils/SerializedRepresentation.h"
#include <vector>
#include <map>
/*----------------------------------------------------------------------------*/
namespace TkUtil {
class Log;
class LogOutputStream;
}
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {

class Property;
class DisplayProperties;

/*----------------------------------------------------------------------------*/
/**
 * \class FilterEntity
 * \brief Permet de mettre un filtre sur les entités
 */
class FilterEntity{
public:
    typedef enum{
		NoneEntity  = 0,
        GeomVertex  = 1,
        GeomCurve   = 1 << 1, // 2
        GeomSurface = 1 << 2, // 4
        GeomVolume  = 1 << 3, // 8
        TopoBlock   = 1 << 4, // 16
        TopoFace    = 1 << 5, // 32
        TopoCoFace  = 1 << 6, // 64
        TopoEdge    = 1 << 7, // 128
        TopoCoEdge  = 1 << 8, // 256
        TopoVertex  = 1 << 9, // 512
        MeshVolume  = 1 << 10, // 1024
        MeshSurface = 1 << 11, // 2048
	MeshLine    = 1 << 12,
        MeshCloud   = 1 << 13,
        MeshSubSurface = 1 << 14,
        MeshSubVolume = 1 << 15,
        Group0D     = 1 << 16,
        Group1D     = 1 << 17,
        Group2D     = 1 << 18,
        Group3D     = 1 << 19,
	SysCoord    = 1 << 20,
	StructuredMesh	= 1 << 21,
		// Penser à actualiser Entity::getFilteredType
		// Penser à actualiser EntitySeizureManager::getFilteredTypes
		// Penser à actualiser NameManager::getTypeDedicatedNameManager
		AllPoints	= GeomVertex | TopoVertex | MeshCloud,
		AllEdges	= GeomCurve | TopoEdge | TopoCoEdge | MeshLine,
		AllSurfaces	= GeomSurface | TopoFace | TopoCoFace | MeshSurface | MeshSubSurface,
		AllVolumes	= GeomVolume | TopoBlock | MeshVolume | MeshSubVolume,
		AllGeom		= GeomVertex | GeomCurve | GeomSurface | GeomVolume,
		AllMesh		= MeshCloud  | MeshLine  | MeshSurface | MeshVolume,
        All         = NoneEntity - 1
    } objectType;

	static std::string filterToObjectTypeName (FilterEntity::objectType ot);
	static std::string filterToFullObjectTypeName (FilterEntity::objectType ot);

};


/*----------------------------------------------------------------------------*/
/** \class Entity
 *  \brief L'interface Entity doit être implémentée par toute entité
 *         métier que l'on souhaiterait afficher graphiquement
 *         Permet de nommer les entités
 */
/*----------------------------------------------------------------------------*/
class Entity{

public:
    /// type de l'objet, à coordonner avec NameManager::NameManager()
	//\see	objectTypeToObjectTypeName
    typedef enum{
	// Penser à adapter objectTypeToObjectTypeName
        GeomVertex = 0,
        GeomCurve,
        GeomSurface,
        GeomVolume,
        TopoVertex,
        TopoCoEdge,
        TopoEdge,
        TopoCoFace,
        TopoFace,
        TopoBlock,
		SysCoord,
        MeshCloud,
		MeshLine,
        MeshSurface,
        MeshSubSurface,
        MeshVolume,
        MeshSubVolume,
        Group0D,
        Group1D,
        Group2D,
        Group3D,
	StructuredMesh,
        undefined // à laisser en dernier
    } objectType;

	static std::string objectTypeToObjectTypeName (objectType t);
	static int objectTypeToDimension (objectType t);

#ifndef SWIG
	typedef unsigned long uniqueID;
#endif

    /*------------------------------------------------------------------------*/
    /**
     *  \return		retourne la dimension de l'entité.
     */
    virtual int getDim() const {return -1;}

    /*------------------------------------------------------------------------*/
    /** \brief  Retourne la boite englobante de l'entité.
	 * \param	En retour, la boite englobante définit comme suit
	 * 			<I>(xmin, xmax, ymin, ymax, zmin, zmax)</I>.
	 * \warning	Par défaut retourne une boite englobante invalide
	 * 			(DBL_MAX, -DBL_MAX, DBL_MAX, -DBL_MAX, DBL_MAX, -DBL_MAX).
	 */
#ifndef SWIG
	virtual void getBounds (double bounds[6]) const;
#endif	// SWIG

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit une représentation de l'entité à afficher. Le type de
     *          représentation fournie dépend de dr. L'instance de
     *          DisplayRepresentation dr fournit à la fois les paramètres de
     *          représentation s'ils existent (par exemple filaire, solide,
     *          discretisation) et stocke la représentation correspondante (
     *          qui sera différente pour les entités géométriques, topologiques
     *          ou de maillage)
     *
     *
     *  \param  dr la représentation que l'on demande à afficher
     *  \param	Lève une exception si checkDestroyed vaut true
     */
#ifndef SWIG
    virtual void getRepresentation(DisplayRepresentation& dr, bool checkDestroyed) const;
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit une représentation textuelle de l'entité.
	 * \param	true si l'entité fourni la totalité de sa description, false si
	 * 			elle ne fournit que les informations non calculées (objectif :
	 * 			optimisation)
	 * \return	Description, à détruire par l'appelant.
     */
#ifndef SWIG
    virtual Mgx3D::Utils::SerializedRepresentation* getDescription (bool alsoComputed) const;
#endif

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit un résumé textuel de l'entité.
     */
#ifndef SWIG
    virtual std::string getSummary ( ) const;
#endif

    /*------------------------------------------------------------------------*/
	/**
	 * \return		Les propriétés d'affichage de l'entité.
	 * \see			setDisplayProperties
	 */
#ifndef SWIG
	virtual DisplayProperties& getDisplayProperties ( ) {return *m_displayProperties;}
	virtual const DisplayProperties& getDisplayProperties ( ) const {return *m_displayProperties;}
#endif

    /*------------------------------------------------------------------------*/
	/**
	 * \param		Les nouvelles propriétés d'affichage de l'entité.
	 * \see			getDisplayProperties
	 * \warning		<B>N'actualise pas les vues courantes.</B>
	 * \return      retourne l'ancienne propriétés d'affichage
	 */
#ifndef SWIG
	virtual  DisplayProperties* setDisplayProperties (DisplayProperties* disp);
#endif

    /*------------------------------------------------------------------------*/
    /**
     * \param       Les nouvelles propriétés de l'entité (nom ...).
     * \return      retourne l'ancienne propriétés
     */
#ifndef SWIG
    virtual  Property* setProperties (Property* prop);
#endif

    /*------------------------------------------------------------------------*/
    /**
     *  \return		Le nom est unique pour un type d'entité donné.
     *  Mais pas unique pour tous type confondus
     *  \see getUniqueName()
     *  \see Property pour le stockage et le changement de nom
     */
    virtual std::string getName() const;

    /*------------------------------------------------------------------------*/
    /** \brief  Donne un nom unique (pour un contexte donné) pour l'entité
     *
     *  Le nom est unique pour tous type d'entités confondus
     *
     */
    virtual std::string getUniqueName() const;

    /*------------------------------------------------------------------------*/
    /** \brief Donne le nom du type d'objet (un nom distinct par type d'objet)
     *
     */
    virtual std::string getTypeName() const {return "TypeUndef";}

    /*------------------------------------------------------------------------*/
    /** \brief Donne le type de l'objet
     *
     */
    virtual objectType getType() const {return undefined;};

    /*------------------------------------------------------------------------*/
    /** \brief Donne le type de l'objet
     *
     */
    virtual FilterEntity::objectType getFilteredType() const;

    /*------------------------------------------------------------------------*/
	/**
	 * \return		<I>true</I> si l'entité représentée est de type géométrique,
	 *				<I>false</I> dans le cas contraire.
	 */
	virtual bool isGeomEntity ( ) const;

    /*------------------------------------------------------------------------*/
	/**
	 * \return		<I>true</I> si l'entité représentée est de type topologique,
	 *				<I>false</I> dans le cas contraire.
	 */
	virtual bool isTopoEntity ( ) const;

    /*------------------------------------------------------------------------*/
	/**
	 * \return		<I>true</I> si l'entité représentée est de type maillage,
	 *				<I>false</I> dans le cas contraire.
	 */
	virtual bool isMeshEntity ( ) const;


    /*------------------------------------------------------------------------*/
    /**
     * \return      <I>true</I> si l'entité représentée est de type groupe,
     *              <I>false</I> dans le cas contraire.
     */
    virtual bool isGroupEntity ( ) const;

    /*------------------------------------------------------------------------*/
    /**
     * \return      <I>true</I> si l'entité représentée est de type sysCoord,
     *              <I>false</I> dans le cas contraire.
     */
    virtual bool isSysCoordEntity ( ) const;

	/*------------------------------------------------------------------------*/
	/** \brief  retourne la liste des noms des éventuelles variables aux
	 *          noeuds.
	 * \see     getCellsValuesNames
	 */
	virtual std::vector<std::string> getNodesValuesNames ( ) const;

	/*------------------------------------------------------------------------*/
	/** \brief  retourne la liste des noms des éventuelles variables aux
	 *          mailles.
	 * \see     getNodesValuesNames
	 */
	virtual std::vector<std::string> getCellsValuesNames ( ) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Récupère une valeur portée par les noeuds.
	 * \see		setNodesValuesNames
	 * \see		getCellsValuesNames
	 *
	 * \param name le nom de la valeur
	 *
	 * \return le tableau contenant cette valeur
     */
    virtual std::map<int,double> getNodesValue (const std::string& name);

    /*------------------------------------------------------------------------*/
    /** \brief   retourne vrai si l'objet est détruit
     */
    virtual bool isDestroyed() const {return m_destroyed;}

    /*------------------------------------------------------------------------*/
    /** \brief   détruit l'objet (mais pas ses dépendances !)
     */
    virtual void setDestroyed(bool b);

    /*------------------------------------------------------------------------*/
    /// Acceseur sur l'état de la visibilité
    virtual bool isVisible() const;

    /*------------------------------------------------------------------------*/
    /** \brief   retourne un identificateur unique quelque soit le type d'entité
     */
    unsigned long getUniqueId() const {return m_unique_id;}


    /*------------------------------------------------------------------------*/
    /**
     * \return      Le gestionnaire de message utilisé pour afficher des
     *              informations sur le déroulement de la commande, ou 0.
     * \see         log
     */
#ifndef SWIG
    inline virtual TkUtil::LogOutputStream* getLogStream ( ) const
    { return m_log_stream; }

    /**
     * Envoit le <I>log</I> transmis en argument dans le flux de messages
     * associé a l'instance.
     * \see     TkUtil::Log
     */
    virtual void log (const TkUtil::Log& log) const;

    /// Fonction de comparaison suivant l'unique id
    static bool compareEntity(Entity* e1, Entity* e2)
    {
        return (e1->getUniqueId()<e2->getUniqueId());
    }

#endif


protected:

    /** Constructeur */
    Entity(unsigned long id, Property* prop, DisplayProperties* disp,
            TkUtil::LogOutputStream* logStream);

    /// Destructeur
    virtual ~Entity();

private :

    /// vrai lorsque l'entité est dite détruite (mais conservée pour un undo éventuel)
    bool                    m_destroyed;

    /** Identificateur unique par entité */
    unsigned long           m_unique_id;

	/** Propriétés (nom ...) */
	Property*               m_prop;

	/** Les propriétés d'affichage. */
	DisplayProperties*		m_displayProperties;

	/** Le gestionnaire de message */
	TkUtil::LogOutputStream* m_log_stream;

};
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* UTIL_ENTITY_H_ */
/*----------------------------------------------------------------------------*/

