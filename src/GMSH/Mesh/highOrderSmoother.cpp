// Gmsh - Copyright (C) 1997-2013 C. Geuzaine, J.-F. Remacle
//
// See the LICENSE.txt file for license information. Please report all
// bugs and problems to the public mailing list <gmsh@geuz.org>.
//
// Contributor(s):
//   Koen Hillewaert
//

#include "GmshConfig.h"

#if defined(HAVE_SOLVER)

#include "MLine.h"
#include "MTriangle.h"
#include "MQuadrangle.h"
#include "MTetrahedron.h"
#include "MHexahedron.h"
#include "MPrism.h"
#include "MPyramid.h"
#include "HighOrder.h"
#include "meshGFaceOptimize.h"
#include "highOrderSmoother.h"
#include "GFace.h"
#include "GRegion.h"
#include "Context.h"
#include "Numeric.h"
#include "dofManager.h"
#include "elasticityTerm.h"
#include "linearSystemCSR.h"
#include "linearSystemFull.h"
#include "linearSystemPETSc.h"

#define SQU(a)      ((a)*(a))

int optimalLocationPN_(GFace *gf, const MEdge &me, MTriangle *t1, MTriangle *t2,
                       highOrderSmoother *s);
static int swapHighOrderTriangles(GFace *gf);
static int swapHighOrderTriangles(GFace *gf, edgeContainer&, faceContainer&,
                                       highOrderSmoother *s);
static int findOptimalLocationsP2(GFace *gf, highOrderSmoother *s);
static int findOptimalLocationsPN(GFace *gf, highOrderSmoother *s);

static double shapeMeasure(MElement *e)
{
  //const double d1 = e->distoShapeMeasure();
  const double d1 = e->angleShapeMeasure();
  //const double d2 = e->gammaShapeMeasure();
  return d1;
}

void highOrderSmoother::moveTo(MVertex *v,  
                                   const std::map<MVertex*, SVector3> &m) const
{
  std::map<MVertex*,SVector3>::const_iterator it = m.find(v);
  if (it != m.end()){
    v->x() = it->second.x();
    v->y() = it->second.y();
    v->z() = it->second.z();
  }
} 

void highOrderSmoother::moveToStraightSidedLocation(MVertex *v) const
{
  moveTo( v, _straightSidedLocation);
}

void highOrderSmoother::moveToTargetLocation(MVertex *v) const
{
  moveTo( v, _targetLocation);
}

void highOrderSmoother::moveToStraightSidedLocation(MElement *e) const
{
  for(int i = 0; i < e->getNumVertices(); i++)
    moveToStraightSidedLocation(e->getVertex(i));
}

void highOrderSmoother::moveToTargetLocation(MElement *e) const
{
  for(int i = 0; i < e->getNumVertices(); i++)
    moveToTargetLocation(e->getVertex(i));
}

void highOrderSmoother::updateTargetLocation(MVertex*v, const SPoint3 &p3, 
                                                 const SPoint2 &p2)
{
  v->x() = p3.x();
  v->y() = p3.y();
  v->z() = p3.z();
  v->setParameter(0,p2.x());
  v->setParameter(1,p2.y());
  _targetLocation[v] = SVector3(p3.x(), p3.y(), p3.z());
}

struct p2data{
  GFace *gf;
  MTriangle *t1, *t2;
  MVertex *n12;
  fullMatrix<double> *m1, *m2;
  highOrderSmoother *s;
  p2data(GFace *_gf, MTriangle *_t1, MTriangle *_t2, MVertex *_n12, 
         highOrderSmoother *_s)
    : gf(_gf), t1(_t1), t2(_t2), n12(_n12), s(_s)
  {
    elasticityTerm el(0, 1.e3, .48,1);
    s->moveToStraightSidedLocation(t1);
    s->moveToStraightSidedLocation(t2);
    m1 = new fullMatrix<double>(3 * t1->getNumVertices(),
                                3 * t1->getNumVertices());
    m2 = new fullMatrix<double>(3 * t2->getNumVertices(),
                                3 * t2->getNumVertices()); 
    SElement se1(t1), se2(t2);
    el.elementMatrix(&se1, *m1);
    el.elementMatrix(&se2, *m2);
    s->moveToTargetLocation(t1);
    s->moveToTargetLocation(t2);
  }
  ~p2data()
  {
    delete m1;
    delete m2;
  }
};

struct pNdata{
  GFace *gf;
  MTriangle *t1,*t2;
  const std::vector<MVertex*> &n;
  fullMatrix<double> *m1,*m2;
  highOrderSmoother *s;
  pNdata(GFace *_gf,MTriangle *_t1,MTriangle *_t2, const std::vector<MVertex*> &_n,
         highOrderSmoother *_s)
    : gf(_gf), t1(_t1), t2(_t2), n(_n), s(_s)
  {
    elasticityTerm el(0, 1.e3, .3333,1);
    s->moveToStraightSidedLocation(t1);
    s->moveToStraightSidedLocation(t2);
    m1 = new  fullMatrix<double>(3 * t1->getNumVertices(),
                                 3 * t1->getNumVertices());
    m2 = new  fullMatrix<double>(3 * t2->getNumVertices(),
                                 3 * t2->getNumVertices()); 
    SElement se1(t1), se2(t2);
    el.elementMatrix(&se1, *m1);
    el.elementMatrix(&se2, *m2);
    s->moveToTargetLocation(t1);
    s->moveToTargetLocation(t2);
  }
  ~pNdata()
  {
    delete m1;
    delete m2;
  }
};

static double _DeformationEnergy(MElement *e, 
                                 fullMatrix<double> *K,
                                 highOrderSmoother *s)
{
  int N = e->getNumVertices();
  fullVector<double> Kdx(N*3),dx(N*3);

  dx.scale(0.0);
  Kdx.scale(0.0);
  for (int i=0;i<N;i++){
    SVector3 disp = s->getDisplacement(e->getVertex(i));
    SVector3 str  = s->getSSL(e->getVertex(i));
    dx(i)      = disp.x();
    dx(i+N)    = disp.y();
    dx(i+2*N)  = disp.z();
    if (0 && (fabs(disp.x())>1.e-12 ||fabs(disp.y())>1.e-12))
      printf("%6d (%12.5E %12.5E %12.5E -- %12.5E %12.5E %12.5E -- %12.5E %12.5E %12.5E)\n",
             e->getVertex(i)->getNum(),
             disp.x(),disp.y(),disp.z(),
             str.x(),str.y(),str.z(),e->getVertex(i)->x(),e->getVertex(i)->y(),
             e->getVertex(i)->z());
  }

  K->mult(dx, Kdx);
  const double E = Kdx * dx;
  return E;
}

static double _DeformationEnergy(p2data *p)
{
  return _DeformationEnergy(p->t1, p->m1, p->s) + 
    _DeformationEnergy(p->t2, p->m2, p->s);
}

static double _DeformationEnergy(pNdata *p)
{
  return _DeformationEnergy(p->t1, p->m1, p->s) + 
    _DeformationEnergy(p->t2, p->m2, p->s);
}

static double _function_p2tB(fullVector<double> &x, void *data)
{
  p2data *p = (p2data*)data;
  GPoint gp12 = p->gf->point(SPoint2(x(0), x(1)));
  double xx = p->n12->x();
  double yy = p->n12->y();
  double zz = p->n12->z();
  p->n12->x() = gp12.x();
  p->n12->y() = gp12.y();
  p->n12->z() = gp12.z();

  double E = _DeformationEnergy(p);
  p->n12->x() = xx;
  p->n12->y() = yy;
  p->n12->z() = zz;  
  return E;
}

static double _function_p2t(fullVector<double> &x, void *data)
{
  p2data *p = (p2data*)data;
  GPoint gp12 = p->gf->point(SPoint2(x(0), x(1)));
  double xx = p->n12->x();
  double yy = p->n12->y();
  double zz = p->n12->z();
  p->n12->x() = gp12.x();
  p->n12->y() = gp12.y();
  p->n12->z() = gp12.z();
  double q = std::min(shapeMeasure(p->t1), shapeMeasure(p->t2));      
  p->n12->x() = xx;
  p->n12->y() = yy;
  p->n12->z() = zz;  
  return -q;
}

static double _function_pNt(fullVector<double> &x, void *data)
{
  pNdata *p = (pNdata*)data;
  static double xx[256];
  static double yy[256];
  static double zz[256];
  for (unsigned int i = 0; i < p->n.size(); i++){
    GPoint gp12 = p->gf->point(SPoint2(x(2 * i), x(2 * i + 1)));
    xx[i] = p->n[i]->x();
    yy[i] = p->n[i]->y();
    zz[i] = p->n[i]->z();
    p->n[i]->x() = gp12.x();
    p->n[i]->y() = gp12.y();
    p->n[i]->z() = gp12.z();
  }
  double q = std::min(shapeMeasure(p->t1), shapeMeasure(p->t2));      
  for (unsigned int i = 0; i < p->n.size(); i++){
    p->n[i]->x() = xx[i];
    p->n[i]->y() = yy[i];
    p->n[i]->z() = zz[i];
  }  
  return -q;
}

