/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppLive2DManager.hpp"
#include <random>
#include <string>
#include <GLES2/gl2.h>
#include <Rendering/CubismRenderer.hpp>
#include "LAppPal.hpp"
#include "LAppDefine.hpp"
#include "LAppDelegate.hpp"
#include "LAppModel.hpp"
#include "LAppView.hpp"

//SGY add start
#include "pthread.h"
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include "JsonLite.h"
#include "stdio.h"
#include <sys/stat.h>
#include <string>
#include "LAppWavFileHandler.hpp"
#include <algorithm>
#include <vector>
#include <cstdlib>
//#include <SLES/OpenSLES.h>
//#include <SLES/OpenSLES_Android.h>

//SGY add end

#include "JniBridgeC.hpp"

using namespace Csm;
using namespace LAppDefine;
using namespace std;

pthread_t pthread;

bool isPthreadOn = false;
bool pthreadExitFlag = false;
char *passFace = "";
int passMotion = -1;
int passModelId = -1;
char *fileContent;
string lastFace = "";
int lastMostion = -1;
string nowModelName = "100100";

//洗牌变量
int soundId = 10000;
vector<int> soundList;

namespace
{
    LAppLive2DManager *s_instance = NULL;

    void FinishedMotion(ACubismMotion *self)
    {
        LAppPal::PrintLog("Motion Finished: %x", self);
    }
}

void floatSleep(int millisecond)
{
    timeval timeout;
    timeout.tv_sec = millisecond / 1000;
    timeout.tv_usec = millisecond % 1000 * 1000;
    ::select(0, NULL, NULL, NULL, &timeout);
    return;
}

void LAppLive2DManager::ChangePassModelName(string name){
        LAppPal::PrintLog("[SGY2] passModelName: %s",(char* )(passModelName).c_str());
        passModelName = name;
    };

LAppLive2DManager *LAppLive2DManager::GetInstance()
{
    if (s_instance == NULL)
    {
        s_instance = new LAppLive2DManager();
    }

    return s_instance;
}

void LAppLive2DManager::ReleaseInstance()
{
    if (s_instance != NULL)
    {
        delete s_instance;
    }

    s_instance = NULL;
}

LAppLive2DManager::LAppLive2DManager()
    : _viewMatrix(NULL), _sceneIndex(0)
{
    _viewMatrix = new CubismMatrix44();

    ChangeScene(_sceneIndex,"100100");
}

LAppLive2DManager::~LAppLive2DManager()
{
    ReleaseAllModel();
}

void LAppLive2DManager::ReleaseAllModel()
{
    //LAppPal::PrintLog("[SGY]releaseModelSize:%d", _models.GetSize());
    for (csmUint32 i = 0; i < _models.GetSize(); i++)
    {

        delete _models[i];
    }

    _models.Clear();
}

LAppModel *LAppLive2DManager::GetModel(csmUint32 no) const
{
    if (no < _models.GetSize())
    {
        return _models[no];
    }

    return NULL;
}

void LAppLive2DManager::OnDrag(csmFloat32 x, csmFloat32 y) const
{
    for (csmUint32 i = 0; i < _models.GetSize(); i++)
    {
        LAppModel *model = GetModel(i);

        model->SetDragging(x, y);
    }
}



void *normalCallBack(void *data)
{
    isPthreadOn = true;

    
    JsonValue json = JsonParser::Parse(fileContent);
    JsonValue story = json.Get("story");

    //洗牌算法
    if((soundId >= story.GetLength()) || (soundList.size() != story.GetLength())){
        soundId = 0;
        vector<int> temp;
        soundList.swap(temp);
        for(int i = 0; i < story.GetLength(); i++) soundList.push_back(i);
        std::shuffle(soundList.begin(),soundList.end(), std::mt19937(std::random_device()()));
    }

    JString motionName = story.GetName(soundList[soundId]);
    soundId++;
    
    JsonValue group_1 = story.Get(motionName);
    for (UINT i = 0, l = group_1.GetLength(); i < l; ++i)
    {
        float interval = (group_1.Get(i)).GetFloat("autoTurnFirst");
        if ((group_1.Get(i)).Get("chara").Get(0).Contain("face"))
        {
            char *temp = (group_1.Get(i)).Get("chara").Get(0).GetString("face").GetString();
            if((string(temp)).size() == 19){
                temp = (char *)(string(temp)).insert(14, "3").c_str();
            }
            passFace = temp;
            //_models[model_id]->SetExpression(face.c_str());
        }
        if ((group_1.Get(i)).Get("chara").Get(0).Contain("motion"))
        {
            passMotion = (group_1.Get(i)).Get("chara").Get(0).GetInteger("motion");
            passMotion += 1000;

            //_models[model_id]->StartMotion(MotionGroupTapBody, -1, PriorityForce, FinishedMotion, ("mtn/motion_" + (std::to_string(motion).substr(1)) + ".motion3.json").c_str());
        }
        if ((group_1.Get(i)).Get("chara").Get(0).Contain("voice"))
        {
            string temp = (group_1.Get(i)).Get("chara").Get(0).GetString("voice").GetString();
            //temp = "sounds/" + temp + "_hca [1].wav";
            temp = "sounds/" + temp + "_hca [1].mp3";
            passVoice = temp;

            LAppPal::PrintLog("[SGY]sound file path:%s", passVoice.c_str());
            //_models[model_id]->SetExpression(face.c_str());
        }
        floatSleep(round(interval * 1000));
        if (pthreadExitFlag)
        {
            isPthreadOn = false;
            pthreadExitFlag = false;
            passVoice = "-1";
            pthread_exit(&pthread);
        }
        //LAppPal::PrintLog("[SGY]Thread is NULL? %s", (pthread == NULL) ? "YES" : "NO");
    }

    LAppPal::PrintLog("[SGY]Thread release");
    isPthreadOn = false;
    pthread_exit(&pthread);
}

