import sys
import pyMagix3D as Mgx3D

vtkfilename = "box.vtk"

def test_export_box():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    # Création de la boite Vol0000
    ctx.getGeomManager().newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))
    # Création d'un bloc topologique non structuré sur une géométrie (Vol0000)
    ctx.getTopoManager().newUnstructuredTopoOnGeometry ("Vol0000")
    # Création du maillage pour toutes les faces
    ctx.getMeshManager().newAllFacesMesh()
    ctx.getGeomManager().exportVTK(vtkfilename)
    assert os.path.exists(vtkfilename)
    assert os.path.getsize(vtkfilename) > 0