static double _function_pNtB(fullVector<double> &x, void *data)
{
  pNdata *p = (pNdata*)data;
  static double xx[256];
  static double yy[256];
  static double zz[256];
  for (unsigned int i = 0; i < p->n.size(); i++){
    GPoint gp12 = p->gf->point(SPoint2(x(2 * i), x(2 * i + 1)));
    xx[i] = p->n[i]->x();
    yy[i] = p->n[i]->y();
    zz[i] = p->n[i]->z();
    p->n[i]->x() = gp12.x();
    p->n[i]->y() = gp12.y();
    p->n[i]->z() = gp12.z();
  }
  double E = _DeformationEnergy(p);
  for (unsigned int i = 0; i < p->n.size(); i++){
    p->n[i]->x() = xx[i];
    p->n[i]->y() = yy[i];
    p->n[i]->z() = zz[i];
  }  
  return E;
}

void getDistordedElements(const std::vector<MElement*> &v,
                          const double & threshold,
                          std::vector<MElement*> &d,
                          double &minD)
{
  d.clear();
  minD = 1;
  for (unsigned int i = 0; i < v.size(); i++){
    const double disto = v[i]->distoShapeMeasure();
    if (disto < threshold)
      d.push_back(v[i]);
    minD = std::min(minD, disto);
  }
}

void addOneLayer(const std::vector<MElement*> &v, 
                 std::vector<MElement*> &d,
                 std::vector<MElement*> &layer)
{
  std::set<MVertex*> all;
  for (unsigned int i = 0; i < d.size(); i++){
    MElement *e = d[i];
    int n = e->getNumPrimaryVertices();
    for (int j = 0; j < n; j++){
      all.insert(e->getVertex(j));
    }
  }
  layer.clear();

  std::sort(d.begin(), d.end());

  for (unsigned int i = 0; i < v.size(); i++){
    MElement *e = v[i];
    bool found = std::binary_search(d.begin(), d.end(), e);
    // element is not yet there
    if (!found){
      int n = e->getNumPrimaryVertices();
      for (int j = 0; j < n; j++){
        MVertex *vert = e->getVertex(j);
        if (all.find(vert) != all.end()){
          layer.push_back(e);
          j = n;
        }
      }
    }
  }
}


void _printJacobiansAtNodes (const char * name, std::vector<MElement*> &v)
{
  FILE *fd = fopen (name,"w");
  fprintf(fd,"$MeshFormat\n2 0 8\n$EndMeshFormat\n$ElementNodeData\n1\n\"det J\"\n1\n0.0\n3\n1\n1\n%d\n", (int) v.size());
  for (int i=0; i<v.size(); i++){
    const polynomialBasis* pb = v[i]->getFunctionSpace();
    fprintf(fd,"%d %d",v[i]->getNum(),v[i]->getNumVertices());
    for (int j=0; j < v[i]->getNumVertices(); j++){
      const double _u = pb->points(j,0);
      const double _v = pb->points(j,1);
      const double _w = pb->points(j,2);
      double jac[3][3];
      double J = v[i]->getJacobian (_u,_v,_w, jac);
      fprintf(fd," %g",J);    
    }
    fprintf(fd,"\n");    
  }
  fprintf(fd,"$EndElementNodeData\n");
  fclose(fd);
}

void highOrderSmoother::smooth(GFace *gf, bool metric)
{
  std::vector<MElement*> v;

  v.insert(v.begin(), gf->triangles.begin(),gf->triangles.end());
  v.insert(v.begin(), gf->quadrangles.begin(),gf->quadrangles.end());
  Msg::Info("Smoothing high order mesh : model face %d (%d elements)", gf->tag(),
            v.size());

  //  gf->model()->writeMSH("before_smoothing.msh");
  //  _printJacobiansAtNodes ("before_smoothing_jac.msh", v);
  //  smooth_with_mixed_formulation(v,0.2);
  //  gf->model()->writeMSH("after_smoothing.msh");
  //  _printJacobiansAtNodes ("after_smoothing_jac.msh", v);
  //  return;

  if (metric)smooth_metric(v,gf);
  else smooth(v);
}

void highOrderSmoother::smooth(GRegion *gr)
{
  std::vector<MElement*> v;
  v.insert(v.begin(), gr->tetrahedra.begin(),gr->tetrahedra.end());
  v.insert(v.begin(), gr->hexahedra.begin(),gr->hexahedra.end());
  v.insert(v.begin(), gr->prisms.begin(),gr->prisms.end());
  Msg::Info("Smoothing high order mesh : model region %d (%d elements)", gr->tag(),
            v.size());
  smooth(v);
}

// Use elastic solver to move the nodes: compute the stiffness matrix
// of an element that correspond to the deformation of a straight
// sided element to a curvilinear one




void highOrderSmoother::optimize(GFace * gf, 
				 edgeContainer &edgeVertices,
				 faceContainer &faceVertices)
{
  //  if (gf->geomType() != GEntity::Plane) return;

    std::vector<MElement*> bad;
    

  while (1) {
    // relocate the vertices using elliptic smoother
    //    smooth(gf);
    //    for (int i = 0; i < 20; i++){
    //      findOptimalLocationsPN(gf,this);
    //      findOptimalLocationsPN(gf,this);
    //    }
    // then try to swap for better configurations  

    //smooth(gf, !CTX::instance()->mesh.highOrderNoMetric);
    
    
    //    for (int i=0;i<100;i++){
      //      int nbSwap = 
	//	swapHighOrderTriangles(gf,edgeVertices,faceVertices,this);
	//      printf("%d swaps\n",nbSwap);
    //    }
    
     smooth(gf,true);
    // smooth(gf,true);
    // smooth(gf,true);
    // smooth(gf,true);
    break;

    // if the smoothing procedure has been able to
    // move the nodes to their right location, break !
    //    break;
    if (_MIDDLE == 1.0) break;
    // if (!nbSwap){
    //   printf("Cannot move thet nodes to their optimal location, "
    //          "splits have to be added\n");
    //   break;
    // }
  }
}

void highOrderSmoother::computeMetricVector(GFace *gf, 
                                                MElement *e, 
                                                fullMatrix<double> &J,
                                                fullMatrix<double> &JT,
                                                fullVector<double> &D)
{
  int nbNodes = e->getNumVertices();
  for (int j = 0; j < nbNodes; j++){
    SPoint2 param;
    reparamMeshVertexOnFace(e->getVertex(j), gf, param);  
    Pair<SVector3,SVector3> der = gf->firstDer(param);    
    int XJ = j;
    int YJ = j + nbNodes;
    int ZJ = j + 2 * nbNodes;
    int UJ = j;
    int VJ = j + nbNodes;
    J(XJ,UJ) = der.first().x();
    J(YJ,UJ) = der.first().y();
    J(ZJ,UJ) = der.first().z();
    J(XJ,VJ) = der.second().x();
    J(YJ,VJ) = der.second().y();
    J(ZJ,VJ) = der.second().z();
    
    JT(UJ,XJ) = der.first().x();
    JT(UJ,YJ) = der.first().y();
    JT(UJ,ZJ) = der.first().z();
    JT(VJ,XJ) = der.second().x();
    JT(VJ,YJ) = der.second().y();
    JT(VJ,ZJ) = der.second().z();

    GPoint gp = gf->point(param);    
    SVector3 ss = getSSL(e->getVertex(j));
    D(XJ) = gp.x() - ss.x();
    D(YJ) = gp.y() - ss.y();
    D(ZJ) = gp.z() - ss.z();
  } 
}

