#include <osgViewer/Viewer>
#include <osg/ShapeDrawable>
#include <osg/Material>
#include <osgDB/ReadFile>
#include <osg/Geometry>
#include <osg/PositionAttitudeTransform>
#include <osgGA/NodeTrackerManipulator>
#include <osgGA/GUIEventHandler>
#include <osgViewer/ViewerEventHandlers>
#include <osg/TexMat>
#include <osg/LightSource>
#include <osgUtil/LineSegmentIntersector>
#include <osgSim/DOFTransform>
#include <osgParticle/SmokeEffect>
#include <osgParticle/ExplosionEffect>
#include <ctime>
#include <cstdlib>
#include "renderToTexture.h"

osgViewer::Viewer viewer;
osg::ref_ptr<osg::Node> terrain;
osg::Node* noeudTourelle;
osg::PositionAttitudeTransform* LECHARRR;
osg::Vec3 posTank, normalTank;
osg::Quat rotation;
osg::ref_ptr<osgParticle::SmokeEffect> fumeeTank;
osg::Group* scene;
float posCanonX, posCanonY, posCanonZ;


class ChercheNoeud : public osg::NodeVisitor
{
public:
	ChercheNoeud ( const std::string& name )
	: osg::NodeVisitor( osg::NodeVisitor::TRAVERSE_ALL_CHILDREN ), _name( name ) {}
	// Méthode appelée pour chaque nœud du graphe. Si son nom correspond à celui passé
	// en paramètre au constructeur, on sauve l'adresse du nœud dans _node
	virtual void apply( osg::Node& node )
	{
	if (node.getName() == _name)
	_node = &node;
	traverse( node ); // On continue le parcours du graphe
	}
	osg::Node* getNode() { return _node.get(); }
protected:
	std::string _name;
	osg::ref_ptr<osg::Node> _node;
};

ChercheNoeud rechercheTourelle("turret");

bool intersection_terrain( float x, float y, osg::Node* terrain, osg::Vec3& inter, osg::Vec3& normal)
{
	osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector =
	new osgUtil::LineSegmentIntersector(osg::Vec3(x,y,1000), osg::Vec3(x, y,-1000));
	osgUtil::IntersectionVisitor iv(intersector.get());
	terrain->accept(iv);
	if (intersector->containsIntersections()){
		osgUtil::LineSegmentIntersector::Intersections& intersections = intersector->getIntersections();
		// Le segment étant vertical, on prend la première (et à priori unique) intersection trouvée
		const osgUtil::LineSegmentIntersector::Intersection& intersection = *(intersections.begin());
		inter = intersection.getWorldIntersectPoint();
		normal = intersection.getWorldIntersectNormal();
		
		return true;
	}
	
	return false;
} 

class Deplacement : public osg::NodeCallback
{
public:
    virtual void operator() (osg::Node* n, osg::NodeVisitor* nv)
    {
		/*osg::PositionAttitudeTransform* pos_tank = (osg::PositionAttitudeTransform*)n;
		osg::Vec3 pos, normal;
		intersection_terrain(pos_tank->getPosition().x(), pos_tank->getPosition().y() + 0.01, terrain, pos, normal);
		pos_tank->setPosition(pos);
		osg::Quat rotation;
		rotation.makeRotate(osg::Vec3f(0, 0, 1), normal);
		pos_tank->setAttitude(rotation);*/
    }
};



class GestionEvenements : public osgGA::GUIEventHandler
{
 public:
 virtual bool handle( const osgGA::GUIEventAdapter& ea,
 osgGA::GUIActionAdapter& aa);
};






osg::Node* creation_terrain(){
	osg::Image* heightMap = osgDB::readImageFile("terrain.tga");
	
	osg::HeightField* terrain = new osg::HeightField();
	terrain->allocate(heightMap->s(), heightMap->t());
	terrain->setOrigin(osg::Vec3(-heightMap->s() / 2, -heightMap->t() / 2, 0));
	terrain->setXInterval(10.0f);
	terrain->setYInterval(10.0f);
	
	for (unsigned int r = 0; r < terrain->getNumRows(); r++)
		for (unsigned int c = 0; c < terrain->getNumColumns(); c++)
			terrain->setHeight(c, r, ((*heightMap->data(c, r)) / 255.0f) * 300.0f);
			
	osg::Geode* geode = new osg::Geode();
	geode->addDrawable(new osg::ShapeDrawable(terrain));
	
	
	
	osg::Material* mat = new osg::Material;
	
	mat->setAmbient (osg::Material::FRONT_AND_BACK, osg::Vec4(0.5, 0.5, 0.5, 1.0));
	mat->setDiffuse (osg::Material::FRONT_AND_BACK, osg::Vec4(0.9, 0.9, 0.9, 1.0));
	mat->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0, 0.0, 0.0, 1.0));
	mat->setShininess(osg::Material::FRONT_AND_BACK, 64);
	geode->getOrCreateStateSet()->setAttributeAndModes(mat);
	
	
	
	
	osg::Texture2D* tex = new osg::Texture2D(osgDB::readImageFile("herbe.tga"));
	tex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
	tex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
	tex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	tex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
	geode->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex);
	
	osg::Matrixd matrix;
	matrix.makeScale(osg::Vec3(10, 10, 1.0));
	osg::ref_ptr<osg::TexMat> matTexture = new osg::TexMat;
	matTexture->setMatrix(matrix);
	geode->getOrCreateStateSet()->setTextureAttributeAndModes(0,
	matTexture.get(), osg::StateAttribute::ON);
	
	return geode;

}

