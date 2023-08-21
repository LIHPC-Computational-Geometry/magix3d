// Gmsh - Copyright (C) 1997-2013 C. Geuzaine, J.-F. Remacle
//
// See the LICENSE.txt file for license information. Please report all
// bugs and problems to the public mailing list <gmsh@geuz.org>.

#include <stdlib.h>
#include <math.h>
#include "GmshConfig.h"
#include "GmshMessage.h"
#include "GModel.h"
#include "MElement.h"
#include "MPoint.h"
#include "MLine.h"
#include "MTriangle.h"
#include "MQuadrangle.h"
#include "MTetrahedron.h"
#include "MHexahedron.h"
#include "MPrism.h"
#include "MPyramid.h"
#include "MElementCut.h"
#include "MSubElement.h"
#include "GEntity.h"
#include "StringUtils.h"
#include "Numeric.h"
#include "Context.h"

#define SQU(a)      ((a)*(a))

double MElement::_isInsideTolerance = 1.e-6;

MElement::MElement(int num, int part) : _visible(1)
{
#pragma omp critical
  {
    // we should make GModel a mandatory argument to the constructor
    GModel *m = GModel::current();
    if(num){
      _num = num;
      m->setMaxElementNumber(std::max(m->getMaxElementNumber(), _num));
    }
    else{
      m->setMaxElementNumber(m->getMaxElementNumber() + 1);
      _num = m->getMaxElementNumber();
    }
    _partition = (short)part;
  }
}

void MElement::_getEdgeRep(MVertex *v0, MVertex *v1,
                           double *x, double *y, double *z, SVector3 *n,
                           int faceIndex)
{
  x[0] = v0->x(); y[0] = v0->y(); z[0] = v0->z();
  x[1] = v1->x(); y[1] = v1->y(); z[1] = v1->z();
  if(faceIndex >= 0){
    n[0] = n[1] = getFace(faceIndex).normal();
  }
  else{
    MEdge e(v0, v1);
    n[0] = n[1] = e.normal();
  }
}

void MElement::_getFaceRep(MVertex *v0, MVertex *v1, MVertex *v2,
                           double *x, double *y, double *z, SVector3 *n)
{
  x[0] = v0->x(); x[1] = v1->x(); x[2] = v2->x();
  y[0] = v0->y(); y[1] = v1->y(); y[2] = v2->y();
  z[0] = v0->z(); z[1] = v1->z(); z[2] = v2->z();
  SVector3 t1(x[1] - x[0], y[1] - y[0], z[1] - z[0]);
  SVector3 t2(x[2] - x[0], y[2] - y[0], z[2] - z[0]);
  SVector3 normal = crossprod(t1, t2);
  normal.normalize();
  for(int i = 0; i < 3; i++) n[i] = normal;
}

char MElement::getVisibility() const
{
  if(CTX::instance()->hideUnselected && _visible < 2) return false;
  return _visible;
}

double MElement::minEdge()
{
  double m = 1.e25;
  for(int i = 0; i < getNumEdges(); i++){
    MEdge e = getEdge(i);
    m = std::min(m, e.getVertex(0)->distance(e.getVertex(1)));
  }
  return m;
}

double MElement::maxEdge()
{
  double m = 0.;
  for(int i = 0; i < getNumEdges(); i++){
    MEdge e = getEdge(i);
    m = std::max(m, e.getVertex(0)->distance(e.getVertex(1)));
  }
  return m;
}

double MElement::rhoShapeMeasure()
{
  double min = minEdge();
  double max = maxEdge();
  if(max)
    return min / max;
  else
    return 0.;
}

void MElement::scaledJacRange(double &jmin, double &jmax)
{
  jmin = jmax = 1.0;
#if defined(HAVE_MESH)
  const JacobianBasis *jac = getJacobianFuncSpace();
  const int numJacNodes = jac->getNumJacNodes();
  fullMatrix<double> nodesXYZ(jac->getNumMapNodes(),3);
  getNodesCoord(nodesXYZ);
  fullVector<double> SJi(numJacNodes), Bi(numJacNodes);
  jac->getScaledJacobian(nodesXYZ,SJi);
  jac->lag2Bez(SJi,Bi);
  jmin = *std::min_element(Bi.getDataPtr(),Bi.getDataPtr()+Bi.size());
  jmax = *std::max_element(Bi.getDataPtr(),Bi.getDataPtr()+Bi.size());
#endif
}

void MElement::getNode(int num, double &u, double &v, double &w)
{
  // only for MElements that don't have a lookup table for this
  // (currently only 1st order elements have)
  double uvw[3];
  MVertex* ver = getVertex(num);
  double xyz[3] = {ver->x(), ver->y(), ver->z()};
  xyz2uvw(xyz, uvw);
  u = uvw[0];
  v = uvw[1];
  w = uvw[2];
}

void MElement::getShapeFunctions(double u, double v, double w, double s[], int o)
{
  const nodalBasis* fs = getFunctionSpace(o);
  if(fs) fs->f(u, v, w, s);
  else Msg::Error("Function space not implemented for this type of element");
}

void MElement::getGradShapeFunctions(double u, double v, double w, double s[][3],
                                     int o)
{
  const nodalBasis* fs = getFunctionSpace(o);
  if(fs) fs->df(u, v, w, s);
  else Msg::Error("Function space not implemented for this type of element");
}

void MElement::getHessShapeFunctions(double u, double v, double w, double s[][3][3],
                                     int o)
{
  const nodalBasis* fs = getFunctionSpace(o);
  if(fs) fs->ddf(u, v, w, s);
  else Msg::Error("Function space not implemented for this type of element");
}

void MElement::getThirdDerivativeShapeFunctions(double u, double v, double w,
                                                double s[][3][3][3], int o)
{
  const nodalBasis* fs = getFunctionSpace(o);
  if(fs) fs->dddf(u, v, w, s);
  else Msg::Error("Function space not implemented for this type of element");
}

SPoint3 MElement::barycenter_infty ()
{
  double xmin =  getVertex(0)->x();
  double xmax = xmin;
  double ymin =  getVertex(0)->y();
  double ymax = ymin;
  double zmin =  getVertex(0)->z();
  double zmax = zmin;
  int n = getNumVertices();
  for(int i = 0; i < n; i++) {
    MVertex *v = getVertex(i);
    xmin = std::min(xmin,v->x());
    xmax = std::max(xmax,v->x());
    ymin = std::min(ymin,v->y());
    ymax = std::max(ymax,v->y());
    zmin = std::min(zmin,v->z());
    zmax = std::max(zmax,v->z());
  }
  return SPoint3(0.5*(xmin+xmax),0.5*(ymin+ymax),0.5*(zmin+zmax));
}

SPoint3 MElement::barycenter()
{
  SPoint3 p(0., 0., 0.);
  int n = getNumVertices();
  for(int i = 0; i < n; i++) {
    MVertex *v = getVertex(i);
    p[0] += v->x();
    p[1] += v->y();
    p[2] += v->z();
  }
  p[0] /= (double)n;
  p[1] /= (double)n;
  p[2] /= (double)n;
  return p;
}

SPoint3 MElement::barycenterUVW()
{
  SPoint3 p(0., 0., 0.);
  int n = getNumVertices();
  for(int i = 0; i < n; i++) {
    double x, y, z;
    getNode(i, x, y, z);
    p[0] += x;
    p[1] += y;
    p[2] += z;
  }
  p[0] /= (double)n;
  p[1] /= (double)n;
  p[2] /= (double)n;
  return p;
}

double MElement::getVolume()
{
  int npts;
  IntPt *pts;
  getIntegrationPoints(getDim() * (getPolynomialOrder() - 1), &npts, &pts);
  double vol = 0.;
  for (int i = 0; i < npts; i++){
    vol += getJacobianDeterminant(pts[i].pt[0], pts[i].pt[1], pts[i].pt[2])
      * pts[i].weight;
  }
  return vol;
}

int MElement::getVolumeSign()
{
  double v = getVolume();
  if(v < 0.) return -1;
  else if(v > 0.) return 1;
  else return 0;
}

bool MElement::setVolumePositive()
{
  if(getDim() < 3) return true;
  int s = getVolumeSign();
  if(s < 0) revert();
  if(!s) return false;
  return true;
}

std::string MElement::getInfoString()
{
  char tmp[256];
  sprintf(tmp, "Element %d", getNum());
  return std::string(tmp);
}

