// Gmsh - Copyright (C) 1997-2013 C. Geuzaine, J.-F. Remacle
//
// See the LICENSE.txt file for license information. Please report all
// bugs and problems to the public mailing list <gmsh@geuz.org>.
#include <limits>
#include "MHexahedron.h"
#include "Numeric.h"
#include "Context.h"
#include "BasisFactory.h"
#include "nodalBasis.h"
#include "polynomialBasis.h"
#include "MQuadrangle.h"
#include "qualityMeasures.h"

int MHexahedron::getVolumeSign()
{
  double mat[3][3];
  mat[0][0] = _v[1]->x() - _v[0]->x();
  mat[0][1] = _v[3]->x() - _v[0]->x();
  mat[0][2] = _v[4]->x() - _v[0]->x();
  mat[1][0] = _v[1]->y() - _v[0]->y();
  mat[1][1] = _v[3]->y() - _v[0]->y();
  mat[1][2] = _v[4]->y() - _v[0]->y();
  mat[2][0] = _v[1]->z() - _v[0]->z();
  mat[2][1] = _v[3]->z() - _v[0]->z();
  mat[2][2] = _v[4]->z() - _v[0]->z();
  double d = det3x3(mat);
  if(d < 0.) return -1;
  else if(d > 0.) return 1;
  else return 0;
}

void MHexahedron::getIntegrationPoints(int pOrder, int *npts, IntPt **pts)
{
  *npts = getNGQHPts(pOrder);
  *pts = getGQHPts(pOrder);
}

double MHexahedron::angleShapeMeasure()
{

#if defined(HAVE_MESH)
   double angleMax = 0.0;
   double angleMin = M_PI;
   double zeta = 0.0;
   for (int i=0; i<getNumFaces(); i++){
     std::vector<MVertex*> vv;
     vv.push_back(getFace(i).getVertex(0));
     vv.push_back(getFace(i).getVertex(1));
     vv.push_back(getFace(i).getVertex(2)); 
     vv.push_back(getFace(i).getVertex(3));
     // MVertex *v0 = new MVertex(0, 0, 0); vv.push_back(v0);
     // MVertex *v1 = new MVertex(1., 0, 0);vv.push_back(v1);
     // MVertex *v2 = new MVertex(2., 1., 0);vv.push_back(v2);
     // MVertex *v3 = new MVertex(1, 1., 0);vv.push_back(v3);
     for (int j=0; j<4; j++){
       SVector3 a(vv[(j+2)%4]->x()-vv[(j+1)%4]->x(),vv[(j+2)%4]->y()-vv[(j+1)%4]->y(),vv[(j+2)%4]->z()-vv[(j+1)%4]->z()  );
       SVector3 b(vv[(j+1)%4]->x()-vv[(j)%4]->x(),  vv[(j+1)%4]->y()-vv[(j)%4]->y(),  vv[(j+1)%4]->z()-vv[(j)%4]->z()  );
       double angle = acos( dot(a,b)/(norm(a)*norm(b))); //*180/M_PI;
       angleMax = std::max(angleMax, angle);
       angleMin = std::min(angleMin, angle);
     }
     //printf("angle max =%g min =%g \n", angleMax*180/M_PI, angleMin*180/M_PI);
   }
   zeta = 1.-std::max((angleMax-0.5*M_PI)/(0.5*M_PI),(0.5*M_PI-angleMin)/(0.5*M_PI));
   return zeta; 
#else
   return 1.;
#endif

}
double MHexahedron::getInnerRadius()
{
  //Only for vertically aligned elements (not inclined)
  double innerRadius=std::numeric_limits<double>::max();
  for (int i=0; i<getNumFaces(); i++){
    MQuadrangle quad(getFace(i).getVertex(0), getFace(i).getVertex(1), getFace(i).getVertex(2), getFace(i).getVertex(3));
    innerRadius=std::min(innerRadius,quad.getInnerRadius());
  }
  return innerRadius;
}

