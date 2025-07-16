import pyMagix3D as Mgx3D
import pytest

def test_issue78(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test

    # Création du sommet Pt0000
    ctx.getGeomManager().newVertex (Mgx3D.Point(0, 0, 0))
    # Création du sommet Pt0001
    ctx.getGeomManager().newVertex (Mgx3D.Point(1, 0, 0))
    # Création du sommet Pt0002
    ctx.getGeomManager().newVertex (Mgx3D.Point(1, 2, 0))
    # Création du sommet Pt0003
    ctx.getGeomManager().newVertex (Mgx3D.Point(0, 2, 0))
    # Création du segment Crb0000
    ctx.getGeomManager().newSegment("Pt0000", "Pt0001")
    # Création du segment Crb0001
    ctx.getGeomManager().newSegment("Pt0001", "Pt0002")
    # Création du segment Crb0002
    ctx.getGeomManager().newSegment("Pt0002", "Pt0003")
    # Création du segment Crb0003
    ctx.getGeomManager().newSegment("Pt0003", "Pt0000")
    # Création de la surface Surf0000
    ctx.getGeomManager().newPlanarSurface( ["Crb0000", "Crb0001", "Crb0002", "Crb0003"] , "")
    # Création d'une face topologique structurée sur une géométrie (Surf0000)
    ctx.getTopoManager().newStructuredTopoOnGeometry ("Surf0000")
    # Découpage de l'arête Ar0000
    ctx.getTopoManager().splitEdge ("Ar0000", .4)
    # Découpage de la face Fa0000 par prolongation
    ctx.getTopoManager().extendSplitFace ("Fa0000", "Som0004")
    assert ctx.getTopoManager().getNbFaces()==2
    ctx.undo()
    assert ctx.getTopoManager().getNbFaces()==1
    # Changement de discrétisation pour les arêtes Ar0002
    emp = Mgx3D.EdgeMeshingPropertyUniform(3)
    ctx.getTopoManager().setMeshingProperty (emp,["Ar0002"])
    with pytest.raises(RuntimeError) as excinfo:
        # Impossible car les discrétisations ne sont pas compatibles
        ctx.getTopoManager().extendSplitFace ("Fa0000", "Som0004")
    expected = "Echec lors du découpage de la face Fa0000, le nombre d'arêtes de maillage est différent de part et d'autre de la face (10 vs 3)"
    assert expected in str(excinfo.value)