char *readFileIntoString(char *filename)
{
    //struct stat st;
    //int result = stat((string(ResourcesPath) + string(filename)).c_str(), &st);
    //long long int size = st.st_size;
    unsigned int outsize;
    char *buf = JniBridgeC::LoadFileAsBytesFromJava(filename, &outsize, false);
    //LAppPal::PrintLog("[SGY]fileSize2:%u", outsize);
    return buf;
}

char* getGeneralFileName(char* name){
    char* data = readFileIntoString("dic.json");
    JsonValue json = JsonParser::Parse(data);
    return json.GetString(JString(name));
}

void LAppLive2DManager::ReadJsonFromFile(char *filename, int model_id)
{
    //在此处识别是否需要替换filename
    string temp2 = string(getGeneralFileName(filename));
    if(temp2 != "000000"){
        string temp = (string("general/") + temp2 + string(".json"));
        fileContent = readFileIntoString((char*) temp.c_str());
        pthread_create(&pthread, NULL, normalCallBack, NULL);
        passModelId = model_id;
    }
    //LAppPal::PrintLog("[SGY]group_1 is NULL:%s", (group_1.Get(1).Get("autoTurnFirst") == NULL ? "YES" : "NO"));
    //LAppPal::PrintLog("[SGY]group_1 float:%f", (group_1.Get(0)).GetFloat("autoTurnFirst"));
    //LAppPal::PrintLog("[SGY]group_1 face:%s", (group_1.Get(0)).Get("chara").Get(0).GetString("face").GetString());
}

void LAppLive2DManager::OnTap(csmFloat32 x, csmFloat32 y,float pointX, float pointY)
{   
    //
    if (DebugLogEnable)
    {
        LAppPal::PrintLog("[APP]tap point: {x:%.2f y:%.2f}", x, y);
    }

    for (csmUint32 i = 0; i < _models.GetSize(); i++)
    {
        //
        if (_models[i]->HitTest(HitAreaNameHead, x, y,pointX,pointY))
        {
            //LAppPal::PrintLog("[SGY]beginChangeScene");
            if (DebugLogEnable)
            {
                LAppPal::PrintLog("[APP]hit area: [%s]", HitAreaNameHead);
            }
            if (isPthreadOn)
            {
                pthreadExitFlag = true;
            }
            else
            {
                
                ReadJsonFromFile((char* )nowModelName.c_str(), i);
            }

            //_models[i]->SetRandomExpression();
            //_models[i]->StartRandomMotion(MotionGroupTapBody, PriorityNormal, FinishedMotion);
            //_models[i]->StartMotion(MotionGroupTapBody, -1, PriorityForce, FinishedMotion, "mtn/motion_400.motion3.json");

            //floatSleep(1500);
            //记得改优先度
            //_models[i]->StartMotion(MotionGroupTapBody, -1, PriorityForce, FinishedMotion, "mtn/motion_300.motion3.json");
        }
        else if (_models[i]->HitTest(HitAreaNameBody, x, y,pointX,pointY))
        {
            if (DebugLogEnable)
            {
                LAppPal::PrintLog("[APP]hit area: [%s]", HitAreaNameBody);
            }
            //_models[i]->StartRandomMotion(MotionGroupTapBody, PriorityNormal, FinishedMotion);
            if (isPthreadOn)
            {
                pthreadExitFlag = true;
            }
            else
            {
                
                ReadJsonFromFile((char* )nowModelName.c_str(), i);
            }
        }
    }
}

