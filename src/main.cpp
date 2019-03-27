#include "bd.h"
#include "lp.h"
#include "workers.h"
#ifdef WIN32
#include <vld.h>
#endif
using namespace std;

BD conf("config.json");
BD bd("dat.json");

int main()
{
    Net::init();
    Lp lp;
    lp.loop();
	Net::deinit();
    return 0;
}
