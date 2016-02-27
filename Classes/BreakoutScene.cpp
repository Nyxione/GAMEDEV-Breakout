#include "BreakoutScene.h"

USING_NS_CC;

Scene* Breakout::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	Vect gravity(0.0f, 0.0f); 
	scene->getPhysicsWorld()->setGravity(gravity);
    
    // 'layer' is an autorelease object
    auto layer = Breakout::create();
	layer->SetPhysicsWorld(scene->getPhysicsWorld());

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool Breakout::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// Edge
	edge = Sprite::create();
	auto boundBody = PhysicsBody::createEdgeBox(visibleSize, PHYSICSBODY_MATERIAL_DEFAULT, 3);
	boundBody->getShape(0)->setRestitution(1.0f);
	boundBody->getShape(0)->setFriction(0.0f);
	boundBody->getShape(0)->setDensity(1.0f);
	edge->setPosition(Point(visibleSize.width / 2, visibleSize.height / 2));
	boundBody->setContactTestBitmask(0x000001);
	edge->setTag(0);
	this->addChild(edge);

	// Bouncy ball
	ball = Sprite::create("ball.png");
	ball->setPosition(100, 100);
	auto ballBody = PhysicsBody::createCircle(ball->getContentSize().width / 2);
	ballBody->getShape(0)->setRestitution(1.0f);
	ballBody->getShape(0)->setFriction(0.0f);
	ballBody->getShape(0)->setDensity(1.0f);
	ballBody->setGravityEnable(false);
	Vect force = Vect(1010000.0f, 1010000.0f);
	ballBody->applyImpulse(force);
	ball->setPhysicsBody(ballBody);
	ballBody->setContactTestBitmask(0x000001);
	ball->setTag(1);
	this->addChild(ball);

	// Paddle pop
	paddle = Sprite::create("paddle.png");
	auto paddleBody = PhysicsBody::createBox(paddle->getContentSize());
	paddleBody->getShape(0)->setRestitution(1.0f);
	paddleBody->getShape(0)->setFriction(0.0f);
	paddleBody->getShape(0)->setDensity(10.0f);
	paddleBody->setGravityEnable(false);
	paddleBody->setDynamic(false);
	paddle->setPosition(visibleSize.width / 2, 50);
	paddle->setPhysicsBody(paddleBody);
	paddleBody->setContactTestBitmask(0x000001);
	paddle->setTag(2);
	this->addChild(paddle);

	// Bollocks
	int ypad = 0;
	for (int j = 0; j < 3; j++) {
		for (int i = 0; i < 11; i++) {
			static int padding = 5;
			auto block = Sprite::create("bollock.png");
			auto blockBody = PhysicsBody::createBox(block->getContentSize());
			blockBody->getShape(0)->setDensity(10.0f);
			blockBody->getShape(0)->setFriction(0.0f);
			blockBody->getShape(0)->setRestitution(1.f);
			blockBody->setDynamic(false);

			int xOffset = padding + block->getContentSize().width / 2 + ((block->getContentSize().width + padding) * i);
			block->setPosition(xOffset, 450+ypad);
			blockBody->setContactTestBitmask(0x000001);
			block->setPhysicsBody(blockBody);
			block->setTag(3);
			this->addChild(block);
		}
		ypad += 35;
	}

	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->setSwallowTouches(true);
	touchListener->onTouchBegan = CC_CALLBACK_2(Breakout::touchBegan, this);
	touchListener->onTouchMoved= CC_CALLBACK_2(Breakout::touchMove, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(Breakout::touchEnd, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	auto dispatcher = Director::getInstance()->getEventDispatcher();
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(Breakout::onContactBegin, this);
	dispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

	auto keyListener = EventListenerKeyboard::create();
	keyListener->onKeyPressed = [](EventKeyboard::KeyCode keyCode, Event* event) {
		Vec2 loc = event->getCurrentTarget()->getPosition();
		switch (keyCode) {
			case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
			case EventKeyboard::KeyCode::KEY_A:
				event->getCurrentTarget()->setPosition(loc.x-30, loc.y); break;

			case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
			case EventKeyboard::KeyCode::KEY_D:
				event->getCurrentTarget()->setPosition(loc.x+30, loc.y); break;

			default: break;
		}
	};
	
	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, paddle);
	
	this->scheduleUpdate();

    return true;
}

bool Breakout::touchBegan(Touch* touch, Event* event) {
	return true;
}

void Breakout::touchEnd(Touch* touch, Event* event) {
	
}

void Breakout::touchMove(Touch* touch, Event* event) {
	Point location = this->convertToWorldSpace(this->convertTouchToNodeSpace(touch));
	paddle->setPositionX(location.x);
}

bool Breakout::onContactBegin(PhysicsContact &contact) {
	auto spriteA = (Sprite*)contact.getShapeA()->getBody()->getNode();
	auto spriteB = (Sprite*)contact.getShapeB()->getBody()->getNode();

	int tagA = spriteA->getTag();
	int tagB = spriteB->getTag();

	if (tagA == 3) { //if brick
		this->removeChild(spriteA, true);
	}
	if (tagB == 3) { //if brick
		this->removeChild(spriteB, true);
	}

	//ball hits ground == gameover
	if ((tagA == 0 || tagB == 0) & (ball->getPositionY() <= paddle->getPositionY())) {
		/*auto sc_gameOver = GameOverScene::create();
		sc_gameOver->getLayer()->getLabel()->setString("GAME OVER");
		Director::getInstance()->replaceScene(gameOverScene); */
		CCLOG("Game Over");
	}

	return true;
}

void Breakout::update(float delta) {
	bool win = true;

	Vector<PhysicsBody*>bodies = sceneWorld->getAllBodies();

	for each (PhysicsBody* body in bodies) {
		//if bricks still exist
		if (body->getNode()->getTag() == 3) {
			win = false;
		}
		if (win == true) {
			/*auto sc_gameOver = GameOverScene::create();
			sc_gameOver->getLayer()->getLabel()->setString("WINNER!!");
			Director::getInstance()->replaceScene(gameOverScene); */
			CCLOG("Win");
		}
	}
	
	/* world->Step(delta, 10, 10);
	for (b2Body *b = world->GetBodyList(); b; b = b->GetNext()) {
		if (b->GetUserData() != NULL) {
			Sprite *spr = (Sprite *)b->GetUserData();
			spr->setPosition(ccp(b->GetPosition().x, b->GetPosition().y));
			spr->setRotation(-1 * CC_RADIANS_TO_DEGREES(b->GetAngle()));

		}
	} */
}