osg::ref_ptr<osg::Group> creation_foret(osg::Node* terrain, int nb_arbres){
	osg::ref_ptr<osg::Group> foret = new osg::Group;
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D(osgDB::readImageFile("arbre.tga"));
	
	
	srand (static_cast <unsigned> (time(0)));
	float arbre_posx, arbre_posy, arbre_taille;
	for(int i=0; i<nb_arbres; ++i){
		
		arbre_posx = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1000));
		arbre_posy = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1000));
		arbre_taille = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/15)) + 5.0;
		osg::ref_ptr<osg::Geometry> quad = osg::createTexturedQuadGeometry(
			osg::Vec3(-arbre_taille/2, 0, 0), // coin de départ
			osg::Vec3(arbre_taille, 0.0, 0.0), // largeur
			osg::Vec3(0.0, 0.0, arbre_taille), // hauteur
			0.0, 0.0, 1.0, 1.0); // coordonnées de texture
		quad->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture.get());
		quad->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
		// On ajoute le quad dans le TRANSPARENT BIN, une collection d'objets transparents,
		// pour qu'ils soient triés selon la profondeur et affichés du plus éloigné au plus proche
		quad->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		// On crée le billboard et on lui attache le quad à orienter face à la caméra
		osg::ref_ptr<osg::Billboard> billboard = new osg::Billboard;
		billboard->setMode(osg::Billboard::AXIAL_ROT);
		billboard->setAxis(osg::Vec3(0,0,1));
		billboard->addDrawable(quad);
		// Optionnel : on désactive l'éclairage sur le billboard
		billboard->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		
		osg::PositionAttitudeTransform* pos_arbre = new osg::PositionAttitudeTransform;
		osg::Vec3 pos, normal;
		intersection_terrain(arbre_posx, arbre_posy, terrain, pos, normal);
		pos_arbre->setPosition(pos);
		pos_arbre->addChild(billboard);
		foret->addChild(pos_arbre);
	}
	
	return foret;
}

osg::PositionAttitudeTransform* creation_CHARRR(float posx, float posy, osg::Node* terrain){
	osg::Node* LECHARRR = osgDB::readNodeFile("t72-tank_des.flt");
	osg::PositionAttitudeTransform* pos_tank = new osg::PositionAttitudeTransform;
	osg::Vec3 pos, normal;
	intersection_terrain(posx, posy, terrain, pos, normal);
	pos_tank->setPosition(pos);
	osg::Quat rotation;
	rotation.makeRotate(osg::Vec3f(0, 0, 1), normal);
	pos_tank->setAttitude(rotation);
	pos_tank->addChild(LECHARRR);
	pos_tank->setUpdateCallback(new Deplacement);
	
	return pos_tank;
}

