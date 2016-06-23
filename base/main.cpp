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
#include <osgSim/DOFTransform>
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

float fieldX = 100.0;
float fieldY = 100.0;

class RefreshSpeed : public osg::NodeCallback
{
public:
    virtual void operator() (osg::Node* n, osg::NodeVisitor* nv)
    {
		double vitesse = 0.0;
		vitesse = manip->getVelocity();
		if(vitesse < 0.0) vitesse *= -1.0;
        if(vitesse > 10.0) vitesse -= 1.0;
        manip->setVelocity(vitesse);
        manip->setIntersectTraversalMask(0);
    }
};

float angleAilesG = 0.0;

class FlapFlapG : public osg::NodeCallback
{
public:
    virtual void operator() (osg::Node* n, osg::NodeVisitor* nv)
    {
		osg::PositionAttitudeTransform* pos = (osg::PositionAttitudeTransform*)n;
		static bool monte = true;
		if(monte){
			angleAilesG += 0.08;
			if(angleAilesG>50) monte = false;
		}
		if(!monte){
			angleAilesG -= 0.08;
			if(angleAilesG<-50) monte = true;
		}
		pos->setAttitude(osg::Quat(osg::DegreesToRadians(angleAilesG), osg::Vec3(0.0, 1.0, 0.0)));
    }
};

float angleAilesD = 0.0;

class FlapFlapD : public osg::NodeCallback
{
public:
    virtual void operator() (osg::Node* n, osg::NodeVisitor* nv)
    {
		osg::PositionAttitudeTransform* pos = (osg::PositionAttitudeTransform*)n;
		static bool monteD = true;
		if(monteD){
			angleAilesD -= 0.08;
			if(angleAilesD<-50) monteD = false;
		}
		if(!monteD){
			angleAilesD += 0.08;
			if(angleAilesD>50) monteD = true;
		}
		pos->setAttitude(osg::Quat(osg::DegreesToRadians(angleAilesD), osg::Vec3(0.0, 1.0, 0.0)));
    }
};

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
	osg::Vec3(fieldX, 0.0, 0.0),  // largeur
	osg::Vec3(0.0, fieldY, 0.0),  // hauteur
	0.0, 0.0, 30.0, 30.0); 		// Coordonnées de texture gauche/bas/droit/haut
								// Si vous mettez 4.0 à la place de 1.0,
								// la texture sera répétée 4 fois
	quadSol->getOrCreateStateSet()->setTextureAttributeAndModes(0, textureSol.get());
	quadSol->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

	geodeSol = new osg::Geode;
	geodeSol->addDrawable(quadSol);
}

void recursiveExtremite(int& x, int& y, const float& tx, const float& ty){
	if( x < 0 or x > tx or y < 0 or y > ty){
		x = rand()%(int)tx;
		y = rand()%(int)ty;

		recursiveExtremite(x, y, tx, ty);
	}	
}

osg::ref_ptr<osg::Group> creation_troupeau_touches(int nb_touche, float taillex, float tailley){
    osg::ref_ptr<osg::Node> feetD = osgDB::readNodeFile("feetD.obj");
    osg::ref_ptr<osg::Node> feetG = osgDB::readNodeFile("feetG.obj");
    osg::ref_ptr<osg::Node> touche = osgDB::readNodeFile("key2.stl");

    osg::ref_ptr<osg::Group> touches = new osg::Group;
    for(unsigned int i=0; i<= nb_touche;  ++i){
        int randX = rand()%(int)taillex;
		int randY = rand()%(int)tailley;
        float angle = rand()%360;

        osg::ref_ptr<osg::PositionAttitudeTransform> tsFeetD = new osg::PositionAttitudeTransform();
        osg::ref_ptr<osg::PositionAttitudeTransform> tsFeetG = new osg::PositionAttitudeTransform();
        osg::ref_ptr<osg::PositionAttitudeTransform> tsTouche = new osg::PositionAttitudeTransform();

        tsTouche->setScale(osg::Vec3(0.2, 0.2, 0.2));
        tsFeetD->setScale(osg::Vec3(0.015, 0.015, 0.015));
        tsFeetG->setScale(osg::Vec3(0.015, 0.015, 0.015));

        tsTouche->setPosition(osg::Vec3(randX, randY, 3.5));
        tsFeetD->setPosition(osg::Vec3(randX-1.0, randY-0.6, 2.7));
        tsFeetG->setPosition(osg::Vec3(randX+1.0, randY-0.6, 2.7));

        tsTouche->addChild(touche);
        tsFeetD->addChild(feetD);
        tsFeetG->addChild(feetG);

		osg::ref_ptr<osg::PositionAttitudeTransform> theTouche = new osg::PositionAttitudeTransform();

		theTouche->addChild(tsTouche);
		theTouche->addChild(tsFeetD);
		theTouche->addChild(tsFeetG);

		theTouche->setAttitude(osg::Quat(osg::DegreesToRadians(angle), osg::Vec3(0.0, 0.0, 1.0)));

		//recursiveExtremite(randX, randY, taillex, tailley);

		//float tempOne = randX+(taillex/2);
		//float tempTwo = randY-(tailley/2);

		//theTouche->setPosition(osg::Vec3(tempOne, tempTwo, -1.0));

		touches->addChild(theTouche);
    }
    return touches;
}



