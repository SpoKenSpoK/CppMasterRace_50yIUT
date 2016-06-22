#include <osgViewer/Viewer>
#include <osg/ShapeDrawable>
#include <osg/Material>
#include <osgDB/ReadFile>
#include <osg/Geometry>
#include <osg/PositionAttitudeTransform>
#include <osgGA/NodeTrackerManipulator>
#include <osgGA/GUIEventHandler>
#include <osgViewer/ViewerEventHandlers>
#include "renderToTexture.h"

osg::ref_ptr<osg::Group> root;
osgViewer::Viewer viewer;
osg::Group* scene;

osg::ref_ptr<osg::Geometry> quad1;
osg::ref_ptr<osg::Texture2D> texture1;
osg::ref_ptr<osg::Geometry> quad2;
osg::ref_ptr<osg::Texture2D> texture2;
osg::ref_ptr<osg::Geometry> quad3;
osg::ref_ptr<osg::Texture2D> texture3;
osg::ref_ptr<osg::Geometry> quad4;
osg::ref_ptr<osg::Texture2D> texture4;
osg::ref_ptr<osg::Geometry> quadSol;
osg::ref_ptr<osg::Texture2D> textureSol;

osg::ref_ptr<osg::Camera> camCube;
osg::ref_ptr<osg::Camera> camSphere;
osg::ref_ptr<osg::Camera> camTank;
osg::ref_ptr<osg::Camera> camVache;

osg::ref_ptr<osg::PositionAttitudeTransform> transformTank;
osg::ref_ptr<osg::Node> tank;
osg::ref_ptr<osg::PositionAttitudeTransform> transformVache;
osg::ref_ptr<osg::Node> vache;
osg::ref_ptr<osg::PositionAttitudeTransform> transformCube;
osg::ref_ptr<osg::Box> cube;
osg::ref_ptr<osg::PositionAttitudeTransform> transformSphere;
osg::ref_ptr<osg::Sphere> sphere;

osg::ref_ptr<osg::Geode> geodeSol;
osg::ref_ptr<osg::Geode> geodeCube;
osg::ref_ptr<osg::Geode> geodeSphere;
osg::ref_ptr<osg::Geode> geodeQuad1;
osg::ref_ptr<osg::Geode> geodeQuad2;
osg::ref_ptr<osg::Geode> geodeQuad3;
osg::ref_ptr<osg::Geode> geodeQuad4;

float angle = 0.0;

class Rotation : public osg::NodeCallback
{
public:
    virtual void operator() (osg::Node* n, osg::NodeVisitor* nv)
    {
        // code pour modifier le nœud, par exemple la position si il s 'agit
        // d'un nœud de type osg::PositionAttitudeTransform :
        osg::PositionAttitudeTransform* pos = (osg::PositionAttitudeTransform*)n;
		angle += 0.04;
		pos->setAttitude(osg::Quat(osg::DegreesToRadians(angle), osg::Vec3(0.0, 0.0, 1.0)));
			
    }
};

class GestionEvenements : public osgGA::GUIEventHandler
{
 public:
 virtual bool handle( const osgGA::GUIEventAdapter& ea,
 osgGA::GUIActionAdapter& aa);
};

bool GestionEvenements::handle( const osgGA::GUIEventAdapter& ea,
 osgGA::GUIActionAdapter& aa)
{
	switch(ea.getEventType()){
		case osgGA::GUIEventAdapter::KEYDOWN :
		
			switch(ea.getKey()){
				
				case 'a':
					break;
				case 'z':
					break;
				case 'e':
					break;
			}
			break;
		
		case osgGA::GUIEventAdapter::PUSH :{
			int x = ea.getX();
			int y = ea.getY();
			break;
 }
		case osgGA::GUIEventAdapter::DOUBLECLICK :
			break;
			}
 return false; //caméra pour que l'événement soit traité par d'autres gestionnaires
}

void CreateSol(){
	textureSol = new osg::Texture2D;
	textureSol->setImage(osgDB::readImageFile("herbe.jpg"));
	textureSol->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
	textureSol->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
	textureSol->setWrap( osg::Texture::WRAP_S, osg::Texture::REPEAT );
	textureSol->setWrap( osg::Texture::WRAP_T, osg::Texture::REPEAT );
	
	quadSol = osg::createTexturedQuadGeometry(
	osg::Vec3(0.0, 0.0, 0.0), // Coin de départ
	osg::Vec3(100.0, 0.0, 0.0),  // largeur
	osg::Vec3(0.0, 100.0, 0.0),  // hauteur
	0.0, 0.0, 1.0, 1.0); 		// Coordonnées de texture gauche/bas/droit/haut
								// Si vous mettez 4.0 à la place de 1.0,
								// la texture sera répétée 4 fois
	quadSol->getOrCreateStateSet()->setTextureAttributeAndModes(0, textureSol.get());
	quadSol->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
								
	geodeSol = new osg::Geode;
	geodeSol->addDrawable(quadSol);
}