void highOrderSmoother::smooth_metric(std::vector<MElement*>  & all, GFace *gf)
{
#ifdef HAVE_TAUCS
  linearSystemCSRTaucs<double> *lsys = new linearSystemCSRTaucs<double>;
#else
  linearSystemCSRGmm<double> *lsys = new linearSystemCSRGmm<double>;
  lsys->setNoisy(1);
  lsys->setGmres(1);
  lsys->setPrec(5.e-8);
#endif
  dofManager<double> myAssembler(lsys);
  elasticityTerm El(0, 1.0, .33, getTag());
  
  std::vector<MElement*> layer, v;
  double minD;
  
  getDistordedElements(all, 0.99, v, minD);

  const int nbLayers = 3; //2, originally :)
  for (int i = 0; i < nbLayers; i++){
    addOneLayer(all, v, layer);
    v.insert(v.end(), layer.begin(), layer.end());
  }

  Msg::Debug("%d elements after adding %d layers", (int)v.size(), nbLayers);

  if (!v.size()) return;

  addOneLayer(all, v, layer);

  std::set<MVertex*>::iterator it;
  std::map<MVertex*,SVector3>::iterator its;
  std::map<MVertex*,SVector3>::iterator itpresent;
  std::map<MVertex*,SVector3>::iterator ittarget;
  std::set<MVertex*> verticesToMove;

  for (unsigned int i = 0; i < layer.size(); i++){
    for (int j = 0; j < layer[i]->getNumVertices(); j++){
      MVertex *vert = layer[i]->getVertex(j);
      myAssembler.fixVertex(vert, 0, getTag(), 0);
      myAssembler.fixVertex(vert, 1, getTag(), 0);
    }
  }

  // printf("%d vertices \n", _displ.size());

  for (unsigned int i = 0; i < v.size(); i++){
    for (int j = 0; j < v[i]->getNumVertices(); j++){
      MVertex *vert = v[i]->getVertex(j);
      // printf("%d %d %d v\n", i, j, v[i]->getNumVertices());
      its = _straightSidedLocation.find(vert);
      if (its == _straightSidedLocation.end()){
        _straightSidedLocation[vert] = 
          SVector3(vert->x(), vert->y(), vert->z());     
        _targetLocation[vert] = 
          SVector3(vert->x(), vert->y(), vert->z());     
      }
      else{
        vert->x() = its->second.x();
        vert->y() = its->second.y();
        vert->z() = its->second.z();
        if (vert->onWhat()->dim() < _dim){
          myAssembler.fixVertex(vert, 0, getTag(), 0);
          myAssembler.fixVertex(vert, 1, getTag(), 0);
        }
      }
    }
  }
  
  // number the other DOFs
  for (unsigned int i = 0; i < v.size(); i++){
    for (int j = 0; j < v[i]->getNumVertices(); j++){
      MVertex *vert = v[i]->getVertex(j);
      myAssembler.numberVertex(vert, 0, getTag());
      myAssembler.numberVertex(vert, 1, getTag());
      verticesToMove.insert(vert);
    } 
  }
  
  double dx0 = smooth_metric_(v, gf, myAssembler, verticesToMove, El);
  double dx = dx0;
  Msg::Debug(" dx0 = %12.5E", dx0);
  int iter = 0;
  while(1){
    double dx2 = smooth_metric_(v, gf, myAssembler, verticesToMove, El);
    Msg::Debug(" dx2  = %12.5E", dx2);
    if (fabs(dx2 - dx) < 1.e-4 * dx0)break;
    if (iter++ > 2)break;
    dx = dx2;
  }

  for (it = verticesToMove.begin(); it != verticesToMove.end(); ++it){
    SPoint2 param;
    reparamMeshVertexOnFace(*it, gf, param);  
    GPoint gp = gf->point(param);    
    if ((*it)->onWhat()->dim() == 2){
      (*it)->x() = gp.x();
      (*it)->y() = gp.y();
      (*it)->z() = gp.z();
      _targetLocation[*it] = SVector3(gp.x(), gp.y(), gp.z());
    }
    else{
      SVector3 p =  _targetLocation[(*it)];
      (*it)->x() = p.x();
      (*it)->y() = p.y();
      (*it)->z() = p.z();      
    }
  }
  delete lsys;
}

double highOrderSmoother::smooth_metric_(std::vector<MElement*>  & v, 
                                         GFace *gf, 
                                         dofManager<double> &myAssembler,
                                         std::set<MVertex*> &verticesToMove,
                                         elasticityTerm &El)
{
  std::set<MVertex*>::iterator it;
  double dx = 0.0;

  //  printf("size %d\n",myAssembler.sizeOfR());

  if (myAssembler.sizeOfR()){

    // while convergence -------------------------------------------------------
    for (unsigned int i = 0; i < v.size(); i++){
      MElement *e = v[i];
      int nbNodes = e->getNumVertices();
      const int n2 = 2 * nbNodes;
      const int n3 = 3 * nbNodes;

      fullMatrix<double> K33(n3, n3);
      fullMatrix<double> K22(n2, n2);
      fullMatrix<double> J32(n3, n2);
      fullMatrix<double> J23(n2, n3);
      fullVector<double> D3(n3);
      fullVector<double> R2(n2);
      fullMatrix<double> J23K33(n2, n3);
      K33.setAll(0.0);
      SElement se(e);
      El.elementMatrix(&se, K33);
      computeMetricVector(gf, e, J32, J23, D3);
      J23K33.gemm(J23, K33, 1, 0);
      K22.gemm(J23K33, J32, 1, 0);
      J23K33.mult(D3, R2);
      for (int j = 0; j < n2; j++){
        Dof RDOF = El.getLocalDofR(&se, j);
        myAssembler.assemble(RDOF, -R2(j));
        for (int k = 0; k < n2; k++){
          Dof CDOF = El.getLocalDofC(&se, k);
          myAssembler.assemble(RDOF, CDOF, K22(j, k));
        }
      }
    }
    myAssembler.systemSolve();
    // for all element, compute detJ at integration points --> material law
    // end while convergence -------------------------------------------------------
  
    for (it = verticesToMove.begin(); it != verticesToMove.end(); ++it){
      if ((*it)->onWhat()->dim() == 2){
	SPoint2 param;
	reparamMeshVertexOnFace((*it), gf, param);  
	SPoint2 dparam;
	myAssembler.getDofValue((*it), 0, getTag(), dparam[0]);
	myAssembler.getDofValue((*it), 1, getTag(), dparam[1]);
	SPoint2 newp = param+dparam;
	dx += newp.x() * newp.x() + newp.y() * newp.y();
	(*it)->setParameter(0, newp.x());
	(*it)->setParameter(1, newp.y());
      }
    }    
    myAssembler.systemClear();
  }
  
  return dx;
}


double highOrderSmoother::apply_incremental_displacement (double max_incr,
							  std::vector<MElement*> & v,
							  bool mixed,
							  double thres,
							  char *meshName,
							  std::vector<MElement*> & disto)
{
#ifdef HAVE_PETSC
  // assume that the mesh is OK, yet already curved
  linearSystemPETSc<double> *lsys = new  linearSystemPETSc<double>;    
  dofManager<double> myAssembler(lsys);
  elasticityMixedTerm El_mixed (0, 1.0, .333, getTag());
  elasticityTerm El (0, 1.0, .333, getTag());

  std::set<MVertex*> _vertices;

  //+++++++++ Boundary Conditions & Numbering +++++++++++++++++++++++++++++++
  // fix all dof that correspond to vertices on the boundary 
  // the value is equal 
  for (unsigned int i = 0; i < v.size(); i++){
    for (int j = 0; j < v[i]->getNumVertices(); j++){
      MVertex *vert = v[i]->getVertex(j);
      _vertices.insert(vert);
    }
  }

  //+++++++++ Fix d tr(eps) = 0 +++++++++++++++++++++++++++++++
  if (mixed){
    for (unsigned int i = 0; i < disto.size(); i++){
      for (int j = 0; j < disto[i]->getNumVertices(); j++){
	MVertex *vert = disto[i]->getVertex(j);
	myAssembler.fixVertex(vert, 4, getTag(), 0.0);
      }
    }
  }

  for (std::set<MVertex*>::iterator it = _vertices.begin(); it != _vertices.end(); ++it){
    MVertex *vert = *it;
    std::map<MVertex*,SVector3>::iterator itt = _targetLocation.find(vert);
    // impose displacement @ boundary
    if (vert->onWhat()->geomType() != GEntity::Line && vert->onWhat()->tag() != 1){ // TEST
      if (itt != _targetLocation.end() && vert->onWhat()->dim() < _dim){
	myAssembler.fixVertex(vert, 0, getTag(), itt->second.x()-vert->x());
	myAssembler.fixVertex(vert, 1, getTag(), itt->second.y()-vert->y());
	myAssembler.fixVertex(vert, 2, getTag(), itt->second.z()-vert->z());
      }
      // ensure we do not touch any vertex that is on the boundary
      else if (vert->onWhat()->dim() < _dim){
	myAssembler.fixVertex(vert, 0, getTag(), 0);
	myAssembler.fixVertex(vert, 1, getTag(), 0);
	myAssembler.fixVertex(vert, 2, getTag(), 0);
      }
    }
    if (_dim == 2)myAssembler.fixVertex(vert, 2, getTag(), 0);
    // number vertices 
    myAssembler.numberVertex(vert, 0, getTag());
    myAssembler.numberVertex(vert, 1, getTag());
    myAssembler.numberVertex(vert, 2, getTag());
    if (mixed){
      myAssembler.numberVertex(vert, 3, getTag());
      myAssembler.numberVertex(vert, 4, getTag());
    }
  }

  //+++++++++ Assembly  & Solve ++++++++++++++++++++++++++++++++++++
  if (myAssembler.sizeOfR()){
    // assembly of the elasticity term on the
    for (unsigned int i = 0; i < v.size(); i++){
      SElement se(v[i]);
      if (mixed)El_mixed.addToMatrix(myAssembler, &se);
      else El.addToMatrix(myAssembler, &se);
    }
    // solve the system
    lsys->systemSolve();
  }
  
  //+++++++++ Move vertices @ maximum ++++++++++++++++++++++++++++++++++++
  FILE *fd = fopen ("d.msh","w");
  fprintf(fd,"$MeshFormat\n2 0 8\n$EndMeshFormat\n$NodeData\n1\n\"tr(sigma)\"\n1\n0.0\n3\n1\n3\n%d\n", (int) _vertices.size());
  for (std::set<MVertex*>::iterator it = _vertices.begin(); it != _vertices.end(); ++it){
    double ax, ay, az;
    myAssembler.getDofValue(*it, 0, getTag(), ax);
    myAssembler.getDofValue(*it, 1, getTag(), ay);
    myAssembler.getDofValue(*it, 2, getTag(), az);    
    (*it)->x() += max_incr*ax;
    (*it)->y() += max_incr*ay;
    (*it)->z() += max_incr*az;
    fprintf(fd,"%d %g %g %g\n",(*it)->getIndex(), ax,ay,az);
  }
  fprintf(fd,"$EndNodeData\n");
  fclose(fd);

  //+------------------- Check now if elements are ok ---------------+++++++

  (*_vertices.begin())->onWhat()->model()->writeMSH(meshName);

  /*
    std::map<MVertex*,double> facts;
    while(1){
    int count = 0; 
    for (unsigned int i = 0; i < v.size(); i++){
    double disto = v[i]->distoShapeMeasure();
    if (disto < thres){
    count ++;
    for (int j = 0; j < v[i]->getNumVertices(); j++){
    MVertex *vert = v[i]->getVertex(j);
    std::map<MVertex*,double>::iterator it = facts.find(vert);
    if (it == facts.end())facts[vert] = .1;
    else if (it->second < 1){
    it->second += .1;
    double ax, ay, az;
    myAssembler.getDofValue(vert, 0, getTag(), ax);
    myAssembler.getDofValue(vert, 1, getTag(), ay);
    myAssembler.getDofValue(vert, 2, getTag(), az);    
    vert->x() -= .1*ax;
    vert->y() -= .1*ay;
    vert->z() -= .1*az;
    }
    }
    }
    if (!count)break;
    Msg::Info("%d elements are negative : reducing displacement",count);
    }
  */

  double percentage = max_incr * 100.;
  while(1){
    std::vector<MElement*> disto;
    double minD;
    getDistordedElements(v, 0.5, disto, minD);    
    if (minD < thres){
      percentage -= 10.;
      for (std::set<MVertex*>::iterator it = _vertices.begin(); it != _vertices.end(); ++it){
	double ax, ay, az;
	myAssembler.getDofValue(*it, 0, getTag(), ax);
	myAssembler.getDofValue(*it, 1, getTag(), ay);
	myAssembler.getDofValue(*it, 2, getTag(), az);    
	(*it)->x() -= .1*ax;
	(*it)->y() -= .1*ay;
	(*it)->z() -= .1*az;
      }
    }
    else break;
  }
    
  delete lsys;
  return percentage;
#endif
  return 0.0;
}

