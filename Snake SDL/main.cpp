#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <chrono>


namespace sc = std::chrono;
// trida pro prace s SDL
class MySDL {
    int SCREEN_WIDTH;
    int SCREEN_HEIGHT;
    bool QUIT = false;

    SDL_Window* window = NULL;
    SDL_Surface* screenSurface = NULL;
    SDL_Renderer* renderer = NULL;
    // funkce pravadi primarni inicializace SDL ( vytvari okno, render, povrh okna)
public:
    int initialize(int width, int height) {
        SCREEN_WIDTH = width;
        SCREEN_HEIGHT = height;
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) return 1; // inicializace SDL
        window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (TTF_Init() < 0) return 1;
        // tvotime okno
        if (window == NULL) return 1;
        // tvorime render pro vizualizace grafiky
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE | SDL_RENDERER_PRESENTVSYNC);
        if (renderer == nullptr) return 1;
        // ziskavame povrh okna
        screenSurface = SDL_GetWindowSurface(window);
        if (!screenSurface) {
            SDL_Quit();
            return 1;
        }
        return 0;
    }

    SDL_Renderer* getRenderer() {
            return renderer;
        }
    // funkce uvolnuje pamet od SDL
public:
    void finalize() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        TTF_Quit();
    }

    // kontrolijeme, zda jste stiskli krizek (zavreni okna)
public:
    int getEvent() {
        SDL_Event event; // vytvarime objekt jevu
        while (SDL_PollEvent(&event)) // pokud mame jevy
        {
            SDL_PumpEvents(); // aktulizujeme stav jevu
            // pokud jev - tick krizeku
            if (event.type == SDL_QUIT) {
                QUIT = true;
                return -1;
            }
            // pokud jev - stisknuti tlacitka
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.scancode == SDL_SCANCODE_UP)
                    return 3;
                if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT)
                    return 4;
                if (event.key.keysym.scancode == SDL_SCANCODE_DOWN)
                    return 5;
                if (event.key.keysym.scancode == SDL_SCANCODE_LEFT)
                    return 6;
            }
        }
        return 0;
    }
    // fukce ceka na ztlaceni krizeka (uzavreni okna)
public:
    void waitQuit() {
        if (QUIT) return;
        while (getEvent() != -1);
    }
    // funkce cisti obrazovku bilou barvou
public:
    void clear() {
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);// nastavime barvu kresleni pro render
        SDL_RenderClear(renderer);// vycisteme aktualni render, a naplnime nastvenou bravou
        SDL_RenderPresent(renderer);// zobrazuje render na obrazovce
    }

    // funkce kresli zed
public:
    void drawBlock(int x, int y, int width, int height) {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);// Nastavte barvu kreslení pro rendererer
        SDL_Rect rect = { x, y, width, height };//Vytvoříme obdélník SDL_Rect s danými souřadnicemi (x, y) a rozměry (width, height).
        SDL_RenderFillRect(renderer, &rect);// // Vyplňte obdélník na renderu pomocí nastavené barvy kreslení. Obdélník se přenáší podle ukazatele (&rect).
    }
public:
    // funkce kresli krouzek (protoze kresleni krouzku v SDL neni, kreslim pomoci malych usecku)
    void drawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b) {
        SDL_SetRenderDrawColor(renderer, r, g, b, 0xFF);// // Nastavte barvu kreslení pro rendererer
        for (double dy = 1; dy <= radius; dy += 1.0)
        {
            double dx = floor(sqrt((2.0 * radius * dy) - (dy * dy)));
            SDL_RenderDrawLine(renderer, x - dx, y + dy - radius, x + dx, y + dy - radius);
            SDL_RenderDrawLine(renderer, x - dx, y - dy + radius, x + dx, y - dy + radius);
        }
    }
public:
    // funkce zpusobuje prekresleni obrazku
    void redraw() {
        SDL_RenderPresent(renderer);
    }
};


class Score {
    int score;
    MySDL* SDL;
    int SCREEN_WIDTH;
    int SCREEN_HEIGHT;
    TTF_Font* font;
    SDL_Color textColor;
    int textWidt;
    int textHeight;

public:
    Score(MySDL* sdl, int screenWidth, int screenHeight) : score(0), SDL(sdl), SCREEN_WIDTH(screenWidth), SCREEN_HEIGHT(screenHeight), font(nullptr) {
        TTF_Init();
        font = TTF_OpenFont("/System/Library/Fonts/Supplemental/Arial Unicode.ttf", 24); // Путь к шрифту и размер
        if (font == nullptr) {
               std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        }
        textColor = {255, 255, 255}; // Белый цвет текста
        
       }

