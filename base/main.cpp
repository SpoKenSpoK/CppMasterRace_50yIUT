#include <osgViewer/Viewer>
#include <osg/ShapeDrawable>
#include <osg/Material>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/Geometry>
#include <osg/PositionAttitudeTransform>
#include <osgGA/NodeTrackerManipulator>
#include <osgGA/GUIEventHandler>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/DriveManipulator>
#include "renderToTexture.h"
//#include "fpsCamera.h"

osg::ref_ptr<osg::Group> root;
osgViewer::Viewer viewer;
osg::Group* scene;

osg::ref_ptr<osg::Geometry> quadSol;
osg::ref_ptr<osg::Texture2D> textureSol;

osg::ref_ptr<osg::PositionAttitudeTransform> transformFeet;
osg::ref_ptr<osg::Node> feet;

osg::ref_ptr<osg::Geode> geodeSol;

osg::ref_ptr<osgGA::DriveManipulator> manip;

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

osg::Group* creation_troupeau_chikoiseau(int nb_chikoiseau, float taillex, float tailley){

	osg::Sphere* corpsChikoiseau = new osg::Sphere(osg::Vec3(0,0,5.0), 1.0);
	osg::ShapeDrawable* shapeDrawable = new osg::ShapeDrawable(corpsChikoiseau);
	osg::Geode* geode = new osg::Geode();
	geode->addDrawable(shapeDrawable);
	
	// create a simple material
	osg::Material *material = new osg::Material();
	material->setEmission(osg::Material::FRONT, osg::Vec4(0.8, 0.8, 0.8, 1.0));

	// create a texture
	// load image for texture
	osg::Image *image = osgDB::readImageFile("remy.jpg");
	if (!image) {
		std::cout << "Couldn't load texture." << std::endl;
		return NULL;
	}
	osg::Texture2D *texture = new osg::Texture2D;
	texture->setDataVariance(osg::Object::DYNAMIC);
	texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
	texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP);
	texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP);
	texture->setImage(image);

	// assign the material and texture to the sphere
	osg::StateSet *sphereStateSet = geode->getOrCreateStateSet();
	sphereStateSet->ref();
	sphereStateSet->setAttribute(material);
	sphereStateSet->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
	
	
	osg::Group* troupeau = new osg::Group;
	for(unsigned int i = 0; i < nb_chikoiseau; ++i){
		osg::PositionAttitudeTransform* transformChikoiseau = new osg::PositionAttitudeTransform();
		transformChikoiseau->setPosition(osg::Vec3(rand()%(int)taillex, rand()%(int)tailley, 0));
		float angle = rand()%360;
		transformChikoiseau->setAttitude(osg::Quat(osg::DegreesToRadians(angle), osg::Vec3(0.0, 0.0, 1.0)));
		transformChikoiseau->addChild(geode);
		//transformChikoiseau->setUpdateCallback(new Deplacement);
		troupeau->addChild(transformChikoiseau);
	}
	return troupeau;
}

void Creationfeet(){
	
	feet = osgDB::readNodeFile("jack.3DS");
	
	transformFeet = new osg::PositionAttitudeTransform;
	transformFeet->setUpdateCallback(new Rotation);
	transformFeet->setPosition(osg::Vec3(0,0,0));
	//transformFeet->setScale(osg::Vec3(0.01,0.01,0.01));
	//transformFeet->setScale(osg::Vec3(1000,1000,1000));
	transformFeet->getOrCreateStateSet()->setMode(GL_NORMALIZE,osg::StateAttribute::ON); 
	transformFeet->addChild(feet);
	
	scene->addChild(transformFeet);
}

int main(void){
	osg::DisplaySettings::instance()->setNumMultiSamples( 4 );
	viewer.setUpViewInWindow( 100, 50, 800, 600 );
	viewer.getCamera()->setClearColor( osg::Vec4( 0.0,0.0,0.0,1) );
	viewer.addEventHandler(new osgViewer::StatsHandler);
	manip = new osgGA::DriveManipulator();
	viewer.setCameraManipulator(manip.get());
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
	Creationfeet();
	scene->addChild(geodeSol);
	scene->addChild(creation_troupeau_chikoiseau(50, 100, 100));
	viewer.setSceneData(root);
	
	osg::ref_ptr<GestionEvenements> gestionnaire = new GestionEvenements();
	viewer.addEventHandler(gestionnaire.get());

	return viewer.run();
}


