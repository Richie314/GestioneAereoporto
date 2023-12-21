#include "framework.h"
#include "GestioneAereoporto.h"

/*

COMPILARE A 32 BIT WINDOWS

*/
#include <Windows.h>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cmath>
#pragma warning(disable: 4996)
//Classe aereo
struct Aereo
{
    struct Posizione
    {
        float lat, lon, h;
    } Pos;
    struct Velocita
    {
        float x, y;
    } V;//[km/h]
    char sNome[32];
    char sCodice[16];
    int iPasseggeri;
    struct InformazioniDisegno
    {
        float r;
        float pX;
        float pY;
        float angolo;
        float angolo1, angolo2;
    } DisInfo;
};
//Variabili globali
Aereo* pAerei = (Aereo*)NULL;
bool bMuovi = true;
#define NUMERO_AEREI 10
#define SCREEN_W 800
#define SCREEN_H 400
float DELTA_T = 0.05F;
float FloatCasuale(float fMin, float fMax)
{
    int rnd = rand();
    if (rand() % 2 == 0)
        rnd *= -1;
    return fMin + (fMax - fMin) * ((float)rnd
        / (float)RAND_MAX);
}

//Funzioni legate ad aerei

Aereo::Posizione PosizioneCasuale()
{
    Aereo::Posizione Pos;
    Pos.lat = FloatCasuale(-90.0, 90.0F);
    Pos.lon = FloatCasuale(-180.0, 180.0F);
    Pos.h = FloatCasuale(7500.0F, 11000.0F);
    return Pos;
}
Aereo::Velocita VelocitaCasuale()
{
    Aereo::Velocita V;
    float vTot = 0.0F;
    do {
        srand(time(NULL));
        V.x = FloatCasuale(-900.0F, 900.0F);
        V.y = FloatCasuale(-900.0F, 900.0F);
        vTot = sqrt(V.x * V.x + V.y * V.y);
    } while (vTot < 700.0F);//La velocita' e' compresa tra 700 e 1000 [km/h]
    return V;
}
void NomeCasuale(char sNome[32])
{
    const char* sNomi[] = {
        "Ryanair",
        "EasyJet",
        "Lufthansa",
        "Alitalia",
        "Air Italy",
        "Air France",
        "Turkish Airlines",
        "British Airways",
        "Delta Air Lines",
        "Allegiant Air",
        "Qatar Airways",
        "Japan Airlines",
        "Emirates",
        "Pan American"
    };
    strcpy(sNome, sNomi[rand() % 14]);
}
void GeneraDati()
{
    bMuovi = false;
    if (pAerei != (Aereo*)NULL)
    {
        free(pAerei);
    }
    pAerei = (Aereo*)malloc(NUMERO_AEREI * sizeof(Aereo));
    if (!pAerei)
        return;
    memset(pAerei, 0, NUMERO_AEREI * sizeof(Aereo));
    srand(time(NULL) - 300000);
    for (int i = 0; i < NUMERO_AEREI; i++)
    {
        pAerei[i].Pos = PosizioneCasuale();
        pAerei[i].V = VelocitaCasuale();
        pAerei[i].iPasseggeri = rand() % 128;
        for (int j = 1; j < 16; j++)
        {
            pAerei[i].sCodice[j] = static_cast<char>(abs(rand()) % 26) + 'A';
        }
        pAerei[i].sCodice[0] = '#';
        NomeCasuale(pAerei[i].sNome);
        pAerei[i].DisInfo.r = pAerei[i].Pos.h / 9000.0F * 15.0F;

        float vTot = sqrt(pAerei[i].V.x * pAerei[i].V.x 
            + pAerei[i].V.y * pAerei[i].V.y);
        //Punta
        pAerei[i].DisInfo.pX = pAerei[i].V.x 
            / vTot * pAerei[i].DisInfo.r;
        pAerei[i].DisInfo.pY = -pAerei[i].V.y 
            / vTot * pAerei[i].DisInfo.r;
        pAerei[i].DisInfo.angolo = 
            acos(pAerei[i].DisInfo.pX / pAerei[i].DisInfo.r);
        if (pAerei[i].DisInfo.pY < 0.0F)
            pAerei[i].DisInfo.angolo = 
            2.0F * 3.1415926F - pAerei[i].DisInfo.angolo;
        //Posteriore
        pAerei[i].DisInfo.angolo1 = pAerei[i].DisInfo.angolo 
            + 5.0F / 6.0F * 3.1415926F;
        pAerei[i].DisInfo.angolo2 = pAerei[i].DisInfo.angolo 
            - 5.0F / 6.0F * 3.1415926F;
        Sleep(500);
    }
    bMuovi = true;
}

//Funzione di gestione eventi
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

