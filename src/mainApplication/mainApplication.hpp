#ifndef _MAINAPPLICATION_
#define _MAINAPPLICATION_

#include <Application.hpp>
#include <bitset>
#include <vector>
#include <iostream>
#include <string>
#include <tinyxml2.h>
#include <PushButton.hpp>
#include <Label.hpp>
#include <thread>

constexpr  int H=96;
constexpr  int W=96;

using namespace tinyxml2;

class mainApplication:public Application{
public:
	mainApplication(std::string,SDL_Rect);
	void initColor();
	void initCellSize();
	void draw();
	void draw(int);
	void event_handle(SDL_Event*);
	void update();
	void update(int,int,std::vector<std::bitset<W>>&);
	void update_Thread(int=6);
	//绘制网格
	void drawGrid();
	//绘制元孢
	void drawCell();
	void drawCell(int,int);
	void initMap();
	void updateCellNum();
	void initAll();
	void loadMap();
	void hideGrid();
private:
	PushButtonPtr button;
	LabelPtr time_label;
	std::vector<std::bitset<W>> map;
	SDL_Color zero_Color;
	SDL_Color one_Color;
	SDL_Color line_Color;
	int width;
	int height;
	int initCellNum;
	int living_cell;
	XMLDocument doc;	
	bool running;
	Uint32  up_times;	
	bool ishideGrid;
};


#endif //_MAINAPPLICATION_
