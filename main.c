//=============================================================================
// Projet : ImpScreen
// Version : 0.1
// Fichier : main.c
// Auteur : AXeL
// Date de création : 17/11/2013
// Date de la dernière modification : 17/11/2013
// Lacunes : La vitesse de capture est parfois trop rapide, la suppression n'est pas fiable aussi
// Idées d'amélioration : - Afficher un aperçu des images capturées dans la DialogBox
//                        - Au démarrage chercher les anciennes captures et les 
//                          insérer dans la liste des captures
//                        - Choix de l'emplacement de sauvegarde des captures
//                        - Meilleur gestion d'erreurs de suppression/copie/renommage...
//                        - Option d'attente d'un temp donné avant capture (timer)
//                        - Touche de capture (Impécr) => Hook clavier
//                        - Boutton pour supprimer tout les éléments de la ListView
//
//=============================================================================

#include "resources.h"


//=============================================================================
//                          Fonction principale
//=============================================================================

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrecedente, LPSTR lpCmdLine, int nCmdShow)
{
   hInst = hInstance;
   DialogBox(hInstance, "DIALOGBOXPRINCIPALE", NULL, (DLGPROC)DlgPrincipaleProc);
   return 0;    
}

//=============================================================================
//                         DialogBox principale
//=============================================================================