static double _computeDeterminantAndRegularize(MElement *ele, double jac[3][3])
{
  double dJ = 0;

  switch (ele->getDim()) {

  case 0:
    {
      dJ = 1.0;
      jac[0][0] = jac[1][1] = jac[2][2] = 1.0;
      jac[0][1] = jac[1][0] = jac[2][0] = 0.0;
      jac[0][2] = jac[1][2] = jac[2][1] = 0.0;
      break;
    }
  case 1:
    {
      dJ = sqrt(SQU(jac[0][0]) + SQU(jac[0][1]) + SQU(jac[0][2]));

      // regularize matrix
      double a[3], b[3], c[3];
      a[0] = jac[0][0];
      a[1] = jac[0][1];
      a[2] = jac[0][2];
      if((fabs(a[0]) >= fabs(a[1]) && fabs(a[0]) >= fabs(a[2])) ||
         (fabs(a[1]) >= fabs(a[0]) && fabs(a[1]) >= fabs(a[2]))) {
        b[0] = a[1]; b[1] = -a[0]; b[2] = 0.;
      }
      else {
        b[0] = 0.; b[1] = a[2]; b[2] = -a[1];
      }
      norme(b);
      prodve(a, b, c);
      norme(c);
      jac[1][0] = b[0]; jac[1][1] = b[1]; jac[1][2] = b[2];
      jac[2][0] = c[0]; jac[2][1] = c[1]; jac[2][2] = c[2];
      break;
    }
  case 2:
    {
      dJ = sqrt(SQU(jac[0][0] * jac[1][1] - jac[0][1] * jac[1][0]) +
                SQU(jac[0][2] * jac[1][0] - jac[0][0] * jac[1][2]) +
                SQU(jac[0][1] * jac[1][2] - jac[0][2] * jac[1][1]));

      // regularize matrix
      double a[3], b[3], c[3];
      a[0] = jac[0][0];
      a[1] = jac[0][1];
      a[2] = jac[0][2];
      b[0] = jac[1][0];
      b[1] = jac[1][1];
      b[2] = jac[1][2];
      prodve(a, b, c);
      norme(c);
      jac[2][0] = c[0]; jac[2][1] = c[1]; jac[2][2] = c[2];
      break;
    }
  case 3:
    {
      dJ = (jac[0][0] * jac[1][1] * jac[2][2] + jac[0][2] * jac[1][0] * jac[2][1] +
	    jac[0][1] * jac[1][2] * jac[2][0] - jac[0][2] * jac[1][1] * jac[2][0] -
	    jac[0][0] * jac[1][2] * jac[2][1] - jac[0][1] * jac[1][0] * jac[2][2]);
      break;
    }
  }
  return dJ;
}

double MElement::getJacobian(double u, double v, double w, double jac[3][3])
{
  jac[0][0] = jac[0][1] = jac[0][2] = 0.;
  jac[1][0] = jac[1][1] = jac[1][2] = 0.;
  jac[2][0] = jac[2][1] = jac[2][2] = 0.;

  double gsf[1256][3];
  getGradShapeFunctions(u, v, w, gsf);
  for (int i = 0; i < getNumShapeFunctions(); i++) {
    const MVertex *ver = getShapeFunctionNode(i);
    double* gg = gsf[i];
    for (int j = 0; j < getDim(); j++) {
      jac[j][0] += ver->x() * gg[j];
      jac[j][1] += ver->y() * gg[j];
      jac[j][2] += ver->z() * gg[j];
    }
    //    printf("GSF (%d,%g %g) = %g %g \n",i,u,v,gg[0],gg[1]);
  }
  return _computeDeterminantAndRegularize(this, jac);
}

double MElement::getJacobian(const fullMatrix<double> &gsf, double jac[3][3])
{
  jac[0][0] = jac[0][1] = jac[0][2] = 0.;
  jac[1][0] = jac[1][1] = jac[1][2] = 0.;
  jac[2][0] = jac[2][1] = jac[2][2] = 0.;

  for (int i = 0; i < getNumShapeFunctions(); i++) {
    const MVertex *v = getShapeFunctionNode(i);
    for (int j = 0; j < gsf.size2(); j++) {
      jac[j][0] += v->x() * gsf(i, j);
      jac[j][1] += v->y() * gsf(i, j);
      jac[j][2] += v->z() * gsf(i, j);
    }
  }
  return _computeDeterminantAndRegularize(this, jac);
}

double MElement::getJacobian(const std::vector<SVector3> &gsf, double jac[3][3])
{
  jac[0][0] = jac[0][1] = jac[0][2] = 0.;
  jac[1][0] = jac[1][1] = jac[1][2] = 0.;
  jac[2][0] = jac[2][1] = jac[2][2] = 0.;

  for (int i = 0; i < getNumShapeFunctions(); i++) {
    const MVertex *v = getShapeFunctionNode(i);
    for (int j = 0; j < 3; j++) {
      double mult = gsf[i][j];
      jac[j][0] += v->x() * mult;
      jac[j][1] += v->y() * mult;
      jac[j][2] += v->z() * mult;
    }
  }
  return _computeDeterminantAndRegularize(this, jac);
}

double MElement::getPrimaryJacobian(double u, double v, double w, double jac[3][3])
{
  jac[0][0] = jac[0][1] = jac[0][2] = 0.;
  jac[1][0] = jac[1][1] = jac[1][2] = 0.;
  jac[2][0] = jac[2][1] = jac[2][2] = 0.;

  double gsf[1256][3];
  getGradShapeFunctions(u, v, w, gsf, 1);
  for(int i = 0; i < getNumPrimaryShapeFunctions(); i++) {
    const MVertex *v = getShapeFunctionNode(i);
    double* gg = gsf[i];
    for (int j = 0; j < 3; j++) {
      jac[j][0] += v->x() * gg[j];
      jac[j][1] += v->y() * gg[j];
      jac[j][2] += v->z() * gg[j];
    }
  }

  return _computeDeterminantAndRegularize(this, jac);
}

void MElement::getSignedJacobian(fullVector<double> &jacobian, int o)
{
  const int numNodes = getNumVertices();
  fullMatrix<double> nodesXYZ(numNodes,3);
  getNodesCoord(nodesXYZ);
  getJacobianFuncSpace(o)->getSignedJacobian(nodesXYZ,jacobian);
}

void MElement::getNodesCoord(fullMatrix<double> &nodesXYZ)
{
  const int numNodes = getNumShapeFunctions();
  for (int i = 0; i < numNodes; i++) {
    MVertex *v = getShapeFunctionNode(i);
    nodesXYZ(i,0) = v->x();
    nodesXYZ(i,1) = v->y();
    nodesXYZ(i,2) = v->z();
  }
}

void MElement::pnt(double u, double v, double w, SPoint3 &p)
{
  double x = 0., y = 0., z = 0.;
  double sf[1256];
  getShapeFunctions(u, v, w, sf);
  for (int j = 0; j < getNumShapeFunctions(); j++) {
    const MVertex *v = getShapeFunctionNode(j);
    x += sf[j] * v->x();
    y += sf[j] * v->y();
    z += sf[j] * v->z();
  }
  p = SPoint3(x, y, z);
}

void MElement::pnt(const std::vector<double> &sf, SPoint3 &p)
{
  double x = 0., y = 0., z = 0.;
  for (int j = 0; j < getNumShapeFunctions(); j++) {
    const MVertex *v = getShapeFunctionNode(j);
    x += sf[j] * v->x();
    y += sf[j] * v->y();
    z += sf[j] * v->z();
  }
  p = SPoint3(x, y, z);
}

void MElement::primaryPnt(double u, double v, double w, SPoint3 &p)
{
  double x = 0., y = 0., z = 0.;
  double sf[1256];
  getShapeFunctions(u, v, w, sf, 1);
  for (int j = 0; j < getNumPrimaryShapeFunctions(); j++) {
    const MVertex *v = getShapeFunctionNode(j);
    x += sf[j] * v->x();
    y += sf[j] * v->y();
    z += sf[j] * v->z();
  }
  p = SPoint3(x,y,z);
}

void MElement::xyz2uvw(double xyz[3], double uvw[3])
{
  // general Newton routine for the nonlinear case (more efficient
  // routines are implemented for simplices, where the basis functions
  // are linear)
  uvw[0] = uvw[1] = uvw[2] = 0.;
  int iter = 1, maxiter = 20;
  double error = 1., tol = 1.e-6;

  while (error > tol && iter < maxiter){
    double jac[3][3];
    if(!getJacobian(uvw[0], uvw[1], uvw[2], jac)) break;
    double xn = 0., yn = 0., zn = 0.;
    double sf[1256];
    getShapeFunctions(uvw[0], uvw[1], uvw[2], sf);
    for (int i = 0; i < getNumShapeFunctions(); i++) {
      MVertex *v = getShapeFunctionNode(i);
      xn += v->x() * sf[i];
      yn += v->y() * sf[i];
      zn += v->z() * sf[i];
    }
    double inv[3][3];
    inv3x3(jac, inv);
    double un = uvw[0] + inv[0][0] * (xyz[0] - xn) +
      inv[1][0] * (xyz[1] - yn) + inv[2][0] * (xyz[2] - zn);
    double vn = uvw[1] + inv[0][1] * (xyz[0] - xn) +
      inv[1][1] * (xyz[1] - yn) + inv[2][1] * (xyz[2] - zn);
    double wn = uvw[2] + inv[0][2] * (xyz[0] - xn) +
      inv[1][2] * (xyz[1] - yn) + inv[2][2] * (xyz[2] - zn);
    error = sqrt(SQU(un - uvw[0]) + SQU(vn - uvw[1]) + SQU(wn - uvw[2]));
    uvw[0] = un;
    uvw[1] = vn;
    uvw[2] = wn;
    iter++ ;
  }
}

