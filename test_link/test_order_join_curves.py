import pyMagix3D as Mgx3D

def test_order_join_curves():
	ctx = Mgx3D.getStdContext()
	gm = ctx.getGeomManager()

	# Création du cylindre Vol0000
	gm.newCylinder (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Vector(1, 0, 0), Mgx3D.Portion.ENTIER)
	# Section par un plan de Vol0000 suivant  [  [ -7.17626e-2, 7.80879e-3, -5.98606e-1] ,  [ 0, 0, 1] ] 
	gm.sectionByPlane (["Vol0000"], Mgx3D.Plane(Mgx3D.Point(-7.17626e-2, 7.80879e-3, -5.98606e-1), Mgx3D.Vector(0, 0, 1)), "")
	# Section par un plan de Vol0002 suivant  [  [ 6.89238e-1, 7.80879e-3, 1.62395e-1] ,  [ 7.07107e-1, 0, 7.07107e-1] ] 
	gm.sectionByPlane (["Vol0002"], Mgx3D.Plane(Mgx3D.Point(6.89238e-1, 7.80879e-3, 1.62395e-1), Mgx3D.Vector(7.07107e-1, 0, 7.07107e-1)), "")
	# Section par un plan de Vol0003 Vol0004 Vol0001 suivant  [  [ 6.89238e-1, -2.63032e-1, 1.62395e-1] ,  [ 0, 1, 0] ] 
	gm.sectionByPlane (["Vol0003","Vol0004","Vol0001"], Mgx3D.Plane(Mgx3D.Point(6.89238e-1, -2.63032e-1, 1.62395e-1), Mgx3D.Vector(0, 1, 0)), "")
	# Translation de tout
	gm.translateAll(Mgx3D.Vector(1, 0, 0))
	# Homothétie de tout
	gm.scaleAll(1.000000e+00, 2.000000e+00, 3.000000e+00)
	# Fusion Booléenne de  Vol0009 Vol0006 Vol0005 Vol0010 Vol0008 ... 
	gm.fuse (["Vol0009","Vol0006","Vol0005","Vol0010","Vol0008","Vol0007"])
	# Fusion de surfaces Surf0030 Surf0038 Surf0037 Surf0032 Surf0026 ... 
	gm.joinSurfaces (["Surf0030","Surf0038","Surf0037","Surf0032","Surf0026","Surf0033"])
	# Fusion de surfaces Surf0025 Surf0020 Surf0024 Surf0021 Surf0023 ... 
	gm.joinSurfaces (["Surf0025","Surf0020","Surf0024","Surf0021","Surf0023","Surf0022"])
	# Fusion de surfaces Surf0019 Surf0042 Surf0017 Surf0034 Surf0016 ... 
	gm.joinSurfaces (["Surf0019","Surf0042","Surf0017","Surf0034","Surf0016","Surf0040","Surf0041"])
	# Fusion de courbes Crb0021 Crb0020 Crb0040 Crb0050 Crb0024 ... 
	gm.joinCurves (["Crb0021","Crb0020","Crb0040","Crb0050","Crb0024","Crb0045","Crb0043"])
	# Fusion de courbes Crb0014 Crb0013 Crb0026 Crb0038 Crb0018 ... 
	gm.joinCurves (["Crb0014","Crb0013","Crb0026","Crb0038","Crb0018","Crb0030","Crb0028"])

	assert gm.getVertices() == ["Pt0000", "Pt0001", "Pt0011", "Pt0012", "Pt0015", "Pt0016"]