    void increaseScore(int value) {
        score += value;
    }
    void clear() {
            SDL_Surface* surface = TTF_RenderText_Solid(font, ("Score: " + std::to_string(score)).c_str(), textColor);
        
            int textWidth = surface->w;
            int textHeight = surface->h;
        
            // Очистить предыдущее значение счета, заполнив его задним фоном
        
            SDL_SetRenderDrawColor(SDL->getRenderer(), 0, 0, 0, 0); // Задний фон черного цвета
            SDL_Rect clearRect = {SCREEN_WIDTH - textWidth - 10, 10, textWidth, textHeight};
            SDL_RenderFillRect(SDL->getRenderer(), &clearRect);
        }

    void render() {
            if (font == nullptr) {
                return; // Avoid rendering if the font is not loaded correctly
            }
        clear();

            SDL_Surface* surface = TTF_RenderText_Solid(font, ("Score: " + std::to_string(score)).c_str(), textColor);
            if (surface == nullptr) {
                std::cerr << "Failed to create text surface: " << TTF_GetError() << std::endl;
                return;
            }

            SDL_Texture* texture = SDL_CreateTextureFromSurface(SDL->getRenderer(), surface);
            if (texture == nullptr) {
                std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
                SDL_FreeSurface(surface);
                return;
            }

            int textWidth = surface->w;
            int textHeight = surface->h;

            SDL_Rect textRect = {SCREEN_WIDTH - textWidth - 10, 10, textWidth, textHeight};
            SDL_RenderCopy(SDL->getRenderer(), texture, NULL, &textRect);

            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
        }

    ~Score() {
        TTF_CloseFont(font);
    }
};



