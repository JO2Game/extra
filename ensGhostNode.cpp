
#include "ensGhostNode.h"
namespace_ens_begin

CghostSprite::CghostSprite():m_program_filterX(nullptr),m_program_filterY(nullptr),m_biggerRT(nullptr),m_filterXRT(nullptr),m_filterYRT(nullptr) ,
m_refSprite(nullptr), m_finalSprite(nullptr)
{
    m_filterR=ghostlike::dft_filterR;
    m_fliterSrcScale=ghostlike::dft_filterSrcScale;
    m_downSample=ghostlike::dft_downSample;
}
CghostSprite::~CghostSprite(){
    CC_SAFE_RELEASE_NULL(m_program_filterX);
    CC_SAFE_RELEASE_NULL(m_program_filterY);
};

CghostSprite* CghostSprite::create(const string&texFileName,float filterR,float fliterSrcScale)
{
    CghostSprite *ghostSprite = new (std::nothrow) CghostSprite();
    if (ghostSprite && ghostSprite->init(texFileName, filterR, fliterSrcScale))
    {
        ghostSprite->autorelease();
        return ghostSprite;
    }
    CC_SAFE_DELETE(ghostSprite);
    return nullptr;
}
CghostSprite* CghostSprite::createWithSpriteFrameName(const string&texFileName,float filterR,float fliterSrcScale)
{
    CghostSprite *ghostSprite = new (std::nothrow) CghostSprite();
    if (ghostSprite && ghostSprite->initWithSpriteFrameName(texFileName, filterR, fliterSrcScale))
    {
        ghostSprite->autorelease();
        return ghostSprite;
    }
    CC_SAFE_DELETE(ghostSprite);
    return nullptr;
}
CghostSprite* CghostSprite::createWithSprite(CCSprite* sprite,float filterR,float fliterSrcScale)
{
    CghostSprite *ghostSprite = new (std::nothrow) CghostSprite();
    if (ghostSprite && ghostSprite->initWithSprite(sprite, filterR, fliterSrcScale))
    {
        ghostSprite->autorelease();
        return ghostSprite;
    }
    CC_SAFE_DELETE(ghostSprite);
    return nullptr;
}