//Punto di ingresso
int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    //Creazione modello form
    WNDCLASSEXW wcex;
    memset(&wcex, 0, sizeof(WNDCLASSEX));
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIconW(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = L"ClasseForm";
    wcex.hIconSm = LoadIconW(wcex.hInstance, IDI_APPLICATION);
    RegisterClassExW(&wcex);
    HMENU hMenu = CreateMenu();
    InsertMenuA(hMenu, 0, MF_BYPOSITION | MF_STRING, 
        1, "Genera");
    InsertMenuA(hMenu, 1, MF_BYPOSITION | MF_STRING, 
        2, "Pausa");
    InsertMenuA(hMenu, 2, MF_BYPOSITION | MF_SEPARATOR, NULL, nullptr);
    InsertMenuA(hMenu, 3, MF_BYPOSITION | MF_STRING, 
        3, "Chiudi");
    GeneraDati();
    //Creazione finestra
    HWND hWnd = CreateWindowW(L"ClasseForm", L"Torre di controllo - Radar", 
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, 0, SCREEN_W, SCREEN_H, 
        nullptr, hMenu, hInstance, nullptr);
    if (!hWnd)
    {
        return FALSE;
    }
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    if (SetTimer(hWnd, 10, 100, NULL) == 0)
    {
        return FALSE;
    }
    //Gestione eventi
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}

