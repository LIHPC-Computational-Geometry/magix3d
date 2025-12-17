#ifndef TOPOENTITYVISITOR_H_
#define TOPOENTITYVISITOR_H_

/*----------------------------------------------------------------------------*/
namespace Mgx3D::Topo
{
/*----------------------------------------------------------------------------*/
class Vertex;
class Edge;
class CoEdge;
class Face;
class CoFace;
class Block;
/*----------------------------------------------------------------------------*/
struct ConstTopoEntityVisitor
{
    virtual void visit(const Vertex*) = 0;
    virtual void visit(const Edge*) = 0;
    virtual void visit(const CoEdge*) = 0;
    virtual void visit(const Face*) = 0;
    virtual void visit(const CoFace*) = 0;
    virtual void visit(const Block*) = 0;
    virtual ~ConstTopoEntityVisitor() = default;
};
/*----------------------------------------------------------------------------*/
struct TopoEntityVisitor
{
    virtual void visit(Vertex*) = 0;
    virtual void visit(Edge*) = 0;
    virtual void visit(CoEdge*) = 0;
    virtual void visit(Face*) = 0;
    virtual void visit(CoFace*) = 0;
    virtual void visit(Block*) = 0;
    virtual ~TopoEntityVisitor() = default;
};
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D::Topo
/*----------------------------------------------------------------------------*/
#endif /* TOPOENTITYVISITOR_H_ */
/*----------------------------------------------------------------------------*/