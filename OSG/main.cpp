po#include <osgViewer/Viewer>
#include <osg/ShapeDrawable>
#include <osg/Material>
#include <osgDB/ReadFile>
#include <osg/Geometry>
#include <osg/PositionAttitudeTransform>
#include <osgGA/NodeTrackerManipulator>
#include <osgGA/GUIEventHandler>
#include <osgViewer/ViewerEventHandlers>

osgViewer::Viewer viewer;
osg::ref_ptr<osgGA::NodeTrackerManipulator> trackCone = new osgGA::NodeTrackerManipulator;
osg::ref_ptr<osgGA::NodeTrackerManipulator> trackBoite = new osgGA::NodeTrackerManipulator;
osg::ref_ptr<osgGA::NodeTrackerManipulator> trackSphere = new osgGA::NodeTrackerManipulator;
osg::PositionAttitudeTransform* transformAregne = new osg::PositionAttitudeTransform();

class Deplacement : public osg::NodeCallback
{
public:
    virtual void operator() (osg::Node* n, osg::NodeVisitor* nv)
    {
        // code pour modifier le nœud, par exemple la position si il s 'agit
        // d'un nœud de type osg::PositionAttitudeTransform :
        osg::PositionAttitudeTransform* pos = (osg::PositionAttitudeTransform*)n;
        pos->setPosition(osg::Vec3(pos->getPosition().x(), pos->getPosition().y(), pos->getPosition().z() + 0.01));
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
					viewer.setCameraManipulator(trackCone.get());
					break;
				case 'z':
					viewer.setCameraManipulator(trackBoite.get());
					break;
				case 'e':
					viewer.setCameraManipulator(trackSphere.get());
					break;
			}
			break;
		
		case osgGA::GUIEventAdapter::PUSH :{
			int x = ea.getX();
			int y = ea.getY();
			if( ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
				std::cout << "bouton gauche" << std::endl;
			if (ea.getButton() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
				std::cout << "bouton milieu" << std::endl;
			if (ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
				std::cout << "bouton droit" << std::endl;
			break;
 }
		case osgGA::GUIEventAdapter::DOUBLECLICK :
			break;
			}
 return false; // pour que l'événement soit traité par d'autres gestionnaires
}

int main(void){
	osg::DisplaySettings::instance()->setNumMultiSamples( 4 );
	viewer.setUpViewInWindow( 100, 50, 800, 600 );
	viewer.getCamera()->setClearColor( osg::Vec4( 0.5,0.5,0.5,1) );
	viewer.addEventHandler(new osgViewer::StatsHandler);
	
	osg::Group* scene = new osg::Group;
	
	// Création d'une boîte centrée à l'origine, de dimensions 2x3x4:
	osg::Box* boite = new osg::Box(osg::Vec3(-10, 0, 0), 2,3,4);
	osg::ShapeDrawable* boiteDrawable = new osg::ShapeDrawable(boite);
	osg::Geode* geodeBoite = new osg::Geode();
	geodeBoite->addDrawable(boiteDrawable);
	
	osg::Sphere* sphere = new osg::Sphere( osg::Vec3(10,0,0), 1.0);
	osg::ShapeDrawable* sphereDrawable = new osg::ShapeDrawable(sphere);
	osg::Geode* geodeSphere = new osg::Geode();
	geodeSphere->addDrawable(sphereDrawable);
	
	osg::Capsule* capsule = new osg::Capsule(osg::Vec3(0, 0, 0), 1.0, 3.0);
	osg::ShapeDrawable* capsuleDrawable = new osg::ShapeDrawable(capsule);
	osg::Geode* geodeCapsule = new osg::Geode();
	geodeCapsule->addDrawable(capsuleDrawable);
	
	osg::Cone* cone = new osg::Cone(osg::Vec3(0, 10, 0), 1, 2);
	osg::ShapeDrawable* coneDrawable = new osg::ShapeDrawable(cone);
	osg::Geode* geodeCone= new osg::Geode();
	geodeCone->addDrawable(coneDrawable);
	
	
	osg::Material* matBoite = new osg::Material;
	matBoite->setAmbient (osg::Material::FRONT_AND_BACK, osg::Vec4(0.5, 0.0, 0.0, 1.0));
	matBoite->setDiffuse (osg::Material::FRONT_AND_BACK, osg::Vec4(0.9, 0.0, 0.0, 1.0));
	matBoite->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.2, 0.2, 0.2, 1.0));
	matBoite->setShininess(osg::Material::FRONT_AND_BACK, 64);
	
