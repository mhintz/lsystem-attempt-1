#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class lsystemApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void lsystemApp::setup()
{
}

void lsystemApp::mouseDown( MouseEvent event )
{
}

void lsystemApp::update()
{
}

void lsystemApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( lsystemApp, RendererGl )
