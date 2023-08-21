/*----------------------------------------------------------------------------*/
/*
 * \file SubSurface.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 3/12/13
 */
/*----------------------------------------------------------------------------*/
#ifndef MESH_SUBSURFACE_H_
#define MESH_SUBSURFACE_H_
/*----------------------------------------------------------------------------*/
#include "Mesh/Surface.h"
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
#include <GMDS/IG/Face.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
/**
   @brief SubSurface (sous groupe de polygones) du maillage
 */
class SubSurface : public Surface {
    static const char* typeNameMeshSubSurface;

public:

    /// Constructeur
    SubSurface(Internal::Context& ctx,
            Utils::Property* prop,
            Utils::DisplayProperties* disp,
            uint gmds_id);

    /// Destructeur
    virtual ~SubSurface();

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
    virtual std::string getTypeName() const {return typeNameMeshSubSurface;}

    /*------------------------------------------------------------------------*/
    /** \brief Donne le type de l'objet
     */
    virtual Utils::Entity::objectType getType() const {return Utils::Entity::MeshSubSurface;}

    /*------------------------------------------------------------------------*/
   /** \brief Donne le nom court du type d'objet (pour le nommage des entités)
    */
    static std::string getTinyName() {return "SS";}

    /*------------------------------------------------------------------------*/
    /** \brief Test si l'entité est un Surface suivant son nom
     */
    static bool isA(std::string& name);

    /*------------------------------------------------------------------------*/
    /// ajoute un polygone à la liste
    void addFace(gmds::Face face);

    /// ajoute une liste de polygones à la liste
    void addFace(std::vector<gmds::Face> face);

    ///  Fournit l'accès aux faces GMDS
    virtual void getGMDSFaces(std::vector<gmds::Face >& faces) const;

    ///  Fournit l'accès aux noeuds GMDS
    virtual void getGMDSNodes(std::vector<gmds::Node >& nodes) const;

    /// Vide le conteneur de polygones
    virtual void clear();

    /*------------------------------------------------------------------------*/
    /// pour l'affichage d'informations
    friend TkUtil::UTF8String & operator << (TkUtil::UTF8String & , const SubSurface &);

private:
    /// Constructeur par copie
    SubSurface(const SubSurface& cl);

    /// Opérateur de copie
    SubSurface & operator = (const SubSurface& cl);

private:
    /// id sur le maillage gmds
    uint m_gmds_id;

    /// Liste des polygones (gmds) associés
    std::vector<gmds::Face> m_poly;
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MESH_SUBSURFACE_H_ */
/*----------------------------------------------------------------------------*/
