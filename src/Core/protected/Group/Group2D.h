#ifndef MGX3D_GROUP_GROUP2D_H_
#define MGX3D_GROUP_GROUP2D_H_
/*----------------------------------------------------------------------------*/
#include "Group/GroupEntity.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D::Internal { class Context; }
/*----------------------------------------------------------------------------*/
namespace Mgx3D::Group {
/*----------------------------------------------------------------------------*/
/**
 * \brief Groupe 2D.
 * Permet l'accès aux entités 2D appartenant à un même groupe, ainsi qu'aux entités
 * de niveaux inférieurs
 */
struct Group2D : public GroupEntity
{
    static const char* typeNameGroup2D;
    static const uint DIM = 2;

    void accept(ConstGroupEntityVisitor& v) const override {v.visit(this);}
    void accept(GroupEntityVisitor& v) override {v.visit(this);}
    Group2D(Internal::Context& ctx, const std::string & nom, bool isDefaultGroup, uint level=1);
    virtual ~Group2D() = default;
    int getDim() const override {return DIM;}
    std::string getTypeName() const override {return typeNameGroup2D;}
    Utils::Entity::objectType getType() const override {return Utils::Entity::Group2D;}
};
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D::Group
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GROUP_GROUP2D_H_ */
/*----------------------------------------------------------------------------*/
