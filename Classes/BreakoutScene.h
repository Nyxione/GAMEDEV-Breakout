#ifndef __BREAKOUT_SCENE_H__
#define __BREAKOUT_SCENE_H__

#include "cocos2d.h"

class Breakout : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
	cocos2d::Sprite *ball;
	cocos2d::Sprite *paddle;
	cocos2d::Sprite *edge;
	

	virtual bool init();
	void update(float) override;
	void touchMove(cocos2d::Touch *touch, cocos2d::Event *event);
	void touchEnd(cocos2d::Touch *touch, cocos2d::Event *event);
	bool touchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
	bool onContactBegin(cocos2d::PhysicsContact &contact);

	cocos2d::PhysicsWorld *sceneWorld;
	void SetPhysicsWorld(cocos2d::PhysicsWorld *world) {
		sceneWorld = world;
	};
    
    // implement the "static create()" method manually
    CREATE_FUNC(Breakout);
};

#endif // __HELLOWORLD_SCENE_H__