int highOrderSmoother::smooth_with_mixed_formulation (std::vector<MElement*> &all, 
						      double alpha)
{
  int ITER = 0;
  double minD, FACT = 1.0;
  std::vector<MElement*> disto;
  // move the points to their straight sided locations
  for (unsigned int i = 0; i < all.size(); i++)
    moveToStraightSidedLocation(all[i]);
  // apply the displacement
  double percentage = 0.0;
  while(1){
    char NN[256];
    sprintf(NN,"smoothing-%d.msh",ITER++);
    //double percentage_of_what_is_left = apply_incremental_displacement (1.,all, false, alpha,NN,disto);
    	  double percentage_of_what_is_left = apply_incremental_displacement (1.,all, true, alpha,NN,all);
    percentage += (1.-percentage) * percentage_of_what_is_left/100.;
    Msg::Info("The smoother was able to do %3d percent of the motion",(int)(percentage*100.));
    if (percentage_of_what_is_left == 0.0) break;
    else if (percentage_of_what_is_left == 100.)break;
  }

  getDistordedElements(all, 0.3, disto, minD);    
  Msg::Info("iter %d : %d elements / %d distorted  min Disto = %g ",ITER,
	    all.size(), disto.size(), minD);       

  if (0 && percentage < 0.99){
    char NN[256];
    sprintf(NN,"smoothing-%d.msh",ITER++);
    double percentage_of_what_is_left = apply_incremental_displacement (1.0,all, true, alpha,NN,disto);    
    percentage += (1.-percentage) * percentage_of_what_is_left/100.;
    Msg::Info("The mixed smoother was able to do %3d percent of the motion",(int)(percentage*100.));
  }
  
  return 1;
}


void highOrderSmoother::smooth(std::vector<MElement*> &all)
{
#if defined(HAVE_TAUCS)
  linearSystemCSRTaucs<double> *lsys = new linearSystemCSRTaucs<double>;
#elif defined(HAVE_PETSC)
  linearSystemPETSc<double> *lsys = new  linearSystemPETSc<double>;    
#else
  linearSystemCSRGmm<double> *lsys = new linearSystemCSRGmm<double>;
  lsys->setNoisy(1);
  lsys->setGmres(1);
  lsys->setPrec(5.e-8);
#endif
  dofManager<double> myAssembler(lsys);
  elasticityTerm El(0, 1.0, .333, getTag());
  
  std::vector<MElement*> layer, v;
  double minD;

  getDistordedElements(all, 0.15, v, minD);

  Msg::Info("%d elements / %d distorted  min Disto = %g",
             all.size(), v.size(), minD);

  if (!v.size());

  const int nbLayers = 3;
  for (int i = 0; i < nbLayers; i++){
    addOneLayer(all, v, layer);
    v.insert(v.end(), layer.begin(), layer.end());
  }

  Msg::Debug("%d elements after adding %d layers\n", (int)v.size(), nbLayers);

  addOneLayer(all, v, layer);

  //  printf("%d elements in the next layer\n",layer.size());

  for (unsigned int i = 0; i < layer.size(); i++){
    for (int j = 0; j < layer[i]->getNumVertices(); j++){
      MVertex *vert = layer[i]->getVertex(j);
      myAssembler.fixVertex(vert, 0, getTag(), 0);
      myAssembler.fixVertex(vert, 1, getTag(), 0);
      myAssembler.fixVertex(vert, 2, getTag(), 0);
    }
  }

  std::map<MVertex*,SVector3>::iterator it;
  std::map<MVertex*,SVector3>::iterator its;
  std::map<MVertex*,SVector3>::iterator itt;
  std::map<MVertex*,SVector3> verticesToMove;

  //  printf("%d vertices \n", _displ.size());

  for (unsigned int i = 0; i < v.size(); i++){
    for (int j = 0; j < v[i]->getNumVertices(); j++){
      MVertex *vert = v[i]->getVertex(j);
      // printf("%d %d %d v\n",i,j,v[i]->getNumVertices());
      its = _straightSidedLocation.find(vert);
      itt = _targetLocation.find(vert);
      if (its != _straightSidedLocation.end() && vert->onWhat()->dim() < _dim){
        myAssembler.fixVertex(vert, 0, getTag(), vert->x()-its->second.x());
        myAssembler.fixVertex(vert, 1, getTag(), vert->y()-its->second.y());
        myAssembler.fixVertex(vert, 2, getTag(), vert->z()-its->second.z());
      }
      // ensure we do not touch any vertex that is on the boundary
      else if (vert->onWhat()->dim() < _dim){
        myAssembler.fixVertex(vert, 0, getTag(), 0);
        myAssembler.fixVertex(vert, 1, getTag(), 0);
        myAssembler.fixVertex(vert, 2, getTag(), 0);
      }
    }
  }

  for (unsigned int i = 0; i < v.size(); i++)
    moveToStraightSidedLocation(v[i]);
  
  // number the other DOFs
  for (unsigned int i = 0; i < v.size(); i++){
    for (int j = 0; j < v[i]->getNumVertices(); j++){
      MVertex *vert = v[i]->getVertex(j);
      myAssembler.numberVertex(vert, 0, getTag());
      myAssembler.numberVertex(vert, 1, getTag());
      myAssembler.numberVertex(vert, 2, getTag());
      // gather all vertices that are supposed to move
      verticesToMove[vert] = SVector3(0.0, 0.0, 0.0);
    } 
  }

  //  Msg::Info("%d vertices FIXED %d NUMBERED", myAssembler.sizeOfF()
  //        , myAssembler.sizeOfR());

  if (myAssembler.sizeOfR()){

    // assembly of the elasticity term on the
    // set of elements
    for (unsigned int i = 0; i < v.size(); i++){
      SElement se(v[i]);
      El.addToMatrix(myAssembler, &se);
    }
    // solve the system
    lsys->systemSolve();
  }
  
  for (it = verticesToMove.begin(); it != verticesToMove.end(); ++it){
    double ax, ay, az;
    myAssembler.getDofValue(it->first, 0, getTag(), ax);
    myAssembler.getDofValue(it->first, 1, getTag(), ay);
    myAssembler.getDofValue(it->first, 2, getTag(), az);
    it->first->x() += ax;
    it->first->y() += ay;
    it->first->z() += az;
  }

  // delete matrices and vectors
  
  delete lsys;
}