void MHexahedron::getFaceInfo(const MFace &face, int &ithFace, int &sign, int &rot) const
{
  for (ithFace = 0; ithFace < 6; ithFace++){
    MVertex *v0 = _v[faces_hexa(ithFace, 0)];
    MVertex *v1 = _v[faces_hexa(ithFace, 1)];
    MVertex *v2 = _v[faces_hexa(ithFace, 2)];
    MVertex *v3 = _v[faces_hexa(ithFace, 3)];

    if (v0 == face.getVertex(0) && v1 == face.getVertex(1) &&
        v2 == face.getVertex(2) && v3 == face.getVertex(3)){
      sign = 1; rot = 0; return;
    }
    if (v0 == face.getVertex(1) && v1 == face.getVertex(2) &&
        v2 == face.getVertex(3) && v3 == face.getVertex(0)){
      sign = 1; rot = 1; return;
    }
    if (v0 == face.getVertex(2) && v1 == face.getVertex(3) &&
        v2 == face.getVertex(0) && v3 == face.getVertex(1)){
      sign = 1; rot = 2; return;
    }
    if (v0 == face.getVertex(3) && v1 == face.getVertex(0) &&
        v2 == face.getVertex(1) && v3 == face.getVertex(2)){
      sign = 1; rot = 3; return;
    }
    // reverse
    if (v0 == face.getVertex(0) && v1 == face.getVertex(3) &&
        v2 == face.getVertex(2) && v3 == face.getVertex(1)){
      sign = -1; rot = 0; return;
    }
    if (v0 == face.getVertex(3) && v1 == face.getVertex(2) &&
        v2 == face.getVertex(1) && v3 == face.getVertex(0)){
      sign = -1; rot = 1; return;
    }
    if (v0 == face.getVertex(2) && v1 == face.getVertex(1) &&
        v2 == face.getVertex(0) && v3 == face.getVertex(3)){
      sign = -1; rot = 2; return;
    }
    if (v0 == face.getVertex(1) && v1 == face.getVertex(0) &&
        v2 == face.getVertex(3) && v3 == face.getVertex(2)){
      sign = -1; rot = 3; return;
    }
  }
  Msg::Error("Could not get face information for hexahedron %d", getNum());
}

void MHexahedronN::getEdgeRep(int num, double *x, double *y, double *z, SVector3 *n)
{
  const int numSubEdges = CTX::instance()->mesh.numSubEdges;
  static double pp[8][3] = {
    {-1,-1,-1},{1,-1,-1},{1,1,-1},{-1,1,-1},
    {-1,-1,1},{1,-1,1},{1,1,1},{-1,1,1} };
  static int ed [12][2] = {
    {0,1},{0,3},{0,4},{1,2},{1,5},{2,3},
    {2,6},{3,7},{4,5},{4,7},{5,6},{7,6}
  };
  int iEdge = num / numSubEdges;
  int iSubEdge = num % numSubEdges;

  int iVertex1 = ed [iEdge][0];
  int iVertex2 = ed [iEdge][1];
  double t1 = (double) iSubEdge / (double) numSubEdges;
  double u1 = pp[iVertex1][0] * (1.-t1) + pp[iVertex2][0] * t1;
  double v1 = pp[iVertex1][1] * (1.-t1) + pp[iVertex2][1] * t1;
  double w1 = pp[iVertex1][2] * (1.-t1) + pp[iVertex2][2] * t1;

  double t2 = (double) (iSubEdge+1) / (double) numSubEdges;
  double u2 = pp[iVertex1][0] * (1.-t2) + pp[iVertex2][0] * t2;
  double v2 = pp[iVertex1][1] * (1.-t2) + pp[iVertex2][1] * t2;
  double w2 = pp[iVertex1][2] * (1.-t2) + pp[iVertex2][2] * t2;

  SPoint3 pnt1, pnt2;
  pnt(u1, v1, w1, pnt1);
  pnt(u2, v2, w2, pnt2);
  x[0] = pnt1.x(); x[1] = pnt2.x();
  y[0] = pnt1.y(); y[1] = pnt2.y();
  z[0] = pnt1.z(); z[1] = pnt2.z();

  // not great, but better than nothing
  //static const int f[6] = {0, 0, 0, 1, 2, 3};
  n[0] = n[1] = 1 ;
}

int MHexahedronN::getNumEdgesRep()
{
  return 12 * CTX::instance()->mesh.numSubEdges;
}
 