BOOL APIENTRY DlgPrincipaleProc(HWND hMain, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch (uMsg)
   {
       case WM_INITDIALOG:
            hGeneral = hMain;
            /* Affichage de l'icone */
            SendMessage(hMain, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICONE)));
            /* Création de la ToolBar */
            TBADDBITMAP tbab; 
            TBBUTTON tbb[TOOLBAR_SIZE];
            InitCommonControls(); /* Initialisation des contrôles communs */
            
            HWND hToolBar = CreateWindowEx(WS_EX_CLIENTEDGE, TOOLBARCLASSNAME, NULL, 
                                           WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT, 
                                           0, 0, 0, 0, hMain, NULL, hInst, NULL);
           
            tbab.hInst =  HINST_COMMCTRL ; 
            tbab.nID   =  IDB_STD_LARGE_COLOR; 
            SendMessage(hToolBar, TB_ADDBITMAP, 1, (WPARAM)&tbab);
            
            /* Création des buttons */
            TBADDBITMAP bitmap;
            bitmap.nID = IDB_CAPTURER;
            bitmap.hInst = hInst;
            creerToolBarButton(hToolBar, tbb, 0, SendMessage(hToolBar, TB_ADDBITMAP, 0, (LPARAM)&bitmap), IDM_CAPTURER, "Capturer");
            creerToolBarButton(hToolBar, tbb, 1, STD_REPLACE, IDM_RENOMMER, "Renommer");
            creerToolBarButton(hToolBar, tbb, 2, STD_FILEOPEN, IDM_OUVRIR, "Ouvrir");
            creerToolBarButton(hToolBar, tbb, 3, STD_COPY, IDM_COPIER, "Copier");
            creerToolBarButton(hToolBar, tbb, 4, STD_DELETE, IDM_SUPPRIMER, "Supprimer");
            creerToolBarSeparateur(tbb, 5); /* Séparateur */
            creerToolBarButton(hToolBar, tbb, 6, STD_HELP, IDM_APROPOS, "À Propos");
             
            SendMessage(hToolBar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
            SendMessage(hToolBar, TB_ADDBUTTONS, TOOLBAR_SIZE, (LPARAM)&tbb); 
            SendMessage(hToolBar, TB_AUTOSIZE, 0, 0);
            /* Création des colones de la ListView */
            HANDLE hListView = GetDlgItem(hMain, LST_CAPTURES);
            creerListViewColone(&hListView, 150, "Capture", 0);
            creerListViewColone(&hListView, 180, "Emplacement", 1);
            creerListViewColone(&hListView, 60, "Taille", 2);
            SendMessage(hListView, LVM_SETEXTENDEDLISTVIEWSTYLE , 0, LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP); /* Gestion des styles */
            break;
       case WM_NOTIFY:
            {
            /* Changement de l'item actuel de la ListView */
            LPNM_LISTVIEW pnlv = (LPNM_LISTVIEW)lParam;
            if (pnlv->hdr.code == LVN_ITEMCHANGED)
               LvItemActuel = pnlv->iItem;
            }
            break;
       case WM_COMMAND:
            switch(HIWORD(wParam))
            {
                case BN_CLICKED:
                     switch (LOWORD(wParam))
                     {
                         case IDM_CAPTURER:
                              DialogBox(hInst, "CAPTURER", hMain, (DLGPROC)CapturerDlgProc); /* Capture de l'écran */
                              break;
                         case IDM_RENOMMER:
                              recupererEmplacementCapture(hMain);
                              if (capture[0] != '\0')
                                 DialogBox(hInst, "RENOMMER", hMain, (DLGPROC)RenommerDlgProc);
                              break;
                         case IDM_OUVRIR:
                              recupererEmplacementCapture(hMain);
                              if (capture[0] != '\0')
                                 ShellExecute(NULL, "open", capture, NULL, NULL, SW_SHOWNORMAL);
                              break;
                         case IDM_COPIER:
                              recupererEmplacementCapture(hMain);
                              if (capture[0] != '\0')
                              {
                                 char explorateurPath[MAX_PATH];
                                 BROWSEINFO explorateur;
                                 LPITEMIDLIST lip;
                                 explorateurPath[0] = 0;
          
                                 explorateur.hwndOwner = hMain;
                                 explorateur.pidlRoot = 0;
                                 explorateur.lpfn = 0;
                                 explorateur.ulFlags = 0;
                                 explorateur.lParam = 0;
                                 explorateur.iImage = 0;
                                 explorateur.pszDisplayName = explorateurPath;
                                 explorateur.lpszTitle = "Copier dans :";
                                 lip = SHBrowseForFolder(&explorateur);
   
                                 if (lip != NULL)
                                 {
                                    SHGetPathFromIDList(lip, explorateurPath);
                                    if (explorateurPath[0] != 0)
                                    {
                                       char tmp[32];
                                       ListView_GetItemText(GetDlgItem(hMain, LST_CAPTURES), LvItemActuel, 0, tmp, 32);
                                       lstrcat(explorateurPath, "\\");
                                       lstrcat(explorateurPath, tmp);
                                       CopyFile(capture, explorateurPath, FALSE);
                                       MessageBox(hMain, "Capture copiée !", NOM_APP, MB_OK | MB_ICONINFORMATION);
                                    }
                                    else
                                       MessageBox(hMain, "Echec de la copie !", NOM_APP, MB_OK | MB_ICONWARNING);
                                    CoTaskMemFree(lip);
                                 }                 
                              }
                              break;
                         case IDM_SUPPRIMER:
                              recupererEmplacementCapture(hMain);
                              if (capture[0] != '\0')
                              {
                                 DeleteFile(capture); /* Suppression de la capture */
                                 /* Suppression de la ListView */
                                 ListView_DeleteItem(GetDlgItem(hMain, LST_CAPTURES), LvItemActuel);
                              }
                              break;
                         case IDM_APROPOS:
                              MessageBox(hMain, NOM_APP " " APP_VERSION
                              "\nPar : AXeL"
                              "\nCompatibilité : Windows XP/7"
                              "\nLibre de droit et d'utilisations."
                              , "À Propos", MB_OK | MB_ICONINFORMATION);
                              break;
                         case IDCANCEL:
                              EndDialog(hMain, 0);
                              break;       
                     }
                     break;
            }
            break;
   }
   
   return 0;
}

//=============================================================================
//               Fonction qui crée les buttons de la ToolBar
//=============================================================================

