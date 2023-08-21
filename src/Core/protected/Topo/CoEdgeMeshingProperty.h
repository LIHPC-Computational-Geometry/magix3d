/*----------------------------------------------------------------------------*/
/*
 * \file CoEdgeMeshingProperty.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 4 janv. 2011
 */
/*----------------------------------------------------------------------------*/


#ifndef COEDGEPROPERTYMESHING_H_
#define COEDGEPROPERTYMESHING_H_
/*----------------------------------------------------------------------------*/
#include <sys/types.h>
#include <TkUtil/UTF8String.h>
#include "Utils/SerializedRepresentation.h"
#include "Utils/Point.h"
#include "Utils/Time.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
class GeomEntity;
class Curve;
}
/*----------------------------------------------------------------------------*/
namespace Topo {
class CoEdge;

/*----------------------------------------------------------------------------*/
/**
   @brief Propriété de la discrétisation d'une arête commune aux différentes discrétisations
   (uniforme, progression géométrique ...)
 */
class CoEdgeMeshingProperty {
public:

    /** Les méthodes de discrétisation pour une arête */
    typedef enum{
    	/// decoupage d'une arete en N segments
        uniforme,
        /// decoupage d'une arete en N segments selon une loi geometrique
        geometrique,
        /// decoupage d'une arete en N segments selon deux lois geometriques aux extrémités
        bigeometrique,
        /// decoupage d'une arete en N segments selon une loi hyperbolique
        hyperbolique,
        /// decoupage d'une arete en N segment tous d'une taille proche donnée
        specific_size,
        /// decoupage d'une arete en tenant compte d'une autre arête pour placer les points
        interpolate,
        /// decoupage d'une arete en tenant compte de 2 séries d'arêtes pour placer les points
        globalinterpolate,
        /// decoupage d'une arete en tenant d'un vecteur de longueurs de bras
        tabulated,
        /// decoupage d'une arete en N segments selon une loi beta de resserrement
        beta_resserrement,
        /// should be used when endpoints have changed position (for example when smoothing)
        /// valid only for a straight line, not for a curved edge
        uniforme_smoothfix
    } meshLaw;

    /** Côté de départ pour les discrétisations orientés avec orthogonalité
     */
    typedef enum{
    	side_undef,
		side_min,
		side_max
    } meshSideLaw;

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le nombre de bras
    int getNbEdges() const {return m_nb_edges;}

    /*------------------------------------------------------------------------*/
    /// Modificateur sur le nombre de bras
    virtual void setNbEdges(const int nb) {m_nb_edges=nb;}

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le nombre de noeuds
    int getNbNodes() const {return m_nb_edges+1;}

    /*------------------------------------------------------------------------*/
    /// Accesseur sur la méthode de maillage
    meshLaw getMeshLaw() const {return m_mesh_law;}

    /// Accesseur sur le nom de la méthode de maillage
    virtual std::string getMeshLawName() const =0;

    /*------------------------------------------------------------------------*/
    /// Modification du sens
    virtual void setDirect(bool sens) {m_sens = sens;}

    /// Accesseur sur le sens
    virtual bool getDirect() const {return m_sens;}

    /*------------------------------------------------------------------------*/
    /// retourne vrai si la discrétisation doit se faire orthogonalement à une extrémité
    virtual bool isOrthogonal() const {return m_nbLayers != 0;}

    /// demande à avoir une discrétisation orthogonale suivant nb de couches
    virtual void setOrthogonal(int nbLayers, bool sens);

    /// retourne le côté / orthogonalité
    virtual uint getSide() const;

    /// retourne le nombre de couches pour orthogonalité
    virtual int getNbLayers() const {return m_nbLayers;}

    /*------------------------------------------------------------------------*/
    /// retourne vrai si le découpage est polaire (découpage en angle / longueur)
    virtual bool isPolarCut() const {return m_is_polar;}

    /// retourne le point centre du découpage polaire
    virtual Utils::Math::Point getPolarCenter() const {return m_polar_center;}

    /// change le centre pour le découpage est polaire
    virtual void setPolarCenter(Utils::Math::Point pt) {m_polar_center = pt;}

    /// active ou non le découpage en mode polaire
    virtual void setPolarCut(bool polar) {m_is_polar = polar;}

#ifndef SWIG
   /*------------------------------------------------------------------------*/
    /// initialisation avant appel à nextCoeff
    virtual void initCoeff() {m_dernierIndice = 0;}