bool CghostSprite::init(const string&texFileName,float filterR,float fliterSrcScale)
{
    Sprite *sprite = Sprite::create(texFileName);
    return initWithSprite(sprite, filterR, fliterSrcScale);
}
bool CghostSprite::initWithSpriteFrameName(const string&spriteFrameName,float filterR,float fliterSrcScale)
{
    Sprite *sprite = Sprite::createWithSpriteFrameName(spriteFrameName);
    return initWithSprite(sprite, filterR, fliterSrcScale);
}
bool CghostSprite::initWithSprite(CCSprite* sprite,float filterR,float fliterSrcScale)
{
    Node::init();
    
    //sprite->setPosition(Point(sprite->getContentSize().width*0.5, sprite->getContentSize().height*0.5));
    m_finalSprite=CCSprite::create();
    this->addChild(m_finalSprite);
    m_finalSprite->setScaleY(-1);
    ccBlendFunc blendFunc={GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA};
    m_finalSprite->setBlendFunc(blendFunc);

    m_filterR=filterR;
    //----m_originTexSprite
    m_refSprite=sprite;
    this->addChild(m_refSprite);
    //m_refSprite->setPosition(Point(sprite->getContentSize().width*0.5, sprite->getContentSize().height*0.5));
    //----calculate bigger size
    CCSize biggerSize;
    {
        CCSize contentSize=m_refSprite->getContentSize();
        float margin=m_filterR*3;//let the margin wider, so we mult 3
        biggerSize=CCSize(contentSize.width+margin*2, contentSize.height+margin*2);
    }
    setContentSize(biggerSize);
    //----m_biggerRT
    m_biggerRT=CCRenderTexture::create(biggerSize.width,biggerSize.height);
    addChild(m_biggerRT);
    //m_biggerRT->setPosition(Point(sprite->getContentSize().width*0.5, sprite->getContentSize().height*0.5));
    
    //----m_filterXRT
    //on mobile, we can let m_filterSrcScale smaller than 1.0, namely let m_filterXRT smaller to save render time.
    m_filterXRT=CCRenderTexture::create(biggerSize.width*m_fliterSrcScale,biggerSize.height*m_fliterSrcScale);
    addChild(m_filterXRT);
    //m_filterXRT->setPosition(Point(sprite->getContentSize().width*0.5, sprite->getContentSize().height*0.5));
    
    //----m_filterYRT
    m_filterYRT=CCRenderTexture::create(biggerSize.width,biggerSize.height);
    addChild(m_filterYRT);
    //m_filterYRT->setPosition(Point(sprite->getContentSize().width*0.5, sprite->getContentSize().height*0.5));
    
    //----m_program_filterX
    {
		GLchar * fragSource = (GLchar*) CCString::createWithContentsOfFile(CCFileUtils::sharedFileUtils()->fullPathForFilename("shaders/ghostlike_filterX.fsh").c_str())->getCString();
		CGLProgramWithUnifos* program = new CGLProgramWithUnifos();
        program->autorelease();
		program->initWithVertexShaderByteArray(ccPositionTextureColor_noMVP_vert, fragSource);
        //            --below code is no longer needed, because bindPredefinedVertexAttribs() is called in link() in 3.x
        //            --bind attribute
        //            --program->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
        //            --program->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
        //            --program->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
        //link  (must after bindAttribute)
		program->link();
        //get cocos2d-x build-in uniforms
		program->updateUniforms();
        //get my own uniforms
        program->attachUniform("u_filterR");
        program->attachUniform("u_ds");
        program->attachUniform("u_s_step");
        //set program
        m_program_filterX=program;
        m_program_filterX->retain();
        //check gl error
		CHECK_GL_ERROR_DEBUG();
	}
    //----m_program_filterY
    {
		GLchar * fragSource = (GLchar*) CCString::createWithContentsOfFile(CCFileUtils::sharedFileUtils()->fullPathForFilename("shaders/ghostlike_filterYAndMerge.fsh").c_str())->getCString();
		CGLProgramWithUnifos* program = new CGLProgramWithUnifos();
        program->autorelease();
		program->initWithVertexShaderByteArray(ccPositionTextureColor_noMVP_vert, fragSource);
        //            --below code is no longer needed, because bindPredefinedVertexAttribs() is called in link() in 3.x
        //            --bind attribute
        //            --program->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
        //            --program->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
        //            --program->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
        //link  (must after bindAttribute)
		program->link();
        //get cocos2d-x build-in uniforms
		program->updateUniforms();
        //get my own uniforms
        program->attachUniform("u_filterR");
        program->attachUniform("u_dt");
        program->attachUniform("u_t_step");
        program->attachUniform("u_color_r");
        program->attachUniform("u_color_g");
        program->attachUniform("u_color_b");
      //  program->attachUniform("u_refTex");
        //set program
        m_program_filterY=program;
        m_program_filterY->retain();
        //check gl error
		CHECK_GL_ERROR_DEBUG();
	}
    //check gl error
	CHECK_GL_ERROR_DEBUG();

	//antiAlias
	this->m_filterYRT->getSprite()->getTexture()->setAntiAliasTexParameters();//only apply antiAlias to m_filterYRT is ok
    return true;
}

