#include "GL/GLFundamentals.hpp"
#include "GL/GLDemonstration.hpp"

//------------------------------------------------------------------------------

class rawk : public demonstration
{
public:

	rawk();
   ~rawk();
};

rawk::rawk() : demonstration("RAWK", 1280, 720)
{
}

rawk::~rawk()
{
}

//------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	rawk app;
	app.run();
}
