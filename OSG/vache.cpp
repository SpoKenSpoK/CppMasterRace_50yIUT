#include <osgViewer/Viewer>
#include <osg/ShapeDrawable>
#include <osg/Material>
#include <osgDB/ReadFile>
#include <osg/Geometry>
#include <osg/PositionAttitudeTransform>
#include <osgGA/NodeTrackerManipulator>
#include <osgGA/GUIEventHandler>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Fog>
#include <osgParticle/PrecipitationEffect>
#include <osg/LightSource>
#include <osgShadow/ShadowedScene>
#include <osgShadow/ShadowMap>
#include <osgGA/DriveManipulator>
#include <osgText/Text>
#include <stdlib.h> 
#include <string> 
#include <sstream>
#include "renderToTexture.h"

osg::ref_ptr<osgGA::DriveManipulator> cameraVoiture = new osgGA::DriveManipulator();
osgText::Text* text = new osgText::Text;

class RefreshSpeed : public osg::NodeCallback
{
public:
    virtual void operator() (osg::Node* n, osg::NodeVisitor* nv)
    {
		double vitesse = 0;
		vitesse = cameraVoiture->getVelocity();
		if(vitesse < 0)
			vitesse *= -1;
			
		if(vitesse > 180)
			text->setPosition(osg::Vec3(50.0 + -20*cos(180 / 180 * 3.14), 50.0f + 20*sin(180 / 180 * 3.14), 0.0f));
		else
			text->setPosition(osg::Vec3(50.0 + -20*cos(vitesse / 180 * 3.14), 50.0f + 20*sin(vitesse / 180 * 3.14), 0.0f));
		
		
		std::ostringstream strs;
		strs << (int)vitesse;
		std::string str = strs.str();
		text->setText(str);
    }
};

osg::Node* creationHUD()
{
	 // On crée une caméra qui correspond à un écran de 1280x1024
	 osg::Camera* camera = new osg::Camera;
	 camera->setProjectionMatrix(osg::Matrix::ortho2D(0, 1280, 0, 1024));
	 camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	 camera->setViewMatrix(osg::Matrix::identity());
	 camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	 // Le sous-graphe de la caméra sera affiché après celui de la caméra principale,
	 // donc par dessus le reste de la scène.
	 camera->setRenderOrder(osg::Camera::POST_RENDER);
	 // Les éléments graphiques du HUD (ici un simple texte) constitueront un sous-graphe
	 // de la caméra que l'on vient de créer
	 text->setPosition(osg::Vec3(50.0f, 50.0f, 0.0f));
	 text->setText("0");
	 text->setCharacterSize(20);
	 text->setFont("arial.ttf");
	 
	 osg::Geode* geode = new osg::Geode();
	 geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	 geode->addDrawable(text);
	 camera->addChild(geode);
	 
	
	 
	 camera->setUpdateCallback(new RefreshSpeed);
	 return camera;
}

class Deplacement : public osg::NodeCallback
{
public:
    virtual void operator() (osg::Node* n, osg::NodeVisitor* nv)
    {
		static bool saute = true;
        // code pour modifier le nœud, par exemple la position si il s 'agit
        // d'un nœud de type osg::PositionAttitudeTransform :
        osg::PositionAttitudeTransform* pos = (osg::PositionAttitudeTransform*)n;

        if(saute){
			pos->setPosition(osg::Vec3(pos->getPosition().x(), pos->getPosition().y(), pos->getPosition().z() + 0.1));
			if(pos->getPosition().z()>10) saute = false;
		}
		if(!saute){
			pos->setPosition(osg::Vec3(pos->getPosition().x(), pos->getPosition().y(), pos->getPosition().z() - 0.1));
			if(pos->getPosition().z() < 0) saute = true;
		}
			
    }
};



osg::Group* creation_troupeau(int nb_vaches, float taillex, float tailley){
	osg::LOD* lod = new osg::LOD;
	lod->setRangeMode( osg::LOD::DISTANCE_FROM_EYE_POINT );
	
	osg::Node* vacheh = osgDB::readNodeFile("cow_high.3ds");
	lod->addChild( vacheh, 0, 10 ); 
	osg::Node* vachem = osgDB::readNodeFile("cow_mid.3ds");
	lod->addChild( vachem, 10, 100 ); 
	osg::Node* vachel = osgDB::readNodeFile("cow_low.3ds");
	lod->addChild( vachel, 100, 10000 ); 
	
	
	osg::Group* troupeau = new osg::Group;
	for(unsigned int i = 0; i < nb_vaches; ++i){
		osg::PositionAttitudeTransform* transformVache = new osg::PositionAttitudeTransform();
		transformVache->setPosition(osg::Vec3(rand()%(int)taillex, rand()%(int)tailley, 0));
		float angle = rand()%360;
		transformVache->setAttitude(osg::Quat(osg::DegreesToRadians(angle), osg::Vec3(0.0, 0.0, 1.0)));
		transformVache->addChild(lod);
		//transformVache->setUpdateCallback(new Deplacement);
		troupeau->addChild(transformVache);
	}
	return troupeau;
}