bool GestionEvenements::handle( const osgGA::GUIEventAdapter& ea,
 osgGA::GUIActionAdapter& aa)
{
	switch(ea.getEventType()){
		case osgGA::GUIEventAdapter::KEYDOWN :
		
			switch(ea.getKey()){
				
				case 'q':
					noeudTourelle = rechercheTourelle.getNode();
					if (noeudTourelle != NULL){
						osgSim::DOFTransform* tourelleDOF = dynamic_cast<osgSim::DOFTransform*>(noeudTourelle);
						tourelleDOF->setCurrentHPR(osg::Vec3(tourelleDOF->getCurrentHPR().x() + osg::DegreesToRadians(20.0), 0.0, 0.0));
					}
					break;
				case 'd':
					noeudTourelle = rechercheTourelle.getNode();
					if (noeudTourelle != NULL){
						osgSim::DOFTransform* tourelleDOF = dynamic_cast<osgSim::DOFTransform*>(noeudTourelle);
						tourelleDOF->setCurrentHPR(osg::Vec3(tourelleDOF->getCurrentHPR().x() - osg::DegreesToRadians(20.0), 0.0, 0.0));
					}
					break;
				case '8':
					intersection_terrain(LECHARRR->getPosition().x(), LECHARRR->getPosition().y() + 3, terrain, posTank, normalTank);
					LECHARRR->setPosition(posTank);
					rotation.makeRotate(osg::Vec3f(0, 0, 1), normalTank);
					LECHARRR->setAttitude(rotation);
					fumeeTank->setPosition(LECHARRR->getPosition());
					break;
				case '2':
					intersection_terrain(LECHARRR->getPosition().x(), LECHARRR->getPosition().y() - 3, terrain, posTank, normalTank);
					LECHARRR->setPosition(posTank);
					rotation.makeRotate(osg::Vec3f(0, 0, 1), normalTank);
					LECHARRR->setAttitude(rotation);
					fumeeTank->setPosition(LECHARRR->getPosition());
					break;
				case '4':
					intersection_terrain(LECHARRR->getPosition().x() - 3, LECHARRR->getPosition().y(), terrain, posTank, normalTank);
					LECHARRR->setPosition(posTank);
					rotation.makeRotate(osg::Vec3f(0, 0, 1), normalTank);
					LECHARRR->setAttitude(rotation);
					fumeeTank->setPosition(LECHARRR->getPosition());
					break;
				case '6':
					intersection_terrain(LECHARRR->getPosition().x() + 3, LECHARRR->getPosition().y(), terrain, posTank, normalTank);
					LECHARRR->setPosition(posTank);
					rotation.makeRotate(osg::Vec3f(0, 0, 1), normalTank);
					LECHARRR->setAttitude(rotation);
					fumeeTank->setPosition(LECHARRR->getPosition());
					break;
				case 'p':
					LECHARRR->setScale(LECHARRR->getScale() + osg::Vec3(1.0, 1.0, 1.0));
					LECHARRR->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
					fumeeTank->setScale(fumeeTank->getScale() + 5);
					break;
				case 'm':
					LECHARRR->setScale(LECHARRR->getScale() - osg::Vec3(1.0, 1.0, 1.0));
					LECHARRR->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
					fumeeTank->setScale(fumeeTank->getScale() - 5);
					break;
				case 'f':
				scene->addChild(new osgParticle::ExplosionEffect(osg::Vec3(posCanonX, posCanonY, posCanonZ), 1.0f, 1.0f));
					break;
			}
			break;
		
		case osgGA::GUIEventAdapter::PUSH :{
			int x = ea.getX();
			int y = ea.getY();
			if( ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
				//std::cout << "bouton gauche" << std::endl;
			if (ea.getButton() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
				//std::cout << "bouton milieu" << std::endl;
			if (ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
				//std::cout << "bouton droit" << std::endl;
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
	
	scene = new osg::Group;
	
	osg::ref_ptr<osg::LightSource> lumiere = new osg::LightSource;
	lumiere->getLight()->setLightNum(0); // GL_LIGHT1
	lumiere->getLight()->setPosition(osg::Vec4(1, -1, 10, 0)); // 0 = directionnel
	lumiere->getLight()->setAmbient(osg::Vec4(0.5, 0.5, 0.5, 1.0));
	lumiere->getLight()->setDiffuse(osg::Vec4(0.9, 0.9, 0.9, 1.0));
	lumiere->getLight()->setSpecular(osg::Vec4(1.0, 1.0, 1.0, 1.0));
	scene->addChild(lumiere.get());
	
	terrain = creation_terrain();
	scene->addChild(terrain.get());
	scene->addChild(creation_foret(terrain, 500));
	
	LECHARRR = creation_CHARRR(0,0,terrain);
	scene->addChild(LECHARRR);
	
	LECHARRR->accept(rechercheTourelle);
	
	fumeeTank = new osgParticle::SmokeEffect;
	fumeeTank->setTextureFileName("fumee.tga");
	fumeeTank->setIntensity(2);
	fumeeTank->setScale(4);
	fumeeTank->setPosition(LECHARRR->getPosition());
	scene->addChild(fumeeTank.get());
	posCanonX = LECHARRR->getPosition().x();
	posCanonY = LECHARRR->getPosition().y() + 3.5;
	posCanonZ = LECHARRR->getPosition().z() + 4.0;
	
	
	
	
	viewer.setSceneData(scene);
	
	osg::ref_ptr<GestionEvenements> gestionnaire = new GestionEvenements();
	viewer.addEventHandler(gestionnaire.get());
	
	return viewer.run();
}



// Rotation de 20° par rapport à l'axe (0,0,1)

