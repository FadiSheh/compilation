import subprocess
from os import listdir
from os.path import isfile, join


class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'



process = subprocess.Popen(['make', 'realclean'])  
process.wait() 
process= subprocess.Popen(['make',])  
process.wait()

print("#######################################################################################",end="\n")
print("		          	      TESTS                                           ",end="\n")
print("#######################################################################################",end="\n")


print(bcolors.BOLD +"				  TESTS SYNTAXE"+bcolors.ENDC)
print(" ")
print("TESTS SYNTAXE KO")

monRepertoire= 'Tests/Syntaxe/KO/'
fichiers = [f for f in listdir(monRepertoire) if isfile(join(monRepertoire, f))]

taille = len(fichiers)
print(taille," fichiers KO à tester:")
print(" ")

for k in range(taille):

	nom = str(fichiers[k]) 
	#print("Fichier testé: ",nom,end="\n")
	atester= monRepertoire + nom

	
	result = subprocess.Popen(['./minicc', atester],stdout=subprocess.PIPE,stderr=subprocess.PIPE)
	result.wait()
	streamdata = result.communicate()[0]
	rc = result.returncode
	
	if rc!=0:

		print(bcolors.OKGREEN + nom + bcolors.ENDC)

	else:

		print(bcolors.FAIL+ "ERREUR " + nom + bcolors.ENDC)


print(" ")
print(" ")
print("TESTS SYNTAXE OK")

monRepertoire= 'Tests/Syntaxe/OK/'
fichiers = [f for f in listdir(monRepertoire) if isfile(join(monRepertoire, f))]

taille = len(fichiers)
print(taille," fichiers OK à tester:")
print(" ")

for k in range(taille):

	nom = str(fichiers[k]) 
	#print("Fichier testé: ",nom,end="\n")
	atester= monRepertoire + nom

	
	result = subprocess.Popen(['./minicc', atester],stdout=subprocess.PIPE,stderr=subprocess.PIPE)
	result.wait()
	streamdata = result.communicate()[0]
	rc = result.returncode
	
	if rc==0:

		print(bcolors.OKGREEN + nom + bcolors.ENDC)

	else:

		print(bcolors.FAIL+ "ERREUR " + nom + bcolors.ENDC)


print(" ")
print("#######################################################################################",end="\n")
print(bcolors.BOLD +"				  TESTS VERIF"+bcolors.ENDC)
print(" ")
print("TESTS VERIF KO")

monRepertoire= 'Tests/Verif/KO/'
fichiers = [f for f in listdir(monRepertoire) if isfile(join(monRepertoire, f))]

taille = len(fichiers)
print(taille," fichiers KO à tester:")
print(" ")

for k in range(taille):

	nom = str(fichiers[k]) 
	#print("Fichier testé: ",nom,end="\n")
	atester= monRepertoire + nom

	
	result = subprocess.Popen(['./minicc', atester],stdout=subprocess.PIPE,stderr=subprocess.PIPE)
	result.wait()
	streamdata = result.communicate()[0]
	rc = result.returncode
	
	if rc!=0:

		print(bcolors.OKGREEN + nom + bcolors.ENDC)

	else:

		print(bcolors.FAIL+ "ERREUR " + nom + bcolors.ENDC)


print(" ")
print(" ")
print("TESTS VERIF OK")

monRepertoire= 'Tests/Verif/OK/'
fichiers = [f for f in listdir(monRepertoire) if isfile(join(monRepertoire, f))]

taille = len(fichiers)
print(taille," fichiers OK à tester:")
print(" ")

for k in range(taille):

	nom = str(fichiers[k]) 
	#print("Fichier testé: ",nom,end="\n")
	atester= monRepertoire + nom

	
	result = subprocess.Popen(['./minicc', atester],stdout=subprocess.PIPE,stderr=subprocess.PIPE)
	result.wait()
	streamdata = result.communicate()[0]
	rc = result.returncode
	
	if rc==0:

		print(bcolors.OKGREEN + nom + bcolors.ENDC)

	else:

		print(bcolors.FAIL+ "ERREUR " + nom + bcolors.ENDC)







print(" ")
print("#######################################################################################",end="\n")
print(bcolors.BOLD +"				  TESTS GENCODE"+bcolors.ENDC)
print(" ")
print("TESTS GENCODE KO")

monRepertoire= 'Tests/Gencode/KO/'
fichiers = [f for f in listdir(monRepertoire) if isfile(join(monRepertoire, f))]

taille = len(fichiers)
print(taille," fichiers KO à tester:")
print(" ")

for k in range(taille):

	nom = str(fichiers[k]) 
	#print("Fichier testé: ",nom,end="\n")
	atester= monRepertoire + nom

	
	result = subprocess.Popen(['./minicc', atester],stdout=subprocess.PIPE,stderr=subprocess.PIPE)
	result.wait()
	streamdata = result.communicate()[0]
	rc = result.returncode
	
	if rc!=0:

		print(bcolors.OKGREEN + nom + bcolors.ENDC)

	else:

		print(bcolors.FAIL+ "ERREUR " + nom + bcolors.ENDC)


print(" ")
print(" ")
print("TESTS GENCODE OK")

monRepertoire= 'Tests/Gencode/OK/'
fichiers = [f for f in listdir(monRepertoire) if isfile(join(monRepertoire, f))]

taille = len(fichiers)
print(taille," fichiers OK à tester:")
print(" ")

for k in range(taille):

	nom = str(fichiers[k]) 
	#print("Fichier testé: ",nom,end="\n")
	atester= monRepertoire + nom

	
	result = subprocess.Popen(['./minicc', atester],stdout=subprocess.PIPE,stderr=subprocess.PIPE)
	result.wait()
	streamdata = result.communicate()[0]
	rc = result.returncode
	
	if rc==0:

		print(bcolors.OKGREEN + nom + bcolors.ENDC)

	else:

		print(bcolors.FAIL+ "ERREUR " + nom + bcolors.ENDC)