void MElement::xyzTouvw(fullMatrix<double> *xu)
{
  double _xyz[3] = {(*xu)(0,0),(*xu)(0,1),(*xu)(0,2)}, _uvw[3];
  xyz2uvw(_xyz, _uvw);
  (*xu)(1,0) = _uvw[0];
  (*xu)(1,1) = _uvw[1];
  (*xu)(1,2) = _uvw[2];
}

void MElement::movePointFromParentSpaceToElementSpace(double &u, double &v, double &w)
{
  if(!getParent()) return;
  SPoint3 p;
  getParent()->pnt(u, v, w, p);
  double xyz[3] = {p.x(), p.y(), p.z()};
  double uvwE[3];
  xyz2uvw(xyz, uvwE);
  u = uvwE[0]; v = uvwE[1]; w = uvwE[2];
}

void MElement::movePointFromElementSpaceToParentSpace(double &u, double &v, double &w)
{
  if(!getParent()) return;
  SPoint3 p;
  pnt(u, v, w, p);
  double xyz[3] = {p.x(), p.y(), p.z()};
  double uvwP[3];
  getParent()->xyz2uvw(xyz, uvwP);
  u = uvwP[0]; v = uvwP[1]; w = uvwP[2];
}

double MElement::interpolate(double val[], double u, double v, double w, int stride,
                             int order)
{
  double sum = 0;
  int j = 0;
  double sf[1256];
  getShapeFunctions(u, v, w, sf, order);
  for(int i = 0; i < getNumShapeFunctions(); i++){
    sum += val[j] * sf[i];
    j += stride;
  }
  return sum;
}

void MElement::interpolateGrad(double val[], double u, double v, double w, double f[],
                               int stride, double invjac[3][3], int order)
{
  double dfdu[3] = {0., 0., 0.};
  int j = 0;
  double gsf[1256][3];
  getGradShapeFunctions(u, v, w, gsf, order);
  for(int i = 0; i < getNumShapeFunctions(); i++){
    dfdu[0] += val[j] * gsf[i][0];
    dfdu[1] += val[j] * gsf[i][1];
    dfdu[2] += val[j] * gsf[i][2];
    j += stride;
  }
  if(invjac){
    matvec(invjac, dfdu, f);
  }
  else{
    double jac[3][3], inv[3][3];
    getJacobian(u, v, w, jac);
    inv3x3(jac, inv);
    matvec(inv, dfdu, f);
  }
}

void MElement::interpolateCurl(double val[], double u, double v, double w, double f[],
                               int stride, int order)
{
  double fx[3], fy[3], fz[3], jac[3][3], inv[3][3];
  getJacobian(u, v, w, jac);
  inv3x3(jac, inv);
  interpolateGrad(&val[0], u, v, w, fx, stride, inv, order);
  interpolateGrad(&val[1], u, v, w, fy, stride, inv, order);
  interpolateGrad(&val[2], u, v, w, fz, stride, inv, order);
  f[0] = fz[1] - fy[2];
  f[1] = -(fz[0] - fx[2]);
  f[2] = fy[0] - fx[1];
}

double MElement::interpolateDiv(double val[], double u, double v, double w,
                                int stride, int order)
{
  double fx[3], fy[3], fz[3], jac[3][3], inv[3][3];
  getJacobian(u, v, w, jac);
  inv3x3(jac, inv);
  interpolateGrad(&val[0], u, v, w, fx, stride, inv, order);
  interpolateGrad(&val[1], u, v, w, fy, stride, inv, order);
  interpolateGrad(&val[2], u, v, w, fz, stride, inv, order);
  return fx[0] + fy[1] + fz[2];
}

double MElement::integrate(double val[], int pOrder, int stride, int order)
{
  int npts; IntPt *gp;
  getIntegrationPoints(pOrder, &npts, &gp);
  double sum = 0;
  for (int i = 0; i < npts; i++){
    double u = gp[i].pt[0];
    double v = gp[i].pt[1];
    double w = gp[i].pt[2];
    double weight = gp[i].weight;
    double detuvw = getJacobianDeterminant(u, v, w);
    sum += interpolate(val, u, v, w, stride, order)*weight*detuvw;
  }
  return sum;
}

static int getTriangleType (int order)
{
  switch(order) {
  case 0 : return MSH_TRI_1;
  case 1 : return MSH_TRI_3;
  case 2 : return MSH_TRI_6;
  case 3 : return MSH_TRI_10;
  case 4 : return MSH_TRI_15;
  case 5 : return MSH_TRI_21;
  case 6 : return MSH_TRI_28;
  case 7 : return MSH_TRI_36;
  case 8 : return MSH_TRI_45;
  case 9 : return MSH_TRI_55;
  case 10 : return MSH_TRI_66;
  default : Msg::Error("triangle order %i unknown", order); return 0;
  }
}

static int getQuadType (int order)
{
  switch(order) {
  case 0 : return MSH_QUA_1;
  case 1 : return MSH_QUA_4;
  case 2 : return MSH_QUA_9;
  case 3 : return MSH_QUA_16;
  case 4 : return MSH_QUA_25;
  case 5 : return MSH_QUA_36;
  case 6 : return MSH_QUA_49;
  case 7 : return MSH_QUA_64;
  case 8 : return MSH_QUA_81;
  case 9 : return MSH_QUA_100;
  case 10 : return MSH_QUA_121;
  default : Msg::Error("quad order %i unknown", order); return 0;
  }
}

static int getLineType (int order)
{
  switch(order) {
  case 0 : return MSH_LIN_1;
  case 1 : return MSH_LIN_2;
  case 2 : return MSH_LIN_3;
  case 3 : return MSH_LIN_4;
  case 4 : return MSH_LIN_5;
  case 5 : return MSH_LIN_6;
  case 6 : return MSH_LIN_7;
  case 7 : return MSH_LIN_8;
  case 8 : return MSH_LIN_9;
  case 9 : return MSH_LIN_10;
  case 10 : return MSH_LIN_11;
  default : Msg::Error("line order %i unknown", order); return 0;
  }
}

double MElement::integrateCirc(double val[], int edge, int pOrder, int order)
{
  if(edge > getNumEdges() - 1){
    Msg::Error("No edge %d for this element", edge);
    return 0;
  }

  std::vector<MVertex*> v;
  getEdgeVertices(edge, v);
  MElementFactory f;
  int type = getLineType(getPolynomialOrder());
  MElement* ee = f.create(type, v);

  double intv[3];
  for(int i = 0; i < 3; i++){
    intv[i] = ee->integrate(&val[i], pOrder, 3, order);
  }
  delete ee;

  double t[3] = {v[1]->x() - v[0]->x(), v[1]->y() - v[0]->y(), v[1]->z() - v[0]->z()};
  norme(t);
  double result;
  prosca(t, intv, &result);
  return result;
}

double MElement::integrateFlux(double val[], int face, int pOrder, int order)
{
  if(face > getNumFaces() - 1){
    Msg::Error("No face %d for this element", face);
    return 0;
  }
  std::vector<MVertex*> v;
  getFaceVertices(face, v);
  MElementFactory f;
  int type = 0;
  switch(getType()) {
  case TYPE_TRI : type = getTriangleType(getPolynomialOrder()); break;
  case TYPE_TET : type = getTriangleType(getPolynomialOrder()); break;
  case TYPE_QUA : type = getQuadType(getPolynomialOrder());     break;
  case TYPE_HEX : type = getQuadType(getPolynomialOrder());     break;
  case TYPE_PYR :
    if(face < 4) type = getTriangleType(getPolynomialOrder());
    else type = getQuadType(getPolynomialOrder());
    break;
  case TYPE_PRI :
    if(face < 2) type = getTriangleType(getPolynomialOrder());
    else type = getQuadType(getPolynomialOrder());
    break;
  default: type = 0; break;
  }
  MElement* fe = f.create(type, v);

  double intv[3];
  for(int i = 0; i < 3; i++){
    intv[i] = fe->integrate(&val[i], pOrder, 3, order);
  }
  delete fe;

  double n[3];
  normal3points(v[0]->x(), v[0]->y(), v[0]->z(),
                v[1]->x(), v[1]->y(), v[1]->z(),
                v[2]->x(), v[2]->y(), v[2]->z(), n);
  double result;
  prosca(n, intv, &result);
  return result;
}

void MElement::writeMSH(FILE *fp, bool binary, int entity,
                        std::vector<short> *ghosts)
{
  int num = getNum();
  int type = getTypeForMSH();
  if(!type) return;

  // if necessary, change the ordering of the vertices to get positive volume
  setVolumePositive();

  std::vector<int> verts;
  getVerticesIdForMSH(verts);

  // FIXME: once we create elements using their own interpretion of data, we
  // should move this also into each element base class
  std::vector<int> data;
  data.insert(data.end(), verts.begin(), verts.end());
  if(getParent())
    data.push_back(getParent()->getNum());
  if(getPartition()){
    if(ghosts){
      data.push_back(1 + ghosts->size());
      data.push_back(getPartition());
      data.insert(data.end(), ghosts->begin(), ghosts->end());
    }
    else{
      data.push_back(1);
      data.push_back(getPartition());
    }
  }
  int numData = data.size();

  if(!binary){
    fprintf(fp, "%d %d %d %d", num, type, entity, numData);
    for(int i = 0; i < numData; i++)
      fprintf(fp, " %d", data[i]);
    fprintf(fp, "\n");
  }
  else{
    fwrite(&num, sizeof(int), 1, fp);
    fwrite(&type, sizeof(int), 1, fp);
    fwrite(&entity, sizeof(int), 1, fp);
    fwrite(&numData, sizeof(int), 1, fp);
    fwrite(&data[0], sizeof(int), numData, fp);
  }
}

