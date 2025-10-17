#ifndef GROUPENTITYVISITOR_H_
#define GROUPENTITYVISITOR_H_
/*----------------------------------------------------------------------------*/
namespace Mgx3D::Group 
{
/*----------------------------------------------------------------------------*/
class Group0D;
class Group1D;
class Group2D;
class Group3D;
/*----------------------------------------------------------------------------*/
struct ConstGroupEntityVisitor
{
    virtual void visit(const Group0D*) = 0;
    virtual void visit(const Group1D*) = 0;
    virtual void visit(const Group2D*) = 0;
    virtual void visit(const Group3D*) = 0;
    virtual ~ConstGroupEntityVisitor() = default;
};
/*----------------------------------------------------------------------------*/
struct GroupEntityVisitor
{
    virtual void visit(Group0D*) = 0;
    virtual void visit(Group1D*) = 0;
    virtual void visit(Group2D*) = 0;
    virtual void visit(Group3D*) = 0;
    virtual ~GroupEntityVisitor() = default;
};
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D::Group
/*----------------------------------------------------------------------------*/
#endif /* GROUPENTITYVISITOR_H_ */
/*----------------------------------------------------------------------------*/