#include <iostream>
#include <SDL2/SDL.h>
#include <math.h>
#include <vector>

using namespace std;

enum MODES {
    MAIN_MENU,
    IN_GAME,
    OPTIONS_MENU
};

struct click {
    int xpos;
    int ypos;
};

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int STEP_SIZE = 20;

SDL_Window* window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
SDL_Renderer* renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );

void rysuj(vector<int> ruchy, int& jakiKolor);
void mainGameLoop(vector<int> ruchy, int& jakiKolor, SDL_Event& e, bool& quit);

class Menu{
    public:

    virtual void render(){
        SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( renderer ); 
    }

    virtual void handleEvents(){

    }

    bool detectClickCollision(click clickTMP, SDL_Rect rect){
        if((clickTMP.xpos >= rect.x && clickTMP.xpos <= rect.x+rect.w) && 
            (clickTMP.ypos >= rect.y && clickTMP.ypos <= rect.y+rect.h)){
            return true;
        }

        else return false;
    }
};

class OptionsMenu : public Menu{
    public:
    SDL_Rect redColorBox {SCREEN_WIDTH/2-30, SCREEN_HEIGHT/2, 20, 20};
    SDL_Rect blackColorBox {redColorBox.x+40, redColorBox.y, 20, 20};
    SDL_Rect stepSizeSlider {redColorBox.x, redColorBox.y+40, 60, 20};
    SDL_Rect sliderPoint {stepSizeSlider.x, stepSizeSlider.y, 10, stepSizeSlider.h};    

    OptionsMenu(){            

    }

    void render(){
        SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( renderer ); 

        SDL_SetRenderDrawColor( renderer, 0xAA, 0x00, 0x00, 0xFF );
        SDL_RenderFillRect(renderer, &redColorBox);

        SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0xFF );
        SDL_RenderFillRect(renderer, &blackColorBox);

        SDL_SetRenderDrawColor( renderer, 0x00, 0xAA, 0x00, 0xFF );
        SDL_RenderFillRect(renderer, &stepSizeSlider);

        SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0xFF );
        SDL_RenderFillRect(renderer, &sliderPoint);
    }

    void handleEvents( vector<click> &clicks, vector<int> &ruchy ){
        click* clickTMP = new click;

        for (int i = 0; i < clicks.size(); i++){
            *clickTMP = clicks.back();

            if (detectClickCollision(*clickTMP, redColorBox)){
                ruchy.push_back(-2);
            }

            else if (detectClickCollision(*clickTMP, blackColorBox)){
                ruchy.push_back(-1);
            }

            else if (detectClickCollision(*clickTMP, stepSizeSlider)){            
                int sliderValTMP = (int)(sliderClickValue(*clickTMP, stepSizeSlider) * 100);            
                ruchy.push_back( sliderValTMP + 1000 );

                float pointValTMP = (float)sliderValTMP/100;
                float sliderW = (float)stepSizeSlider.w;

                sliderPoint.x = (int)(pointValTMP * sliderW + stepSizeSlider.x); 
                SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0xFF );
                SDL_RenderFillRect(renderer, &sliderPoint);
            }
        } 

        delete clickTMP;
    }

    private:
    float sliderClickValue(click clickTMP, SDL_Rect slider){
        float ret = 0;
        float x = clickTMP.xpos-slider.x;
        float y = slider.w;
        ret = x/y;    
        return ret; 
    }
};

class StartMenu : public Menu{    
    SDL_Rect startButton {SCREEN_WIDTH/2-50, SCREEN_HEIGHT/2-10, 100, 20};
    SDL_Rect optionsButton {SCREEN_WIDTH/2-50, SCREEN_HEIGHT/2-10 + 40, 100, 20};

    public:

    StartMenu(){

    }