const nodalBasis* MHexahedron::getFunctionSpace(int o) const
{
  int order = (o == -1) ? getPolynomialOrder() : o;

  int nv = getNumVolumeVertices();

  if ((nv == 0) && (o == -1)) {
    switch (order) {
    case 0: return BasisFactory::create(MSH_HEX_1);
    case 1: return BasisFactory::create(MSH_HEX_8);
    case 2: return BasisFactory::create(MSH_HEX_20);
    case 3: return BasisFactory::create(MSH_HEX_56);
    case 4: return BasisFactory::create(MSH_HEX_98);
    case 5: return BasisFactory::create(MSH_HEX_152);
    case 6: return BasisFactory::create(MSH_HEX_218);
    case 7: return BasisFactory::create(MSH_HEX_296);
    case 8: return BasisFactory::create(MSH_HEX_386);
    case 9: return BasisFactory::create(MSH_HEX_488);
    default: Msg::Error("Order %d hex function space not implemented", order); break;
    }
  }
  else {
    switch (order) {
    case 0: return BasisFactory::create(MSH_HEX_1);
    case 1: return BasisFactory::create(MSH_HEX_8);
    case 2: return BasisFactory::create(MSH_HEX_27);
    case 3: return BasisFactory::create(MSH_HEX_64);
    case 4: return BasisFactory::create(MSH_HEX_125);
    case 5: return BasisFactory::create(MSH_HEX_216);
    case 6: return BasisFactory::create(MSH_HEX_343);
    case 7: return BasisFactory::create(MSH_HEX_512);
    case 8: return BasisFactory::create(MSH_HEX_729);
    case 9: return BasisFactory::create(MSH_HEX_1000);
    default: Msg::Error("Order %d hex function space not implemented", order); break;
    }
  }
  return 0;
}

const JacobianBasis* MHexahedron::getJacobianFuncSpace(int o) const
{
  int order = (o == -1) ? getPolynomialOrder() : o;

  int nv = getNumVolumeVertices();

  if ((nv == 0) && (o == -1)) {
    switch (order) {
    case 0: return JacobianBasis::find(MSH_HEX_1);
    case 1: return JacobianBasis::find(MSH_HEX_8);
    case 2: return JacobianBasis::find(MSH_HEX_20);
    case 3: return JacobianBasis::find(MSH_HEX_56);
    case 4: return JacobianBasis::find(MSH_HEX_98);
    case 5: return JacobianBasis::find(MSH_HEX_152);
    case 6: return JacobianBasis::find(MSH_HEX_218);
    case 7: return JacobianBasis::find(MSH_HEX_296);
    case 8: return JacobianBasis::find(MSH_HEX_386);
    case 9: return JacobianBasis::find(MSH_HEX_488);
    default: Msg::Error("Order %d hex incomplete Jacobian function space not implemented", order); break;
    }
  }
  else {
    switch (order) {
    case 0: return JacobianBasis::find(MSH_HEX_1);
    case 1: return JacobianBasis::find(MSH_HEX_8);
    case 2: return JacobianBasis::find(MSH_HEX_27);
    case 3: return JacobianBasis::find(MSH_HEX_64);
    case 4: return JacobianBasis::find(MSH_HEX_125);
    case 5: return JacobianBasis::find(MSH_HEX_216);
    case 6: return JacobianBasis::find(MSH_HEX_343);
    case 7: return JacobianBasis::find(MSH_HEX_512);
    case 8: return JacobianBasis::find(MSH_HEX_729);
    case 9: return JacobianBasis::find(MSH_HEX_1000);
    default: Msg::Error("Order %d hex Jacobian function space not implemented", order); break;
    }
  }
  return 0;
}

