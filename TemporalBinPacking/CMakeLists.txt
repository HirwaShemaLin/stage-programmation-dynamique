# L'outil CMake permet de générér automatiquement le fichier Makefile d'un projet. Pour utiliser cet outil, il faut écrire un fichier nommé CMakeLists.txt. L'intérêt est que son écriture est relativement simple.
# Où copier le fichier ?
#        Il faut le placer à la racine du projet (à l'endroit où se trouve les dossiers src et data)
#
#    Comment compiler la première fois ?
#       1. A la racine de votre projet (où vous avez placé votre fichier CMakeLists.txt), créer un dossier appelé build avec la commande suivante : "mkdir build")
#       2. Se placer dans le dossier build avec la commande suivante : "cd build"
#       3. Créer le fichier Makefile associé à votre projet avec la commande suivante : "cmake .." (compilation en mode RELEASE)  OU "cmake -DCMAKE_BUILD_TYPE=Debug .." (compilation en mode DEBUG) OU "cmake -DCMAKE_BUILD_TYPE=Release .." (compilation en mode RELEASE)
#       4. Si la création du fichier Makefile a fonctionné (vous voyez dans la console "-- Generation done"), vous pouvez simplement lancer la compilation comme vous le faites habituellement. Pusique la cible s'appelle ici gurobiSolverCWLP (il est possible de modifier ce nom dans le fichier CMakeLists.txt), il faut taper la commande suivante : make gurobiSolverCWLP.
#       5. Si la compilation n'échoue pas, un exécutable appelé gurobiSolverCWLP a été créé. Vous pouvez le lancer en tapant la commande suivante :  ./gurobiSolverCWLP

#    Comment compiler après avoir effectué des modifications ?
#        si vous n'avez pas ajouté pas de nouveaux fichiers au dossier src,il suffit de taper à nouveau la commande suivante : make gurobiSolverCWLP (en se placant d'abord dans le dossier build)
 #       si vous avez ajouté de nouveaux fichiers au dossier src,il suffit de reprendre à partir de l'étape 2 ci-dessus

cmake_minimum_required (VERSION 2.6)

#------------------- CREATION DES VARIABLES POUR LA BIBLIOTHEQUE GUROBI -----------------------
# ATTENTION -> Il faut modifier le chemin GUROBI_ROOT ci-dessous
#set(GUROBI_ROOT "/opt/local/stow/gurobi910/linux64/" )

# On créé une variable stockant le dossier où se situent les fichiers en-tête du solveur GUROBI (pour la compilation)
#set(GUROBI_INCLUDE_DIR "${GUROBI_ROOT}include")
#message(STATUS "GUROBI_INCLUDE_DIR : ${GUROBI_INCLUDE_DIR}") #On affiche le dossier précédent (pour DEBUG)

# On créé des variables stockant l'emplacement des biblothèques/librairies du solveur GUROBI (pour l'édition des liens)
#set(GUROBI_LIBRARY_CPLUS "${GUROBI_ROOT}lib/libgurobi_c++.a")
# ATTENTION -> Selon la version utilisée, il faut modifier le nom de la librairie ci-dessous
#set(GUROBI_LIBRARY "${GUROBI_ROOT}lib/libgurobi91.so")
#set(GUROBI_LIBRARIES ${GUROBI_LIBRARY_CPLUS} ${GUROBI_LIBRARY})
#message(STATUS "GUROBI_LIBRARIES : ${GUROBI_LIBRARIES}") #On affiche les chemins vers les biblothèques (pour DEBUG)o


#------------------- PARTIE PRINCIPALE -----------------------
# Notre projet est étiqueté gurobiSolverCWLP
set(name "TBPP")
project (${name})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
find_library(GUROBI_LIBRARY_CPLUS libgurobi_c++.a OR gurobi_c++mdd2019.lib PATHS /opt/local/stow/gurobi910/linux64/lib/ /Library/gurobi903/mac64/lib/ /Library/gurobi951/macos_universal2/lib C:/gurobi951/win64/lib/ /opt/gurobi951/linux64/lib/)
find_library(GUROBI_LIBRARY libgurobi91.so OR libgurobi90.dylib OR libgurobi95.dylib OR libgurobi95.so OR gurobi95.lib PATHS /opt/local/stow/gurobi910/linux64/lib/ /Library/gurobi903/mac64/lib/ /Library/gurobi951/macos_universal2/lib/ C:/gurobi951/win64/lib/ /opt/gurobi951/linux64/lib/)
set(GUROBI_LIBRARIES ${GUROBI_LIBRARY_CPLUS} ${GUROBI_LIBRARY})
message(STATUS "GUROBI_LIBRARIES : ${GUROBI_LIBRARIES}")

set(GUROBI_INCLUDE_DIR /opt/local/stow/gurobi910/linux64/include/ /Library/gurobi903/mac64/include/ /Library/gurobi951/macos_universal2/include/  C:/gurobi951/win64/include/ /opt/gurobi951/linux64/include/)



# On indique où trouver les définitions des fonctions du solveur GUROBI
include_directories(include ${GUROBI_INCLUDE_DIR})

# On cherche tous les fichiers .cpp et .hpp dans le dossier src (stockage dans la variable SRCS)
file(GLOB_RECURSE SRCS src/*.cpp include/*.hpp)
message(STATUS "SRCS : ${SRCS}") #On affiche les fichiers trouvés

# On indique que l'on veut un exécutable "gurobiSolverCWLP" compilé à partir des fichiers dans la variable SRCS
add_executable(${name} ${SRCS})

# On indique où trouver les biblothèques du solveur GUROBI
target_link_libraries(${name} ${GUROBI_LIBRARIES})



# On ajoute un lien symbolique vers le dossier data dans le dossier où se situera l'exécutable
# (cela permet d'indiquer le chemin d'une instance contenu dans le dossier data en indiquant simplement un chemin relatif et non absolu)
# Décommenter la ligne ci-dessous si vous avez un dossier data à la racine de votre projet
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/datas/ ${CMAKE_CURRENT_BINARY_DIR}/datas)
