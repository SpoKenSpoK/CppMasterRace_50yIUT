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

osg::ref_ptr<osg::Geometry> quadSol;
osg::ref_ptr<osg::Texture2D> textureSol;

osg::ref_ptr<osg::PositionAttitudeTransform> transformTank;
osg::ref_ptr<osg::Node> tank;

osg::ref_ptr<osg::Geode> geodeSol;

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
	textureSol->setImage(osgDB::readImageFile("test_motherboard.jpg"));
	textureSol->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR );
	textureSol->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
	textureSol->setWrap( osg::Texture::WRAP_S, osg::Texture::REPEAT );
	textureSol->setWrap( osg::Texture::WRAP_T, osg::Texture::REPEAT );
	
	quadSol = osg::createTexturedQuadGeometry(
	osg::Vec3(0.0, 0.0, 0.0), // Coin de départ
	osg::Vec3(100.0, 0.0, 0.0),  // largeur
	osg::Vec3(0.0, 100.0, 0.0),  // hauteur
	0.0, 0.0, 5.0, 5.0); 		// Coordonnées de texture gauche/bas/droit/haut
								// Si vous mettez 4.0 à la place de 1.0,
								// la texture sera répétée 4 fois
	quadSol->getOrCreateStateSet()->setTextureAttributeAndModes(0, textureSol.get());
	quadSol->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
								
	geodeSol = new osg::Geode;
	geodeSol->addDrawable(quadSol);
}

void CreationTank(){
	
	tank = osgDB::readNodeFile("t72-tank_des.flt");
	
	transformTank = new osg::PositionAttitudeTransform;
	transformTank->setUpdateCallback(new Rotation);
	transformTank->setPosition(osg::Vec3(0,0,0));
	//transformTank->setScale(osg::Vec3(2,2,2));
	transformTank->getOrCreateStateSet()->setMode(GL_NORMALIZE,osg::StateAttribute::ON); 
	transformTank->addChild(tank);
	
	scene->addChild(transformTank);
}

int main(void){
	osg::DisplaySettings::instance()->setNumMultiSamples( 4 );
	viewer.setUpViewInWindow( 100, 50, 800, 600 );
	viewer.getCamera()->setClearColor( osg::Vec4( 0.0,0.0,0.0,1) );
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
	
	root->addChild(scene);
	CreateSol();
	CreationTank();
	scene->addChild(geodeSol);
	
	viewer.setSceneData(root);
	
	osg::ref_ptr<GestionEvenements> gestionnaire = new GestionEvenements();
	viewer.addEventHandler(gestionnaire.get());

	return viewer.run();
}