osg::Node* creation_sol(float taillex, float tailley){
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	texture->setImage(osgDB::readImageFile("herbe.jpg"));
	texture->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
	texture->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
	texture->setWrap( osg::Texture::WRAP_S, osg::Texture::REPEAT );
	texture->setWrap( osg::Texture::WRAP_T, osg::Texture::REPEAT );
	
	osg::ref_ptr<osg::Geometry> quad = osg::createTexturedQuadGeometry(
		osg::Vec3(0.0, 0.0, 0.0), // Coin de départ
		osg::Vec3(taillex, 0.0, 0.0), // largeur
		osg::Vec3(0.0, tailley, 0.0), // hauteur
		0.0, 0.0, 1.0, 1.0); // Coordonnées de texture gauche/bas/droit/haut
	// Si vous mettez 4.0 à la place de 1.0,
	// la texture sera répétée 4 fois
	quad->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture.get());
	quad->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	osg::Geode* geode = new osg::Geode;
	geode->addDrawable(quad);
	return geode;
}

int main(void){
	float taillex = 200, tailley = 200;
	int nb_vaches = 2000;
	
	osgViewer::Viewer viewer;
	osg::DisplaySettings::instance()->setNumMultiSamples( 4 );
	viewer.setUpViewInWindow( 100, 50, 800, 600 );
	viewer.getCamera()->setClearColor( osg::Vec4( 0.5,0.5,0.5,1) );
	viewer.addEventHandler(new osgViewer::StatsHandler);
	osg::Group* scene = new osg::Group;
	
	osg::ref_ptr<osgShadow::ShadowedScene> shadowScene = new osgShadow::ShadowedScene;
	osg::ref_ptr<osgShadow::ShadowMap> sm = new osgShadow::ShadowMap;
	shadowScene->setShadowTechnique(sm.get());
	
	
	shadowScene->addChild(creation_sol(taillex,tailley));
	shadowScene->addChild(creation_troupeau(nb_vaches,taillex,tailley));
	//Brouillard
	osg::Fog* pFog = new osg::Fog();
	pFog->setMode(osg::Fog::LINEAR);
	pFog->setColor(osg::Vec4(1, 1, 1, 1));
	pFog->setStart(10);
	pFog->setEnd(800);
	//scene->getOrCreateStateSet()->setAttribute(pFog, osg::StateAttribute::ON);
	//scene->getOrCreateStateSet()->setMode(GL_FOG, osg::StateAttribute::ON);
	
	//pluie/neige
	osg::ref_ptr<osgParticle::PrecipitationEffect> precipNode = new osgParticle::PrecipitationEffect;
	precipNode->setWind(osg::Vec3(1,1,1));
	precipNode->setParticleSpeed(0.4);
	precipNode->snow(0.3); // ou « snow » pour de la neige
	scene->addChild(precipNode.get());

	osg::StateSet* state = scene->getOrCreateStateSet(); 
	state->setMode( GL_LIGHT0, osg::StateAttribute::OFF );
	
	osg::ref_ptr<osg::LightSource> lumiere = new osg::LightSource;
	lumiere->getLight()->setLightNum(1); // GL_LIGHT1
	lumiere->getLight()->setPosition(osg::Vec4(0,0, 50, 1)); // 0 = directionnel
	lumiere->getLight()->setAmbient(osg::Vec4(0.5, 0.0, 0.0, 1.0));
	lumiere->getLight()->setDiffuse(osg::Vec4(1, 0, 0, 1.0));
	lumiere->getLight()->setSpecular(osg::Vec4(1, 0, 0, 1.0));
	scene->addChild(lumiere.get());
	state->setMode( GL_LIGHT1, osg::StateAttribute::ON ); 

	shadowScene->addChild(lumiere.get());



	// Création d'une texture
	osg::ref_ptr<osg::Texture2D> tex2D = new osg::Texture2D;
	tex2D->setTextureSize(1024, 1024);
	tex2D->setInternalFormat(GL_RGBA);
	// Création d'une caméra qui effectuera son rendu dans la texture
	osg::ref_ptr<osg::Camera> rttCamera =
	 createRTTCamera(osg::Camera::COLOR_BUFFER, tex2D.get(), true);
	// On indique la partie du graphe que la caméra devra rendre, ici toute la scène :
	rttCamera->addChild(scene);
	// Positionnement de la caméra
	rttCamera->setProjectionMatrixAsPerspective(30, 4.0/3.0, 0.1, 1000);
	rttCamera->setViewMatrixAsLookAt(
	osg::Vec3(20, 20, 1), // position de la caméra
	osg::Vec3(20, 0, 1), // point visé par la caméra
	osg::Vec3(0, 0, 1)); // vecteur perpendiculaire à la caméra
	// Création d'une nouvelle racine du graphe, à laquelle on rattache la caméra
	// de rendu dans une texture et la racine du graphe de la scène
	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(rttCamera.get());
	root->addChild(scene);
	// Création d'un quadrilatère texturé avec la texture provenant de la caméra
	osg::ref_ptr<osg::Geometry> quad = osg::createTexturedQuadGeometry(
	osg::Vec3(0.0, 50.0, 0.0), // Coin de départ
	osg::Vec3(20.0, 0.0, 0.0), // Largeur
	osg::Vec3(0.0, 0.0, 15.0), // Hauteur
	0.0, 0.0, 1.0, 1.0); // Coordonnées de texture
	quad->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex2D.get());
	quad->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	osg::Geode* geode = new osg::Geode;
	geode->addDrawable(quad);
	shadowScene->addChild(geode);
	// Indique au viewer la scène à afficher
	viewer.setSceneData(root.get());



	scene->addChild(shadowScene);
	scene->addChild(creationHUD());
	//viewer.setCameraManipulator(cameraVoiture.get());
	//viewer.setSceneData(scene);
	


	return viewer.run();
}