void highOrderSmoother::smooth_cavity(std::vector<MElement*>& cavity,
                                      std::vector<MElement*>& old_elems,
                                      GFace* gf) {
 
  //  printf("Smoothing a cavity...\n");
  //  printf("Old elems : %d and %d\n", old_elems[0]->getNum(), old_elems[1]->getNum());
  //  printf("Cavity elems : %d and %d\n", cavity[0]->getNum(), cavity[1]->getNum());
  linearSystem<double> *lsys;
#ifdef HAVE_TAUCS
  if (cavity.size() < 20)
    lsys = new linearSystemFull<double>;
  else
    lsys = new linearSystemCSRTaucs<double>;
#else
  linearSystemCSRGmm<double> *lsys_ = new linearSystemCSRGmm<double>;
  lsys_->setNoisy(1);
  lsys_->setGmres(1);
  lsys_->setPrec(5.e-8);
  lsys = lsys_;
#endif

  dofManager<double> myAssembler(lsys);
  elasticityTerm El(0, 1.0, 0.333, getTag());

  std::vector<MElement*> layer,v;

  /* Debug only :p */
  //cavity = old_elems;

  v.insert(v.end(), gf->triangles.begin(),gf->triangles.end());
  v.insert(v.end(), gf->quadrangles.begin(),gf->quadrangles.end());

  addOneLayer(v,cavity,layer);

  for (unsigned int i = 0; i < layer.size(); i++){
    if (find(old_elems.begin(), old_elems.end(), layer[i]) == old_elems.end()) {
      for (int j = 0; j < layer[i]->getNumVertices(); j++){
	MVertex *vert = layer[i]->getVertex(j);
	myAssembler.fixVertex(vert, 0, getTag(), 0);
	myAssembler.fixVertex(vert, 1, getTag(), 0);
	//printf("Fixing vertex %d\n", vert->getNum());
      }
    }
  }
  
  //printf("%d vertices \n", _displ.size());

  std::set<MVertex*>::iterator it;
  std::map<MVertex*,SVector3>::iterator its;
  std::map<MVertex*,SVector3>::iterator itpresent;
  std::map<MVertex*,SVector3>::iterator ittarget;
  //std::map<MVertex*,SVector3> verticesToMove;
  std::set<MVertex*> verticesToMove;

  printf(" size = %d\n", (int)_straightSidedLocation.size());

  for (unsigned int i = 0; i < cavity.size(); i++){
    for (int j = 0; j < cavity[i]->getNumVertices(); j++){
      MVertex *vert = cavity[i]->getVertex(j);
      its = _straightSidedLocation.find(vert);
      if (its == _straightSidedLocation.end()) {
	//printf("SETTING LOCATIONS for %d\n",vert->getNum());
        _straightSidedLocation[vert] = 
          SVector3(vert->x(), vert->y(), vert->z());     
        _targetLocation[vert] = 
          SVector3(vert->x(), vert->y(), vert->z());
      }else {
        vert->x() = its->second.x();
        vert->y() = its->second.y();
        vert->z() = its->second.z();
        if (vert->onWhat()->dim() < _dim){
          myAssembler.fixVertex(vert, 0, getTag(), 0);
          myAssembler.fixVertex(vert, 1, getTag(), 0);
	  //printf("Fixing vertex %d\n", vert->getNum());
        }
      }
    }
  }

  // number the other DOFs
  for (unsigned int i = 0; i < cavity.size(); i++){
    for (int j = 0; j < cavity[i]->getNumVertices(); j++){
      MVertex *vert = cavity[i]->getVertex(j);
      //printf("Numbering vertex %d\n",vert->getNum());
      myAssembler.numberVertex(vert, 0, getTag());
      myAssembler.numberVertex(vert, 1, getTag());
      verticesToMove.insert(vert);
    } 
  }

  //Msg::Info("%d vertices FIXED %d NUMBERED\n", myAssembler.sizeOfF()
  //          , myAssembler.sizeOfR());
  
  double dx0 = smooth_metric_(cavity, gf, myAssembler, verticesToMove, El);
  double dx = dx0;
  //printf(" dx0 = %12.5E\n", dx0);
  int iter = 0;
  while(1){
    double dx2 = smooth_metric_(cavity, gf, myAssembler, verticesToMove, El);
    //printf(" dx2  = %12.5E\n", dx2);
    if (fabs(dx2 - dx) < 1.e-4 * dx0)break;
    if (iter++ > 2)break;
    dx = dx2;
  }

  for (it = verticesToMove.begin(); it != verticesToMove.end(); ++it){
    SPoint2 param;
    reparamMeshVertexOnFace(*it, gf, param);  
    GPoint gp = gf->point(param);    
    if ((*it)->onWhat()->dim() == 2){
      (*it)->x() = gp.x();
      (*it)->y() = gp.y();
      (*it)->z() = gp.z();
      _targetLocation[*it] = SVector3(gp.x(), gp.y(), gp.z());
    }
    else{
      SVector3 p =  _targetLocation[(*it)];
      (*it)->x() = p.x();
      (*it)->y() = p.y();
      (*it)->z() = p.z();      
    }
    //printf("  Moving %d to %g %g %g\n", (*it)->getNum(),_targetLocation[(*it)][0], _targetLocation[(*it)][1],_targetLocation[(*it)][2] );
  }

  delete lsys;
}


/*
  n3    n23     n2
  x-----x-------x
  |            /|
  |    t1    /  |
  |        /    |
  x n13  x n12  x n24
  |    /        |
  |  /    t2    |
  |/            |
  x------x------x
  n1    n14     n4
*/

/*
  n34 is the new vertex, we'd like to put it at the 
  best place i.e. at a place where it maximizes the 
  minimal smoothness of the neighboring elements.

  One can actually maximize this value or one can use
  a standard interpolation scheme (transfinite or elliptic)
  to place the point.
*/

static void getNodesP2(const MEdge &me, MTriangle *t1, MTriangle *t2,
                       MVertex * &n1,MVertex * &n2,MVertex * &n3,MVertex * &n4,
                       MVertex * &n12,MVertex * &n14,MVertex * &n24,MVertex * &n23,
                       MVertex * &n13){

  n1 = me.getVertex(0);
  n2 = me.getVertex(1);    
  
  if (t1->getVertex(0) != n1 && t1->getVertex(0) != n2) n3 = t1->getVertex(0);
  else if (t1->getVertex(1) != n1 && t1->getVertex(1) != n2) n3 = t1->getVertex(1);
  else if (t1->getVertex(2) != n1 && t1->getVertex(2) != n2) n3 = t1->getVertex(2);
  int iStart = 0;
  for (;iStart<3;iStart++)
    if (t1->getVertex(iStart) == n1)
      break;
  if (n2 == t1->getVertex((iStart+1)%3)){
    n12 = t1->getVertex((iStart+0)%3 + 3);
    n23 = t1->getVertex((iStart+1)%3 + 3);
    n13 = t1->getVertex((iStart+2)%3 + 3);
  }
  else{
    n13 = t1->getVertex((iStart+0)%3 + 3);
    n23 = t1->getVertex((iStart+1)%3 + 3);
    n12 = t1->getVertex((iStart+2)%3 + 3);
  }
  
  if (t2->getVertex(0) != n1 && t2->getVertex(0) != n2) n4 = t2->getVertex(0);
  else if (t2->getVertex(1) != n1 && t2->getVertex(1) != n2) n4 = t2->getVertex(1);
  else if (t2->getVertex(2) != n1 && t2->getVertex(2) != n2) n4 = t2->getVertex(2);
  iStart = 0;
  for (;iStart<3;iStart++)
    if (t2->getVertex(iStart) == n1)
      break;
  if (n2 == t2->getVertex((iStart+1)%3)){
    n24 = t2->getVertex((iStart+1)%3 + 3);
    n14 = t2->getVertex((iStart+2)%3 + 3);
  }
  else{
    n14 = t2->getVertex((iStart+0)%3 + 3);
    n24 = t2->getVertex((iStart+1)%3 + 3);
  }
}

static void getNodesPN (const MEdge &me, MTriangle *t1, MTriangle *t2,
                        MVertex * &n1,MVertex * &n2,MVertex * &n3,MVertex * &n4,
                        std::vector<MVertex *> &n12,
                        std::vector<MVertex *> &n14,
                        std::vector<MVertex *> &n24,
                        std::vector<MVertex *> &n23,
                        std::vector<MVertex *> &n13){

  n1 = me.getVertex(0);
  n2 = me.getVertex(1);    
  
  int order = t1->getPolynomialOrder();

  if (t1->getVertex(0) != n1 && t1->getVertex(0) != n2) n3 = t1->getVertex(0);
  else if (t1->getVertex(1) != n1 && t1->getVertex(1) != n2) n3 = t1->getVertex(1);
  else if (t1->getVertex(2) != n1 && t1->getVertex(2) != n2) n3 = t1->getVertex(2);
  int iStart = 0;
  for (;iStart<3;iStart++)
    if (t1->getVertex(iStart) == n1)
      break;
  if (n2 == t1->getVertex((iStart+1)%3)){
    int start12 = 3 + ((iStart+0)%3) * (order-1);
    for (int i=0;i<order-1;i++)n12.push_back(t1->getVertex(start12+i));
    int start23 = 3 + ((iStart+1)%3) * (order-1);
    for (int i=0;i<order-1;i++)n23.push_back(t1->getVertex(start23+i));
    int start13 = 3 + ((iStart+2)%3) * (order-1);
    for (int i=0;i<order-1;i++)n13.push_back(t1->getVertex(start13+i));
  }
  else{
    int start13 = 3 + ((iStart+0)%3) * (order-1);
    for (int i=order-2;i>=0;i--)n13.push_back(t1->getVertex(start13+i));
    int start23 = 3 + ((iStart+1)%3) * (order-1);
    for (int i=order-2;i>=0;i--)n23.push_back(t1->getVertex(start23+i));
    int start12 = 3 + ((iStart+2)%3) * (order-1);
    for (int i=order-2;i>=0;i--)n12.push_back(t1->getVertex(start12+i));
  }
  
  if (t2->getVertex(0) != n1 && t2->getVertex(0) != n2) n4 = t2->getVertex(0);
  else if (t2->getVertex(1) != n1 && t2->getVertex(1) != n2) n4 = t2->getVertex(1);
  else if (t2->getVertex(2) != n1 && t2->getVertex(2) != n2) n4 = t2->getVertex(2);
  iStart = 0;
  for (;iStart<3;iStart++)
    if (t2->getVertex(iStart) == n1)
      break;

  if (n2 == t2->getVertex((iStart+1)%3)){
    int start24 = 3 + ((iStart+1)%3) * (order-1);
    for (int i=0;i<order-1;i++)n24.push_back(t2->getVertex(start24+i));
    int start14 = 3 + ((iStart+2)%3) * (order-1);
    for (int i=0;i<order-1;i++)n14.push_back(t2->getVertex(start14+i));
  }
  else{
    int start14 = 3 + ((iStart+0)%3) * (order-1);
    for (int i=order-2;i>=0;i--)n14.push_back(t1->getVertex(start14+i));
    int start24 = 3 + ((iStart+1)%3) * (order-1);
    for (int i=order-2;i>=0;i--)n24.push_back(t1->getVertex(start24+i));
  }
}

