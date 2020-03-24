/*
 * MyApp.cpp
 *
 *  Created on: 10 nov. 2019
 *      Author: joe
 */

#include "MyApp.h"
#include <iostream>

MyApp::MyApp() {
	_root=nullptr;
	_sceneManager=nullptr;
	_framelistener=nullptr;
}

MyApp::~MyApp() {
	delete _sceneManager;
	delete _framelistener;
	delete _root;
}

int MyApp::start() {
	_root = new Ogre::Root("plugins.cfg", "ogre.cfg", "Ogre.log");
	if (! _root->restoreConfig()) {
		_root->showConfigDialog();
		_root->saveConfig();
	}

	Ogre::RenderWindow* window = _root->initialise(true, "Mixing Light Colors");
	_sceneManager = _root->createSceneManager(Ogre::ST_GENERIC); // SceneTypeMask typeMask,
	_sceneManager->setAmbientLight(Ogre::ColourValue(1,1,1));

	Ogre::Camera* cam = _sceneManager->createCamera("MainCamera");
	cam->setPosition(Ogre::Vector3(-60,100,0));
	cam->lookAt(Ogre::Vector3(0,0,0));
	cam->setNearClipDistance(5);
	cam->setFarClipDistance(10000);

	Ogre::Viewport* viewport = window->addViewport(cam);
	viewport->setBackgroundColour(Ogre::ColourValue(0,0,0));
	double width=viewport->getActualWidth();
	double height=viewport->getActualHeight();
	cam->setAspectRatio(width/height);

	loadResources();
	createScene();

	_framelistener = new MyFrameListener(window, cam, _sceneManager);
	_root->addFrameListener(_framelistener);

	_root->startRendering();

	return 0;
}

void MyApp::loadResources() {
	Ogre::ConfigFile cf;
	cf.load("resource.cfg");

	Ogre::ConfigFile::SectionIterator sI = cf.getSectionIterator();
	Ogre::String sectionstr, typestr, datastr;

	while(sI.hasMoreElements()) {
		sectionstr = sI.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = sI.getNext();

		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i=settings->begin(); i!=settings->end(); ++i) {
			typestr=i->first; datastr=i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(datastr, typestr, sectionstr);
		}
	}
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void MyApp::createScene() {

	// Plano del suelo y luz direccional
	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, -10);
	Ogre::MeshManager::getSingleton().createPlane("plane",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
			1500, 1500, 200, 200, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);
	Ogre::Entity* ent = _sceneManager->createEntity("LightPlaneEntity", "plane");

	// crea un nodo al que se le asigna un nombre automáticamente y se añade una entidad
	_sceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
	// vamos a obtener el nombre del nodo
	Ogre::LogManager::getSingletonPtr()->logMessage(ent->getParentNode()->getName());

	ent->setMaterialName("Examples/BeachStones");


	Ogre::SceneNode* node = _sceneManager->createSceneNode("Node1");
	_sceneManager->getRootSceneNode()->addChild(node); // lo colgamos de root sin posicionar
	// Pongo en el nodo un cubo para ver el giro
	Ogre::Entity* ent1 = _sceneManager->createEntity("Cubo", "cube.mesh");
	node->attachObject(ent1);
	std::cout << "node: " << node->getPosition() << std::endl; // está en O,0,0

	// Punto de Luz
	Ogre::SceneNode* node2 = node->createChildSceneNode("node2");
	node2->setPosition(0.0f,100.0f,0.0f); // relativa al nodo padre
	Ogre::Light* light = _sceneManager->createLight("Light1");
	light->setType(Ogre::Light::LT_SPOTLIGHT);

	light->setDirection(Ogre::Vector3(1.0f,-1.0f,0.0f));
	light->setSpotlightInnerAngle(Ogre::Degree(5.0f));
	light->setSpotlightOuterAngle(Ogre::Degree(45.0f));
	light->setSpotlightFalloff(0.0f);

	light->setDiffuseColour(Ogre::ColourValue(0.0f,1.0f,0.0f));
	node2->attachObject(light);

	std::cout << "node2: " << node2->getPosition() << std::endl; // respecto nodo padre

	Ogre::Entity* LightEnt = _sceneManager->createEntity("MyEntity", "Sphere.mesh");
	Ogre::SceneNode* node3 = node->createChildSceneNode("node3");
	node3->setScale(0.8f,0.8f,0.8f);
	node3->setPosition(0.0f,20.0f,0.0f); // Si cambiamos a 0.5,20,0 vemos el giro
	// cuando esta en la misma vertical que node (Node1) cuando giramos Node1
	// en framelistener::frameStarted no se aprecia el giro
	node3->attachObject(LightEnt);

	std::cout << "node3: " << node3->getPosition() << std::endl;
	std::cout << "Nodo padre de nodo3: " << node3->getParent()->getName() << std::endl;

	Ogre::SceneNode* node4 = node->createChildSceneNode("node4", Ogre::Vector3(0.0f,20.0f,20.0f));
	Ogre::Entity* LightEnt2 = _sceneManager->createEntity("MyEntity2", "Sphere.mesh");
	node4->setScale(0.8f,0.8f,0.8f);
	node4->attachObject(LightEnt2);

	// Ogre::SceneNode* node5= node->createChildSceneNode("node5", Ogre::Vector3(0,100,0));
	Ogre::Light* light2 = _sceneManager->createLight("Light2");
	light2->setType(Ogre::Light::LT_SPOTLIGHT);

	light2->setDirection(1.0f,-1.0f,0.5f);
	light2->setSpotlightInnerAngle(Ogre::Degree(5.0f));
	light2->setSpotlightOuterAngle(Ogre::Degree(45.0f));
	light2->setSpotlightFalloff(0.0f);

	light2->setDiffuseColour(Ogre::ColourValue(1.0f,0.0f,0.0f));
	node2->attachObject(light2);


	// _sceneManager->setDisplaySceneNodes(true);
}


