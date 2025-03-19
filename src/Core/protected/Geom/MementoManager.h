#ifndef MEMENTO_MANAGER_H_
#define MEMENTO_MANAGER_H_
/*----------------------------------------------------------------------------*/
#include "Geom/MementoEntity.h"
/*----------------------------------------------------------------------------*/
#include <map>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GeomEntity;
/*----------------------------------------------------------------------------*/
class MementoManager
{
public:
	MementoManager() = default;
    ~MementoManager() = default;

    const MementoEntity createMemento(const GeomEntity* e) const;
    void saveMemento(GeomEntity* e, const MementoEntity& mem);
    void saveMemento(GeomEntity* e) { saveMemento(e, createMemento(e)); }
    void permMementos();

private :
    void setFromMemento(GeomEntity* e, const MementoEntity& mem);

    /** pour les entitiés géométriques modifiées lors de l'opération, on
     * stocke leur "état interne" sous forme de mémento. Ce stockage est de la
     * responsabilité de chaque entité */
    std::map<GeomEntity*, MementoEntity> m_mementos;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MEMENTO_MANAGER_H_ */
/*----------------------------------------------------------------------------*/
