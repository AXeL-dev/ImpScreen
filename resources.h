//=============================================================================
// Projet : ImpScreen
// Fichier : resources.h
//
//=============================================================================

#define TBSTYLE_FLAT 0x0800
#include <windows.h>
#include <stdio.h>
#include <commctrl.h> // -lcomctl32
#include "jpeglib.h" // -lgdi32 
                    // libjpeg.a
#include <Shlobj.h> // -lole32


//=============================================================================
//                               Constantes
//=============================================================================

#define NOM_APP                       "ImpScreen"

#define APP_VERSION                   "v 0.1"

#define SAVE_DIRECTORY                "Captures"

#define IDI_ICONE                     100

#define TOOLBAR_SIZE                  6+1   /* +Séparateur */

/* Constantes de la ListView */
#define LVS_EX_FULLROWSELECT          0x20
#define LVS_EX_HEADERDRAGDROP         0x10
#define LVM_FIRST                     0x1000
#define LVM_SETEXTENDEDLISTVIEWSTYLE  LVM_FIRST|0x36

/* DialogBox principale */
#define IDB_CAPTURER                  101
#define IDM_CAPTURER                  102
#define IDM_RENOMMER                  103
#define IDM_OUVRIR                    104
#define IDM_COPIER                    105
#define IDM_SUPPRIMER                 106
#define IDM_APROPOS                   107
#define LST_CAPTURES                  108

/* DialogBox de capture d'écran */
#define TRB_TRACKBAR                  109
#define IDI_QUALITY                   110
#define TXT_QUALITY                   111
#define CHB_CACHER_FENETRE            112
#define BT_CAPTURER                   113
#define BT_ANNULER                    114

/* DialogBox de renommage de capture */
#define IDE_NOM_CAPTURE               115
#define IDE_NVNOM_CAPTURE             116
#define BT_RENOMMER                   117
#define BT_ANNULER2                   118

//=============================================================================
//                           Variables globales
//=============================================================================

HINSTANCE hInst;

HANDLE hGeneral;

unsigned short LvItemActuel;

char capture[MAX_PATH];
//int nbrCapture;

//=============================================================================
//                              Prototypes
//=============================================================================

BOOL APIENTRY DlgPrincipaleProc(HWND hMain, UINT uMsg, WPARAM wParam, LPARAM lParam);
void creerToolBarButton(HWND htb, TBBUTTON *tbb, UINT i, UINT bitmap, UINT id, char *nom);
void creerToolBarSeparateur(TBBUTTON *tbb, UINT i);
void creerListViewColone(HANDLE *htmp, UINT largeurColone, char *nomColone, UINT idColone);
BOOL APIENTRY CapturerDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void capturerEcran(UINT quality);
BOOL FillJpegBuffer(LPBYTE pBits, LPBITMAPINFOHEADER pbmih, int nSampsPerRow, JSAMPARRAY jsmpPixels);
BOOL JpegFromDib(LPBYTE pBits, LPBITMAPINFOHEADER pbmih, int nQuality, char *pathJpeg);
void recupererEmplacementCapture(HWND hwnd);
BOOL APIENTRY RenommerDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
