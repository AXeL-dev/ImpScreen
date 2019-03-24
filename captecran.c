//=============================================================================
// Projet : ImpScreen
// Fichier : captecran.c
//
//=============================================================================

#include "resources.h"


//=============================================================================
//                        Fonction de capture d'écran
//=============================================================================

void capturerEcran(UINT quality)
{
   /* Récupération de la date actuelle */
   SYSTEMTIME Time;
   GetSystemTime(&Time);
   
   /* Emplacement+nom de la capture */
   sprintf(capture, "%s\\%02d-%02d-%d - %02dh%02dm%02ds%02dms.jpg", SAVE_DIRECTORY, Time.wDay
                    , Time.wMonth, Time.wYear, Time.wHour, Time.wMinute, Time.wSecond, Time.wMilliseconds);
  
   HDC memDC,hDC;
   HBITMAP Hbmp;
   BITMAPINFO BmpInfo;
   void *pBits;
   DWORD Width, Height;
   LPBITMAPINFOHEADER pBmpInfoHeader;
  
   HWND HFen = 0;
  
   if ((memDC = GetDC(HFen)) != 0)
   {
      if ((hDC = CreateCompatibleDC(memDC)) != 0)
      {
         Height = GetDeviceCaps(memDC,VERTRES);
         Width = GetDeviceCaps(memDC,HORZRES);

         if (Height && Width)
         {
            /* Information de la strcuture du BMP (Bitmap) */
            BmpInfo.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
            BmpInfo.bmiHeader.biWidth         = Width;
            BmpInfo.bmiHeader.biHeight        = Height;
            BmpInfo.bmiHeader.biPlanes        = 1;
            BmpInfo.bmiHeader.biBitCount      = 24;
            BmpInfo.bmiHeader.biCompression   = BI_RGB;
            BmpInfo.bmiHeader.biSizeImage     = 0;
            BmpInfo.bmiHeader.biSizeImage     = 0;
            BmpInfo.bmiHeader.biXPelsPerMeter = 0;
            BmpInfo.bmiHeader.biYPelsPerMeter = 0;
            BmpInfo.bmiHeader.biClrUsed       = 0;
            BmpInfo.bmiHeader.biClrImportant  = 0;
         
            pBmpInfoHeader = &BmpInfo.bmiHeader;
      
            if ((Hbmp = CreateDIBSection(0, &BmpInfo, DIB_RGB_COLORS, &pBits, 0, 0)) != 0)
            {
               if (SelectObject(hDC,Hbmp)!=0)
               {
                  if (BitBlt(hDC, 0, 0, Width, Height, memDC, 0, 0, SRCCOPY))
                     JpegFromDib(pBits, pBmpInfoHeader, quality, capture);
               }
               //free(pBits);
               //free(pBmpInfoHeader);
            }
         }
      }
      DeleteObject(Hbmp);
      DeleteDC(memDC);
      DeleteDC(hDC);
   }     
}

//=============================================================================
//           Fonctions de conversion direct du hdc/BMP en jpeg
//=============================================================================

BOOL FillJpegBuffer(LPBYTE pBits, LPBITMAPINFOHEADER pbmih, int nSampsPerRow, JSAMPARRAY jsmpPixels)
{
   if (pBits == NULL || nSampsPerRow <= 0) return 0;
   
   int r=0, p=0, q=0, nUnused=0, nBytesWide=0, nRow=0, nPixel=0;

   nBytesWide =  (pbmih->biWidth*3);
   nUnused    =  (((nBytesWide + 3) / 4) * 4) - nBytesWide;
   nBytesWide += nUnused;

   for (r=0; r<pbmih->biHeight; r++)
   {
      for (p=0, q=0; p<(nBytesWide-nUnused); p+=3, q+=3)
      { 
         nRow = (pbmih->biHeight-r-1) * nBytesWide;
         nPixel  = nRow + p;

         jsmpPixels[r][q+0] = pBits[nPixel+2]; /* Red */
         jsmpPixels[r][q+1] = pBits[nPixel+1]; /* Green */
         jsmpPixels[r][q+2] = pBits[nPixel+0]; /* Blue */
      }
   }
   return TRUE;
}

BOOL JpegFromDib(LPBYTE pBits, LPBITMAPINFOHEADER pbmih, int nQuality, char *pathJpeg)             
{
    if (nQuality < 0 || nQuality > 100 || pBits   == NULL || pathJpeg == "") return 0;
    
    /* Utilisation de la libjpeg(.a) pour écrire les scanlines sur le disque */
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr       jerr;
    
    FILE*      pOutFile;     /* Fichier cible/de sortie */
    int        nSampsPerRow; /* Longueur d'une ligne dans le buffer */
    JSAMPARRAY jsmpArray;    /* Buffer des pixels RGB pour le fichier jpeg */
    
    cinfo.err = jpeg_std_error(&jerr); /* Utilisation du default error handling */

    jpeg_create_compress(&cinfo);

    if ((pOutFile = fopen(pathJpeg, "wb")) == NULL) /* Lecture/écriture binaire */
    {
       jpeg_destroy_compress(&cinfo);
       return FALSE;
    }
    
    jpeg_stdio_dest(&cinfo, pOutFile);
    cinfo.image_width      = pbmih->biWidth;  /* Largeur et hauteur de l'image en pixels */
    cinfo.image_height     = pbmih->biHeight;
    cinfo.input_components = 3;              /* Composants couleur par pixel */                                     
    cinfo.in_color_space   = JCS_RGB; 	     /* Colorspace of input image */
    jpeg_set_defaults(&cinfo);               /* Paramétres par défaut dans cinfo */
    jpeg_set_quality(&cinfo, nQuality, 1);    
    jpeg_start_compress(&cinfo, TRUE);
    nSampsPerRow = cinfo.image_width * cinfo.input_components;
    jsmpArray = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, nSampsPerRow, cinfo.image_height);
                 
    if (FillJpegBuffer(pBits, pbmih, nSampsPerRow, jsmpArray))
       jpeg_write_scanlines(&cinfo, jsmpArray, cinfo.image_height);
    
    jpeg_finish_compress(&cinfo);
    fclose(pOutFile);
    jpeg_destroy_compress(&cinfo);

    return TRUE;
}