static double surfaceTriangleUV(SPoint2 &v1, SPoint2 &v2, SPoint2 &v3)           
{
  const double v12[2] = {v2.x() - v1.x(),v2.y() - v1.y()};
  const double v13[2] = {v3.x() - v1.x(),v3.y() - v1.y()};
  return 0.5 * fabs (v12[0] * v13[1] - v12[1] * v13[0]);
}

struct swap_triangles_p2
{
  MTriangle *t1, *t2;
  MTriangle *t3, *t4;
  double quality_old;
  double quality_new;
  double s_before,s_after;
  MVertex *n1, *n2, *n3, *n4;
  MVertex *n12, *n13, *n23, *n14, *n24;
  MVertex *n34;

  double evalConfiguration (GFace *gf, SPoint2 & p) const
  {
    GPoint gp34 = gf->point(p);
    MFaceVertex _test (gp34.x(),gp34.y(),gp34.z(),gf,p.x(),p.y());        
    std::vector<MVertex *>vv;
    vv.push_back(n13);vv.push_back(&_test);vv.push_back(n14);
    MTriangleN t3_test (n1,n3,n4,vv,2,t1->getNum(),t1->getPartition());
    vv.clear();
    vv.push_back(&_test);vv.push_back(n23);vv.push_back(n24);
    MTriangleN t4_test (n4,n3,n2,vv,2,t2->getNum(),t2->getPartition());
    const double q3 = shapeMeasure(&t3_test);
    const double q4 = shapeMeasure(&t4_test);
    return std::min(q3,q4);
  }

  MVertex *optimalLocation (GFace *gf, 
                            SPoint2 &p3,
                            SPoint2 &p4) const
  {
    SPoint2 p34_linear = (p3+p4)*.5;
    
    GPoint gp34 = gf->point(p34_linear);
    MFaceVertex *_test = new MFaceVertex (gp34.x(),gp34.y(),gp34.z(),
                                          gf,p34_linear.x(),p34_linear.y());        
    std::vector<MVertex *>vv;
    vv.push_back(n13);vv.push_back(_test);vv.push_back(n14);
    MTriangleN t3_test (n1,n3,n4,vv,2,t1->getNum(),t1->getPartition());
    vv.clear();
    vv.push_back(_test);vv.push_back(n23);vv.push_back(n24);
    MTriangleN t4_test (n4,n3,n2,vv,2,t2->getNum(),t2->getPartition());
    p2data data(gf,&t3_test,&t4_test,_test,0);
    fullVector<double> pp(2);
    pp(0) = p34_linear.x();
    pp(1) = p34_linear.y();
    minimize_grad_fd (_function_p2tB, pp, &data);
    return _test;
  }

  swap_triangles_p2(const MEdge &me, MTriangle *_t1, MTriangle *_t2, GFace *gf)
    : t1(_t1), t2(_t2),n1(0),n2(0),n3(0),n12(0),n13(0), n23(0), n14(0), n24(0),n34(0)
  {
    const double qold1 = shapeMeasure(t1);
    const double qold2 = shapeMeasure(t2);

    getNodesP2 (me,t1,t2,n1,n2,n3,n4,n12,n14,n24,n23,n13);
    SPoint2 p1,p2,p3,p4,p13,p24,p23,p14;
    reparamMeshEdgeOnFace(n1,n2,gf,p1,p2);
    reparamMeshEdgeOnFace(n3,n4,gf,p3,p4);
    reparamMeshEdgeOnFace(n13,n24,gf,p13,p24);
    reparamMeshEdgeOnFace(n23,n14,gf,p23,p14);

    s_before = surfaceTriangleUV(p1,p2,p4) + surfaceTriangleUV(p1,p2,p3);
    s_after =  surfaceTriangleUV(p3,p4,p1) + surfaceTriangleUV(p3,p4,p2);

    n34 = optimalLocation (gf,p3,p4);

    std::vector<MVertex *>vv;
    vv.push_back(n13);vv.push_back(n34);vv.push_back(n14);
    t3 = new MTriangleN (n1,n3,n4,vv,2,t1->getNum(),t1->getPartition());
    vv.clear();
    vv.push_back(n34);vv.push_back(n23);vv.push_back(n24);
    t4 = new MTriangleN (n4,n3,n2,vv,2,t2->getNum(),t2->getPartition());

    const double qnew1 = shapeMeasure(t3);
    const double qnew2 = shapeMeasure(t4);

    quality_new = std::min(qnew1,qnew2);
    quality_old = std::min(qold1,qold2);
  }
  bool operator < (const swap_triangles_p2 &other) const
  {
    return  other.quality_new - other.quality_old < quality_new - quality_old;
  }
  void print() const
  {
    printf("%g <--- %g\n",quality_new,quality_old);
    printf("%d %d %d %d %d %d\n",t1->getVertex(0)->getNum(),t1->getVertex(1)->getNum(),t1->getVertex(2)->getNum(),
           t1->getVertex(3)->getNum(),t1->getVertex(4)->getNum(),t1->getVertex(5)->getNum());
    printf("%d %d %d %d %d %d\n",t2->getVertex(0)->getNum(),t2->getVertex(1)->getNum(),t2->getVertex(2)->getNum(),
           t2->getVertex(3)->getNum(),t2->getVertex(4)->getNum(),t2->getVertex(5)->getNum());
    printf("%d %d %d %d %d %d\n",t3->getVertex(0)->getNum(),t3->getVertex(1)->getNum(),t3->getVertex(2)->getNum(),
           t3->getVertex(3)->getNum(),t3->getVertex(4)->getNum(),t3->getVertex(5)->getNum());
    printf("%d %d %d %d %d %d\n",t4->getVertex(0)->getNum(),t4->getVertex(1)->getNum(),t4->getVertex(2)->getNum(),
           t4->getVertex(3)->getNum(),t4->getVertex(4)->getNum(),t4->getVertex(5)->getNum());
    printf("%d %d %d %d %d %d %d %d %d\n",n1->getNum(),
           n2->getNum(),n3->getNum(),n4->getNum(),
           n12->getNum(),n23->getNum(),n13->getNum(),n24->getNum(),n14->getNum());
  }
  
};

struct swap_triangles_pN
{
  MTriangle *t1, *t2;
  MTriangle *t3, *t4;
  double quality_old;
  double quality_new;
  double s_before,s_after;
  MVertex *n1, *n2, *n3, *n4;
  std::vector<MVertex*> n12, n13, n23, n14, n24, n34;
  std::vector<MVertex*> n123, n124, n134, n234;
  edgeContainer &edgeVertices;
  faceContainer &faceVertices;
  highOrderSmoother *s;

  void cleanupDeletedEdge (bool swapWasSuccessfull )
  {
    MEdge _temp(swapWasSuccessfull ? n1 : n3 , swapWasSuccessfull ? n2 : n4 );
    std::pair<MVertex*, MVertex*> _temp2(_temp.getMinVertex(), _temp.getMaxVertex());
    edgeVertices.erase(_temp2);
    // do the same for internal vertices !!
  } 

