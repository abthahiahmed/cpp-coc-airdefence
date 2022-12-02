#include <iostream>
#include <vector>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>

using namespace std;

float random(float low, float high){
	
	return low + static_cast<float>(rand()) * static_cast<float>(high - low) / RAND_MAX;
}



SDL_Window *win = SDL_CreateWindow("COC - AirDefence | Abthahi Ahmed Rifat", 10,10,1000,600, 0);

SDL_Renderer *ren = SDL_CreateRenderer(win, -1, 0);

SDL_Event event;


class Balloon{
	
	
private:
	
	
	int tx, ty;
	SDL_Texture *texture;
	SDL_Rect holder, bar;
public:
	float x, y;
	int life;
	Balloon(int x, int y, int tx, int ty){
		this->x = x;
		this->y = y;
			
		this->tx = tx;
		this->ty = ty;
		
		this->life = 150;
		
		this->texture = IMG_LoadTexture(ren, "res/balloon.png");
		
		this->holder = {(int)this->x - 25, (int)this->y - 180, 50, 6};
		this->bar = {(int)this->x - 25, (int)this->y - 180, (int)this->life / 3, 6};
		
	}
	
	void update(){
		
		if (this->x < this->tx) this->x+= 0.7;
		
		if (this->x > this->tx) this->x-= 0.7;
		
		if (this->y < this->ty) this->y+= 0.7;
		
		if (this->y > this->ty) this->y-= 0.7;
		
		this->holder = {(int)this->x - 25, (int)this->y - 180, 50, 6};
		this->bar = {(int)this->x - 25, (int)this->y - 180, (int)this->life / 3, 6};
		
	}
	
	
	
	bool isDestroyed(){
		
		return this->life < 10;
	}
	
	void draw(){
		filledEllipseRGBA(ren, (int)this->x, (int)this->y, 15, 10, 0, 0, 0, 40);
		SDL_Rect geo = {(int)this->x - 30, (int)this->y - 160, 60, 84};
		SDL_RenderCopy(ren, this->texture, NULL, &geo);		
			
		SDL_SetRenderDrawColor(ren, 10, 10, 10, 100);
		SDL_RenderFillRect(ren, &holder);
		SDL_SetRenderDrawColor(ren, 114, 9, 183, 255);
		SDL_RenderFillRect(ren, &bar);
		
	}
	
	
	
	
};
struct Color{
	int r;
	int g;
	int b;
	int a;
};


class Particle{
	
private:
	float x, y;
	float r;
	Color color;
	float vx, vy;
	float alpha;
	
	bool fire;
	
public:
	Particle(float x, float y, Color color, int alpha = 255, bool fire = false, int sSize = 35){
		
		this->x = x;
		this->y = y;
		this->color = color;
		
		this->fire = fire;
		
		if (this->fire)
			this->r = random(3, 5);
		else 
			this->r = random(3, 5);
		
		this->vx = random(-0.02, 0.02);
		this->vy = random(0.1, 0.1);
		this->alpha = alpha;
		
		if (random(0, 5) > 2){
			this->color = {255, 145, 0, 255};
		}else {
			this->color = {252, 243, 0, 255};
		}
		
	}
	
	void update(){
		
		this->x += this->vx;
		this->y += this->vy;
		this->alpha -= 5;
		if (this->r > 4 && this->fire){
			this->r -= 0.03;
		}
	}
	
	bool finished(){
		return this->alpha < 2;
	}
	
	void draw(){
		
		filledCircleRGBA(ren, (int)this->x, (int)this->y, this->r, this->color.r, this->color.g, this->color.b, this->alpha);		
	
	}
	
};



class Rocket{
	
private:
	int tx, ty;
	int d;	
	double angle;
	SDL_Texture *texture;
	vector<Particle> fires;
	int t;
	
public:

	float x, y;
	Rocket(int x, int y, int tx, int ty){
		
		this->x = x;
		this->y = y;
		this->tx = tx;
		this->ty = ty;
		
		this->d = sqrt(pow(tx - x, 2) + pow(ty - y, 2));
		
		this->texture = IMG_LoadTexture(ren, "res/rocket.png");
		
		this->angle = 0;
		
		this->t = 0;
		
	}
	
	
	void update(){
		

		
		float vx = 5 * ((this->tx - this->x) / d);
		float vy = 5 * ((this->ty - this->y) / d);
		
		this->x += vx;
		this->y += vy;
		
		this->angle = (atan2(vy, vx) * 180 / 3.141592) + 90;
		

		float px = 15 * (cos(this->angle ) * M_PI / 180);
		float py = 15 * (sin(this->angle ) * M_PI / 180);
		
		
		
		if (t >= 2){
			
			this->fires.push_back(Particle(x + 2 + px, y + 15 + py, {255, 255, 255, 255}, 200, true, 8));
			t = 0;
			
		}
		t++;	
		
	}
	