// trida kontrolujicici prace hry (pohyb, kresleni)
class Painter {
    int NUM_CELL_X;
    int NUM_CELL_Y;
    int CELL_SIZE;
    int HALF_SIZE;
    int HEAD_SIZE;
    int BODY_SIZE;
    bool GAME_OVER;
    int head_x, head_y, tale_x, tale_y;
    int direction, prev_direction;
    int free_cells;
    MySDL* SDL;
    int** F;
    Score* score;
   
public:
    // metoda inicializaci tridy (neni konstructor)
    void initialize(MySDL* sdl, int** f, int num_cell_x, int num_cell_y, int cell_size, Score* sc) {
        // zapamatujeme si ruzne paramety
        GAME_OVER = false;
        SDL = sdl;
        F = f;
        score = sc;
        NUM_CELL_X = num_cell_x;
        NUM_CELL_Y = num_cell_y;
        CELL_SIZE = cell_size;
        HALF_SIZE = CELL_SIZE / 2;
        HEAD_SIZE = CELL_SIZE / 2.1;
        BODY_SIZE = CELL_SIZE / 3;
        // kreslime hada, zapamatujeme si kde je hlava, ocas, a kam leze
        head_x = 1;
        head_y = 3;
        tale_x = 1;
        tale_y = 1;
        prev_direction = 5;
        direction = 5;
        F[1][1] = 5; // Telo. cislo ukazuje kde je nasledujici( smerem k hlave) cast tela : 3 - nahoru, 4 - vpravo, 5 - dolu, 6 - vlevo
        F[2][1] = 5; // telo
        F[3][1] = 2; // Hlava
        // pocitame pocet prazdnych bunek a pridame jabloko
        free_cells = (NUM_CELL_X - 2) * (NUM_CELL_Y - 2) - 3;
        addApple();
    }
    // metoda pridani jabloka
    void addApple() {
        int r = rand() % free_cells;
        for (int i = 0; i < NUM_CELL_Y; ++i) {
            for (int j = 0; j < NUM_CELL_X; ++j) {
                if (r == 0) {
                    --free_cells;
                    F[i][j] = 8;
                    drawApple(j, i);
                    return;
                }
                if (F[i][j] == 0) --r;
            }
        }
    }
    // metoda pocatecniho kresleni celeho poli (pak budeme prekreslit castecne)
    void drawField() {
        for (int i = 0; i < NUM_CELL_Y; ++i) {
            for (int j = 0; j < NUM_CELL_X; ++j) {
                if (F[i][j] == 9)
                    drawWall(j, i);
                if (F[i][j] == 2)
                    drawHead(j, i);
                if (F[i][j] > 2 && F[i][j] < 7)
                    drawBody(j, i);
                if (F[i][j] == 8)
                    drawApple(j, i);
                
            }
        }
    }
    // metoda kresleni steny podle zadanych souradnic v poli
    void drawWall(int x, int y) {
        SDL->drawBlock(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE);
    }
    // metoda kresleni hlavy hadu podle zadanych souradnic v poli
    void drawHead(int x, int y, bool clear = false) {
        if (clear)
            SDL->drawCircle(x * CELL_SIZE + HALF_SIZE, y * CELL_SIZE + HALF_SIZE, HEAD_SIZE, 0xFF, 0xFF, 0xFF);
        else
            SDL->drawCircle(x * CELL_SIZE + HALF_SIZE, y * CELL_SIZE + HALF_SIZE, HEAD_SIZE, 0x9A, 0xCD, 0x32);
    }
    // metoda kresleni tela hadu podle zadanych souradnic v poli
    void drawBody(int x, int y, bool clear = false) {
        if (clear)
            SDL->drawCircle(x * CELL_SIZE + HALF_SIZE, y * CELL_SIZE + HALF_SIZE, BODY_SIZE, 0xFF, 0xFF, 0xFF);
        else
            SDL->drawCircle(x * CELL_SIZE + HALF_SIZE, y * CELL_SIZE + HALF_SIZE, BODY_SIZE, 0x9A, 0xCD, 0x32);
    }
    // metoda kresleni jabloka podle zadanych souradnic v poli
    void drawApple(int x, int y) {
        SDL->drawCircle(x * CELL_SIZE + HALF_SIZE, y * CELL_SIZE + HALF_SIZE, BODY_SIZE, 0xDC, 0x14, 0x3C);
    }
    // metoda urceni smeru pohybu hada, kontrolujeme aby nebylo pohybu na 180 stupnu
    void setDirection(int dir) {
        if (abs(prev_direction - dir) == 2) return; // pokud se snazim otocit na 180 stupnu
        if (dir > 0) direction = dir;
    }
    // metoda pohybuje hada, zmeni hodnoty v poli, prekresli obrazovku a kontroluje srazku
    int move() {
        int old_x = head_x, old_y = head_y;
        bool apple = false;
        if (GAME_OVER) return 1;
        // Měníme souřadnice hlavy podle aktuálního směru.
        switch (direction) {
            case 3:
                --head_y;
                break;
            case 4:
                ++head_x;
                break;
            case 5:
                ++head_y;
                break;
            case 6:
                --head_x;
                break;
        }
        // Kontrolujeme, zda hlava nenarazí na jiné předměty než jablko (hodnota 8) a prázdnou buňku (hodnota 0).
        if (F[head_y][head_x] != 0 && F[head_y][head_x] != 8) {
            GAME_OVER = true;
            return 1;
        }
        // Aktualizujeme předchozí směr na aktuální směr.
        prev_direction = direction;
        // pohybujeme hlavu
        drawHead(old_x, old_y, true);
        drawBody(old_x, old_y);
        drawHead(head_x, head_y);
        F[old_y][old_x] = direction;
        // Aktualizujeme hodnotu předchozí hlavy v poli tak, aby odpovídala novému směru.
        if (F[head_y][head_x] == 8) {
            apple = true;
            score->increaseScore(1);
            addApple();
        }
        // Aktualizujeme hodnotu hlavy v poli na 2 (Označení hlavy).
        F[head_y][head_x] = 2;
        // pohybujeme ocas
        if (!apple) {
            old_x = tale_x;
            old_y = tale_y;
            // Zachováme aktuální souřadnice ocasu.
            switch (F[tale_y][tale_x]) {
                //Měníme souřadnice ocasu podle hodnoty v poli.
                case 3:
                    --tale_y;
                    break;
                case 4:
                    ++tale_x;
                    break;
                case 5:
                    ++tale_y;
                    break;
                case 6:
                    --tale_x;
                    break;
            }
            drawBody(old_x, old_y, true);
            // Nakreslíme tělo hada na nové souřadnice a vyčistíme předchozí zobrazení.
            F[old_y][old_x] = 0;
            // Aktualizujeme hodnotu předchozího ocasu v poli na 0 (prázdná buňka).
        }
        SDL->redraw();
        // Aktualizujeme zobrazení na obrazovce
        return 0;
    }
};

class Menu {
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Rect startButton{};
    bool startClicked;
   


public:
    Menu() : window(nullptr), renderer(nullptr), font(nullptr), startClicked(false) {}

    // Функция инициализации меню
    int initialize() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;
        if (TTF_Init() < 0) return 1;

        window = SDL_CreateWindow("Menu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
        if (window == nullptr) return 1;

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == nullptr) return 1;

        font = TTF_OpenFont("/System/Library/Fonts/Supplemental/Arial Unicode.ttf", 36); // Замените на путь к вашему шрифту и размер
        if (font == nullptr) return 1;

        // Устанавливаем размеры и положение кнопки "Старт"
        startButton = {300, 250, 200, 100};