static void _myGetFaceRep(MHexahedron *hex, int num, double *x, double *y, double *z,
                          SVector3 *n, int numSubEdges)
{
  static double pp[8][3] = {
    {-1,-1,-1},{1,-1,-1},{1,1,-1},{-1,1,-1},
    {-1,-1,1},{1,-1,1},{1,1,1},{-1,1,1} };

  int iFace    = num / (2*numSubEdges * numSubEdges);
  int iSubFace = num % (2*numSubEdges * numSubEdges);

  int iVertex1 = hex->faces_hexa(iFace,0);
  int iVertex2 = hex->faces_hexa(iFace,1);
  int iVertex3 = hex->faces_hexa(iFace,2);
  int iVertex4 = hex->faces_hexa(iFace,3);

  SPoint3 pnt1, pnt2, pnt3;
 // double J1[3][3], J2[3][3], J3[3][3];

  /*
    0
    0 1
    0 1 2
    0 1 2 3
    0 1 2 3 4
    0 1 2 3 4 5
  */

  // on each layer, we have (numSubEdges) * 2 triangles
  // ix and iy are the coordinates of the sub-quadrangle

  int io = iSubFace%2;
  int ix = (iSubFace/2)/numSubEdges;
  int iy = (iSubFace/2)%numSubEdges;

  const double d = 2. / numSubEdges;
  double ox = -1. + d*ix;
  double oy = -1. + d*iy;

  if (io == 0){
    double U1 =
      pp[iVertex1][0] * (1.-ox)*(1-oy)*.25 +
      pp[iVertex2][0] * (1.+ox)*(1-oy)*.25 +
      pp[iVertex3][0] * (1.+ox)*(1+oy)*.25 +
      pp[iVertex4][0] * (1.-ox)*(1+oy)*.25;
    double V1 =
      pp[iVertex1][1] * (1.-ox)*(1-oy)*.25 +
      pp[iVertex2][1] * (1.+ox)*(1-oy)*.25 +
      pp[iVertex3][1] * (1.+ox)*(1+oy)*.25 +
      pp[iVertex4][1] * (1.-ox)*(1+oy)*.25;
    double W1 =
      pp[iVertex1][2] * (1.-ox)*(1-oy)*.25 +
      pp[iVertex2][2] * (1.+ox)*(1-oy)*.25 +
      pp[iVertex3][2] * (1.+ox)*(1+oy)*.25 +
      pp[iVertex4][2] * (1.-ox)*(1+oy)*.25;

    ox += d;

    double U2 =
      pp[iVertex1][0] * (1.-ox)*(1-oy)*.25 +
      pp[iVertex2][0] * (1.+ox)*(1-oy)*.25 +
      pp[iVertex3][0] * (1.+ox)*(1+oy)*.25 +
      pp[iVertex4][0] * (1.-ox)*(1+oy)*.25;
    double V2 =
      pp[iVertex1][1] * (1.-ox)*(1-oy)*.25 +
      pp[iVertex2][1] * (1.+ox)*(1-oy)*.25 +
      pp[iVertex3][1] * (1.+ox)*(1+oy)*.25 +
      pp[iVertex4][1] * (1.-ox)*(1+oy)*.25;
    double W2 =
      pp[iVertex1][2] * (1.-ox)*(1-oy)*.25 +
      pp[iVertex2][2] * (1.+ox)*(1-oy)*.25 +
      pp[iVertex3][2] * (1.+ox)*(1+oy)*.25 +
      pp[iVertex4][2] * (1.-ox)*(1+oy)*.25;

    oy += d;

    double U3 =
      pp[iVertex1][0] * (1.-ox)*(1-oy)*.25 +
      pp[iVertex2][0] * (1.+ox)*(1-oy)*.25 +
      pp[iVertex3][0] * (1.+ox)*(1+oy)*.25 +
      pp[iVertex4][0] * (1.-ox)*(1+oy)*.25;
    double V3 =
      pp[iVertex1][1] * (1.-ox)*(1-oy)*.25 +
      pp[iVertex2][1] * (1.+ox)*(1-oy)*.25 +
      pp[iVertex3][1] * (1.+ox)*(1+oy)*.25 +
      pp[iVertex4][1] * (1.-ox)*(1+oy)*.25;
    double W3 =
      pp[iVertex1][2] * (1.-ox)*(1-oy)*.25 +
      pp[iVertex2][2] * (1.+ox)*(1-oy)*.25 +
      pp[iVertex3][2] * (1.+ox)*(1+oy)*.25 +
      pp[iVertex4][2] * (1.-ox)*(1+oy)*.25;

    hex->pnt(U1, V1, W1, pnt1);
    hex->pnt(U2, V2, W2, pnt2);
    hex->pnt(U3, V3, W3, pnt3);
  }
  else{
    double U1 =
      pp[iVertex1][0] * (1.-ox)*(1-oy)*.25 +
      pp[iVertex2][0] * (1.+ox)*(1-oy)*.25 +
      pp[iVertex3][0] * (1.+ox)*(1+oy)*.25 +
      pp[iVertex4][0] * (1.-ox)*(1+oy)*.25;
    double V1 =
      pp[iVertex1][1] * (1.-ox)*(1-oy)*.25 +
      pp[iVertex2][1] * (1.+ox)*(1-oy)*.25 +
      pp[iVertex3][1] * (1.+ox)*(1+oy)*.25 +
      pp[iVertex4][1] * (1.-ox)*(1+oy)*.25;
    double W1 =
      pp[iVertex1][2] * (1.-ox)*(1-oy)*.25 +
      pp[iVertex2][2] * (1.+ox)*(1-oy)*.25 +
      pp[iVertex3][2] * (1.+ox)*(1+oy)*.25 +
      pp[iVertex4][2] * (1.-ox)*(1+oy)*.25;

    ox += d;
    oy += d;

    double U2 =
      pp[iVertex1][0] * (1.-ox)*(1-oy)*.25 +
      pp[iVertex2][0] * (1.+ox)*(1-oy)*.25 +
      pp[iVertex3][0] * (1.+ox)*(1+oy)*.25 +
      pp[iVertex4][0] * (1.-ox)*(1+oy)*.25;
    double V2 =
      pp[iVertex1][1] * (1.-ox)*(1-oy)*.25 +
      pp[iVertex2][1] * (1.+ox)*(1-oy)*.25 +
      pp[iVertex3][1] * (1.+ox)*(1+oy)*.25 +
      pp[iVertex4][1] * (1.-ox)*(1+oy)*.25;
    double W2 =
      pp[iVertex1][2] * (1.-ox)*(1-oy)*.25 +
      pp[iVertex2][2] * (1.+ox)*(1-oy)*.25 +
      pp[iVertex3][2] * (1.+ox)*(1+oy)*.25 +
      pp[iVertex4][2] * (1.-ox)*(1+oy)*.25;

    ox -= d;

    double U3 =
      pp[iVertex1][0] * (1.-ox)*(1-oy)*.25 +
      pp[iVertex2][0] * (1.+ox)*(1-oy)*.25 +
      pp[iVertex3][0] * (1.+ox)*(1+oy)*.25 +
      pp[iVertex4][0] * (1.-ox)*(1+oy)*.25;
    double V3 =
      pp[iVertex1][1] * (1.-ox)*(1-oy)*.25 +
      pp[iVertex2][1] * (1.+ox)*(1-oy)*.25 +
      pp[iVertex3][1] * (1.+ox)*(1+oy)*.25 +
      pp[iVertex4][1] * (1.-ox)*(1+oy)*.25;
    double W3 =
      pp[iVertex1][2] * (1.-ox)*(1-oy)*.25 +
      pp[iVertex2][2] * (1.+ox)*(1-oy)*.25 +
      pp[iVertex3][2] * (1.+ox)*(1+oy)*.25 +
      pp[iVertex4][2] * (1.-ox)*(1+oy)*.25;

    hex->pnt(U1, V1, W1, pnt1);
    hex->pnt(U2, V2, W2, pnt2);
    hex->pnt(U3, V3, W3, pnt3);
  }

  x[0] = pnt1.x(); x[1] = pnt2.x(); x[2] = pnt3.x();
  y[0] = pnt1.y(); y[1] = pnt2.y(); y[2] = pnt3.y();
  z[0] = pnt1.z(); z[1] = pnt2.z(); z[2] = pnt3.z();

  SVector3 d1(x[1] - x[0], y[1] - y[0], z[1] - z[0]);
  SVector3 d2(x[2] - x[0], y[2] - y[0], z[2] - z[0]);
  n[0] = crossprod(d1, d2);
  n[0].normalize();
  n[1] = n[0];
  n[2] = n[0];
}

void MHexahedronN::getFaceRep(int num, double *x, double *y, double *z, SVector3 *n)
{
  _myGetFaceRep(this, num, x, y, z, n, CTX::instance()->mesh.numSubEdges);
}

int MHexahedronN::getNumFacesRep()
{
  return 6 * (CTX::instance()->mesh.numSubEdges * CTX::instance()->mesh.numSubEdges * 2);
}
