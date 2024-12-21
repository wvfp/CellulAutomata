#include "Application.hpp"
#include <FPSControler.hpp>
#include  "./mainApplication/mainApplication.hpp"


int FPS = 1;
FPSControler fps(FPS);

void initFPS(){
	XMLDocument doc;
	doc.LoadFile("./res/xml/speed.xml");
	XMLElement * root=doc.RootElement();
	FPS = root->FindAttribute("fps")->IntValue();
	fps = FPSControler(FPS);
	fps.start();
}


int main(int argc, char *argv[])
{
	std::string name{"CelluAutomata"};
	SDL_Rect size={SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,1620,1620};
	mainApplication *app=new mainApplication(name,size);
	size.w = size.h = 500;
	bool running = true;
	SDL_Event *e = new SDL_Event();	
	initFPS();
	//app->hideGrid();
	
	while (running)
	{
		while (SDL_PollEvent(e))
		{
			switch (e->type)
			{
			case SDL_QUIT:
				running = false;

				break;
			default:
				break;
			}
			app->event_handle(e);
		}

		app->draw();
		app->update_Thread(8);
		fps.tick();
		SDL_Delay(1000/100);
		app->present();
	}
	return 0;
}

