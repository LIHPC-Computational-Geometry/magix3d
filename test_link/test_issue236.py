import pyMagix3D as Mgx3D

# Issue236 : segfault when generating the mesh, when a surfacic
# smoother is applied on a group of geometric surfaces, if one
# of those surfaces has no topological co-face associated to it.
# Here, we only test if the mesh generation succeed, by checking
# the final number of mesh nodes. We don't check the result of
# the smoother in terms of final mesh node positions.

def test_issue236():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()
    # Création de la boite Vol0000
    ctx.getGeomManager().newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))
    # Création d'un bloc topologique structuré sans projection (Vol0000)
    ctx.getTopoManager().newFreeTopoOnGeometry ("Vol0000")
    # Affectation d'une projection vers Surf0005 pour les entités topologiques Fa0005
    ctx.getTopoManager().setGeomAssociation (["Fa0005"], "Surf0005", True)
    # Modifie le groupe toSmooth
    ctx.getGeomManager().addToGroup (["Surf0001", "Surf0005"], 2, "toSmooth")
    # Ajoute une modification de maillage
    ctx.getGroupManager().addSmoothing ("toSmooth",Mgx3D.SurfacicSmoothing())
    # Création du maillage pour tous les blocs
    ctx.getMeshManager().newAllBlocksMesh()
    assert(mm.getNbNodes() == 1331)