	bool isReached(){
		
		float dx = this->tx - this->x;
		float dy = this->ty - this->y;
		
		float dr = 40;
		
//		*tx = this->tx;
//		*ty = this->ty - 100;
		
		return pow(dx, 2) + pow(dy, 2) <= pow(dr, 2);
		
	}
	
	
	void draw(){
		
		
		for (unsigned i = 0; i < this->fires.size(); i++){
			this->fires[i].update();
			this->fires[i].draw();
			if (this->fires[i].finished()){
				this->fires.erase(this->fires.begin() + i);
			}
		}
		
		SDL_Rect geo = {(int)this->x, (int)this->y, 5, 30};
		SDL_RenderCopyEx(ren, this->texture, NULL, &geo, this->angle, NULL, SDL_FLIP_NONE);
		
	}
	
};




class AirDefence{
	
private:
	
	int x, y;
	int tx, ty;
	int range;
	SDL_Texture *texture;
	
	int mx, my;
	
public:
	
	AirDefence(int x, int y){
		
		this->range = 200;
		this->x = x;
		this->y = y;
		
		this->texture = IMG_LoadTexture(ren, "res/air-defence.png");
		
		
		
	}
	
	void update(){
		
		
		
	}
	bool shootBalloon(Balloon b, int *tx, int *ty){
		
		float dx = b.x - this->x;
		float dy = b.y - this->y;
		
		float dr = this->range;
		
		*tx = b.x;
		*ty = b.y - 150;
		
		return pow(dx, 2) + pow(dy, 2) <= pow(dr, 2);
		
		
	}	
	void draw(){
		
//		ellipseRGBA(ren, this->x, this->y, 120, 255, 255, 255, 255);
		
		ellipseRGBA(ren, this->x, this->y, 200, 190, 255, 255, 255, 170);
		
		filledEllipseRGBA(ren, this->x, this->y, 40, 39, 0, 0, 0, 40);
		
		SDL_Rect geo = {this->x - 20, this->y - 60, 40, 73};
		SDL_RenderCopy(ren, this->texture, NULL, &geo);
		
		
		
	}
	
	
};





int main(int argc, char *argv[]){
	SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
	
	bool running = true;
	
	int ax = 500;
	int ay = 300;
	
	AirDefence ad = AirDefence(ax, ay);
	
	
	
	vector<Balloon> balloons;
	
	vector<Rocket> rockets;
	
	
	int _x, _y;
	
	SDL_Texture *grass = IMG_LoadTexture(ren, "res/grass.png");
	
	bool rBusy = false;
	
	unsigned shootOn = -1;
	
	while (running){
		
		
		SDL_PollEvent(&event);
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
		SDL_RenderClear(ren);
		
		
		
		for (int i = 0; i < 10; i++){
			for (int j = 0; j < 17; j++){
				SDL_Rect rect = {j * 60, i * 60, 60, 60};
				SDL_RenderCopy(ren, grass, NULL, &rect);
			}
		}
		
		
		ad.update();
		ad.draw();
		
		for (unsigned i = 0; i < rockets.size(); i++){
			rockets[i].update();
			rockets[i].draw();
			if (rockets[i].isReached()) {
				rockets.erase(rockets.begin() + i);
				rBusy = false;
				balloons[shootOn].life -= 50;
			}
			
		}
		
		
		for (unsigned i = 0; i < balloons.size(); i++){
			if (balloons[i].isDestroyed()){
				balloons.erase(balloons.begin() + i);
				shootOn = -1;
			}
			
			balloons[i].update();
			balloons[i].draw();
			int tx, ty;
			
			
			
			if (ad.shootBalloon(balloons[i], &tx, &ty)){
//				cout<<tx<<" "<<ty<<endl;
				if (!rBusy){
					rockets.push_back(Rocket(ax + 10, ay - 70, tx, ty));
					rBusy = true;
					
					if ((int)shootOn == -1) shootOn = i;
				}
			}
			
		}
		
		
		
		
		
		if(event.type == SDL_QUIT){
			running = false;
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN){
			
			SDL_GetMouseState(&_x, &_y);
			
			Balloon bl = Balloon(_x, _y, ax, ay);
			
			balloons.push_back(bl);
			
			
		}
		
		SDL_RenderPresent(ren);
		SDL_Delay(1000/60);
	}
	cout<<"Thank You So Much!"<<endl;
	
	
	
	return 0;
}