void CreationFace1(){
	
	geodeCube = new osg::Geode;
	cube = new osg::Box(osg::Vec3(100, 0, 1), 10);
	
	transformCube = new osg::PositionAttitudeTransform;
	transformCube->setUpdateCallback(new Rotation);
	geodeCube->addDrawable(new osg::ShapeDrawable(cube.get()));
	transformCube->addChild(geodeCube);
	
	
	texture1 = new osg::Texture2D;
	texture1->setTextureSize(1024, 1024);
	texture1->setInternalFormat(GL_RGBA);
	
	camCube =
	 createRTTCamera(osg::Camera::COLOR_BUFFER, texture1.get(), true);
	
	camCube->setClearColor( osg::Vec4( 0.0,0.0,0.5,1) );
	camCube->addChild(transformCube);
	
	camCube->setProjectionMatrixAsPerspective(30, 1.0, 0.1, 1000);
	camCube->setViewMatrixAsLookAt(
	osg::Vec3(110, -10, 1), // position de la caméra
	osg::Vec3(100, 0, 1), // point visé par la caméra
	osg::Vec3(0, 0, 1)); // vecteur perpendiculaire à la caméra
	
	root->addChild(camCube.get());
	
	quad1 = osg::createTexturedQuadGeometry(
	osg::Vec3(55.0, 45.0, 0.0), // Coin de départ
	osg::Vec3(-10.0, 0.0, 0.0), // Largeur
	osg::Vec3(0.0, 0.0, 10.0), // Hauteur
	0.0, 0.0, 1.0, 1.0); // Coordonnées de texture
	quad1->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture1.get());
	quad1->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	
	geodeQuad1 = new osg::Geode;
	geodeQuad1->addDrawable(quad1);
	scene->addChild(geodeQuad1);
}

void CreationFace2(){
	
	geodeSphere = new osg::Geode;
	sphere = new osg::Sphere( osg::Vec3(0,0,1), 3.0);
	
	transformSphere = new osg::PositionAttitudeTransform;
	transformSphere->setUpdateCallback(new Rotation);
	geodeSphere->addDrawable(new osg::ShapeDrawable(sphere.get()));
	transformSphere->addChild(geodeSphere);
	
	
	texture2 = new osg::Texture2D;
	texture2->setTextureSize(1024, 1024);
	texture2->setInternalFormat(GL_RGBA);
	
	camSphere =
	 createRTTCamera(osg::Camera::COLOR_BUFFER, texture2.get(), true);
	
	camSphere->setClearColor( osg::Vec4( 0.0,0.5,0.0,1) );
	camSphere->addChild(transformSphere);
	
	camSphere->setProjectionMatrixAsPerspective(30, 1.0, 0.1, 1000);
	camSphere->setViewMatrixAsLookAt(
	osg::Vec3(-10, -10, 1), // position de la caméra
	osg::Vec3(0, 0, 1), // point visé par la caméra
	osg::Vec3(0, 0, 1)); // vecteur perpendiculaire à la caméra
	
	root->addChild(camSphere.get());
	
	quad2 = osg::createTexturedQuadGeometry(
	osg::Vec3(45.0, 45.0, 0.0), // Coin de départ
	osg::Vec3(0.0, 10.0, 0.0), // Largeur
	osg::Vec3(0.0, 0.0, 10.0), // Hauteur
	0.0, 0.0, 1.0, 1.0); // Coordonnées de texture
	quad2->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture2.get());
	quad2->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	
	geodeQuad2 = new osg::Geode;
	geodeQuad2->addDrawable(quad2);
	scene->addChild(geodeQuad2);
}

void CreationFace3(){
	
	vache = osgDB::readNodeFile("cow_high.3ds");
	
	transformVache = new osg::PositionAttitudeTransform;
	transformVache->setUpdateCallback(new Rotation);
	transformVache->setPosition(osg::Vec3(0,99,0));
	transformVache->setScale(osg::Vec3(2,2,2));
	transformVache->getOrCreateStateSet()->setMode(GL_NORMALIZE,osg::StateAttribute::ON); 
	transformVache->addChild(vache);
	
	
	texture3 = new osg::Texture2D;
	texture3->setTextureSize(1024, 1024);
	texture3->setInternalFormat(GL_RGBA);
	
	camVache =
	 createRTTCamera(osg::Camera::COLOR_BUFFER, texture3.get(), true);
	
	camVache->setClearColor( osg::Vec4( 0.5,0.0,0.0,1) );
	camVache->addChild(transformVache);
	
	camVache->setProjectionMatrixAsPerspective(30, 1.0, 0.1, 1000);
	camVache->setViewMatrixAsLookAt(
	osg::Vec3(-10, 110, 1), // position de la caméra
	osg::Vec3(0, 100, 1), // point visé par la caméra
	osg::Vec3(0, 0, 1)); // vecteur perpendiculaire à la caméra
	
	root->addChild(camVache.get());
	
	quad3 = osg::createTexturedQuadGeometry(
	osg::Vec3(55.0, 55.0, 0.0), // Coin de départ
	osg::Vec3(-10.0, 0.0, 0.0), // Largeur
	osg::Vec3(0.0, 0.0, 10.0), // Hauteur
	0.0, 0.0, 1.0, 1.0); // Coordonnées de texture
	quad3->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture3.get());
	quad3->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	
	geodeQuad3 = new osg::Geode;
	geodeQuad3->addDrawable(quad3);
	scene->addChild(geodeQuad3);
}