void creerToolBarButton(HWND htb, TBBUTTON *tbb, UINT i, UINT bitmap, UINT id, char *nom)
{
   tbb[i].iBitmap = bitmap;
   tbb[i].idCommand = id; 
   tbb[i].fsState = TBSTATE_ENABLED; 
   tbb[i].fsStyle = TBSTYLE_BUTTON; 
   tbb[i].dwData = 0; 
   tbb[i].iString = SendMessage(htb, TB_ADDSTRING, 0, (LPARAM)nom);
}

//=============================================================================
//              Fonction qui crée les séparateurs de la ToolBar
//=============================================================================

void creerToolBarSeparateur(TBBUTTON *tbb, UINT i)
{
   tbb[i].iBitmap = 0; 
   tbb[i].idCommand = -1; 
   tbb[i].fsState = 0; 
   tbb[i].fsStyle = TBSTYLE_SEP; 
   tbb[i].dwData = 0; 
   tbb[i].iString = -1;   
}

//=============================================================================
//              Fonction qui crée les colones de la ListView
//=============================================================================

void creerListViewColone(HANDLE *htmp, UINT largeurColone, char *nomColone, UINT idColone)
{
   LVCOLUMN lvc;
   lvc.mask = LVCF_TEXT | LVCF_WIDTH;
   lvc.cx = largeurColone;
   lvc.pszText = nomColone;
   ListView_InsertColumn(*htmp, idColone, &lvc);         
}

//=============================================================================
//                     DialogBox de capture d'écran
//=============================================================================

BOOL APIENTRY CapturerDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch (uMsg)
   {
       case WM_INITDIALOG:
            /* Affichage de l'icone */
            SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICONE)));
            /* Affichage de la Bitmap du button Capturer */
            SendDlgItemMessage(hwnd, BT_CAPTURER, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_CAPTURER)));
            /* Gestion de la TrackBar */
            HWND hTrackBar = GetDlgItem(hwnd, TRB_TRACKBAR); 
            SendMessage(hTrackBar, TBM_SETRANGE,FALSE, MAKELONG(1, 20)); 
            SendMessage(hTrackBar, TBM_SETPOS,TRUE, 15); /* 75% */
            SetDlgItemInt(hwnd, TRB_TRACKBAR, 10, FALSE); 
            break;
       case WM_HSCROLL:
            /* TrackBar en mouvement */
            SetDlgItemInt(hwnd, TXT_QUALITY, SendMessage(GetDlgItem(hwnd, TRB_TRACKBAR), TBM_GETPOS, 0, 0)*5, FALSE); 
            break;
       case WM_COMMAND:
            switch(HIWORD(wParam))
            {
                case BN_CLICKED:
                     switch (LOWORD(wParam))
                     {
                         case BT_CAPTURER:
                              EndDialog(hwnd, 0); /* Fermeture de la boîte de dialog */
                              /* Si on ne veux pas que la fenêtre s'affiche dans la capture, on la cache */
                              if (IsDlgButtonChecked(hwnd, CHB_CACHER_FENETRE) == BST_CHECKED)
                              {
                                 ShowWindow(hGeneral, SW_HIDE);
                                 Sleep(100); /* On attend que la fenêtre se cache */
                              }
                              /* Création du dossier de sauvegarde, s'il existe ça ne changera rien */
                              CreateDirectory(SAVE_DIRECTORY, NULL);
                              capturerEcran(GetDlgItemInt(hwnd, TXT_QUALITY, 0, FALSE)); /* Impr écran */
                              /* Ajout à la ListView */
                              WIN32_FIND_DATA FindFileData; /* Structure d'informations sur le fichier trouvé par FindFirstFile() */
                              if (FindFirstFile(capture, &FindFileData) != INVALID_HANDLE_VALUE)
                              {
                                 HANDLE htmp = GetDlgItem(hGeneral, LST_CAPTURES);
   
                                 LVITEM lvi;
                                 lvi.mask = LVIF_TEXT | LVIF_PARAM;
                                 lvi.iSubItem = 0;
                                 lvi.lParam = LVM_SORTITEMS;
                                 lvi.pszText = "";
                                 //lvi.iItem = nbrCapture++;
                              
                                 int itemPos = ListView_InsertItem(htmp, &lvi); /* Position actuelle dans la ListView */
                                 ListView_SetItemText(htmp, itemPos, 0, FindFileData.cFileName); /* Nom capture */
                                 sprintf(capture, "%d ko", FindFileData.nFileSizeLow/1024); /* Taille (conversion en ko) */
                                 ListView_SetItemText(htmp, itemPos, 2, capture);
                                 GetCurrentDirectory(MAX_PATH, capture);
                                 lstrcat(capture, "\\");
                                 lstrcat(capture, SAVE_DIRECTORY);
                                 ListView_SetItemText(htmp, itemPos, 1, capture); /* Emplacement */
                              }
                              ShowWindow(hGeneral, SW_SHOW); /* Réaffichage de la fenêtre */
                              break;
                         case BT_ANNULER:
                         case IDCANCEL:
                              EndDialog(hwnd, 0);
                              break;      
                     }
                     break;
            }
            break;
   }
   
   return 0;
}