void MElement::writeMSH2(FILE *fp, double version, bool binary, int num,
                         int elementary, int physical, int parentNum,
                         int dom1Num, int dom2Num, std::vector<short> *ghosts)
{
  int type = getTypeForMSH();

  if(!type) return;

  // if necessary, change the ordering of the vertices to get positive
  // volume
  setVolumePositive();
  int n = getNumVerticesForMSH();
  int par = (parentNum) ? 1 : 0;
  int dom = (dom1Num) ? 2 : 0;
  bool poly = (type == MSH_POLYG_ || type == MSH_POLYH_ || type == MSH_POLYG_B);

  // if polygon loop over children (triangles and tets)
  if(CTX::instance()->mesh.saveTri){
    if(poly){
      for (int i = 0; i < getNumChildren() ; i++){
         MElement *t = getChild(i);
         t->writeMSH2(fp, version, binary, num++, elementary, physical, 0, 0, 0, ghosts);
      }
      return;
    }
    if(type == MSH_TRI_B){
      MTriangle *t = new MTriangle(getVertex(0), getVertex(1), getVertex(2));
      t->writeMSH2(fp, version, binary, num++, elementary, physical, 0, 0, 0, ghosts);
      delete t;
      return;
    }
    if(type == MSH_LIN_B || type == MSH_LIN_C){
      MLine *l = new MLine(getVertex(0), getVertex(1));
      l->writeMSH2(fp, version, binary, num++, elementary, physical, 0, 0, 0, ghosts);
      delete l;
      return;
    }
  }

  if(!binary){
    fprintf(fp, "%d %d", num ? num : _num, type);
    if(version < 2.0)
      fprintf(fp, " %d %d %d", abs(physical), elementary, n);
    else if (version < 2.2)
      fprintf(fp, " %d %d %d", abs(physical), elementary, _partition);
    else if(!_partition && !par && !dom)
      fprintf(fp, " %d %d %d", 2 + par + dom, abs(physical), elementary);
    else if(!ghosts)
      fprintf(fp, " %d %d %d 1 %d", 4 + par + dom, abs(physical), elementary, _partition);
    else{
      int numGhosts = ghosts->size();
      fprintf(fp, " %d %d %d %d %d", 4 + numGhosts + par + dom, abs(physical),
              elementary, 1 + numGhosts, _partition);
      for(unsigned int i = 0; i < ghosts->size(); i++)
        fprintf(fp, " %d", -(*ghosts)[i]);
    }
    if(version >= 2.0 && par)
      fprintf(fp, " %d", parentNum);
    if(version >= 2.0 && dom)
      fprintf(fp, " %d %d", dom1Num, dom2Num);
    if(version >= 2.0 && poly)
      fprintf(fp, " %d", n);
  }
  else{
    int numTags, numGhosts = 0;
    if(!_partition) numTags = 2;
    else if(!ghosts) numTags = 4;
    else{
      numGhosts = ghosts->size();
      numTags = 4 + numGhosts;
    }
    numTags += par;
    // we write elements in blobs of single elements; this will lead
    // to suboptimal reads, but it's much simpler when the number of
    // tags change from element to element (third-party codes can
    // still write MSH file optimized for reading speed, by grouping
    // elements with the same number of tags in blobs)
    int blob[60] = {type, 1, numTags, num ? num : _num, abs(physical), elementary,
                    1 + numGhosts, _partition};
    if(ghosts)
      for(int i = 0; i < numGhosts; i++) blob[8 + i] = -(*ghosts)[i];
    if(par) blob[8 + numGhosts] = parentNum;
    if(poly) Msg::Error("Unable to write polygons/polyhedra in binary files.");
    fwrite(blob, sizeof(int), 4 + numTags, fp);
  }

  if(physical < 0) revert();

  std::vector<int> verts;
  getVerticesIdForMSH(verts);

  if(!binary){
    for(int i = 0; i < n; i++)
      fprintf(fp, " %d", verts[i]);
    fprintf(fp, "\n");
  }
  else{
    fwrite(&verts[0], sizeof(int), n, fp);
  }

  if(physical < 0) revert();
}

void MElement::writePOS(FILE *fp, bool printElementary, bool printElementNumber,
                        bool printGamma, bool printEta, bool printRho,
                        bool printDisto, double scalingFactor, int elementary)
{
  const char *str = getStringForPOS();
  if(!str) return;

  setVolumePositive();
  int n = getNumVertices();
  fprintf(fp, "%s(", str);
  for(int i = 0; i < n; i++){
    if(i) fprintf(fp, ",");
    fprintf(fp, "%g,%g,%g", getVertex(i)->x() * scalingFactor,
            getVertex(i)->y() * scalingFactor, getVertex(i)->z() * scalingFactor);
  }
  fprintf(fp, "){");
  bool first = true;
  if(printElementary){
    for(int i = 0; i < n; i++){
      if(first) first = false; else fprintf(fp, ",");
      fprintf(fp, "%d", elementary);
    }
  }
  if(printElementNumber){
    for(int i = 0; i < n; i++){
      if(first) first = false; else fprintf(fp, ",");
      fprintf(fp, "%d", getNum());
    }
  }
  if(printGamma){
    double gamma = gammaShapeMeasure();
    for(int i = 0; i < n; i++){
      if(first) first = false; else fprintf(fp, ",");
      fprintf(fp, "%g", gamma);
    }
  }
  if(printEta){
    double eta = etaShapeMeasure();
    for(int i = 0; i < n; i++){
      if(first) first = false; else fprintf(fp, ",");
      fprintf(fp, "%g", eta);
    }
  }
  if(printRho){
    double rho = rhoShapeMeasure();
    for(int i = 0; i < n; i++){
      if(first) first = false; else fprintf(fp, ",");
      fprintf(fp, "%g", rho);
    }
  }
  if(printDisto){
    double disto = distoShapeMeasure();
    for(int i = 0; i < n; i++){
      if(first) first = false; else fprintf(fp, ",");
      fprintf(fp, "%g", disto);
    }
  }
  fprintf(fp, "};\n");
}

void MElement::writeSTL(FILE *fp, bool binary, double scalingFactor)
{
  if(getType() != TYPE_TRI && getType() != TYPE_QUA) return;
  int qid[3] = {0, 2, 3};
  SVector3 n = getFace(0).normal();
  if(!binary){
    fprintf(fp, "facet normal %g %g %g\n", n[0], n[1], n[2]);
    fprintf(fp, "  outer loop\n");
    for(int j = 0; j < 3; j++)
      fprintf(fp, "    vertex %g %g %g\n",
              getVertex(j)->x() * scalingFactor,
              getVertex(j)->y() * scalingFactor,
              getVertex(j)->z() * scalingFactor);
    fprintf(fp, "  endloop\n");
    fprintf(fp, "endfacet\n");
    if(getNumVertices() == 4){
      fprintf(fp, "facet normal %g %g %g\n", n[0], n[1], n[2]);
      fprintf(fp, "  outer loop\n");
      for(int j = 0; j < 3; j++)
        fprintf(fp, "    vertex %g %g %g\n",
                getVertex(qid[j])->x() * scalingFactor,
                getVertex(qid[j])->y() * scalingFactor,
                getVertex(qid[j])->z() * scalingFactor);
      fprintf(fp, "  endloop\n");
      fprintf(fp, "endfacet\n");
    }
  }
  else{
    char data[50];
    float *coords = (float*)data;
    coords[0] = (float)n[0];
    coords[1] = (float)n[1];
    coords[2] = (float)n[2];
    for(int j = 0; j < 3; j++){
      coords[3 + 3 * j] = (float)(getVertex(j)->x() * scalingFactor);
      coords[3 + 3 * j + 1] = (float)(getVertex(j)->y() * scalingFactor);
      coords[3 + 3 * j + 2] = (float)(getVertex(j)->z() * scalingFactor);
    }
    data[48] = data[49] = 0;
    fwrite(data, sizeof(char), 50, fp);
    if(getNumVertices() == 4){
      for(int j = 0; j < 3; j++){
        coords[3 + 3 * j] = (float)(getVertex(qid[j])->x() * scalingFactor);
        coords[3 + 3 * j + 1] = (float)(getVertex(qid[j])->y() * scalingFactor);
        coords[3 + 3 * j + 2] = (float)(getVertex(qid[j])->z() * scalingFactor);
      }
      fwrite(data, sizeof(char), 50, fp);
    }
  }
}

void MElement::writePLY2(FILE *fp)
{
  setVolumePositive();
  fprintf(fp, "3 ");
  for(int i = 0; i < getNumVertices(); i++)
    fprintf(fp, " %d", getVertex(i)->getIndex() - 1);
  fprintf(fp, "\n");
}