        return 0;
    }

    // Функция отрисовки меню
    void render() {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Устанавливаем цвет фона (белый)
        SDL_RenderClear(renderer);

        // Рисуем кнопку "Старт"
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &startButton);

        // Рисуем текст на кнопке "Старт"
        SDL_Color textColor = {0, 0, 0};
        SDL_Surface* surface = TTF_RenderText_Solid(font, "Start", textColor);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        int textWidth = surface->w;
        int textHeight = surface->h;

        SDL_Rect textRect = {startButton.x + (startButton.w - textWidth) / 2, startButton.y + (startButton.h - textHeight) / 2, textWidth, textHeight};
        SDL_RenderCopy(renderer, texture, NULL, &textRect);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
        renderSnakeControls();

        SDL_RenderPresent(renderer);
    }
    
    void renderSnakeControls() {
        SDL_Color textColor = {0, 0, 0}; // Черный цвет текста

        // Отображаем текст с описанием управления
        SDL_Surface* surface = TTF_RenderText_Solid(font, "Use arrow keys on the keyboard to move ", textColor);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        int textWidth = surface->w;
        int textHeight = surface->h;

        SDL_Rect textRect = {10, startButton.y + startButton.h + 10, textWidth, textHeight};
        SDL_RenderCopy(renderer, texture, NULL, &textRect);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }


    // Функция обработки событий
    void handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                startClicked = true; // Закрытие окна меню означает запуск игры
                return;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;

                if (mouseX >= startButton.x && mouseX <= startButton.x + startButton.w &&
                    mouseY >= startButton.y && mouseY <= startButton.y + startButton.h) {
                    startClicked = true;
                    return;
                }
            }
        }
        
    }

    // Функция для проверки, была ли нажата кнопка "Старт"
    bool isStartClicked() {
        return startClicked;
    }

    // Функция завершения работы меню
    void finalize() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
    }
};




int main(int argc, char* args[])
{
    Menu menu;
    if (menu.initialize() != 0) {
        std::cerr << "Failed to initialize menu" << std::endl;
        return 1;
    }

    menu.render();
    while (!menu.isStartClicked()) {
        menu.handleEvents();
        
    }

    menu.finalize();


    int NUM_CELL_X = 20;// počet buněk vodorovně na hrací ploše.
    int NUM_CELL_Y = 20;// počet vertikálních buněk na hřišti.
    int CELL_SIZE = 40;// velikost buňky na hřišti v pikselu
    int width = NUM_CELL_X * CELL_SIZE;// sirka v pikselech
    int height = NUM_CELL_Y * CELL_SIZE;// vyska v pikselech
    int** F; // hriste
    int keycode;// uložení kódu stisknuté klávesy
    std::chrono::system_clock::time_point time, curr_time;// měření časových intervalů ve hře.
    MySDL* sdl = new MySDL();
    Painter* painter = new Painter();
    Score* score = new Score(sdl, width, height);

    // vylucujeme pamet
    F = new int* [NUM_CELL_Y];
    for (int i = 0; i < NUM_CELL_Y; ++i) {
        F[i] = new int[NUM_CELL_X];
    }
    // nasatavime poli
    for (int i = 0; i < NUM_CELL_Y; ++i) {
        for (int j = 0; j < NUM_CELL_X; ++j) {
            F[i][j] = 0; // prazdna bunka
            if (i == 0 || j == 0 || i == NUM_CELL_Y - 1 || j == NUM_CELL_X - 1)
                F[i][j] = 9; // zed
        }
    }
    sdl->initialize(width, height); // inicializace pole
    painter->initialize(sdl, F, NUM_CELL_X, NUM_CELL_Y, CELL_SIZE, score); // inicializace kresleni
    sdl->clear();// vycisteni hriste
    painter->drawField();// kresleni hriste
    sdl->redraw(); // aktualizace obrazovky
    //score->render();
    keycode = sdl->getEvent(); //Záznam aktuálního casu
    time = sc::system_clock::now();
    curr_time = time;
    // zakladni cyklus
    while (keycode != -1) {
        keycode = sdl->getEvent();//Získání kódu stisknutím klávesy uvnitř herního cyklu.
        painter->setDirection(keycode);//Nastavení směru pohybu hada
        curr_time = sc::system_clock::now();
        if ((curr_time - time).count() > 500000) { // rychlost hry, za teto podminky pohybuje had
            time = curr_time;
            if (painter->move()) break;
            score->render();//provedení kroku hry (pohyb hada a aktualizace stavu)
        }
    }
    sdl->waitQuit();
    sdl->finalize();
    // uvolnujeme pamet
    for (int i = 0; i < NUM_CELL_Y; ++i) {
        delete[] F[i];
    }
    delete[] F;
    delete sdl;
    delete score;
    return 0;
}
