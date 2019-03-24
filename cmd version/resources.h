//=============================================================================
// Projet : ImpScreen
// Fichier : resources.h
//
//=============================================================================

#include <windows.h>
#include <stdio.h>
#include "jpeglib.h" // -lgdi32 
                    // libjpeg.a


//=============================================================================
//                               Constantes
//=============================================================================

#define NOM_APP                       "impscreen"

//=============================================================================
//                              Prototypes
//=============================================================================

void capturerEcran(char *emplacement, UINT quality);
BOOL FillJpegBuffer(LPBYTE pBits, LPBITMAPINFOHEADER pbmih, int nSampsPerRow, JSAMPARRAY jsmpPixels);
BOOL JpegFromDib(LPBYTE pBits, LPBITMAPINFOHEADER pbmih, int nQuality, char *pathJpeg);
