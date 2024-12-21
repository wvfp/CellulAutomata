#include "mainApplication.hpp"
#include <bitset>
#include <random>
#include <fstream>
#include <ctime>

mainApplication::mainApplication(std::string name,SDL_Rect r_t):Application(name,r_t),
map(std::vector<std::bitset<W>>(H)){
	initAll();

	button =PushButtonPtr( new PushButton(render,{1600,0,200,200}));
	button->setFont(font);
	button->setText("Restart");
	button->setBackgroundColor({0xff,0xff,0xff,0xff});	
	Action<PushButtonEvent> act([=](void*)->void{
		this->initAll();
		std::cout<<"Restart"<<std::endl;
	},PushButtonEvent::ON_RELEASED);
	button->bindAction(act);
	button->hide();
	// time_label = LabelPtr(new Label(render,{0,0,120*(width*W/1024 + 1),44*(height*H/1024 + 1)}));
	time_label = LabelPtr(new Label(render,{0,0,140,30}));
	time_label->setFont(font);
	time_label->hideBackground();
	time_label->hideFrame();
	ishideGrid=false;
}

void mainApplication::hideGrid(){
	ishideGrid = !ishideGrid;
}

void mainApplication::initAll(){
	up_times = 0;
	SDL_Rect r_t = this->WindowPos_Size;
	running = true;
	std::string xmlpath="./res/xml/color.xml";
	if(doc.LoadFile(xmlpath.c_str())!=XML_SUCCESS){
		std::cout<<"XML:"<<xmlpath<<" can't be load"<<std::endl;
		zero_Color={0,0,0,255};
		one_Color={255,255,255,255};
		line_Color={255,255,255,255};
		WindowColor={0,0,0,155};
		width = 4;
		height = 4;

	}else{
		initColor();
		initCellSize();
	}
	living_cell = 0;
	//initCellNum = 63;

	//初始化 0
	for(auto i:map)
		i.set();
	r_t = {0,0,W*width,H*height};
	SDL_RenderSetLogicalSize(render.get(),W*width,H*height);
	SDL_RenderSetViewport(render.get(),&r_t);

	initMap();
	loadMap();
	updateCellNum();
}

void mainApplication::draw(){
	SDL_SetRenderDrawColor(render.get(), WindowColor.r,WindowColor.g ,
							WindowColor.b, WindowColor.a); // 设置绘制颜色
	SDL_RenderClear(render.get());	//设置背景	
	drawCell();
	drawGrid();
	SDL_Rect rect={0,0,0,0};
	SDL_RenderGetLogicalSize(render.get(),&rect.w,&rect.h);
	SDL_RenderSetLogicalSize(render.get(),640,640);
	button->draw();
	time_label->draw();
	SDL_RenderSetLogicalSize(render.get(),rect.w,rect.h);
}

void mainApplication::draw(int a){
	SDL_SetRenderDrawColor(render.get(), WindowColor.r,WindowColor.g ,
							WindowColor.b, WindowColor.a); // 设置绘制颜色
	SDL_RenderClear(render.get());	//设置背景	
	//drawGrid();
	drawGrid();
	// std::vector<std::thread> vec_thread;
	// int thread_size = 8;
	// int offset = H/thread_size;
	std::thread t1([=](void)->void{this->drawCell(0,H/2);});
	std::thread t2([=](void)->void{this->drawCell(H/2,H);});
	t1.join();
	t2.join();
	// for(int i=0;i<thread_size-1;i++){
	// 	vec_thread.push_back(std::thread([=](void)->void{this->drawCell(i*offset,(i+1)*offset);
	// 							std::cout<<"out:"<<i<<std::endl;}));
	// }
	// //vec_thread.push_back(std::thread([=](void)->void{this->drawCell(offset*(thread_size-1),H);}));
	// for(int i=0;i<thread_size;i++){
	// 	if(vec_thread[i].joinable())
	// 		vec_thread[i].join();
	// }

	
	button->draw();
	time_label->draw();	

}

