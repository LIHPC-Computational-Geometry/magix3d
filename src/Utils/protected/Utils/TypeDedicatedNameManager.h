/*----------------------------------------------------------------------------*/
/*
 * \file TypeDedicatedNameManager.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 17 janv. 2011
 */
/*----------------------------------------------------------------------------*/


#ifndef TYPEDEDICATEDNAMEMANAGER_H_
#define TYPEDEDICATEDNAMEMANAGER_H_

#include <string>

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/
/** \class TypeDedicatedNameManager
 * \brief Gestionnaire de noms pour un type défini
 */
class TypeDedicatedNameManager{
public:
    /**
     * Constructeur
     * \param la base qui sert à la construction du nom des entités
     */
    TypeDedicatedNameManager(std::string base_name);

    /// retourne un nom pour une nouvelle entité, incrémente la numérotation
    std::string getName();

    /// retourne un id nouveau pour le type d'entité, incrémente la numérotation
    unsigned long getId();

    /// retourne la valeur du compteur d'id (sans incrémenter)
    unsigned long getLastId();

    /// modifie la valeur du compteur d'id
    void setLastId(unsigned long id);

    /// met de côté l'id le plus grand
    void memorizeLastId();

    /// renomme une entité en y ajoutant l'id mémorisé
    std::string renameWithShiftingId(const std::string& name) const;

    /// retourne la base du nom
    std::string getBaseName() const {return m_base_name;}

private:
    /// base qui sert à la construction du nom des entités
    std::string m_base_name;

    /// id par type d'entité
    unsigned long m_id;

    /// id par type d'entité momorisé
    unsigned long m_memorized_id;
};
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* TYPEDEDICATEDNAMEMANAGER_H_ */