    void render(){
        SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( renderer );

        SDL_SetRenderDrawColor( renderer, 0xFF, 0x00, 0xFF, 0xFF );
        SDL_RenderFillRect(renderer, &startButton);

        SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xAA, 0xFF );
        SDL_RenderFillRect(renderer, &optionsButton);
    }

    void handleEvents(int &currentMode, vector<click> &clicks){
        click* clickTMP = new click;
        clickTMP->xpos = 0;
        clickTMP->ypos = 0;

        for (int i = 0; i < clicks.size(); i++){        

            *clickTMP = clicks.back();

            if(detectClickCollision(*clickTMP, startButton)){                                
                currentMode = IN_GAME;
                break;
            }

            else if(detectClickCollision(*clickTMP, optionsButton)){                                
                currentMode = OPTIONS_MENU;
                clicks.clear();
                break;
            }
            clicks.pop_back();        
        }
        
        delete clickTMP;
    }
};


int main() {
    SDL_Init( SDL_INIT_VIDEO );    

    vector<int> ruchy;
    click clickTMP {0, 0}; 
    vector<click> clicks;

    // 0-czarny 1-czerwony
    int jakiKolor = 0;
    int currentMode = MAIN_MENU;

    OptionsMenu optionsmenu;
    StartMenu startmenu;
    

    bool quit = false;
    SDL_Event e;

    while(!quit){
        while(SDL_PollEvent(&e) != 0){
            if( e.type == SDL_QUIT ){
                quit = true;
            }

            else if (e.type==SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT &&( currentMode == MAIN_MENU || currentMode == OPTIONS_MENU )){
                SDL_GetMouseState(&clickTMP.xpos, &clickTMP.ypos);
                clicks.push_back(clickTMP);
            }

            else if (e.type == SDL_KEYDOWN){
                if(e.key.keysym.sym == SDLK_ESCAPE ){
                    clicks.clear();
                    if(currentMode == IN_GAME || currentMode == OPTIONS_MENU ){
                        currentMode = MAIN_MENU;
                    }
                }

                else ruchy.push_back(e.key.keysym.sym);
            }
        }

        switch (currentMode){
            case MAIN_MENU:
                startmenu.render();
                startmenu.handleEvents(currentMode, clicks);
                break;

            case IN_GAME:
                mainGameLoop(ruchy, jakiKolor, e, quit);                
                break;

            case OPTIONS_MENU:
                optionsmenu.render();
                optionsmenu.handleEvents(clicks, ruchy);
                break;       
        }

        SDL_RenderPresent( renderer );

    }   
    return 0;
}

void mainGameLoop(vector<int> ruchy, int& jakiKolor, SDL_Event& e, bool& quit){
    SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
    SDL_RenderClear( renderer );     

    rysuj(ruchy, jakiKolor);
} 

void rysuj(vector<int> ruchy, int& jakiKolor){  
    int stepsize = STEP_SIZE;

    SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0xFF );

    int currentPosX = SCREEN_WIDTH/2;
    int currentPosY = SCREEN_HEIGHT/2;
    int nextPosX = currentPosX;
    int nextPosY = currentPosY;

    for (int i = 0; i < ruchy.size(); i++){
        if(ruchy[i]==SDLK_UP){
            nextPosX = currentPosX;
            nextPosY = currentPosY - stepsize;
        }

        else if(ruchy[i]==SDLK_DOWN){
            nextPosX = currentPosX;
            nextPosY = currentPosY + stepsize;
        }

        else if(ruchy[i]==SDLK_LEFT){
            nextPosX = currentPosX- stepsize;
            nextPosY = currentPosY ;
        }

        else if(ruchy[i]==SDLK_RIGHT){
            nextPosX = currentPosX + stepsize;
            nextPosY = currentPosY;
        }

        else if(ruchy[i]==-1){
            jakiKolor = 0;
            SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0xFF );
        }

        else if(ruchy[i]==-2){
            jakiKolor = 1;
            SDL_SetRenderDrawColor( renderer, 0xAA, 0x00, 0x00, 0xFF );
        }

        else if(ruchy[i] >= 1000){
            stepsize = ruchy[i]-1000;
        }        

        SDL_RenderDrawLine(renderer, currentPosX, currentPosY, nextPosX, nextPosY);

        currentPosX = nextPosX;
        currentPosY = nextPosY;
    }

}