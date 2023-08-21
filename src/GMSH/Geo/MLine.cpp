// Gmsh - Copyright (C) 1997-2013 C. Geuzaine, J.-F. Remacle
//
// See the LICENSE.txt file for license information. Please report all
// bugs and problems to the public mailing list <gmsh@geuz.org>.

#include "GmshDefines.h"
#include "MLine.h"
#include "nodalBasis.h"
#include "BasisFactory.h"
#include "GaussLegendre1D.h"
#include "Context.h"
#include "qualityMeasures.h"

const nodalBasis* MLine::getFunctionSpace(int o) const
{
  int order = (o == -1) ? getPolynomialOrder() : o;
  
  switch (order) {
  case 0: return BasisFactory::create(MSH_LIN_1);
  case 1: return BasisFactory::create(MSH_LIN_2);
  case 2: return BasisFactory::create(MSH_LIN_3);
  case 3: return BasisFactory::create(MSH_LIN_4);
  case 4: return BasisFactory::create(MSH_LIN_5);
  case 5: return BasisFactory::create(MSH_LIN_6);
  case 6: return BasisFactory::create(MSH_LIN_7);
  case 7: return BasisFactory::create(MSH_LIN_8);
  case 8: return BasisFactory::create(MSH_LIN_9);
  case 9: return BasisFactory::create(MSH_LIN_10);
  case 10: return BasisFactory::create(MSH_LIN_11);
  default: Msg::Error("Order %d line function space not implemented", order);
  }
  return 0;
}

const JacobianBasis* MLine::getJacobianFuncSpace(int o) const
{
  int order = (o == -1) ? getPolynomialOrder() : o;
  
  switch (order) {
  case 1: return JacobianBasis::find(MSH_LIN_2);
  case 2: return JacobianBasis::find(MSH_LIN_3);
  case 3: return JacobianBasis::find(MSH_LIN_4);
  case 4: return JacobianBasis::find(MSH_LIN_5);
  case 5: return JacobianBasis::find(MSH_LIN_6);
  case 6: return JacobianBasis::find(MSH_LIN_7);
  case 7: return JacobianBasis::find(MSH_LIN_8);
  case 8: return JacobianBasis::find(MSH_LIN_9);
  case 9: return JacobianBasis::find(MSH_LIN_10);
  case 10: return JacobianBasis::find(MSH_LIN_11);
  default: Msg::Error("Order %d line function space not implemented", order);
  }
  return 0;
}

void MLine::getIntegrationPoints(int pOrder, int *npts, IntPt **pts)
{
  *npts = getNGQLPts(pOrder);
  *pts = getGQLPts(pOrder);
}

double MLine::getInnerRadius()
{
  return _v[0]->distance(_v[1]) * .5;
}

double MLine::getLength()
{
  return _v[0]->distance(_v[1]);
}

double MLine::getVolume()
{
  return getLength();
}