void mainApplication::drawGrid(){
	if(width>3&&!ishideGrid){
	//设置网格颜色
	SDL_SetRenderDrawColor(render.get(),line_Color.r,line_Color.g,
							line_Color.b,line_Color.a);
	
	SDL_Point p1,p2;
	p1 = p2 ={0,0};
	//横向
	for(int i=width;i<=W*width;i+=width){
		p1 = {i,0};
		p2 = {i,H*height};
		SDL_RenderDrawLine(render.get(),p1.x,p1.y,p2.x,p2.y);
	}
	//纵向
	for(int i=height;i<=H*height;i+=height){
		p1 = {0,i};
		p2 = {W*width,i};
		SDL_RenderDrawLine(render.get(),p1.x,p1.y,p2.x,p2.y);
	}
	}
}

void mainApplication::drawCell(){
	SDL_SetRenderDrawColor(render.get(),one_Color.r,one_Color.g,one_Color.b,0xff);
	for(int i=0;i<map.size();i++){
		for(int j=0;j<map[0].size();j++){
			if(map[i][j]){
				SDL_Rect rect = {width*j,height*i,width,height};
				SDL_RenderFillRect(render.get(),&rect);
			}
		}
	}
}

void mainApplication::drawCell(int b,int e){
	for(int i=b;i<e;i++){
		for(int j=0;j<map[0].size();j++){
			if(map[i][j]){
				SDL_Rect rect = {width*j,height*i,width,height};
				SDL_RenderFillRect(render.get(),&rect);
			}
		}
	}
}

void mainApplication::event_handle(SDL_Event*event){
	button->event_handle(event);
}
void mainApplication::update(){
	auto next_map = map;
	//复位
	for(auto i:next_map){
		i.reset();
	}
	for(int i=0;i<map.size();i++){
		for(int j=0;j<map[0].size();j++){
			if(i==0||j==0||i==map.size()||j==map[0].size()){
				//当cell处于地图边界时,死亡
				map[i][j] = 0; 
				continue;
			}else{
				//不处于边界时,获取除自身外周围3*3中cell的存活数
				int living = 0;
				for(int k=0;k<9;k++){
					if(k==4){
						continue;
					}
					else{
						living += (map[i - 1 + k/3][j -1 + k % 3] == 1);
					}
				}
				//存活数==3 繁殖
				if(living==3){
					next_map[i][j] = 1;	
				}else if(living>3||living<2){
					next_map[i][j]=0;
				}
			}
		}
	}
	map = next_map;	
	updateCellNum();
	up_times++;
	std::string str =std::string("times: ")+ std::to_string(up_times)+
						std::string("\nliving: ")+std::to_string(living_cell);
	time_label->setText(str,{0xff,0xff,0xff,0xff});
}

void mainApplication::update_Thread(int thread_size){
	auto next_map = map;
	//复位
	for(auto i:next_map){
		i.reset();
	}

	std::vector<std::thread> vec_thread;
	// int thread_size = 6;
	int offset = H/thread_size;
	for(int i=0;i<thread_size-1;i++){
		vec_thread.push_back(std::thread([=,&next_map](void)->void{this->update(i*offset,(i+1)*offset,next_map);}));
	}
	vec_thread.push_back(std::thread([=,&next_map](void)->void{this->update(offset*(thread_size-1),H,next_map);}));
	for(int i=0;i<thread_size;i++){
		if(vec_thread[i].joinable())
			vec_thread[i].join();
	}
	map = next_map;	
	updateCellNum();
	up_times++;
	std::string str =std::string("times: ")+ std::to_string(up_times)+
						std::string("\nliving: ")+std::to_string(living_cell);
	time_label->setText(str,{0xff,0xff,0xff,0xff});
}