void CreationFace4(){
	
	tank = osgDB::readNodeFile("t72-tank_des.flt");
	
	transformTank = new osg::PositionAttitudeTransform;
	transformTank->setUpdateCallback(new Rotation);
	transformTank->setPosition(osg::Vec3(100,100,0));
	//transformTank->setScale(osg::Vec3(2,2,2));
	transformTank->getOrCreateStateSet()->setMode(GL_NORMALIZE,osg::StateAttribute::ON); 
	transformTank->addChild(tank);
	
	
	texture4 = new osg::Texture2D;
	texture4->setTextureSize(1024, 1024);
	texture4->setInternalFormat(GL_RGBA);
	
	camTank =
	 createRTTCamera(osg::Camera::COLOR_BUFFER, texture4.get(), true);
	
	camTank->setClearColor( osg::Vec4( 0.5,0.5,0.0,1) );
	camTank->addChild(transformTank);
	
	camTank->setProjectionMatrixAsPerspective(30, 1.0, 0.1, 1000);
	camTank->setViewMatrixAsLookAt(
	osg::Vec3(110, 110, 1), // position de la caméra
	osg::Vec3(100, 100, 1), // point visé par la caméra
	osg::Vec3(0, 0, 1)); // vecteur perpendiculaire à la caméra
	
	root->addChild(camTank.get());
	
	quad4 = osg::createTexturedQuadGeometry(
	osg::Vec3(55.0, 55.0, 0.0), // Coin de départ
	osg::Vec3(0.0,-10.0, 0.0), // Largeur
	osg::Vec3(0.0, 0.0, 10.0), // Hauteur
	0.0, 0.0, 1.0, 1.0); // Coordonnées de texture
	quad4->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture4.get());
	quad4->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	
	geodeQuad4 = new osg::Geode;
	geodeQuad4->addDrawable(quad4);
	scene->addChild(geodeQuad4);
}

int main(void){
	osg::DisplaySettings::instance()->setNumMultiSamples( 4 );
	viewer.setUpViewInWindow( 100, 50, 800, 600 );
	viewer.getCamera()->setClearColor( osg::Vec4( 0.5,0.5,0.5,1) );
	viewer.addEventHandler(new osgViewer::StatsHandler);
	scene = new osg::Group;
	root = new osg::Group;
	
	osg::ref_ptr<osg::LightSource> lumiere = new osg::LightSource;
	lumiere->getLight()->setLightNum(1); // GL_LIGHT1
	lumiere->getLight()->setPosition(osg::Vec4(50, 50, 10, 0)); // 0 = directionnel
	lumiere->getLight()->setAmbient(osg::Vec4(0.3, 0.3, 0.3, 1.0));
	lumiere->getLight()->setDiffuse(osg::Vec4(0.5, 0.5, 0.5, 1.0));
	lumiere->getLight()->setSpecular(osg::Vec4(1.0, 1.0, 1.0, 1.0));
	osg::StateSet* state = scene->getOrCreateStateSet();
	state->setMode( GL_LIGHT0, osg::StateAttribute::OFF );
	state->setMode( GL_LIGHT1, osg::StateAttribute::ON ); 
	root->addChild(lumiere);
	/*osg::ref_ptr<osg::PositionAttitudeTransform> lum1 = new osg::PositionAttitudeTransform;
	osg::ref_ptr<osg::PositionAttitudeTransform> lum2 = new osg::PositionAttitudeTransform;
	osg::ref_ptr<osg::PositionAttitudeTransform> lum3 = new osg::PositionAttitudeTransform;
	osg::ref_ptr<osg::PositionAttitudeTransform> lum4 = new osg::PositionAttitudeTransform;
	lum1->setPosition(osg::Vec3(0,0,20));
	lum1->addChild(lumiere);
	lum2->setPosition(osg::Vec3(100,0,20));
	lum2->addChild(lumiere);
	lum3->setPosition(osg::Vec3(100,100,20));
	lum3->addChild(lumiere);
	lum4->setPosition(osg::Vec3(0,100,20));
	lum4->addChild(lumiere);
	
	root->addChild(lum1.get());
	root->addChild(lum2.get());
	root->addChild(lum3.get());
	root->addChild(lum4.get());*/
	
	
	root->addChild(scene);
	CreateSol();
	CreationFace1();
	CreationFace2();
	CreationFace3();
	CreationFace4();
	scene->addChild(geodeSol);

	
	
	viewer.setSceneData(root);
	
	osg::ref_ptr<GestionEvenements> gestionnaire = new GestionEvenements();
	viewer.addEventHandler(gestionnaire.get());

	return viewer.run();
}


