#include "FluidSimulator.h"



FluidSimulator::FluidSimulator()
{
	w = Window("Index Buffers!", 1920, 1080, false);
	_controlfp(_MCW_PC, _PC_24);
	 renderer = new Renderer(w);
	w.LockMouseToWindow(false);
	w.ShowOSPointer(true);



	//fluid = new HeightMap();


}


FluidSimulator::~FluidSimulator()
{
}
