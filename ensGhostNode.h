

#ifndef HelloWorld_ensGhostNode_h
#define HelloWorld_ensGhostNode_h
//???????????????????????????????????????
#include "cocos2d.h"
using namespace cocos2d;
#include "ensCommon.h"
namespace_ens_begin
namespace ghostlike
{
    const float dft_filterR=16;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32||CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    //PC and Mac is powerful :D
	const float dft_filterSrcScale=1.0;
    const float dft_downSample=1.0;
#else
    //on mobile platform we make a discount
    const float dft_filterSrcScale=0.5;
    const float dft_downSample=0.4;
#endif
}

class CghostSprite : public Node
{
public:
    CghostSprite();
    virtual ~CghostSprite();
    static CghostSprite* create(const string&texFileName,float filterR=ghostlike::dft_filterR,float fliterSrcScale=ghostlike::dft_filterSrcScale);
    static CghostSprite* createWithSpriteFrameName(const string&texFileName,float filterR=ghostlike::dft_filterR,float fliterSrcScale=ghostlike::dft_filterSrcScale);
    static CghostSprite* createWithSprite(CCSprite* sprite,float filterR=ghostlike::dft_filterR,float fliterSrcScale=ghostlike::dft_filterSrcScale);
    
	bool init(const string&texFileName,float filterR,float fliterSrcScale);
	bool initWithSpriteFrameName(const string&spriteFrameName,float filterR,float fliterSrcScale);
	bool initWithSprite(CCSprite* sprite,float filterR,float fliterSrcScale);
    
    virtual void visit(Renderer* renderer, const Mat4 &parentTransform, uint32_t parentFlags);
    void setDownSample(float value){m_downSample=value;}
    float getDownSample()const{return m_downSample;}
    CCSprite*getRefSprite(){return m_refSprite;}
    
    void onBiggerRTPassUnifoAndBindTex(const Mat4 &transform, uint32_t flags);
    void onFilterXRTPassUnifoAndBindTex(const Mat4 &transform, uint32_t flags);
public:
    float m_filterR;
    CGLProgramWithUnifos*m_program_filterX;
    CGLProgramWithUnifos*m_program_filterY;
    CCRenderTexture*m_biggerRT;
    CCRenderTexture*m_filterXRT;
    CCRenderTexture*m_filterYRT;
    CCSprite*m_refSprite;
    float m_fliterSrcScale;//let filter source texture smaller, can make filter op faster
    float m_downSample;
    
    CustomCommand _biggerRTpassUnifoAndBindTexCommand;
    CustomCommand _filterXRTpassUnifoAndBindTexCommand;
    
    CCSprite* m_finalSprite;
};
namespace_ens_end
#endif
/*
#ifndef TOLUA_DISABLE_tolua_JOEnsEffect_luabinding_CghostSprite_getRefSprite00
static int tolua_JOEnsEffect_luabinding_CghostSprite_getRefSprite00(lua_State* tolua_S)
{
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
    if (
        !tolua_isusertype(tolua_S,1,"CghostSprite",0,&tolua_err) ||
        !tolua_isnoobj(tolua_S,2,&tolua_err)
        )
        goto tolua_lerror;
    else
#endif
    {
        CghostSprite* self = (CghostSprite*)  tolua_tousertype(tolua_S,1,0);
#if COCOS2D_DEBUG >= 1
        if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getRefSprite'", NULL);
#endif
        {
            const cocos2d::Sprite* ret = self->getRefSprite();
            object_to_luaval<cocos2d::Sprite>(tolua_S, "cc.Sprite",(cocos2d::Sprite*)ret);
        }
    }
    return 1;
#if COCOS2D_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'getRefSprite'.",&tolua_err);
    return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE
 */