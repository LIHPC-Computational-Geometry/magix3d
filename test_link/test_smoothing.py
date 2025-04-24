import math
import pyMagix3D as Mgx3D

def test_smoothing_sphere(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    re = 6.456
    ro = math.sqrt(1.0086544512)
    ratio = ro / re

    ctx.getTopoManager().newSphereWithTopo (Mgx3D.Point(0, 0, 0), re, Mgx3D.Portion.HUITIEME, True, ratio, 11, 89, "MAT")
    # Modifie le groupe PX0
    ctx.getGeomManager().addToGroup (["Surf0003"], 2, "PX0")
    # Modifie le groupe PY0
    ctx.getGeomManager().addToGroup (["Surf0002"], 2, "PY0")
    # Modifie le groupe PZ0
    ctx.getGeomManager().addToGroup (["Surf0001"], 2, "PZ0")
    # Modifie le groupe EXTERN
    ctx.getGeomManager().addToGroup (["Surf0000"], 2, "EXTERN")
    # Ajoute une modification de maillage
    ctx.getGroupManager().addSmoothing ("EXTERN",Mgx3D.SurfacicSmoothing())
    # Ajoute une modification de maillage
    ctx.getGroupManager().addSmoothing ("PX0",Mgx3D.SurfacicSmoothing())
    # Ajoute une modification de maillage
    ctx.getGroupManager().addSmoothing ("PY0",Mgx3D.SurfacicSmoothing())
    # Ajoute une modification de maillage
    ctx.getGroupManager().addSmoothing ("PZ0",Mgx3D.SurfacicSmoothing())
    # Ajoute une modification de maillage => nécessite smooth3D pas dispo en logiciel libre
    #ctx.getGroupManager().addSmoothing ("MAT",Mgx3D.VolumicSmoothing())
    # Création du maillage pour tous les blocs
    ctx.getMeshManager().newAllBlocksMesh()

    out, err = capfd.readouterr()
    assert len(err) == 0