void MElement::writeVRML(FILE *fp)
{
  setVolumePositive();
  for(int i = 0; i < getNumVertices(); i++)
    fprintf(fp, "%d,", getVertex(i)->getIndex() - 1);
  fprintf(fp, "-1,\n");
}

void MElement::writeVTK(FILE *fp, bool binary, bool bigEndian)
{
  if(!getTypeForVTK()) return;

  setVolumePositive();

  int n = getNumVertices();
  if(binary){
    int verts[60];
    verts[0] = n;
    for(int i = 0; i < n; i++)
      verts[i + 1] = getVertexVTK(i)->getIndex() - 1;
    // VTK always expects big endian binary data
    if(!bigEndian) SwapBytes((char*)verts, sizeof(int), n + 1);
    fwrite(verts, sizeof(int), n + 1, fp);
  }
  else{
    fprintf(fp, "%d", n);
    for(int i = 0; i < n; i++)
      fprintf(fp, " %d", getVertexVTK(i)->getIndex() - 1);
    fprintf(fp, "\n");
  }
}

void MElement::writeUNV(FILE *fp, int num, int elementary, int physical)
{
  int type = getTypeForUNV();
  if(!type) return;

  setVolumePositive();
  int n = getNumVertices();
  int physical_property = elementary;
  int material_property = abs(physical);
  int color = 7;
  fprintf(fp, "%10d%10d%10d%10d%10d%10d\n",
          num ? num : _num, type, physical_property, material_property, color, n);
  if(type == 21 || type == 24) // linear beam or parabolic beam
    fprintf(fp, "%10d%10d%10d\n", 0, 0, 0);

  if(physical < 0) revert();

  for(int k = 0; k < n; k++) {
    fprintf(fp, "%10d", getVertexUNV(k)->getIndex());
    if(k % 8 == 7)
      fprintf(fp, "\n");
  }
  if(n - 1 % 8 != 7)
    fprintf(fp, "\n");

  if(physical < 0) revert();
}

void MElement::writeMESH(FILE *fp, int elementTagType, int elementary,
                         int physical)
{
  setVolumePositive();
  for(int i = 0; i < getNumVertices(); i++)
    if (getTypeForMSH() == MSH_TET_10 && i == 8)
      fprintf(fp, " %d", getVertex(9)->getIndex());
    else if (getTypeForMSH() == MSH_TET_10 && i == 9)
      fprintf(fp, " %d", getVertex(8)->getIndex());
    else
      fprintf(fp, " %d", getVertex(i)->getIndex());
  fprintf(fp, " %d\n", (elementTagType == 3) ? _partition :
          (elementTagType == 2) ? physical : elementary);
}

void MElement::writeIR3(FILE *fp, int elementTagType, int num, int elementary,
                        int physical)
{
  int numVert = getNumVertices();
  bool ok = setVolumePositive();
  if(getDim() == 3 && !ok) Msg::Error("Element %d has zero volume", num);
  fprintf(fp, "%d %d %d", num, (elementTagType == 3) ? _partition :
          (elementTagType == 2) ? physical : elementary, numVert);
  for(int i = 0; i < numVert; i++)
    fprintf(fp, " %d", getVertex(i)->getIndex());
  fprintf(fp, "\n");
}

void MElement::writeBDF(FILE *fp, int format, int elementTagType, int elementary,
                        int physical)
{
  const char *str = getStringForBDF();
  if(!str) return;

  setVolumePositive();
  int n = getNumVertices();
  const char *cont[4] = {"E", "F", "G", "H"};
  int ncont = 0;

  int tag =  (elementTagType == 3) ? _partition : (elementTagType == 2) ?
    physical : elementary;

  if(format == 0){ // free field format
    fprintf(fp, "%s,%d,%d", str, _num, tag);
    for(int i = 0; i < n; i++){
      fprintf(fp, ",%d", getVertexBDF(i)->getIndex());
      if(i != n - 1 && !((i + 3) % 8)){
        fprintf(fp, ",+%s%d\n+%s%d", cont[ncont], _num, cont[ncont], _num);
        ncont++;
      }
    }
    if(n == 2) // CBAR
      fprintf(fp, ",0.,0.,0.");
    fprintf(fp, "\n");
  }
  else{ // small or large field format
    fprintf(fp, "%-8s%-8d%-8d", str, _num, tag);
    for(int i = 0; i < n; i++){
      fprintf(fp, "%-8d", getVertexBDF(i)->getIndex());
      if(i != n - 1 && !((i + 3) % 8)){
        fprintf(fp, "+%s%-6d\n+%s%-6d", cont[ncont], _num, cont[ncont], _num);
        ncont++;
      }
    }
    if(n == 2) // CBAR
      fprintf(fp, "%-8s%-8s%-8s", "0.", "0.", "0.");
    fprintf(fp, "\n");
  }
}

void MElement::writeDIFF(FILE *fp, int num, bool binary, int physical_property)
{
  const char *str = getStringForDIFF();
  if(!str) return;

  setVolumePositive();

  int n = getNumVertices();
  if(binary){
    // TODO
  }
  else{
    fprintf(fp, "%d %s %d ", num, str, physical_property);
    for(int i = 0; i < n; i++)
      fprintf(fp, " %d", getVertexDIFF(i)->getIndex());
    fprintf(fp, "\n");
  }
}

void MElement::writeINP(FILE *fp, int num)
{
  setVolumePositive();
  fprintf(fp, "%d", num);
  for(int i = 0; i < getNumVertices(); i++)
    fprintf(fp, ", %d", getVertexINP(i)->getIndex());
  fprintf(fp, "\n");
}

