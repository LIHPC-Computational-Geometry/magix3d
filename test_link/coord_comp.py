import math
import csv
import itertools

def ecrire_points(gm, fichier):
    points = gm.getVertices()

    with open(fichier + "_points.csv", "w", encoding="utf-8") as f:
        # Écriture d'un en-tête
        f.write("Nom_Point;X;Y;Z\n")
        
        # Parcours de tous les points
        for nom_point in points:
            coord = gm.getCoord(nom_point)
            x = coord.getX()
            y = coord.getY()
            z = coord.getZ()
            
            # Écriture de la ligne correspondante
            f.write(f"{nom_point};{x};{y};{z}\n")


def distance(p1, p2):
    """Calcule la distance euclidienne entre deux points 3D."""
    return math.sqrt((p1[0] - p2[0])**2 +
                    (p1[1] - p2[1])**2 +
                    (p1[2] - p2[2])**2)


def lire_points(fichier):
    """Lit un fichier de points et retourne un dict {nom: (x, y, z)}"""
    points = {}
    with open(fichier + "_points.csv", newline='', encoding='utf-8') as f:
        lecteur = csv.DictReader(f, delimiter=';')
        for ligne in lecteur:
            nom = ligne['Nom_Point']
            x = float(ligne['X'])
            y = float(ligne['Y'])
            z = float(ligne['Z'])
            points[nom] = (x, y, z)
    return points


def ecrire_courbes(gm, fichier):
    with open(fichier + "_courbes.csv", "w", encoding="utf-8", newline='') as f:
        writer = csv.writer(f, delimiter=';')
        writer.writerow(["Nom_Courbe", "Points"])
        
        for curve_name in gm.getCurves():
            # Récupération de la liste des points de la courbe
            pts = gm.getInfos(curve_name, 1).vertices()
            writer.writerow([curve_name] + pts)


def ecrire_surfaces(gm, fichier):
    """
    Exporte les surfaces d'un objet gm dans un fichier texte.
    Chaque ligne contient : Nom_Surface;Point1;Point2;Point3;...
    """

    ecrire_points(gm, fichier)
    ecrire_courbes(gm, fichier)

    with open(fichier + "_surfaces.csv", "w", encoding="utf-8", newline='') as f:
        writer = csv.writer(f, delimiter=';')
        writer.writerow(["Nom_Surface", "Points"])
        
        for surf_name in gm.getSurfaces():
            # Récupération de la liste des points de la surface
            pts = gm.getInfos(surf_name, 2).vertices()
            writer.writerow([surf_name] + pts)


def lire_courbes(fichier):
    courbes = {}
    with open(fichier + "_courbes.csv", newline='', encoding='utf-8') as f:
        lecteur = csv.reader(f, delimiter=';')
        next(lecteur)  # ignorer l'en-tête
        for ligne in lecteur:
            nom_courbe = ligne[0]
            points = ligne[1:]
            courbes[nom_courbe] = points
    return courbes


def lire_surfaces(fichier):
    """
    Lit un fichier de surfaces et retourne un dict :
    {nom_surface: [liste_points]}
    """
    surfaces = {}
    with open(fichier + "_surfaces.csv", newline='', encoding='utf-8') as f:
        lecteur = csv.reader(f, delimiter=';')
        next(lecteur)  # ignorer l'en-tête
        for ligne in lecteur:
            nom_surface = ligne[0]
            points = ligne[1:]
            surfaces[nom_surface] = points
    return surfaces


def ecrire_correspondances(fichier1, fichier2, epsilon=1e-8):
    # Trouve les correspondances entre deux fichiers de points.
    pts1 = lire_points(fichier1)
    pts2 = lire_points(fichier2)
    corr_pts = {}
    for nom1, coord1 in pts1.items():
        candidats = []
        for nom2, coord2 in pts2.items():
            if math.dist(coord1, coord2) <= epsilon:
                candidats.append(nom2)
        corr_pts[nom1] = candidats
    print(corr_pts)

    # Trouve les correspondances entre les courbes.
    cvr1 = lire_courbes(fichier1)
    cvr2 = lire_courbes(fichier2)

    corr_crvs = []
    for nom1, pts1 in cvr1.items():
        # Liste de candidats pour chaque point
        candidats_par_point = [corr_pts[p] for p in pts1]
        # Test de toutes les combinaisons possibles
        for combinaison in itertools.product(*candidats_par_point):
            if set(combinaison) in [set(s) for s in cvr2.values()]:
                # Récupérer le nom de la courbe correspondante dans cvr2
                for nom2, pts2 in cvr2.items():
                    if set(combinaison) == set(pts2):
                        corr_crvs.append((nom1, nom2))
                        break
                break
    print(corr_crvs)

    # Trouve les correspondances entre les surfaces
    surf1 = lire_surfaces(fichier1)
    surf2 = lire_surfaces(fichier2)

    corr_surfs = []
    for nom1, pts1 in surf1.items():
        # Liste de candidats pour chaque point
        candidats_par_point = [corr_pts[p] for p in pts1]
        # Test de toutes les combinaisons possibles
        for combinaison in itertools.product(*candidats_par_point):
            if set(combinaison) in [set(s) for s in surf2.values()]:
                # Récupérer le nom de la surface correspondante dans surf2
                for nom2, pts2 in surf2.items():
                    if set(combinaison) == set(pts2):
                        corr_surfs.append((nom1, nom2))
                        break
                break
    print(corr_surfs)