//Funzioni di disegno
void DisegnaTorre(HDC hdc)
{
    //Base torre
    MoveToEx(hdc, SCREEN_W / 2 - 18, SCREEN_H / 2, NULL);
    LineTo(hdc, SCREEN_W / 2 - 18, SCREEN_H / 2 + 60);
    MoveToEx(hdc, SCREEN_W / 2 + 18, SCREEN_H / 2, NULL);
    LineTo(hdc, SCREEN_W / 2 + 18, SCREEN_H / 2 + 60);

    //Parte superiore
    Ellipse(hdc, SCREEN_W / 2 - 30, SCREEN_H / 2 - 20,
        SCREEN_W / 2 + 30, SCREEN_H / 2 + 20);
    Ellipse(hdc, SCREEN_W / 2 - 24, SCREEN_H / 2 - 14,
        SCREEN_W / 2 + 24, SCREEN_H / 2 + 14);
    MoveToEx(hdc, SCREEN_W / 2 - 24, SCREEN_H / 2, NULL);
    LineTo(hdc, SCREEN_W / 2 - 24, SCREEN_H / 2 - 30);
    MoveToEx(hdc, SCREEN_W / 2 + 24, SCREEN_H / 2, NULL);
    LineTo(hdc, SCREEN_W / 2 + 24, SCREEN_H / 2 - 30);
    Ellipse(hdc, SCREEN_W / 2 - 24, SCREEN_H / 2 - 44,
        SCREEN_W / 2 + 24, SCREEN_H / 2 - 16);
}
void DisegnaAereo(HDC hdc, const Aereo& a)
{
    //Centro
    float x = SCREEN_W * (a.Pos.lon / 360.0F + 0.5F);
    float y = SCREEN_H * (a.Pos.lat / -180.0F + 0.5F);
    //Punte laterali
    float angolo1X = a.DisInfo.r * cos(a.DisInfo.angolo1) + x, 
        angolo1Y = a.DisInfo.r * sin(a.DisInfo.angolo1) + y;
    float angolo2X = a.DisInfo.r * cos(a.DisInfo.angolo2) + x, 
        angolo2Y = a.DisInfo.r * sin(a.DisInfo.angolo2) + y;

    Ellipse(hdc, x - a.DisInfo.r, y - a.DisInfo.r, 
        x + a.DisInfo.r, y + a.DisInfo.r);
    MoveToEx(hdc, (int)angolo1X, (int)angolo1Y, NULL);
    LineTo(hdc, a.DisInfo.pX + x, a.DisInfo.pY + y);
    LineTo(hdc, angolo2X, angolo2Y);
    TextOutA(hdc, x - 2 * a.DisInfo.r, y - 2.5 * a.DisInfo.r, a.sNome, strlen(a.sNome));
}
void Aggiorna(HWND hwnd)
{
    if (pAerei && bMuovi)
        for (int i = 0; i < NUMERO_AEREI; ++i)
        {
            float x = pAerei[i].Pos.lon / 180.0F * 20000.0F + 
                DELTA_T * pAerei[i].V.x;
            float y = pAerei[i].Pos.lat / 90.0F * 10000.0F +
                DELTA_T * pAerei[i].V.y;
            if (x > 20000.0F)
                x -= 40000.0F;
            else if (x < -20000.0F)
                x += 40000.0F;
            if (y > 10000.0F)
                y -= 20000.0F;
            else if (y < -10000.0F)
                y += 20000.0F;
            pAerei[i].Pos.lon = 180.0F * x / 20000.0F;
            pAerei[i].Pos.lat =  90.0F * y / 10000.0F;
        }
}
int InformazioniAereo(HWND hwnd, const Aereo& a)
{
    char* testo = (char*)malloc(256);
    if (!testo)
        return 0;
    char* titolo = (char*)malloc(64);
    if (!titolo)
        return 0;
    bool bRicorda = bMuovi;
    bMuovi = false;
    memset(titolo, 0, 64);
    memset(testo, 0, 256);

    strcpy(testo, "Compagnia: ");
    strcat(testo, a.sNome);
    strcat(testo, "\r\n");

    strcat(testo, "Codice: ");
    strcat(testo, a.sCodice);
    strcat(testo, "\r\n");

    strcat(testo, "Passeggeri a bordo: ");
    sprintf(testo + strlen(testo), "%d", a.iPasseggeri);
    strcat(testo, "\r\n");

    strcat(testo, "Posizione attuale:\r\n");
    sprintf(testo + strlen(testo), "\tlat %.2f N\r\n\tlon %.2f E\r\n", 
        a.Pos.lat, a.Pos.lon);
    strcat(testo, "Altitudine: ");
    sprintf(testo + strlen(testo), "%.2f [m]\r\n", a.Pos.h);
    sprintf(testo + strlen(testo), "Velocita': %.2f [km/h]\r\n", 
        sqrt(a.V.x * a.V.x + a.V.y * a.V.y));

    strcpy(titolo, "Informazioni su ");
    strcat(titolo, a.sCodice);
    int r = MessageBoxA(hwnd, testo, titolo, MB_OK);
    free(testo);
    free(titolo);
    bMuovi = bRicorda;
    return r;
}
//Gestione eventi form
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        MessageBoxA(hWnd, 
            "Sei il gestore della torre di controllo.\r\n"
            "Puoi ottenere le informazioni sugli aerei cliccando col tasto destro su di essi.\r\n"
            "Premendo SPAZIO puoi mettere in pausa, premendo CTRL si genera una nuova simulazione.\r\n"
            "Premendo '+' si fa scorrere il tempo piu' velocemente, mentre premendo '-' lo si rallenta.\r\n"
            "\r\n\r\n\tCreato da Riccardo Ciucci.\r\n\tClasse 4BSA Liceo Cecioni Livorno A.S. 2020-21",
            "Informazioni:", MB_OK | MB_ICONINFORMATION);
        break;
    case WM_TIMER:
        Aggiorna(hWnd);
        InvalidateRect(hWnd, NULL, TRUE);//Disegna di nuovo
        break;
    case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
            case IDM_EXIT://X
                DestroyWindow(hWnd);
                break;
            case 1://Genera
                SetWindowTextA(hWnd, "Generando nuovi dati...");
                GeneraDati();
                SetWindowTextA(hWnd, "Fatto!");
                Sleep(100);
                SetWindowTextA(hWnd, "Torre di controllo - Radar");
                break;
            case 2://Pausa
                bMuovi = !bMuovi;
                break;
            case 3://Chiudi
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProcW(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            RECT rect;
            GetClientRect(hWnd, &rect);
            FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));//Sfondo bianco
            SelectObject(hdc, GetStockObject(DC_PEN));
            SetDCPenColor(hdc, RGB(0, 0, 0));
            DisegnaTorre(hdc);
            if (pAerei)
                for (int i = 0; i < NUMERO_AEREI; ++i)
                {
                    DisegnaAereo(hdc, pAerei[i]);
                }
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        KillTimer(hWnd, 10);
        PostQuitMessage(0);
        break;
    case WM_CONTEXTMENU:
    case WM_RBUTTONDOWN:
    {
        tagPOINT CursorPos = { 0, 0 };
        if (pAerei && GetCursorPos(&CursorPos))
        {
            ScreenToClient(hWnd, &CursorPos);
            for (int i = 0; i < NUMERO_AEREI; i++)
            {
                float dX = SCREEN_W * (pAerei[i].Pos.lon + 180.0F) 
                    / 360.0F - (float)CursorPos.x;
                float dY = SCREEN_H * (90.0F - pAerei[i].Pos.lat)
                    / 180.0F - (float)CursorPos.y;
                if (sqrt(dX * dX + dY * dY) < pAerei[i].DisInfo.r)
                {//ha cliccato sull'aereo
                    InformazioniAereo(hWnd, pAerei[i]);
                    return 0;
                }
            }
            //MessageBoxA(hWnd, "Clicca su un aereo",
                //"Errore", MB_ICONERROR);
        } 
        else
            MessageBoxA(hWnd, "Non e' stato possibile ottenere la posizione del muose",
                "Errore", MB_ICONERROR);
    } break;
    case WM_KEYDOWN:
    {
        switch (wParam)
        {
        case VK_SPACE:
            bMuovi = !bMuovi;
            break;
        case VK_SUBTRACT:
            DELTA_T = max(DELTA_T - 0.01F, 0.01F);
            break;
        case VK_ADD:
            DELTA_T = min(DELTA_T + 0.01F, 0.1F);
            break;
        case VK_CONTROL:
            SetWindowTextA(hWnd, "Generando nuovi dati...");
            GeneraDati();
            SetWindowTextA(hWnd, "Fatto!");
            Sleep(100);
            SetWindowTextA(hWnd, "Torre di controllo - Radar");
            break;
        }
    } break;
    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}