osg::Group* creation_troupeau_chikoiseau(int nb_chikoiseau, float taillex, float tailley){

	osg::Sphere* corpsChikoiseau = new osg::Sphere(osg::Vec3(0.0,0.0,5.0), 1.0);
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
    //assign the material and texture to the sphere
    osg::StateSet *sphereStateSet = geode->getOrCreateStateSet();
    sphereStateSet->ref();
	sphereStateSet->setAttribute(material);
	sphereStateSet->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);

	osg::Node* aileD = osgDB::readNodeFile("wingD.obj");
	osg::Node* aileG = osgDB::readNodeFile("wingG.obj");

	osg::Group* troupeau = new osg::Group;
	for(unsigned int i = 0; i < nb_chikoiseau; ++i){
		int randX = rand()%(int)taillex;
		int randY = rand()%(int)tailley;
		osg::PositionAttitudeTransform* transformChikoiseau = new osg::PositionAttitudeTransform();
		transformChikoiseau->setPosition(osg::Vec3(randX, randY, 1.0));
		float angle = rand()%360;
		transformChikoiseau->setAttitude(osg::Quat(osg::DegreesToRadians(angle), osg::Vec3(0.0, 0.0, 1.0)));
		transformChikoiseau->addChild(geode);
		//transformChikoiseau->setUpdateCallback(new Deplacement);
		osg::PositionAttitudeTransform* transformAileG = new osg::PositionAttitudeTransform();
		transformAileG->setPosition(osg::Vec3(randX, randY, 6.0));
		transformAileG->setAttitude(osg::Quat(osg::DegreesToRadians(angle+90), osg::Vec3(0.0, 0.0, 1.0)));
		osg::PositionAttitudeTransform* transformAileD = new osg::PositionAttitudeTransform();
		transformAileD->setAttitude(osg::Quat(osg::DegreesToRadians(angle+90), osg::Vec3(0.0, 0.0, 1.0)));
		transformAileD->setPosition(osg::Vec3(randX, randY, 6.0));

		transformAileG->setScale(osg::Vec3(0.5,0.5,0.5));
		transformAileD->setScale(osg::Vec3(0.5,0.5,0.5));

		osg::PositionAttitudeTransform* Chikoiseau = new osg::PositionAttitudeTransform();

		osg::PositionAttitudeTransform* aileDRotate = new osg::PositionAttitudeTransform();
		osg::PositionAttitudeTransform* aileGRotate = new osg::PositionAttitudeTransform();
		aileDRotate->addChild(aileG);
		aileGRotate->addChild(aileD);
		aileDRotate->setUpdateCallback(new FlapFlapD);
		aileGRotate->setUpdateCallback(new FlapFlapG);
		transformAileG->addChild(aileGRotate);
		transformAileD->addChild(aileDRotate);
		Chikoiseau->addChild(transformAileG);
		Chikoiseau->addChild(transformAileD);
		Chikoiseau->addChild(transformChikoiseau);

		troupeau->addChild(Chikoiseau);
	}
	return troupeau;
}

int main(void){
	osg::DisplaySettings::instance()->setNumMultiSamples( 4 );
	viewer.setUpViewInWindow( 100, 50, 800, 600 );
	viewer.getCamera()->setClearColor( osg::Vec4( 0.0,0.0,0.0,1) );
	viewer.addEventHandler(new osgViewer::StatsHandler);
	manip = new osgGA::DriveManipulator();
	//viewer.setCameraManipulator(manip.get());
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
	scene->addChild(geodeSol);
	scene->addChild(creation_troupeau_chikoiseau(50, fieldX, fieldY));
    //scene->addChild(creation_troupeau_touches(15, fieldX, fieldY));

	osg::ref_ptr<GestionEvenements> gestionnaire = new GestionEvenements();
	viewer.addEventHandler(gestionnaire.get());

	return viewer.run();
}