	osg::Material* matCone = new osg::Material;
	matCone->setAmbient (osg::Material::FRONT_AND_BACK, osg::Vec4(0.5, 0.0, 0.5, 1.0));
	matCone->setDiffuse (osg::Material::FRONT_AND_BACK, osg::Vec4(0.9, 0.0, 0.9, 1.0));
	matCone->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.2, 0.2, 0.2, 1.0));
	matCone->setShininess(osg::Material::FRONT_AND_BACK, 64);
	
	osg::Node* aregne = osgDB::readNodeFile("cow_high.3ds"); 
	
	transformAregne->setPosition(osg::Vec3(5, 0, 0));
	transformAregne->setScale(osg::Vec3(0.2, 0.2, 0.2));
	transformAregne->getOrCreateStateSet()->setMode(GL_NORMALIZE,osg::StateAttribute::ON); 
	transformAregne->addChild(aregne);
	
	boiteDrawable->getOrCreateStateSet()->setAttributeAndModes(matBoite);
	coneDrawable->getOrCreateStateSet()->setAttributeAndModes(matCone);
	
	
	/*scene->addChild(geodeCapsule);
	scene->addChild(geodeCone);
	scene->addChild(geodeBoite);
	scene->addChild(geodeSphere);
	scene->addChild(transformAregne);*/
	scene->addChild(aregne);
	
	// Création d'une texture
osg::ref_ptr<osg::Texture2D> tex2D = new osg::Texture2D;
tex2D->setTextureSize(1024, 1024);
tex2D->setInternalFormat(GL_RGBA);
// Création d'une caméra qui effectuera son rendu dans la texture
osg::ref_ptr<osg::Camera> rttCamera =
 createRTTCamera(osg::Camera::COLOR_BUFFER, tex2D.get());
// On indique la partie du graphe que la caméra devra rendre, ici toute la scène :
rttCamera->addChild(scene);
// Création d'une caméra permettant d'afficher un HUD qui couvrira tout l'écran
osg::ref_ptr<osg::Camera> hudCamera = createHUDCamera();
osg::Geode* screenQuad = createScreenQuad();
hudCamera->addChild(screenQuad);
osg::StateSet* stateset = screenQuad->getOrCreateStateSet();
stateset->setTextureAttributeAndModes(0, tex2D.get());
// VOUS METTREZ ICI LE CODE DE LA QUESTION 7
// Création d'une nouvelle racine du graphe, à laquelle on rattache la caméra
// de rendu dans une texture, la caméra du HUD et la racine du graphe de la scène
osg::ref_ptr<osg::Group> root = new osg::Group;
root->addChild(rttCamera.get());
root->addChild(hudCamera.get());
root->addChild(scene);
// Indique au viewer la scène à affich
	
	trackCone->setTrackNode(geodeCone);
	trackCone->setTrackerMode(osgGA::NodeTrackerManipulator::NODE_CENTER);
	
	
	trackBoite->setTrackNode(geodeBoite);
	trackBoite->setTrackerMode(osgGA::NodeTrackerManipulator::NODE_CENTER);
	
	trackSphere->setTrackNode(geodeSphere);
	trackSphere->setTrackerMode(osgGA::NodeTrackerManipulator::NODE_CENTER);
	
	transformAregne->setUpdateCallback(new Deplacement);
	
	viewer.setSceneData(scene);
	
	osg::ref_ptr<GestionEvenements> gestionnaire = new GestionEvenements();
	viewer.addEventHandler(gestionnaire.get());

	return viewer.run();
}



// Rotation de 20° par rapport à l'axe (0,0,1)