  swap_triangles_pN(const MEdge &me, MTriangle *_t1, MTriangle *_t2, GFace *gf,
                    edgeContainer &_edgeVertices,
                    faceContainer &_faceVertices,
                    highOrderSmoother *_s)
    : t1(_t1), t2(_t2),edgeVertices(_edgeVertices),faceVertices(_faceVertices),s(_s)
  {

    const double qold1 = shapeMeasure(t1);
    const double qold2 = shapeMeasure(t2);

    getNodesPN (me,t1,t2,n1,n2,n3,n4,n12,n14,n24,n23,n13);
    SPoint2 p1,p2,p3,p4,p13,p24,p23,p14;
    reparamMeshEdgeOnFace(n1,n2,gf,p1,p2);
    reparamMeshEdgeOnFace(n3,n4,gf,p3,p4);

    s_before = surfaceTriangleUV(p1,p2,p4) + surfaceTriangleUV(p1,p2,p3);
    s_after =  surfaceTriangleUV(p1,p4,p3) + surfaceTriangleUV(p3,p4,p2);

    MTriangle t3lin(n1,n4,n3);
    MTriangle t4lin(n3,n4,n2);

    t3 =  setHighOrder(&t3lin,gf,edgeVertices,faceVertices,false,
                       !t1->getNumFaceVertices(),
                       t1->getPolynomialOrder()-1,s);
    t4 =  setHighOrder(&t4lin,gf,edgeVertices,faceVertices,false,
                       !t1->getNumFaceVertices(),
                       t1->getPolynomialOrder()-1,s);

    optimalLocationPN_ (gf,me, t3, t4,s);
    std::vector<MElement*> cavity, old_elems;
    cavity.push_back(t3);
    cavity.push_back(t4);
    old_elems.push_back(t1);
    old_elems.push_back(t2);
    //s->smooth_cavity(cavity, old_elems, gf);
      
    const double qnew1 = shapeMeasure(t3);
    const double qnew2 = shapeMeasure(t4);

    quality_new = std::min(qnew1,qnew2);
    quality_old = std::min(qold1,qold2);

    //    if (quality_old < quality_new)
    //printf("QUALITY GOING FROM %12.5E TO %12.5E\n",quality_old,quality_new);

  }
  bool operator < (const swap_triangles_pN &other) const
  {
    return  other.quality_new - other.quality_old < quality_new - quality_old;
  }
};

static int optimalLocationP2_(GFace *gf, 
                              const MEdge &me,
                              MTriangle *t1, MTriangle *t2, 
                              highOrderSmoother *s)
{
  double qini = std::min(shapeMeasure(t1),shapeMeasure(t2));

  if (qini > 0.6) return 0;
  
  MVertex *n1,*n2,*n3=0,*n4=0,*n12,*n14,*n24,*n23,*n13;
  getNodesP2 (me,t1,t2,n1,n2,n3,n4,n12,n14,n24,n23,n13);
  SPoint2 p1,p2,p3,p4,p12;
  reparamMeshVertexOnFace(n12,gf,p12);
  reparamMeshEdgeOnFace(n3,n4,gf,p3,p4);
  reparamMeshEdgeOnFace(n3,n4,gf,p3,p4);
  SPoint2 p34_linear = (p1+p2)*.5;
  SPoint2 dirt = p4-p3;

  fullVector<double> pp(2);
  pp(0) = p12.x();
  pp(1) = p12.y();
  p2data data(gf,t1,t2,n12,s);

  double init = _DeformationEnergy (&data);
  double opti = minimize_grad_fd (_function_p2tB, pp, &data);

  printf("opti %g init %g\n",opti,init);

  if (init-opti < 1.e-5*(init))return 0;

  double u0 = gf->parBounds(0).low();
  double u1 = gf->parBounds(0).high();
  double v0 = gf->parBounds(1).low();
  double v1 = gf->parBounds(1).high();
  if (pp(0) < u0 || pp(0) > u1 || pp(1) < v0 || pp(1) > v1)return 0;   
  
  GPoint gp12 = gf->point(SPoint2(pp(0),pp(1)));
  n12->x() = gp12.x();
  n12->y() = gp12.y();
  n12->z() = gp12.z();
  n12->setParameter(0,pp(0));
  n12->setParameter(1,pp(1));
  printf("Hum, order 2 here...");
  return 1;
}

int optimalLocationPN_ (GFace *gf, const MEdge &me, MTriangle *t1, MTriangle *t2,
                        highOrderSmoother *s)
{
  // if quality is sufficient, do nothing (this is an expensive
  //  optimization process)
  double qopt = std::min(shapeMeasure(t1),shapeMeasure(t2));
  if (qopt > 0.6) return 0;
  // get the 2 end vertex of the edge
  MVertex *n1 = me.getVertex(0);
  MVertex *n2 = me.getVertex(1);
  // get all the other nodes that are on the edge
  int N = t1->getNumVertices();
  int NE = t1->getNumEdgeVertices();
  std::vector<MVertex*> toOptimize;
  for (int i=3;i<3+NE;i++){
    MVertex *v1 = t1->getVertex(i);
    for (int j=3;j<3+NE;j++){
      MVertex *v2 = t2->getVertex(j);
      if (v1 == v2 && v1 != n1 && v1 != n2){
        toOptimize.push_back(v1);
        break;
      }
    }
  }

  for (int i=3+NE;i<N;i++){
    toOptimize.push_back(t1->getVertex(i));
    toOptimize.push_back(t2->getVertex(i));
  }

  fullVector<double> pp(2*toOptimize.size());
  for (unsigned int i=0;i<toOptimize.size();i++){
    SPoint2 pt;
    reparamMeshVertexOnFace(toOptimize[i],gf,pt);
    pp(2*i)   = pt[0];
    pp(2*i+1) = pt[1];
  }

  pNdata data(gf,t1,t2,toOptimize,s);
  double init = _DeformationEnergy (&data);
  double opti = minimize_grad_fd (_function_pNtB, pp, &data);
  ///double opti = minimize_grad_fd (_function_pNt, pp, &data);
  if (init-opti < 1.e-5*(init))return 0;
  printf("Optimization has reduced the deformation energy %g -> %g\n",
         init,opti);
  for (unsigned int i=0;i<toOptimize.size();i++){
    GPoint gp12 = gf->point(SPoint2(pp(2*i),pp(2*i+1)));
    toOptimize[i]->x() = gp12.x();
    toOptimize[i]->y() = gp12.y();
    toOptimize[i]->z() = gp12.z();
    toOptimize[i]->setParameter(0,pp(2*i));
    toOptimize[i]->setParameter(1,pp(2*i+1));
  }
  return 1;
}

static int findOptimalLocationsP2(GFace *gf, highOrderSmoother *s)
{
  e2t_cont adj;
  buildEdgeToTriangle(gf->triangles, adj);
  int N=0;
  for (e2t_cont::iterator it = adj.begin(); it!= adj.end(); ++it){
    if (it->second.second)
      N += optimalLocationP2_(gf,it->first, dynamic_cast<MTriangle*>(it->second.first),
                              dynamic_cast<MTriangle*>(it->second.second),s);
  }
  return N;
}

static int findOptimalLocationsPN(GFace *gf,highOrderSmoother *s)
{

  e2t_cont adj;
  buildEdgeToTriangle(gf->triangles, adj);
  int N=0;
  
  for (e2t_cont::iterator it = adj.begin(); it!= adj.end(); ++it){
    if (it->second.second)
      N += optimalLocationPN_(gf,it->first, dynamic_cast<MTriangle*>(it->second.first),
                              dynamic_cast<MTriangle*>(it->second.second),s);
  }
  return N;
}