int MElement::getInfoMSH(const int typeMSH, const char **const name)
{
  switch(typeMSH){
  case MSH_PNT     : if(name) *name = "Point";            return 1;
  case MSH_LIN_2   : if(name) *name = "Line 2";           return 2;
  case MSH_LIN_3   : if(name) *name = "Line 3";           return 2 + 1;
  case MSH_LIN_4   : if(name) *name = "Line 4";           return 2 + 2;
  case MSH_LIN_5   : if(name) *name = "Line 5";           return 2 + 3;
  case MSH_LIN_6   : if(name) *name = "Line 6";           return 2 + 4;
  case MSH_LIN_7   : if(name) *name = "Line 7";           return 2 + 5;
  case MSH_LIN_8   : if(name) *name = "Line 8";           return 2 + 6;
  case MSH_LIN_9   : if(name) *name = "Line 9";           return 2 + 7;
  case MSH_LIN_10  : if(name) *name = "Line 10";          return 2 + 8;
  case MSH_LIN_11  : if(name) *name = "Line 11";          return 2 + 9;
  case MSH_LIN_B   : if(name) *name = "Line Border";      return 2;
  case MSH_LIN_C   : if(name) *name = "Line Child";       return 2;
  case MSH_TRI_3   : if(name) *name = "Triangle 3";       return 3;
  case MSH_TRI_6   : if(name) *name = "Triangle 6";       return 3 + 3;
  case MSH_TRI_9   : if(name) *name = "Triangle 9";       return 3 + 6;
  case MSH_TRI_10  : if(name) *name = "Triangle 10";      return 3 + 6 + 1;
  case MSH_TRI_12  : if(name) *name = "Triangle 12";      return 3 + 9;
  case MSH_TRI_15  : if(name) *name = "Triangle 15";      return 3 + 9 + 3;
  case MSH_TRI_15I : if(name) *name = "Triangle 15I";     return 3 + 12;
  case MSH_TRI_21  : if(name) *name = "Triangle 21";      return 3 + 12 + 6;
  case MSH_TRI_28  : if(name) *name = "Triangle 28";      return 3 + 15 + 10;
  case MSH_TRI_36  : if(name) *name = "Triangle 36";      return 3 + 18 + 15;
  case MSH_TRI_45  : if(name) *name = "Triangle 45";      return 3 + 21 + 21;
  case MSH_TRI_55  : if(name) *name = "Triangle 55";      return 3 + 24 + 28;
  case MSH_TRI_66  : if(name) *name = "Triangle 66";      return 3 + 27 + 36;
  case MSH_TRI_18  : if(name) *name = "Triangle 18";      return 3 + 15;
  case MSH_TRI_21I : if(name) *name = "Triangle 21I";     return 3 + 18;
  case MSH_TRI_24  : if(name) *name = "Triangle 24";      return 3 + 21;
  case MSH_TRI_27  : if(name) *name = "Triangle 27";      return 3 + 24;
  case MSH_TRI_30  : if(name) *name = "Triangle 30";      return 3 + 27;
  case MSH_TRI_B   : if(name) *name = "Triangle Border";  return 3;
  case MSH_QUA_4   : if(name) *name = "Quadrilateral 4";  return 4;
  case MSH_QUA_8   : if(name) *name = "Quadrilateral 8";  return 4 + 4;
  case MSH_QUA_9   : if(name) *name = "Quadrilateral 9";  return 9;
  case MSH_QUA_16  : if(name) *name = "Quadrilateral 16"; return 16;
  case MSH_QUA_25  : if(name) *name = "Quadrilateral 25"; return 25;
  case MSH_QUA_36  : if(name) *name = "Quadrilateral 36"; return 36;
  case MSH_QUA_49  : if(name) *name = "Quadrilateral 49"; return 49;
  case MSH_QUA_64  : if(name) *name = "Quadrilateral 64"; return 64;
  case MSH_QUA_81  : if(name) *name = "Quadrilateral 81"; return 81;
  case MSH_QUA_100 : if(name) *name = "Quadrilateral 100";return 100;
  case MSH_QUA_121 : if(name) *name = "Quadrilateral 121";return 121;
  case MSH_QUA_12  : if(name) *name = "Quadrilateral 12"; return 12;
  case MSH_QUA_16I : if(name) *name = "Quadrilateral 16I";return 16;
  case MSH_QUA_20  : if(name) *name = "Quadrilateral 20"; return 20;
  case MSH_POLYG_  : if(name) *name = "Polygon";          return 0;
  case MSH_POLYG_B : if(name) *name = "Polygon Border";   return 0;
  case MSH_TET_4   : if(name) *name = "Tetrahedron 4";    return 4;
  case MSH_TET_10  : if(name) *name = "Tetrahedron 10";   return 4 + 6;
  case MSH_TET_20  : if(name) *name = "Tetrahedron 20";   return 4 + 12 + 4;
  case MSH_TET_34  : if(name) *name = "Tetrahedron 34";   return 4 + 18 + 12 + 0;
  case MSH_TET_35  : if(name) *name = "Tetrahedron 35";   return 4 + 18 + 12 + 1;
  case MSH_TET_52  : if(name) *name = "Tetrahedron 52";   return 4 + 24 + 24 + 0;
  case MSH_TET_56  : if(name) *name = "Tetrahedron 56";   return 4 + 24 + 24 + 4;
  case MSH_TET_84  : if(name) *name = "Tetrahedron 84";   return (7*8*9)/6;
  case MSH_TET_120 : if(name) *name = "Tetrahedron 120";  return (8*9*10)/6;
  case MSH_TET_165 : if(name) *name = "Tetrahedron 165";  return (9*10*11)/6;
  case MSH_TET_220 : if(name) *name = "Tetrahedron 220";  return (10*11*12)/6;
  case MSH_TET_286 : if(name) *name = "Tetrahedron 286";  return (11*12*13)/6;
  case MSH_HEX_8   : if(name) *name = "Hexahedron 8";     return 8;
  case MSH_HEX_20  : if(name) *name = "Hexahedron 20";    return 8 + 12;
  case MSH_HEX_27  : if(name) *name = "Hexahedron 27";    return 8 + 12 + 6 + 1;
  case MSH_HEX_64  : if(name) *name = "Hexahedron 64";    return 64;
  case MSH_HEX_125 : if(name) *name = "Hexahedron 125";   return 125;
  case MSH_HEX_216 : if(name) *name = "Hexahedron 216";   return 216;
  case MSH_HEX_343 : if(name) *name = "Hexahedron 343";   return 343;
  case MSH_HEX_512 : if(name) *name = "Hexahedron 512";   return 512;
  case MSH_HEX_729 : if(name) *name = "Hexahedron 729";   return 729;
  case MSH_HEX_1000: if(name) *name = "Hexahedron 1000";  return 1000;
  case MSH_HEX_56  : if(name) *name = "Hexahedron 56";    return 56;
  case MSH_HEX_98  : if(name) *name = "Hexahedron 98";    return 98;
  case MSH_HEX_152 : if(name) *name = "Hexahedron 152";   return 152;
  case MSH_HEX_218 : if(name) *name = "Hexahedron 218";   return 218;
  case MSH_HEX_296 : if(name) *name = "Hexahedron 296";   return 296;
  case MSH_HEX_386 : if(name) *name = "Hexahedron 386";   return 386;
  case MSH_HEX_488 : if(name) *name = "Hexahedron 488";   return 488;
  case MSH_PRI_6   : if(name) *name = "Prism 6";          return 6;
  case MSH_PRI_15  : if(name) *name = "Prism 15";         return 6 + 9;
  case MSH_PRI_18  : if(name) *name = "Prism 18";         return 6 + 9 + 3;
  case MSH_PYR_5   : if(name) *name = "Pyramid 5";        return 5;
  case MSH_PYR_13  : if(name) *name = "Pyramid 13";       return 5 + 8;
  case MSH_PYR_14  : if(name) *name = "Pyramid 14";       return 5 + 8 + 1;
  case MSH_PYR_30  : if(name) *name = "Pyramid 30";       return 5 + 8*2 + 4*1 + 1*4 + 1;
  case MSH_PYR_55  : if(name) *name = "Pyramid 55";       return 5 + 8*3 + 4*3 + 1*9 + 5;
  case MSH_PYR_91  : if(name) *name = "Pyramid 91";       return 91;
  case MSH_PYR_140 : if(name) *name = "Pyramid 140";      return 140;
  case MSH_PYR_204 : if(name) *name = "Pyramid 204";      return 204;
  case MSH_PYR_285 : if(name) *name = "Pyramid 285";      return 285;
  case MSH_PYR_385 : if(name) *name = "Pyramid 385";      return 385;
  case MSH_POLYH_  : if(name) *name = "Polyhedron";       return 0;
  case MSH_PNT_SUB : if(name) *name = "Point Xfem";       return 1;
  case MSH_LIN_SUB : if(name) *name = "Line Xfem";        return 2;
  case MSH_TRI_SUB : if(name) *name = "Triangle Xfem";    return 3;
  case MSH_TET_SUB : if(name) *name = "Tetrahedron Xfem"; return 4;
  default:
    Msg::Error("Unknown type of element %d", typeMSH);
    if(name) *name = "Unknown";
    return 0;
  }
}

void MElement::getVerticesIdForMSH(std::vector<int> &verts)
{
  int n = getNumVerticesForMSH();
  verts.resize(n);
  for(int i = 0; i < n; i++)
    verts[i] = getVertex(i)->getIndex();
}

MElement *MElement::copy(std::map<int, MVertex*> &vertexMap,
                         std::map<MElement*, MElement*> &newParents,
                         std::map<MElement*, MElement*> &newDomains)
{
  if(newDomains.count(this))
    return newDomains.find(this)->second;
  std::vector<MVertex*> vmv;
  int eType = getTypeForMSH();
  MElement *eParent = getParent();
  if(getNumChildren() == 0) {
    for(int i = 0; i < getNumVertices(); i++) {
      MVertex *v = getVertex(i);
      int numV = v->getNum(); //Index();
      if(vertexMap.count(numV))
        vmv.push_back(vertexMap[numV]);
      else {
        MVertex *mv = new MVertex(v->x(), v->y(), v->z(), 0, numV);
        vmv.push_back(mv);
        vertexMap[numV] = mv;
      }
    }
  }
  else {
    for(int i = 0; i < getNumChildren(); i++) {
      for(int j = 0; j < getChild(i)->getNumVertices(); j++) {
        MVertex *v = getChild(i)->getVertex(j);
        int numV = v->getNum(); //Index();
        if(vertexMap.count(numV))
          vmv.push_back(vertexMap[numV]);
        else {
          MVertex *mv = new MVertex(v->x(), v->y(), v->z(), 0, numV);
          vmv.push_back(mv);
          vertexMap[numV] = mv;
        }
      }
    }
  }

  MElement *parent=0;
  if(eParent && !getDomain(0) && !getDomain(1)) {
    std::map<MElement*, MElement*>::iterator it = newParents.find(eParent);
    MElement *newParent;
    if(it == newParents.end()) {
      newParent = eParent->copy(vertexMap, newParents, newDomains);
      newParents[eParent] = newParent;
    }
    else
      newParent = it->second;
    parent = newParent;
  }

  MElementFactory f;
  MElement *newEl = f.create(eType, vmv, getNum(), _partition, ownsParent(), parent);

  for(int i = 0; i < 2; i++) {
    MElement *dom = getDomain(i);
    if(!dom) continue;
    std::map<MElement*, MElement*>::iterator it = newDomains.find(dom);
    MElement *newDom;
    if(it == newDomains.end()) {
      newDom = dom->copy(vertexMap, newParents, newDomains);
      newDomains[dom] = newDom;
    }
    else
      newDom = newDomains.find(dom)->second;
    newEl->setDomain(newDom, i);
  }
  return newEl;
}

