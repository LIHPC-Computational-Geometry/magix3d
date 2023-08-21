/*----------------------------------------------------------------------------*/
/*
 * \file SubVolume.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 6/12/12
 */
/*----------------------------------------------------------------------------*/
#ifndef MESH_SUBVOLUME_H_
#define MESH_SUBVOLUME_H_
/*----------------------------------------------------------------------------*/
#include "Mesh/Volume.h"
#include <TkUtil/UTF8String.h>
#include <GMDS/IG/Region.h>
/*----------------------------------------------------------------------------*/
namespace gmds {
class Region;
}
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
/**
   @brief SubVolume (sous groupe de polyèdres) du maillage
 */
class SubVolume : public Volume {
    static const char* typeNameMeshSubVolume;

public:

    /// Constructeur
    SubVolume(Internal::Context& ctx,
            Utils::Property* prop,
            Utils::DisplayProperties* disp,
            uint gmds_id);

    /// Destructeur
    virtual ~SubVolume();

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit une représentation affichable de l'entité en se basant
     *          sur la représentation passée en paramètre
     *
     *  \param  dr la représentation que l'on demande à afficher
     *  \param	Lève une exception si checkDestroyed vaut true
     */
    virtual void getRepresentation(
			Mgx3D::Utils::DisplayRepresentation& dr, bool checkDestroyed) const;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit une représentation textuelle de l'entité.
	 * \param	true si l'entité fourni la totalité de sa description, false si
	 * 			elle ne fournit que les informations non calculées (objectif :
	 * 			optimisation)
     * \return  Description, à détruire par l'appelant.
     */
#ifndef SWIG
    virtual Mgx3D::Utils::SerializedRepresentation* getDescription (
													bool alsoComputed) const;
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Donne le nom du type d'objet (un nom distinct par type d'objet)
     */
    virtual std::string getTypeName() const {return typeNameMeshSubVolume;}

    /*------------------------------------------------------------------------*/
    /** \brief Donne le type de l'objet
     */
    virtual Utils::Entity::objectType getType() const {return Utils::Entity::MeshSubVolume;}

    /*------------------------------------------------------------------------*/
   /** \brief Donne le nom court du type d'objet (pour le nommage des entités)
    */
    static std::string getTinyName() {return "SV";}

    /*------------------------------------------------------------------------*/
    /** \brief Test si l'entité est un Volume suivant son nom
     */
    static bool isA(std::string& name);

    /*------------------------------------------------------------------------*/
    /// ajoute un polyèdre à la liste
    void addRegion(gmds::Region& reg);

    /// ajoute un vecteur de polyèdres à la liste
    void addRegion(std::vector<gmds::Region> reg);

    ///  Fournit l'accès aux regions (polyèdres) GMDS
    virtual void getGMDSRegions(std::vector<gmds::Region>& ARegions) const;

    /// Vide le conteneur de polyèdres
    virtual void clear();
     /*------------------------------------------------------------------------*/
    /// pour l'affichage d'informations
    friend TkUtil::UTF8String & operator << (TkUtil::UTF8String & , const SubVolume &);

private:
    /// Constructeur par copie
    SubVolume(const SubVolume& cl);

    /// Opérateur de copie
    SubVolume & operator = (const SubVolume& cl);

private:
    /// id sur le maillage gmds
    uint m_gmds_id;

    /// Liste des polyêdres (gmds) associés
    std::vector<gmds::Region> m_poly;
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MESH_SUBVOLUME_H_ */
/*----------------------------------------------------------------------------*/