void CghostSprite::visit(Renderer* renderer, const Mat4 &parentTransform, uint32_t parentFlags)
{
    // quick return if not visible. children won't be drawn.
    if (!_visible)
    {
        return;
    }
    uint32_t flags = processParentFlags(parentTransform, parentFlags);
    
    Director* director = Director::getInstance();
    director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _modelViewTransform);
 
    //----draw m_originTexSprite to m_biggerRT (use m_program_renderAlpha)
    {
        m_refSprite->setPosition(ccp(m_biggerRT->getSprite()->getContentSize().width/2, m_biggerRT->getSprite()->getContentSize().height/2));
        m_biggerRT->beginWithClear(0.0,0.0,0.0,0.0);
        {
            m_refSprite->visit();
        }
        m_biggerRT->end();
    }
   
    //----draw m_biggerRT to m_filterXRT (use m_program_fliterX)
    {
        //----m_biggerRT change shader
        m_biggerRT->getSprite()->setShaderProgram(m_program_filterX);
        //passUnifo and bindTex
        _biggerRTpassUnifoAndBindTexCommand.init(_globalZOrder);
        _biggerRTpassUnifoAndBindTexCommand.func = CC_CALLBACK_0(CghostSprite::onBiggerRTPassUnifoAndBindTex, this,_modelViewTransform,flags);
        Director::getInstance()->getRenderer()->addCommand(&_biggerRTpassUnifoAndBindTexCommand);

        //----draw m_biggerRT to m_filterXRT
        
        m_biggerRT->getSprite()->setScaleX(m_fliterSrcScale);
        m_biggerRT->getSprite()->setScaleY(m_fliterSrcScale);
        
        CCPoint offset=ccp(m_filterXRT->getSprite()->getContentSize().width/2,m_filterXRT->getSprite()->getContentSize().height/2);
        m_biggerRT->getSprite()->setPosition(offset);
        m_filterXRT->beginWithClear(0.0,0.0,0.0,0.0);
        {
            m_biggerRT->getSprite()->visit();
        }
        m_filterXRT->end();
    }

    //----draw m_filterXRT to m_filterYRT (use m_program_fliterY)
    {
        //----m_filterXRT change shader
        m_filterXRT->getSprite()->setShaderProgram(m_program_filterY);
        //passUnifo and bindTex
        _filterXRTpassUnifoAndBindTexCommand.init(_globalZOrder);
        _filterXRTpassUnifoAndBindTexCommand.func = CC_CALLBACK_0(CghostSprite::onFilterXRTPassUnifoAndBindTex, this,_modelViewTransform,flags);
        Director::getInstance()->getRenderer()->addCommand(&_filterXRTpassUnifoAndBindTexCommand);
        //----draw m_filterXRT to m_filterYRT
        m_filterXRT->getSprite()->setScaleX(1.0/m_fliterSrcScale);
        m_filterXRT->getSprite()->setScaleY(1.0/m_fliterSrcScale);
        m_filterXRT->getSprite()->setPosition(ccp(m_filterYRT->getSprite()->getContentSize().width/2,m_filterYRT->getSprite()->getContentSize().height/2));
        
        m_filterYRT->beginWithClear(0.0,0.0,0.0,0.0);
        {
            m_filterXRT->getSprite()->visit();
        }
        m_filterYRT->end();
    }
    
    //set the result tex to this sprite
    {
        CCSprite*sp=m_filterYRT->getSprite();
        m_finalSprite->setTexture(sp->getTexture());
        m_finalSprite->setTextureRect(sp->getTextureRect());
        //in cocos2dx 2.x, if we reset texture, the blendfunc will be lost, so we need to reset blendfunc
        //but now, maybe it have been fixed, investigate it later.
        ccBlendFunc blendFunc={GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA};
        m_finalSprite->setBlendFunc(blendFunc);
        m_finalSprite->setPosition(m_refSprite->getPosition());
    }

    //show m_finalSprite
    m_finalSprite->visit();
    
    director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void CghostSprite::onBiggerRTPassUnifoAndBindTex(const Mat4 &transform, uint32_t flags){
    CGLProgramWithUnifos*program=(CGLProgramWithUnifos*)m_biggerRT->getSprite()->getShaderProgram();
    program->use();
    program->setUniformsForBuiltins();
    //prepare uniform values
    float ds_tmp=1.0/m_biggerRT->getSprite()->getContentSize().width;
    //pass values for my own uniforms
    program->passUnifoValue1f("u_filterR", m_filterR*m_fliterSrcScale);
    program->passUnifoValue1f("u_ds", ds_tmp);
    program->passUnifoValue1f("u_s_step", ds_tmp/m_downSample);
}
void CghostSprite::onFilterXRTPassUnifoAndBindTex(const Mat4 &transform, uint32_t flags){
    
    CGLProgramWithUnifos*program=(CGLProgramWithUnifos*)m_filterXRT->getSprite()->getShaderProgram();
    program->use();
    program->setUniformsForBuiltins();
    //prepare uniform values
    float dt_tmp=1.0/m_filterXRT->getSprite()->getContentSize().height;
    //pass values for my own uniforms
    program->passUnifoValue1f("u_filterR", m_filterR*m_fliterSrcScale);
    program->passUnifoValue1f("u_dt", dt_tmp);
    program->passUnifoValue1f("u_t_step", dt_tmp/m_downSample);
    Color3B color3b = getColor();
    program->passUnifoValue1f("u_color_r", color3b.r/255.0);
    program->passUnifoValue1f("u_color_g", color3b.g/255.0);
    program->passUnifoValue1f("u_color_b", color3b.b/255.0);
    
    program->attachUniform("");
    program->attachUniform("");
    program->attachUniform("");
    //pass texture attach point id to sampler uniform
    //  program->passUnifoValue1i("u_refTex", 1);
    //attach texture to texture attach point
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_biggerRT->getSprite()->getTexture()->getName());
    glActiveTexture(GL_TEXTURE0);//back to GL_TEXTURE0

}
namespace_ens_end