// Gives the parent type corresponding to
// any element type.
// Ex. : MSH_TRI_3 -> TYPE_TRI
int MElement::ParentTypeFromTag(int tag)
{
  switch(tag) {
    case(MSH_PNT):
      return TYPE_PNT;
    case(MSH_LIN_2):    case(MSH_LIN_3):
    case(MSH_LIN_4):    case(MSH_LIN_5):
    case(MSH_LIN_6):    case(MSH_LIN_7):
    case(MSH_LIN_8):    case(MSH_LIN_9):
    case(MSH_LIN_10):   case(MSH_LIN_11):
    case(MSH_LIN_B):    case(MSH_LIN_C):
    case(MSH_LIN_1):
      return TYPE_LIN;
    case(MSH_TRI_3):    case(MSH_TRI_6):
    case(MSH_TRI_9):    case(MSH_TRI_10):
    case(MSH_TRI_12):   case(MSH_TRI_15):
    case(MSH_TRI_15I):  case(MSH_TRI_21):
    case(MSH_TRI_28):   case(MSH_TRI_36):
    case(MSH_TRI_45):   case(MSH_TRI_55):
    case(MSH_TRI_66):   case(MSH_TRI_18):
    case(MSH_TRI_21I):  case(MSH_TRI_24):
    case(MSH_TRI_27):   case(MSH_TRI_30):
    case(MSH_TRI_B):    case(MSH_TRI_1):
      return TYPE_TRI;
    case(MSH_QUA_4):    case(MSH_QUA_9):
    case(MSH_QUA_8):    case(MSH_QUA_16):
    case(MSH_QUA_25):   case(MSH_QUA_36):
    case(MSH_QUA_12):   case(MSH_QUA_16I):
    case(MSH_QUA_20):   case(MSH_QUA_49):
    case(MSH_QUA_64):   case(MSH_QUA_81):
    case(MSH_QUA_100):  case(MSH_QUA_121):
    case(MSH_QUA_24):   case(MSH_QUA_28):
    case(MSH_QUA_32):   case(MSH_QUA_36I):
    case(MSH_QUA_40):   case(MSH_QUA_1):
      return TYPE_QUA;
    case(MSH_TET_4):    case(MSH_TET_10):
    case(MSH_TET_20):   case(MSH_TET_35):
    case(MSH_TET_56):   case(MSH_TET_34):
    case(MSH_TET_52):   case(MSH_TET_84):
    case(MSH_TET_120):  case(MSH_TET_165):
    case(MSH_TET_220):  case(MSH_TET_286):
    case(MSH_TET_74):   case(MSH_TET_100):
    case(MSH_TET_130):  case(MSH_TET_164):
    case(MSH_TET_202):  case(MSH_TET_1):
      return TYPE_TET;
    case(MSH_PYR_5):    case(MSH_PYR_14):
    case(MSH_PYR_13):   case(MSH_PYR_30):
    case(MSH_PYR_55):   case(MSH_PYR_91):
    case(MSH_PYR_140):  case(MSH_PYR_204):
    case(MSH_PYR_285):  case(MSH_PYR_385):
    case(MSH_PYR_29):   case(MSH_PYR_50):
    case(MSH_PYR_77):   case(MSH_PYR_110):
    case(MSH_PYR_149):  case(MSH_PYR_194):
    case(MSH_PYR_245):  case(MSH_PYR_1):
      return TYPE_PYR;
    case(MSH_PRI_6):    case(MSH_PRI_18):
    case(MSH_PRI_15):   case(MSH_PRI_1):
    case(MSH_PRI_40):   case(MSH_PRI_75):
    case(MSH_PRI_126):  case(MSH_PRI_196):
    case(MSH_PRI_288):  case(MSH_PRI_405):
    case(MSH_PRI_550):  case(MSH_PRI_38):
    case(MSH_PRI_66):   case(MSH_PRI_102):
    case(MSH_PRI_146):  case(MSH_PRI_198):
    case(MSH_PRI_258):  case(MSH_PRI_326):
      return TYPE_PRI;
    case(MSH_HEX_8):    case(MSH_HEX_27):
    case(MSH_HEX_20):   case(MSH_HEX_1):
    case(MSH_HEX_64):   case(MSH_HEX_125):
    case(MSH_HEX_216):  case(MSH_HEX_343):
    case(MSH_HEX_512):  case(MSH_HEX_729):
    case(MSH_HEX_1000): case(MSH_HEX_56):
    case(MSH_HEX_98):   case(MSH_HEX_152):
    case(MSH_HEX_218):  case(MSH_HEX_296):
    case(MSH_HEX_386):  case(MSH_HEX_488):
      return TYPE_HEX;
    case(MSH_POLYG_): case(MSH_POLYG_B):
      return TYPE_POLYG;
    case(MSH_POLYH_):
      return TYPE_POLYH;
    default:
      Msg::Error("Unknown type %i, assuming tetrahedron.", tag);
      return TYPE_TET;
  }
}

// Gives the order corresponding to any element type.
int MElement::OrderFromTag(int tag)
{

  switch (tag) {
  case MSH_PNT     : return 0;
  case MSH_LIN_1   : return 0;
  case MSH_LIN_2   : return 1;
  case MSH_LIN_3   : return 2;
  case MSH_LIN_4   : return 3;
  case MSH_LIN_5   : return 4;
  case MSH_LIN_6   : return 5;
  case MSH_LIN_7   : return 6;
  case MSH_LIN_8   : return 7;
  case MSH_LIN_9   : return 8;
  case MSH_LIN_10  : return 9;
  case MSH_LIN_11  : return 10;
  case MSH_TRI_1   : return 0;
  case MSH_TRI_3   : return 1;
  case MSH_TRI_6   : return 2;
  case MSH_TRI_10  : return 3;
  case MSH_TRI_15  : return 4;
  case MSH_TRI_21  : return 5;
  case MSH_TRI_28  : return 6;
  case MSH_TRI_36  : return 7;
  case MSH_TRI_45  : return 8;
  case MSH_TRI_55  : return 9;
  case MSH_TRI_66  : return 10;
  case MSH_TRI_9   : return 3;
  case MSH_TRI_12  : return 4;
  case MSH_TRI_15I : return 5;
  case MSH_TRI_18  : return 6;
  case MSH_TRI_21I : return 7;
  case MSH_TRI_24  : return 8;
  case MSH_TRI_27  : return 9;
  case MSH_TRI_30  : return 10;
  case MSH_TET_1   : return 0;
  case MSH_TET_4   : return 1;
  case MSH_TET_10  : return 2;
  case MSH_TET_20  : return 3;
  case MSH_TET_35  : return 4;
  case MSH_TET_56  : return 5;
  case MSH_TET_84  : return 6;
  case MSH_TET_120 : return 7;
  case MSH_TET_165 : return 8;
  case MSH_TET_220 : return 9;
  case MSH_TET_286 : return 10;
  case MSH_TET_34  : return 4;
  case MSH_TET_52  : return 5;
  case MSH_TET_74  : return 6;
  case MSH_TET_100 : return 7;
  case MSH_TET_130 : return 8;
  case MSH_TET_164 : return 9;
  case MSH_TET_202 : return 10;
  case MSH_QUA_1   : return 0;
  case MSH_QUA_4   : return 1;
  case MSH_QUA_9   : return 2;
  case MSH_QUA_16  : return 3;
  case MSH_QUA_25  : return 4;
  case MSH_QUA_36  : return 5;
  case MSH_QUA_49  : return 6;
  case MSH_QUA_64  : return 7;
  case MSH_QUA_81  : return 8;
  case MSH_QUA_100 : return 9;
  case MSH_QUA_121 : return 10;
  case MSH_QUA_8   : return 2;
  case MSH_QUA_12  : return 3;
  case MSH_QUA_16I : return 4;
  case MSH_QUA_20  : return 5;
  case MSH_QUA_24  : return 6;
  case MSH_QUA_28  : return 7;
  case MSH_QUA_32  : return 8;
  case MSH_QUA_36I : return 9;
  case MSH_QUA_40  : return 10;
  case MSH_PRI_1   : return 0;
  case MSH_PRI_6   : return 1;
  case MSH_PRI_18  : return 2;
  case MSH_PRI_40  : return 3;
  case MSH_PRI_75  : return 4;
  case MSH_PRI_126 : return 5;
  case MSH_PRI_196 : return 6;
  case MSH_PRI_288 : return 7;
  case MSH_PRI_405 : return 8;
  case MSH_PRI_550 : return 9;
  case MSH_PRI_15  : return 2;
  case MSH_PRI_38  : return 3;
  case MSH_PRI_66  : return 4;
  case MSH_PRI_102 : return 5;
  case MSH_PRI_146 : return 6;
  case MSH_PRI_198 : return 7;
  case MSH_PRI_258 : return 8;
  case MSH_PRI_326 : return 9;
  case MSH_HEX_1   : return 0;
  case MSH_HEX_8   : return 1;
  case MSH_HEX_27  : return 2;
  case MSH_HEX_64  : return 3;
  case MSH_HEX_125 : return 4;
  case MSH_HEX_216 : return 5;
  case MSH_HEX_343 : return 6;
  case MSH_HEX_512 : return 7;
  case MSH_HEX_729 : return 8;
  case MSH_HEX_1000: return 9;
  case MSH_HEX_20  : return 2;
  case MSH_HEX_56  : return 3;
  case MSH_HEX_98  : return 4;
  case MSH_HEX_152 : return 5;
  case MSH_HEX_218 : return 6;
  case MSH_HEX_296 : return 7;
  case MSH_HEX_386 : return 8;
  case MSH_HEX_488 : return 9;
  case MSH_PYR_5   : return 1;
  case MSH_PYR_14  : return 2;
  case MSH_PYR_30  : return 3;
  case MSH_PYR_55  : return 4;
  case MSH_PYR_91  : return 5;
  case MSH_PYR_140 : return 6;
  case MSH_PYR_204 : return 7;
  case MSH_PYR_285 : return 8;
  case MSH_PYR_385 : return 9;
  case MSH_PYR_13  : return 2;
  case MSH_PYR_29  : return 3;
  case MSH_PYR_50  : return 4;
  case MSH_PYR_77  : return 5;
  case MSH_PYR_110 : return 6;
  case MSH_PYR_149 : return 7;
  case MSH_PYR_194 : return 8;
  case MSH_PYR_245 : return 9;
  default :
    Msg::Error("Unknown element type %d: reverting to order 1",tag);
    return 1;
  }

}