void mainApplication::update(int b,int e,std::vector<std::bitset<W>>& next_map){
	//复位
	//next_map[0]=next_map[1]=next_map[3].set();
	for(int i=b;i<e;i++){
		for(int j=0;j<map[0].size();j++){
			if(i==0||j==0||i==map.size()||j==map[0].size()){
				//当cell处于地图边界时,死亡
				map[i][j] = 0; 
				continue;
			}else{
				//不处于边界时,获取除自身外周围3*3中cell的存活数
				int living = 0;
				for(int k=0;k<9;k++){
					if(k==4){
						continue;
					}
					else{
						living += (map[i - 1 + k/3][j -1 + k % 3] == 1);
					}
				}
				//存活数==3 繁殖
				if(living==3){
					next_map[i][j] = 1;	
				}else if(living>3||living<2){
					next_map[i][j]=0;
				}
			}
		}
		
	}
}


void mainApplication::initColor(){
	//初始化颜色
	XMLElement* root = doc.RootElement();
	XMLElement* color = root->FirstChildElement("zero");
	
	const XMLAttribute* attrib = color->FindAttribute("r");
	zero_Color.r = attrib->IntValue();
	attrib = attrib->Next();
	zero_Color.g = attrib->IntValue();
	attrib = attrib->Next();
	zero_Color.b = attrib->IntValue();

	color = color->NextSiblingElement("one");
	attrib = color->FindAttribute("r");
	one_Color.r = attrib->IntValue();
	attrib = attrib->Next();
	one_Color.g = attrib->IntValue();
	attrib = attrib->Next();
	one_Color.b = attrib->IntValue();

	color = color->NextSiblingElement("line");
	attrib = color->FindAttribute("r");
	line_Color.r = attrib->IntValue();
	attrib = attrib->Next();
	line_Color.g = attrib->IntValue();
	attrib = attrib->Next();
	line_Color.b = attrib->IntValue();


	color = color->NextSiblingElement("background");
	attrib = color->FindAttribute("r");
	WindowColor.r = attrib->IntValue();
	attrib = attrib->Next();
	WindowColor.g = attrib->IntValue();
	attrib = attrib->Next();
	WindowColor.b = attrib->IntValue();

	return;
}

void mainApplication::initCellSize(){
	XMLElement* root = doc.RootElement();
	XMLElement* size = root->FirstChildElement("size");
	
	const XMLAttribute* attrib = size->FindAttribute("width");

	width = attrib->IntValue();
	attrib = size->FindAttribute("height");
	height = attrib ->IntValue();

	size = size->NextSiblingElement("initCellNum");
	attrib = size->FindAttribute("num");
	initCellNum = attrib->IntValue();
	initCellNum *=(W+H)/2+1;
	return;
}

void mainApplication::loadMap(){
	XMLElement* root = doc.RootElement();
	XMLElement* f = root->FirstChildElement("file");
	if(f){
		std::string path = f->GetText();
		std::ifstream ifs(path);
		std::string bit;
		if(ifs.is_open()){
			int i=0;
			for(;i<map.size()&&!ifs.eof();i++){
				std::getline(ifs,bit);
				if(bit.empty())
					bit = "0";
				map[i] = std::bitset<W>(bit);
			}
			for(;i<map.size();i++){
				map[i].reset();
			}
		}
	}
}

void mainApplication::initMap(){
	// std::random_device rd;
	std::default_random_engine gen(std::time(0)%25418524);
	std::uniform_int_distribution distrib_W(1,W-1);
	std::uniform_int_distribution distrib_Y(1,H-1);
	int i=0;
	while(i<initCellNum){
		map[distrib_W(gen)][distrib_Y(gen)] = 1;
		i++;
	}
	std::ofstream file;
	int times = std::time(0);
	std::string file_name = std::string("./res/files/file_") + std::to_string(times);
	file.open(file_name,std::ios::out);
	for(auto j:map){
		file<<j.to_string()<<std::endl;
	}
}

void mainApplication::updateCellNum(){
	living_cell = 0;
	for(auto i:map){
		living_cell += i.count();
	}
}