//=============================================================================
// Fonction qui récupère l'emplacement de la capture séléctionnée dans la ListView
//=============================================================================

void recupererEmplacementCapture(HWND hwnd)
{
   HANDLE htmp = GetDlgItem(hwnd, LST_CAPTURES);
   char buffer[32];
   
   /* Récupération de l'emplacement de la capture  */
   sprintf(capture, ""); /* Rénitialisation */
   ListView_GetItemText(htmp, LvItemActuel, 1, capture, MAX_PATH);
   if (capture[0] != '\0')
   {
      ListView_GetItemText(htmp, LvItemActuel, 0, buffer, 32);
      lstrcat(capture, "\\");
      lstrcat(capture, buffer);
   }     
}

//=============================================================================
//                   DialogBox de renommage de capture
//=============================================================================

BOOL APIENTRY RenommerDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch (uMsg)
   {
       case WM_INITDIALOG:
            /* Affichage de l'icone */
            SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICONE)));
            /* Récupération du nom de la capture + Affichage */
            char buffer[32];
            ListView_GetItemText(GetDlgItem(hGeneral, LST_CAPTURES), LvItemActuel, 0, buffer, 32);
            buffer[lstrlen(buffer) - 4] = '\0'; /* On enlève l'extension */
            SetDlgItemText(hwnd, IDE_NOM_CAPTURE, buffer);
            break;
       case WM_COMMAND:
            switch(HIWORD(wParam))
            {
                case BN_CLICKED:
                     switch (LOWORD(wParam))
                     {
                         case BT_RENOMMER:
                              {
                              char nvNomCapture[64], tmp[32];
                              GetDlgItemText(hwnd, IDE_NVNOM_CAPTURE, tmp, 32);
                              if (lstrlen(tmp))
                              {
                                 lstrcat(tmp, ".jpg");
                                 sprintf(nvNomCapture, "%s\\%s", SAVE_DIRECTORY, tmp);
                                 if (MoveFile(capture, nvNomCapture) != 0) /* Rennomage */
                                 {
                                    /* Modification du nom de la capture dans la ListView */
                                    ListView_SetItemText(GetDlgItem(hGeneral, LST_CAPTURES), LvItemActuel, 0, tmp);
                                    EndDialog(hwnd, 0);
                                    MessageBox(hGeneral, "Capture renommée !", NOM_APP, MB_OK | MB_ICONINFORMATION);
                                 }
                                 else
                                    MessageBox(hwnd, "Erreur, nom invalide !", NOM_APP, MB_OK | MB_ICONWARNING);
                              }
                              else
                                 MessageBox(hwnd, "Veuillez entrer un nom !", NOM_APP, MB_OK | MB_ICONSTOP);
                               break;
                              }
                         case BT_ANNULER2:
                         case IDCANCEL:
                              EndDialog(hwnd, 0);
                              break;      
                     }
                     break;
            }
            break;   
   }
   
   return 0;
}