void LAppLive2DManager::OnUpdate() 
{
    int width = LAppDelegate::GetInstance()->GetWindowWidth();
    int height = LAppDelegate::GetInstance()->GetWindowHeight();

    csmUint32 modelCount = _models.GetSize();
    for (csmUint32 i = 0; i < modelCount; ++i)
    {
        CubismMatrix44 projection;
        LAppModel *model = GetModel(i);
        if (model->GetModel()->GetCanvasWidth() > 1.0f && width < height)
        {
            // 横に長いモデルを縦長ウィンドウに表示する際モデルの横サイズでscaleを算出する
            model->GetModelMatrix()->SetWidth(2.0f);
            projection.Scale(1.0f, static_cast<float>(width) / static_cast<float>(height));
        }
        else
        {
            projection.Scale(static_cast<float>(height) / static_cast<float>(width), 1.0f);
        }

        // 必要があればここで乗算
        if (_viewMatrix != NULL)
        {
            projection.MultiplyByMatrix(_viewMatrix);
        }

        // モデル1体描画前コール
        LAppDelegate::GetInstance()->GetView()->PreModelDraw(*model);

        model->Update();
        model->Draw(projection); ///< 参照渡しなのでprojectionは変質する

        // モデル1体描画後コール
        LAppDelegate::GetInstance()->GetView()->PostModelDraw(*model);
    }

    string tempString = passFace;
    if (tempString != lastFace)
    {
        _models[passModelId]->SetExpression(passFace);
        lastFace = passFace;
    }

    if (lastMostion != passMotion)
    {
        //LAppPal::PrintLog("[SGY]readMotion: %d", (passMotion));
        _models[passModelId]->StartMotion(MotionGroupTapBody, -1, PriorityForce, FinishedMotion, ("mtn/motion_" + (std::to_string(passMotion).substr(1)) + ".motion3.json").c_str());
        lastMostion = passMotion;
    }

    //LAppPal::PrintLog("[SGY2]nowModelName: %s, passModelName: %s", (char* )(nowModelName).c_str(),(char* )(passModelName).c_str());
    if(passModelName != nowModelName){
        Csm::csmInt32 temp = 0;
        ChangeScene(temp,passModelName);
        nowModelName = passModelName;
        LAppPal::PrintLog("[SGY]nowModelName: %s", (char* )(nowModelName).c_str());
    }
}

void LAppLive2DManager::NextScene()
{
    csmInt32 no = (_sceneIndex + 1) % ModelDirSize;
    ChangeScene(no);
}

void LAppLive2DManager::ChangeScene(Csm::csmInt32 index, string modelName)
{
    _sceneIndex = index;
    if (DebugLogEnable)
    {
        LAppPal::PrintLog("[APP]model index: %d", _sceneIndex);
    }

    // ModelDir[]に保持したディレクトリ名から
    // model3.jsonのパスを決定する.
    // ディレクトリ名とmodel3.jsonの名前を一致させておくこと.
    if (isPthreadOn)
    {
        pthreadExitFlag = true;
    }
    std::string model;
    if(modelName == ""){
        model = ModelDir[index];
    }else{
        model = modelName;
        nowModelName = modelName;
    }
    LAppPal::PrintLog("[SGY]model name: %s", model.c_str());
    std::string modelPath = ResourcesPath + model + "/";
    //std::string modelJsonName = ModelDir[index];
    std::string modelJsonName = "model";
    modelJsonName += ".model3.json";
    ReleaseAllModel();
    _models.PushBack(new LAppModel());
    _models[0]->LoadAssets(modelPath.c_str(), modelJsonName.c_str());

    /*
     * モデル半透明表示を行うサンプルを提示する。
     * ここでUSE_RENDER_TARGET、USE_MODEL_RENDER_TARGETが定義されている場合
     * 別のレンダリングターゲットにモデルを描画し、描画結果をテクスチャとして別のスプライトに張り付ける。
     */
    {
#if defined(USE_RENDER_TARGET)
        // LAppViewの持つターゲットに描画を行う場合、こちらを選択
        LAppView::SelectTarget useRenderTarget = LAppView::SelectTarget_ViewFrameBuffer;
#elif defined(USE_MODEL_RENDER_TARGET)
        // 各LAppModelの持つターゲットに描画を行う場合、こちらを選択
        LAppView::SelectTarget useRenderTarget = LAppView::SelectTarget_ModelFrameBuffer;
#else
        // デフォルトのメインフレームバッファへレンダリングする(通常)
        LAppView::SelectTarget useRenderTarget = LAppView::SelectTarget_None;
#endif

#if defined(USE_RENDER_TARGET) || defined(USE_MODEL_RENDER_TARGET)
        // モデル個別にαを付けるサンプルとして、もう1体モデルを作成し、少し位置をずらす
        _models.PushBack(new LAppModel());
        _models[1]->LoadAssets(modelPath.c_str(), modelJsonName.c_str());
        _models[1]->GetModelMatrix()->TranslateX(0.2f);
#endif

        LAppDelegate::GetInstance()->GetView()->SwitchRenderingTarget(useRenderTarget);

        // 別レンダリング先を選択した際の背景クリア色
        float clearColor[3] = {1.0f, 1.0f, 1.0f};
        LAppDelegate::GetInstance()->GetView()->SetRenderTargetClearColor(clearColor[0], clearColor[1], clearColor[2]);
    }
}

csmUint32 LAppLive2DManager::GetModelNum() const
{
    return _models.GetSize();
}

void LAppLive2DManager::SetViewMatrix(CubismMatrix44 *m)
{
    for (int i = 0; i < 16; i++)
    {
        _viewMatrix->GetArray()[i] = m->GetArray()[i];
    }
}

