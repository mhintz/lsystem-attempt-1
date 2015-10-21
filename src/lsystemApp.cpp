#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "LSystem.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class lsystemApp : public App {
  public:
	void setup() override;
	void update() override;
	void draw() override;

	LSystem theSystem;
	CameraPersp theCamera;
	gl::GlslProgRef theProgram;
	gl::BatchRef theBatch;
};

void lsystemApp::setup() {
	gl::enableDepthRead();
	gl::enableDepthWrite();

	theSystem = LSystem("FA")
		.colors()
		.iterations(3);

	theCamera.lookAt(vec3(0, 0, 5), vec3(0, 0, 0));

	auto shader = gl::ShaderDef().color();

	theProgram = gl::getStockShader(shader);

	theBatch = gl::Batch::create(theSystem, theProgram);
}

void lsystemApp::update() {
}

void lsystemApp::draw() {
	gl::clear( Color( 0, 0, 0 ) ); 

	theBatch->draw();
}

CINDER_APP( lsystemApp, RendererGl )
