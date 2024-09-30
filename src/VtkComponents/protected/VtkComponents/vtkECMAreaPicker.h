/**
 * Classe vtkECMAreaPicker : réimplémentation de la classe vtkAreaPicker
 * de VTK 7.1.1 afin d'ajouter une option permettant de ne sélectionner 
 * que des entités situées complétement à l'intérieur du tronc de 
 * visualisation.
 * Cette classe n'hérite malheureusement pas de la classe vtkAreaPicker
 * car elle repose sur des appels à des fonctions non virtuelles de la 
 * classe vtkExtractSelectedFrustum.
 * <B>>Les différences avec le code initial de VTK sont repérées à l'aide
 * de la balise "Magix3D".</B>
 */
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkECMAreaPicker.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkECMAreaPicker
 * @brief   Picks props behind a selection rectangle on a viewport.
 *
 *
 * The vtkECMAreaPicker picks all vtkProp3Ds that lie behind the screen space
 * rectangle from x0,y0 and x1,y1. The selection is based upon the bounding
 * box of the prop and is thus not exact.
 *
 * Like vtkPicker, a pick results in a list of Prop3Ds because many props may
 * lie within the pick frustum. You can also get an AssemblyPath, which in this
 * case is defined to be the path to the one particular prop in the Prop3D list
 * that lies nearest to the near plane.
 *
 * This picker also returns the selection frustum, defined as either a
 * vtkPlanes, or a set of eight corner vertices in world space. The vtkPlanes
 * version is an ImplicitFunction, which is suitable for use with the
 * vtkExtractGeometry. The six frustum planes are in order: left, right,
 * bottom, top, near, far
 *
 * Because this picker picks everything within a volume, the world pick point
 * result is ill-defined. Therefore if you ask this class for the world pick
 * position, you will get the centroid of the pick frustum. This may be outside
 * of all props in the prop list.
 *
 * @sa
 * vtkInteractorStyleRubberBandPick, vtkExtractSelectedFrustum.
*/

#ifndef vtkECMAreaPicker_h
#define vtkECMAreaPicker_h

#include "vtkRenderingCoreModule.h" // For export macro
#include "vtkAbstractPropPicker.h"

class vtkRenderer;
class vtkPoints;
class vtkPlanes;
class vtkProp3DCollection;
class vtkAbstractMapper3D;
class vtkDataSet;
//class vtkExtractSelectedFrustum;
class vtkECMExtractSelectedFrustum;
class vtkProp;

class VTKRENDERINGCORE_EXPORT vtkECMAreaPicker : public vtkAbstractPropPicker
{
public:
  static vtkECMAreaPicker *New();
  vtkTypeMacro(vtkECMAreaPicker, vtkAbstractPropPicker);
  void PrintSelf(ostream& os, vtkIndent indent);

  void SelectCompletelyInside (bool on);			// Magix3D
  bool GetSelectCompletelyInside ( ) const;			// Magix3D

  /**
   * Set the default screen rectangle to pick in.
   */
  void SetPickCoords(double x0, double y0, double x1, double y1);

  /**
   * Set the default renderer to pick on.
   */
  void SetRenderer(vtkRenderer *);

  /**
   * Perform an AreaPick within the default screen rectangle and renderer.
   */
  virtual int Pick();

  /**
   * Perform pick operation in volume behind the given screen coordinates.
   * Props intersecting the selection frustum will be accessible via GetProp3D.
   * GetPlanes returns a vtkImplicitFunciton suitable for vtkExtractGeometry.
   */
  virtual int AreaPick(double x0, double y0, double x1, double y1, vtkRenderer *renderer = NULL);

  /**
   * Perform pick operation in volume behind the given screen coordinate.
   * This makes a thin frustum around the selected pixel.
   * Note: this ignores Z in order to pick everying in a volume from z=0 to z=1.
   */
  virtual int Pick(double x0, double y0, double vtkNotUsed(z0), vtkRenderer *renderer = NULL)
    { return this->AreaPick(x0, y0, x0+1.0, y0+1.0, renderer); }

  //@{
  /**
   * Return mapper that was picked (if any).
   */
  vtkGetObjectMacro(Mapper, vtkAbstractMapper3D);
  //@}

  //@{
  /**
   * Get a pointer to the dataset that was picked (if any). If nothing
   * was picked then NULL is returned.
   */
  vtkGetObjectMacro(DataSet, vtkDataSet);
  //@}

  /**
   * Return a collection of all the prop 3D's that were intersected
   * by the pick ray. This collection is not sorted.
   */
  vtkProp3DCollection *GetProp3Ds()
    { return this->Prop3Ds; }

  //@{
  /**
   * Return the six planes that define the selection frustum. The implicit
   * function defined by the planes evaluates to negative inside and positive
   * outside.
   */
  vtkGetObjectMacro(Frustum, vtkPlanes);
  //@}

  //@{
  /**
   * Return eight points that define the selection frustum.
   */
  vtkGetObjectMacro(ClipPoints, vtkPoints);
  //@}

protected:
  vtkECMAreaPicker();
  ~vtkECMAreaPicker();

  virtual void Initialize();
  void DefineFrustum(double x0, double y0, double x1, double y1, vtkRenderer *);
  virtual int PickProps(vtkRenderer *renderer);
  int TypeDecipher(vtkProp *, vtkAbstractMapper3D **);

  int ABoxFrustumIsect(double bounds[], double &mindist);

  vtkPoints *ClipPoints;
  vtkPlanes *Frustum;

  vtkProp3DCollection *Prop3Ds; //candidate actors (based on bounding box)
  vtkAbstractMapper3D *Mapper; //selected mapper (if the prop has a mapper)
  vtkDataSet *DataSet; //selected dataset (if there is one)

  //used internally to do prop intersection tests
  vtkECMExtractSelectedFrustum *FrustumExtractor;				// Magix3D

  double X0;
  double Y0;
  double X1;
  double Y1;

private:
  vtkECMAreaPicker(const vtkECMAreaPicker&) VTK_DELETE_FUNCTION;
  void operator=(const vtkECMAreaPicker&) VTK_DELETE_FUNCTION;
};

#endif
