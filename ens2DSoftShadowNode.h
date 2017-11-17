//
//  ens2DSoftShadowNode.h
//  HelloCpp
//
//  Created by yang chao (wantnon) on 13-12-16.
//
//

#ifndef HelloWorld_ens2DSoftShadowNode_h
#define HelloWorld_ens2DSoftShadowNode_h
#include "cocos2d.h"
using namespace cocos2d;
#include "ensCommon.h"

namespace_ens_begin
//this effect was inspired by:
//http://www.gamedev.net/page/resources/_/technical/graphics-programming-and-theory/dynamic-2d-soft-shadows-r2032

class Cedge{
public:
    CCPoint m_start;
    CCPoint m_end;
    Cedge(){}
    Cedge(const CCPoint&start,const CCPoint&end){
        m_start=start;
        m_end=end;
    }
};
class ClightNode:public Sprite{
public:
    ClightNode();
    static ClightNode* create(const std::string& filename);
    static ClightNode* createWithSpriteFrameName(const std::string& spriteFrameName);
    bool initWithFile(const std::string& filename);
    bool initWithSpriteFrameName(const std::string& spriteFrameName);
    
    float getR()const{return m_r;}
    void setR(float r){m_r = r;};
    
    void draw(Renderer* renderer, const Mat4 &transform, uint32_t flags){
        Sprite::draw(renderer, transform, flags);
        if(m_isDrawDebug){
            
            _customCommand_debug.init(_globalZOrder);
            _customCommand_debug.func = CC_CALLBACK_0(ClightNode::onDrawDebug,this,transform, flags);
            renderer->addCommand(&_customCommand_debug);
        }
    }
    void onDrawDebug(const Mat4 &transform, uint32_t flags){
        const Mat4&m=transform;
            glLineWidth(2);
            ccDrawColor4F(0, 0, 1, 1);
            ccDrawCircle(M4TransV2(m,CCPoint(0,0)), m_r, 360, 30, false, 1, 1);
        ccDrawColor4F(1, 1, 1, 1);
    }
    void setIsDrawDebug(bool value){m_isDrawDebug=value;}
    bool getIsDrawDebug()const{return m_isDrawDebug;}
protected:
    float m_r;
    bool m_isDrawDebug;
    
    cocos2d::CustomCommand _customCommand_debug;
};
class CPointType{
public:
    bool m_isLeftUmbraPoint;
    CCPoint m_leftUmbraT;
    bool m_isRightUmbraPoint;
    CCPoint m_rightUmbraT;
    bool m_isLeftPenumbraPoint;
    CCPoint m_leftPenumbraT;
    bool m_isRightPenumbraPoint;
    CCPoint m_rightPenumbraT;
    CPointType(){
         m_isLeftUmbraPoint=false;
         m_isRightUmbraPoint=false;
         m_isLeftPenumbraPoint=false;
         m_isRightPenumbraPoint=false;
    }
};

enum eSideType{
    eLeftSide=0,
    eRightSide,
    eOn,
};

class C2DSoftShadowSprite:public CCSprite
{
public:
    C2DSoftShadowSprite();
    virtual~C2DSoftShadowSprite();
    static C2DSoftShadowSprite* create(Size size, const std::string& shadowFilename);
    
    void setPolygon(const Cpolygon &polygon);
    void setLight(ClightNode*light);
    void setShadowImg(const std::string& filename);
    
    void setOneDivObjCount(float value){m_oneDivObjCount=value;}
    float getOneDivObjCount()const{return m_oneDivObjCount;}
    void setIsDrawDebug(bool value){m_isDrawDebug=value;}
    bool getIsDrawDebug()const{return m_isDrawDebug;}
    void setIsDrawNonDebug(bool value){m_isDrawNonDebug=value;}
    bool getIsDrawNonDebug()const{return m_isDrawNonDebug;}
    
    void update(float dt);
    bool updateShadow();
    void draw(Renderer* renderer, const Mat4 &transform, uint32_t flags);
    void onDraw(const Mat4 &transform, uint32_t flags);
    void onDrawDebug(const Mat4 &transform, uint32_t flags);
    