static int swapHighOrderTriangles(GFace *gf, 
                                  edgeContainer &edgeVertices,
                                  faceContainer &faceVertices,
                                  highOrderSmoother *s)
{
  printf ("Initial Size of the map %d\n", (int)edgeVertices.size());
  printf ("Initial Size of the face map %d\n", (int)faceVertices.size());

  e2t_cont adj;
  buildEdgeToTriangle(gf->triangles, adj);

  std::set<swap_triangles_pN> pairs;

  for (e2t_cont::iterator it = adj.begin(); it!= adj.end(); ++it){
    if (it->second.second){
      MTriangle *t1 = dynamic_cast<MTriangle*>(it->second.first);
      MTriangle *t2 = dynamic_cast<MTriangle*>(it->second.second);

      const double qold1 = shapeMeasure(t1);
      const double qold2 = shapeMeasure(t2);

      //if (qold1 < 0.6 || qold2 < 0.06)
        pairs.insert(swap_triangles_pN(it->first,t1,t2,gf,
                                       edgeVertices,faceVertices,s));
    }
  }

  std::set<swap_triangles_pN>::iterator itp = pairs.begin();

  int nbSwap = 0;

  std::set<MTriangle*> t_removed;
  std::set<MVertex*> v_removed;

  std::vector<MTriangle*> triangles2;
  std::vector<MVertex*> mesh_vertices2;

  itp = pairs.begin();
  while(itp != pairs.end()){
    double diff = fabs(itp->s_before - itp->s_after);

    std::vector<MVertex*> v1,v2,v3,v4;
    int o1 = itp->t1->getPolynomialOrder();
    int o2 = itp->t2->getPolynomialOrder();
    int o3 = itp->t3->getPolynomialOrder();
    int o4 = itp->t4->getPolynomialOrder();

    itp->t1->getFaceVertices(0,v1);
    itp->t2->getFaceVertices(0,v2);
    itp->t3->getFaceVertices(0,v3);
    itp->t4->getFaceVertices(0,v4);

    std::vector<MVertex*> ve1(v1.begin()+3,v1.begin()+3*o1);
    std::vector<MVertex*> ve2(v2.begin()+3,v2.begin()+3*o2);
    std::vector<MVertex*> ve3(v3.begin()+3,v3.begin()+3*o3);
    std::vector<MVertex*> ve4(v4.begin()+3,v4.begin()+3*o4);
    std::vector<MVertex*> vf1(v1.begin()+3*o1,v1.end());
    std::vector<MVertex*> vf2(v2.begin()+3*o2,v2.end());
    std::vector<MVertex*> vf3(v3.begin()+3*o3,v3.end());
    std::vector<MVertex*> vf4(v4.begin()+3*o4,v4.end());

    bool t1_rem = (t_removed.find(itp->t1) != t_removed.end());
    bool t2_rem = (t_removed.find(itp->t2) != t_removed.end());

    if ( !t1_rem && !t2_rem &&
         itp->quality_new > 0 && //itp->quality_old &&
         diff < 1.e-9){

      swap_triangles_pN &sw = (swap_triangles_pN &) *itp;
      sw.cleanupDeletedEdge (true);

      t_removed.insert(itp->t1);
      t_removed.insert(itp->t2);
      v_removed.insert(vf1.begin(),vf1.end());
      v_removed.insert(vf2.begin(),vf2.end());

      triangles2.push_back(itp->t3);
      triangles2.push_back(itp->t4);
      mesh_vertices2.insert(mesh_vertices2.end(),vf3.begin(),vf3.end());
      mesh_vertices2.insert(mesh_vertices2.end(),vf4.begin(),vf4.end());

      for(std::vector<MVertex*>::iterator vit = ve1.begin(); vit != ve1.end(); vit++) {
        if (find(ve2.begin(),ve2.end(),*vit)!=ve2.end())
          v_removed.insert(*vit);
      }

      nbSwap++;
    }
    else {
      for(std::vector<MVertex*>::iterator vit = ve3.begin(); vit != ve3.end(); vit++) {
        if (find(ve4.begin(),ve4.end(),*vit)!=ve4.end())
          v_removed.insert(*vit);
	  //delete  *vit;
      }
      
      swap_triangles_pN &sw = (swap_triangles_pN &) *itp;
      sw.cleanupDeletedEdge (false);

      delete itp->t3;
      delete itp->t4;
    }
    ++itp;
  }

  for (unsigned int i = 0; i < gf->mesh_vertices.size(); i++){
    if (v_removed.find(gf->mesh_vertices[i]) == v_removed.end()){
      mesh_vertices2.push_back(gf->mesh_vertices[i]);
    } 
  }

  gf->mesh_vertices.clear();
  gf->mesh_vertices = mesh_vertices2;

  for (unsigned int i = 0; i < gf->triangles.size(); i++){
    if (t_removed.find(gf->triangles[i]) == t_removed.end()){
      triangles2.push_back(gf->triangles[i]);
    }
    else {
      delete gf->triangles[i];
    }    
  }
  gf->triangles.clear();
  gf->triangles = triangles2;
  printf("%d swaps performed\n", nbSwap);
  printf("Final Size of the map %d\n", (int)edgeVertices.size());
  printf("Final Size of the face map %d\n", (int)faceVertices.size());
  return nbSwap;
}

static int swapHighOrderTriangles(GFace *gf)
{
  e2t_cont adj;
  buildEdgeToTriangle(gf->triangles, adj);

  std::set<swap_triangles_p2> pairs;

  for (e2t_cont::iterator it = adj.begin(); it!= adj.end(); ++it){
    if (it->second.second){
      MTriangle *t1 = dynamic_cast<MTriangle*>(it->second.first);
      MTriangle *t2 = dynamic_cast<MTriangle*>(it->second.second);
      const double qold1 = shapeMeasure(t1);
      const double qold2 = shapeMeasure(t2);
      if (qold1 < 0.6 || qold2 < 0.6)
        pairs.insert(swap_triangles_p2(it->first,t1,t2,gf));
    }
  }
  std::set<swap_triangles_p2>::iterator itp = pairs.begin();

  int nbSwap = 0;

  std::set<MTriangle*> t_removed;
  std::set<MVertex*> v_removed;

  std::vector<MTriangle*> triangles2;
  std::vector<MVertex*> mesh_vertices2;

  itp = pairs.begin();
  while(itp != pairs.end()){
    double diff = fabs(itp->s_before - itp->s_after);
    if ( t_removed.find(itp->t1) == t_removed.end() &&
         t_removed.find(itp->t2) == t_removed.end() &&
         itp->quality_new > itp->quality_old &&
         diff < 1.e-9){
      //      itp->print();
      t_removed.insert(itp->t1);
      t_removed.insert(itp->t2);
      triangles2.push_back(itp->t3);
      triangles2.push_back(itp->t4);
      if (itp->n34 != itp->n12){
        v_removed.insert(itp->n12);
        mesh_vertices2.push_back(itp->n34);
      }
      nbSwap++;
    }
    else{
      delete itp->t3;
      delete itp->t4;
      if (itp->n34 != itp->n12) delete itp->n34;
    }
    ++itp;
  }
    
  for (unsigned int i = 0; i < gf->mesh_vertices.size(); i++){
    if (v_removed.find(gf->mesh_vertices[i]) == v_removed.end()){
      mesh_vertices2.push_back(gf->mesh_vertices[i]);
    }
    else {
      delete gf->mesh_vertices[i];
    }    
  }
  gf->mesh_vertices = mesh_vertices2;

  for (unsigned int i = 0; i < gf->triangles.size(); i++){
    if (t_removed.find(gf->triangles[i]) == t_removed.end()){
      triangles2.push_back(gf->triangles[i]);
    }
    else {
      delete gf->triangles[i];
    }    
  }
  //  printf("replacing %d by %d\n",gf->triangles.size(),triangles2.size());
  gf->triangles = triangles2;
  return nbSwap;
}

void  highOrderSmoother::swap(GFace *gf, 
                                  edgeContainer &edgeVertices,
                                  faceContainer &faceVertices)
{
  // swapHighOrderTriangles(gf);
  swapHighOrderTriangles(gf,edgeVertices,faceVertices,this);
  // swapHighOrderTriangles(gf);
  // swapHighOrderTriangles(gf);
  // swapHighOrderTriangles(gf);
}

void  highOrderSmoother::smooth_p2point(GFace *gf)
{
  findOptimalLocationsP2(gf,this);
  // findOptimalLocationsP2(gf);
  // findOptimalLocationsP2(gf);
}

void  highOrderSmoother::smooth_pNpoint(GFace *gf)
{
  findOptimalLocationsPN(gf,this);
}

//-------------------------------------------------
// Assume a GModel that is meshed with high order nodes that
// are on their final position (at least they all lie on
// surfaces and curves and the smoother may move them)
//-------------------------------------------------

highOrderSmoother::highOrderSmoother (GModel *gm)
  : _tag(111)
{
  _clean();
  // compute straigh sided positions that are actually X,Y,Z positions
  // that are NOT always on curves and surfaces 

  // compute stright sided positions of vertices that are classified on model edges
  for(GModel::eiter it = gm->firstEdge(); it != gm->lastEdge(); ++it){    
    for (int i=0;i<(*it)->lines.size();i++){
      MLine *l = (*it)->lines[i];
      int N = l->getNumVertices()-2;
      SVector3 p0((*it)->lines[i]->getVertex(0)->x(),
		  (*it)->lines[i]->getVertex(0)->y(),
		  (*it)->lines[i]->getVertex(0)->z());
      SVector3 p1((*it)->lines[i]->getVertex(1)->x(),
		  (*it)->lines[i]->getVertex(1)->y(),
		  (*it)->lines[i]->getVertex(1)->z());
      for (int j=1;j<=N;j++){
	const double xi = (double)j/(N+1);
	const SVector3 straightSidedPoint = p0 *(1.-xi) + p1*xi;
	_straightSidedLocation [(*it)->lines[i]->getVertex(j)] = straightSidedPoint;
      }
    }
  }

  // compute stright sided positions of vertices that are classified on model faces
  for(GModel::fiter it = gm->firstFace(); it != gm->lastFace(); ++it){
    for (int i=0;i<(*it)->triangles.size();i++){
      MTriangle *t = (*it)->triangles[i];
      MFace face = t->getFace(0);
      const polynomialBasis* fs = t->getFunctionSpace();
      for (int j=0;j<t->getNumVertices();j++){
	if (t->getVertex(j)->onWhat() == *it){
	  const double t1 = fs->points(j, 0);
	  const double t2 = fs->points(j, 1);
	  SPoint3 pc = face.interpolate(t1, t2);
	  _straightSidedLocation [t->getVertex(j)] = 
	    SVector3(pc.x(),pc.y(),pc.z()); 
	}
      }
    }
    for (int i=0;i<(*it)->quadrangles.size();i++){
      MQuadrangle *q = (*it)->quadrangles[i];
      MFace face = q->getFace(0);
      const polynomialBasis* fs = q->getFunctionSpace();
      for (int j=0;j<q->getNumVertices();j++){
	if (q->getVertex(j)->onWhat() == *it){
	  const double t1 = fs->points(j, 0);
	  const double t2 = fs->points(j, 1);
	  SPoint3 pc = face.interpolate(t1, t2);
	  _straightSidedLocation [q->getVertex(j)] = 
	    SVector3(pc.x(),pc.y(),pc.z()); 
	}
      }
    }
  }
}


#endif

