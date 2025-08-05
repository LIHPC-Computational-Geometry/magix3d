import pyMagix3D as Mgx3D

def test_order_join_curves():
	ctx = Mgx3D.getStdContext()
	ctx.clearSession() # Clean the session after the previous test
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
	gm.fuse (["Vol0005","Vol0006","Vol0007","Vol0008","Vol0009","Vol0010"])
	gm.joinSurfaces (["Surf0035","Surf0037","Surf0044","Surf0043","Surf0022","Surf0024"])
	gm.joinSurfaces (["Surf0021","Surf0020","Surf0032","Surf0031","Surf0041","Surf0040"])
	gm.joinSurfaces (["Surf0019","Surf0028","Surf0017","Surf0025","Surf0016","Surf0045","Surf0046"])
	gm.joinCurves (["Crb0021","Crb0020","Crb0048","Crb0050","Crb0024","Crb0035","Crb0033"])
	gm.joinCurves (["Crb0014","Crb0013","Crb0044","Crb0046","Crb0018","Crb0028","Crb0026"])

	assert gm.getVertices() == ["Pt0000", "Pt0001", "Pt0011", "Pt0013", "Pt0014", "Pt0015"]
