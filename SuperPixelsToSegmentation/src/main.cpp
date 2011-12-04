#include "SuperPixelsToSegmentation.h"

int main(int argc, char** argv)
{
	SuperPixelsToSegmentation processor(argc, argv);
	return processor.run();
}