MElement *MElementFactory::create(int type, std::vector<MVertex*> &v,
                                  int num, int part, bool owner, MElement *parent,
                                  MElement *d1, MElement *d2)
{
  switch (type) {
  case MSH_PNT:     return new MPoint(v, num, part);
  case MSH_LIN_2:   return new MLine(v, num, part);
  case MSH_LIN_3:   return new MLine3(v, num, part);
  case MSH_LIN_4:   return new MLineN(v, num, part);
  case MSH_LIN_5:   return new MLineN(v, num, part);
  case MSH_LIN_6:   return new MLineN(v, num, part);
  case MSH_LIN_7:   return new MLineN(v, num, part);
  case MSH_LIN_8:   return new MLineN(v, num, part);
  case MSH_LIN_9:   return new MLineN(v, num, part);
  case MSH_LIN_10:  return new MLineN(v, num, part);
  case MSH_LIN_11:  return new MLineN(v, num, part);
  case MSH_LIN_B:   return new MLineBorder(v, num, part, d1, d2);
  case MSH_LIN_C:   return new MLineChild(v, num, part, owner, parent);
  case MSH_TRI_3:   return new MTriangle(v, num, part);
  case MSH_TRI_6:   return new MTriangle6(v, num, part);
  case MSH_TRI_9:   return new MTriangleN(v, 3, num, part);
  case MSH_TRI_10:  return new MTriangleN(v, 3, num, part);
  case MSH_TRI_12:  return new MTriangleN(v, 4, num, part);
  case MSH_TRI_15:  return new MTriangleN(v, 4, num, part);
  case MSH_TRI_15I: return new MTriangleN(v, 5, num, part);
  case MSH_TRI_21:  return new MTriangleN(v, 5, num, part);
  case MSH_TRI_28:  return new MTriangleN(v, 6, num, part);
  case MSH_TRI_36:  return new MTriangleN(v, 7, num, part);
  case MSH_TRI_45:  return new MTriangleN(v, 8, num, part);
  case MSH_TRI_55:  return new MTriangleN(v, 9, num, part);
  case MSH_TRI_66:  return new MTriangleN(v,10, num, part);
  case MSH_TRI_B:   return new MTriangleBorder(v, num, part, d1, d2);
  case MSH_QUA_4:   return new MQuadrangle(v, num, part);
  case MSH_QUA_8:   return new MQuadrangle8(v, num, part);
  case MSH_QUA_9:   return new MQuadrangle9(v, num, part);
  case MSH_QUA_12:  return new MQuadrangleN(v, 3, num, part);
  case MSH_QUA_16:  return new MQuadrangleN(v, 3, num, part);
  case MSH_QUA_25:  return new MQuadrangleN(v, 4, num, part);
  case MSH_QUA_36:  return new MQuadrangleN(v, 5, num, part);
  case MSH_QUA_49:  return new MQuadrangleN(v, 6, num, part);
  case MSH_QUA_64:  return new MQuadrangleN(v, 7, num, part);
  case MSH_QUA_81:  return new MQuadrangleN(v, 8, num, part);
  case MSH_QUA_100: return new MQuadrangleN(v, 9, num, part);
  case MSH_QUA_121: return new MQuadrangleN(v, 10, num, part);
  case MSH_POLYG_:  return new MPolygon(v, num, part, owner, parent);
  case MSH_POLYG_B: return new MPolygonBorder(v, num, part, d1, d2);
  case MSH_TET_4:   return new MTetrahedron(v, num, part);
  case MSH_TET_10:  return new MTetrahedron10(v, num, part);
  case MSH_HEX_8:   return new MHexahedron(v, num, part);
  case MSH_HEX_20:  return new MHexahedron20(v, num, part);
  case MSH_HEX_27:  return new MHexahedron27(v, num, part);
  case MSH_PRI_6:   return new MPrism(v, num, part);
  case MSH_PRI_15:  return new MPrism15(v, num, part);
  case MSH_PRI_18:  return new MPrism18(v, num, part);
  case MSH_TET_20:  return new MTetrahedronN(v, 3, num, part);
  case MSH_TET_34:  return new MTetrahedronN(v, 3, num, part);
  case MSH_TET_35:  return new MTetrahedronN(v, 4, num, part);
  case MSH_TET_52:  return new MTetrahedronN(v, 5, num, part);
  case MSH_TET_56:  return new MTetrahedronN(v, 5, num, part);
  case MSH_TET_84:  return new MTetrahedronN(v, 6, num, part);
  case MSH_TET_120: return new MTetrahedronN(v, 7, num, part);
  case MSH_TET_165: return new MTetrahedronN(v, 8, num, part);
  case MSH_TET_220: return new MTetrahedronN(v, 9, num, part);
  case MSH_TET_286: return new MTetrahedronN(v, 10, num, part);
  case MSH_POLYH_:  return new MPolyhedron(v, num, part, owner, parent);
  case MSH_HEX_56:  return new MHexahedronN(v, 3, num, part);
  case MSH_HEX_64:  return new MHexahedronN(v, 3, num, part);
  case MSH_HEX_125: return new MHexahedronN(v, 4, num, part);
  case MSH_HEX_216: return new MHexahedronN(v, 5, num, part);
  case MSH_HEX_343: return new MHexahedronN(v, 6, num, part);
  case MSH_HEX_512: return new MHexahedronN(v, 7, num, part);
  case MSH_HEX_729: return new MHexahedronN(v, 8, num, part);
  case MSH_HEX_1000:return new MHexahedronN(v, 9, num, part);
  case MSH_PNT_SUB: return new MSubPoint(v, num, part, owner, parent);
  case MSH_LIN_SUB: return new MSubLine(v, num, part, owner, parent);
  case MSH_TRI_SUB: return new MSubTriangle(v, num, part, owner, parent);
  case MSH_TET_SUB: return new MSubTetrahedron(v, num, part, owner, parent);
  case MSH_PYR_5:   return new MPyramid(v, num, part);
  case MSH_PYR_13:  return new MPyramidN(v, 2, num, part);
  case MSH_PYR_14:  return new MPyramidN(v, 2, num, part);
  case MSH_PYR_30:  return new MPyramidN(v, 3, num, part);
  case MSH_PYR_55:  return new MPyramidN(v, 4, num, part);
  case MSH_PYR_91:  return new MPyramidN(v, 5, num, part);
  case MSH_PYR_140: return new MPyramidN(v, 6, num, part);
  case MSH_PYR_204: return new MPyramidN(v, 7, num, part);
  case MSH_PYR_285: return new MPyramidN(v, 8, num, part);
  case MSH_PYR_385: return new MPyramidN(v, 9, num, part);
  default:          return 0;
  }
}

MElement *MElementFactory::create(int num, int type, const std::vector<int> &data,
                                  GModel *model)
{
  // This should be rewritten: each element should register itself in a static
  // factory owned e.g. directly by MElement, and interpret its data by
  // itself. This would remove the ugly switch in the routine above.

  int numVertices = MElement::getInfoMSH(type), startVertices = 0;
  if(data.size() && !numVertices){
    startVertices = 1;
    numVertices = data[0];
  }

  std::vector<MVertex*> vertices(numVertices);
  if((int) data.size() > startVertices + numVertices - 1){
    for(int i = 0; i < numVertices; i++){
      int numVertex = data[startVertices + i];
      MVertex *v = model->getMeshVertexByTag(numVertex);
      if(v){
        vertices[i] = v;
      }
      else{
        Msg::Error("Unknown vertex %d in element %d", numVertex, num);
        return 0;
      }
    }
  }
  else{
    Msg::Error("Missing data in element %d", num);
    return 0;
  }

  int part = 0;
  int startPartitions = startVertices + numVertices;

  MElement *parent = 0;
  if((type == MSH_PNT_SUB || type == MSH_LIN_SUB ||
      type == MSH_TRI_SUB || type == MSH_TET_SUB)){
    parent = model->getMeshElementByTag(data[startPartitions]);
    startPartitions += 1;
  }

  std::vector<short> ghosts;
  if((int) data.size() > startPartitions){
    int numPartitions = data[startPartitions];
    if(numPartitions > 0 && (int) data.size() > startPartitions + numPartitions - 1){
      part = data[startPartitions + 1];
      for(int i = 1; i < numPartitions; i++)
        ghosts.push_back(data[startPartitions + 1 + i]);
    }
  }

  MElement *element = create(type, vertices, num, part, false, parent);

  for(unsigned int j = 0; j < ghosts.size(); j++)
    model->getGhostCells().insert(std::pair<MElement*, short>(element, ghosts[j]));
  if(part) model->getMeshPartitions().insert(part);

  return element;
}
