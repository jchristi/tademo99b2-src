// ElementHandler.cpp: implementation of the CElementHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "ElementHandler.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CElementHandler::CElementHandler()
{

}

CElementHandler::~CElementHandler()
{
	for(int y=0;y<ELEMENT_HASH_SIZE;y++)
		for(int x=0;x<ELEMENT_HASH_SIZE;x++){
			while(!map[y][x].empty()){
				delete map[y][x].back();
				map[y][x].pop_back();
			}	
		}
}

CElementHandler::AddElement(GraphicElement *ge)
{
	int x=(ge->x1>>HASH_SQUARE_SIZE) & (ELEMENT_HASH_SIZE-1);
	int y=(ge->y1>>HASH_SQUARE_SIZE) & (ELEMENT_HASH_SIZE-1);

	map[y][x].push_back(ge);
}

CElementHandler::DeleteOn(int x, int y)
{
	DeleteBetween(x-10,y-10,x+10,y+10);
}

CElementHandler::DeleteBetween(int x1, int y1, int x2, int y2)
{
	int sx1=(x1>>HASH_SQUARE_SIZE) & (ELEMENT_HASH_SIZE-1);
	int sy1=(y1>>HASH_SQUARE_SIZE) & (ELEMENT_HASH_SIZE-1);
	int sx2=(x2>>HASH_SQUARE_SIZE) & (ELEMENT_HASH_SIZE-1);
	int sy2=(y2>>HASH_SQUARE_SIZE) & (ELEMENT_HASH_SIZE-1);

	if(sx1>sx2){
		int tmp=sx1;
		sx1=sx2;
		sx2=tmp;
	}
	if(sy1>sy2){
		int tmp=sy1;
		sy1=sy2;
		sy2=tmp;
	}
	std::vector<GraphicElement*> toBeDeleted;

	for(int y=sy1;y<=sy2;y++)
		for(int x=sx1;x<=sx2;x++){
			ElementList::iterator eli;
			for(eli=map[y][x].begin();eli!=map[y][x].end();++eli){
				if(((*eli)->x1 >= x1) && ((*eli)->y1 >= y1))
					toBeDeleted.push_back(*eli);
			}
			while(!toBeDeleted.empty()){
				delete toBeDeleted.back();
				map[y][x].remove(toBeDeleted.back());
				toBeDeleted.pop_back();
			}
		}
}

std::vector<GraphicElement*> CElementHandler::GetArea(int x1, int y1, int x2, int y2)
{
	int sx1=(x1>>HASH_SQUARE_SIZE) & (ELEMENT_HASH_SIZE-1);
	int sy1=(y1>>HASH_SQUARE_SIZE) & (ELEMENT_HASH_SIZE-1);
	int sx2=(x2>>HASH_SQUARE_SIZE) & (ELEMENT_HASH_SIZE-1);
	int sy2=(y2>>HASH_SQUARE_SIZE) & (ELEMENT_HASH_SIZE-1);

	if(sx1>sx2){
		int tmp=sx1;
		sx1=sx2;
		sx2=tmp;
	}
	if(sy1>sy2){
		int tmp=sy1;
		sy1=sy2;
		sy2=tmp;
	}
	std::vector<GraphicElement*> r;

	for(int y=sy1;y<=sy2;y++)
		for(int x=sx1;x<=sx2;x++){
			ElementList::iterator eli;
			for(eli=map[y][x].begin();eli!=map[y][x].end();++eli){
				if((((*eli)->x1 >= x1) && ((*eli)->x1 <= x2) && ((*eli)->y1 >= y1) && ((*eli)->y1 <= y2)))
					r.push_back(*eli);
			}
		}
	return r;
}

GraphicElement* CElementHandler::GetClosestElement(int x, int y)
{

	return 0;
}

GraphicElement* CElementHandler::MoveTextElement(GraphicElement* GE, int x, int y)
{

	try
	{
		//add a new identical element then remove the old
		GraphicText *ge = (GraphicText*)GE;
		GraphicText *Nge = new GraphicText(x, y, ge->text, ge->Color);

		int mx=(ge->x1>>HASH_SQUARE_SIZE) & (ELEMENT_HASH_SIZE-1);
		int my=(ge->y1>>HASH_SQUARE_SIZE) & (ELEMENT_HASH_SIZE-1);
		map[my][mx].remove(ge);
		if(ge)
			delete ge;

		AddElement(Nge);

		  //returns the new element
		return Nge;
	}
	catch(...)
	{
		return NULL;
	}
}
