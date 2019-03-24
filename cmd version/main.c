//=============================================================================
// Projet : ImpScreen (cmd version)
// Version : 0.1
// Fichier : main.c
// Auteur : AXeL
// Date de création : 26/01/2014
// Date de la dernière modification : 26/01/2014
// Lacunes :
//
//=============================================================================

#include "resources.h"


//=============================================================================
//                          Fonction principale
//=============================================================================

int main(int argc, char *argv[])
{
   if (!lstrcmp(argv[0], NOM_APP))
      capturerEcran(argv[1], strtol(argv[2], NULL, 10)); /* On capture l'écran */       
      
   return 0;
}