    bool initWithFile(Size size, const std::string& shadowFilename);
    
protected:
    CCPoint getLightPosLocal();
    void makeFullWindowRectMesh();
    void updateMesh();
    void submit(GLenum usage);
protected:
    Cpolygon m_polygon;
    vector<CPointType> m_pointTypeList;
    vector<CCPoint> m_leftPenumbraLine;//m_leftPenumbraLine have three elements, [0] is start point, [1] is mid point, [2] is end point
    vector<CCPoint> m_rightPenumbraLine;
    vector<CCPoint> m_leftUmbraLine;
    vector<CCPoint> m_rightUmbraLine;
    int m_leftPenumbraPointID;
    int m_rightPenumbraPointID;
    int m_leftUmbraPointID;
    int m_rightUmbraPointID;
    CCPoint m_intersectPoint;
    ClightNode *m_light;
    float m_shadowLength;
    CCTexture2D*m_finTexture;
    Cmesh*m_mesh;
    CindexVBO*m_indexVBO;
    CGLProgramWithUnifos*m_program;
    float m_oneDivObjCount;
    bool m_isDrawDebug;
    bool m_isDrawNonDebug;
    bool m_isUpdateShadowSucc;
    bool m_isDirty;
    Point m_lightPosLocal;
    Point m_selfPosLocal;
    
    cocos2d::CustomCommand _customCommand;
    cocos2d::CustomCommand _customCommand_debug;
};

class C2DSoftShadowObj:public Node
{
public:
    C2DSoftShadowObj();
    virtual~C2DSoftShadowObj();
    static C2DSoftShadowObj* create(const std::string& filename, const std::string& shadowFilename);
    static C2DSoftShadowObj* createWithSpriteFrameName(const std::string& spriteFrameName, const std::string& shadowFilename);
    bool initWithFile(const std::string& filename, const std::string& shadowFilename);
    bool initWithSpriteFrameName(const std::string& spriteFrameName, const std::string& shadowFilename);
    bool init(Sprite *sprite, const std::string& shadowFilename);
    void setPolygon(const Cpolygon &polygon);
    void setLight(ClightNode*light);
    void setShadowImg(const std::string& filename);
    
    void setOneDivObjCount(float value);
    float getOneDivObjCount()const;
    void setIsDrawDebug(bool value);
    bool getIsDrawDebug()const;
    void setIsDrawNonDebug(bool value);
    bool getIsDrawNonDebug()const;
    Sprite * getDisplaySprite(){return _displaySprite;};
    C2DSoftShadowSprite * getShadowSprite(){ return _shadowSprite;};
private:
    Sprite *_displaySprite;
    C2DSoftShadowSprite *_shadowSprite;
};

class C2DSoftShadowRoot:public Node
{
public:
    C2DSoftShadowRoot();
    virtual~C2DSoftShadowRoot();
    static C2DSoftShadowRoot* createShadowRoot(ClightNode* light, Size size);
    
    void setLight(ClightNode*light);
    void addObj(C2DSoftShadowObj*obj);

    void setIsDrawDebug(bool value){m_isDrawDebug=value;}
    bool getIsDrawDebug()const{return m_isDrawDebug;}
    /*0~1.0*/
    void setShadowDarkness(float value){m_shadowDarkness=value;}
    float getShadowDarkness()const{return m_shadowDarkness;}
    
    CCRenderTexture* getShadowRT(){return m_shadowRT;}
    
    bool init(ClightNode* light, Size size);
    virtual void visit(Renderer* renderer, const Mat4 &parentTransform, uint32_t parentFlags);
protected:
    CCRenderTexture* m_shadowRT;
    CGLProgramWithUnifos*m_program;
    vector<C2DSoftShadowObj*> m_objList;
    ClightNode*m_light;
    bool m_isDrawDebug;
    float m_shadowDarkness;
    
    CCSprite* m_tempSprite;
};
namespace_ens_end
#endif
/*
 method: createShadowRoot of class  C2DSoftShadowRoot
#ifndef TOLUA_DISABLE_tolua_JOEnsEffect_luabinding_C2DSoftShadowRoot_createShadowRoot00
static int tolua_JOEnsEffect_luabinding_C2DSoftShadowRoot_createShadowRoot00(lua_State* tolua_S)
{
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
    if (
        !tolua_isusertable(tolua_S,1,"C2DSoftShadowRoot",0,&tolua_err) ||
        !tolua_isusertype(tolua_S,2,"ClightNode",0,&tolua_err) ||
        tolua_isvaluenil(tolua_S,3,&tolua_err) ||
        !tolua_isnoobj(tolua_S,4,&tolua_err)
        )
        goto tolua_lerror;
    else
#endif
    {
        ClightNode* light = ((ClightNode*)  tolua_tousertype(tolua_S,2,0));
        
        {
            cocos2d::Size arg1;
            luaval_to_size(tolua_S, 3, &arg1, "CCHTMLLabel:initWithString");
            C2DSoftShadowRoot* tolua_ret = (C2DSoftShadowRoot*)  C2DSoftShadowRoot::createShadowRoot(light,arg1);
            tolua_pushusertype(tolua_S,(void*)tolua_ret,"C2DSoftShadowRoot");
        }
    }
    return 1;
#if COCOS2D_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'createShadowRoot'.",&tolua_err);
    return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE
*/