    /// initialisation avant appel à nextCoeff pour le cas où la longueur est utile
    virtual void initCoeff(double length){ initCoeff(); }

    /// retourne le coefficient suivant pour les noeuds internes (ratio dans ]0, 1[, entre distance au premier point / longueur de l'ensemble)
    virtual double nextCoeff() =0;

    /// retourne vrai pour les méthodes qui nécessitent d'avoir la longueur du contour pour l'initialisation
    virtual bool needLengthToInitialize() {return false;}

    /*------------------------------------------------------------------------*/
    /// Script pour la commande de création Python, en 2 parties
    virtual TkUtil::UTF8String getScriptCommandBegin() const;
    virtual TkUtil::UTF8String getScriptCommandRef() const;

    /*------------------------------------------------------------------------*/
    /** Création d'un clone, on copie toutes les informations */
    virtual CoEdgeMeshingProperty* clone() =0;

    /*------------------------------------------------------------------------*/
    /** Ajoute les informations spécifiques de cette discrétisation à
     *  la représentation textuelle
     */
    virtual void addDescription(Utils::SerializedRepresentation& topoProprietes) const =0;

    /*------------------------------------------------------------------------*/
    /// Retourne vrai s'il est nécessaire de faire une initialisation à l'aide de la géométrie
    virtual bool needGeomUpdate() const {return false;}

    /// Initialisation à l'aide de la géométrie
    virtual void update(Geom::GeomEntity* ge){}

    /*------------------------------------------------------------------------*/
    /// Destructeur
    virtual ~CoEdgeMeshingProperty();

    /*------------------------------------------------------------------------*/
    /// Comparaison de 2 propriétés.
    virtual bool operator == (const CoEdgeMeshingProperty&) const;
    virtual bool operator != (const CoEdgeMeshingProperty&) const;

    /*------------------------------------------------------------------------*/
    /// accesseur sur l'heure de la dernière modification
    const Utils::Time& getModificationTime() const {return m_modification_time;}

    /// met à jour l'heure de modification
    void updateModificationTime() {m_modification_time.update();}

protected:

    /*------------------------------------------------------------------------*/
    /// Script pour la commande de création Python
    virtual TkUtil::UTF8String getScriptCommand() const =0;

    /*------------------------------------------------------------------------*/
    /// Constructeur avec un nombre de bras spécifié
    CoEdgeMeshingProperty(int nb, meshLaw law, bool isDirect);

    /*------------------------------------------------------------------------*/
    /// Constructeur avec un nombre de bras spécifié et un centre pour le cas polaire
    CoEdgeMeshingProperty(int nb, Utils::Math::Point polar_center, meshLaw law, bool isDirect);

    /*------------------------------------------------------------------------*/
    /// Constructeur par copie, il faut faire un clone pour l'utiliser
    CoEdgeMeshingProperty(const CoEdgeMeshingProperty& pm);

    /// on protège la copie, il faut faire un clone
    virtual CoEdgeMeshingProperty& operator=(const CoEdgeMeshingProperty&);

    /*------------------------------------------------------------------------*/
    /// recherche la position des points des arêtes de référence en partant d'une extrémité
    virtual void getPointsRef(std::vector<Utils::Math::Point> &points_ref,
    		std::vector<Topo::CoEdge*>& coedges_ref);

    /*------------------------------------------------------------------------*/
    /// on inverse ou non le vecteur suivant les sens relatifs au départ du vecteur
    virtual void adaptDirection(Utils::Math::Point& pt0, Utils::Math::Point& pt1,
    		std::vector<Utils::Math::Point>& points);

    /*------------------------------------------------------------------------*/



protected:
    /// nombre de bras
    int m_nb_edges;

    /// type de discrétisation
    meshLaw m_mesh_law;

    /// dernier indice utilisé par nextCoeff
    int m_dernierIndice;

    /// vrai si le sens est normal, faux s'il faut parcourir en sens inverse
    bool m_sens;

    /// nombre de couches pour orthogonalité (0 si pas d'orthogonalité)
    int m_nbLayers;

    /// extrémité pour le cas orthogonal
    meshSideLaw m_side;

    /// vrai si le découpage se fait suivant une répartition angulaire (en longueur sinon)
    bool m_is_polar;

    /// centre pour le découpage polaire
    Utils::Math::Point m_polar_center;

    /// heure de la dernière modification pouvant affecter les positions des noeuds à venir (ou des points du pre-maillage)
    mutable Utils::Time m_modification_time;

#endif

};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* COEDGEPROPERTYMESHING_H_ */
