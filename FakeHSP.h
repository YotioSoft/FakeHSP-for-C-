/*----------------------------------------------------------------------*
 *                                                                      *
 *                             FakeHSP.h                                *
 *                             Ver.0.1.0                                *
 *                                                                      *
 *                     (C) YotioSoft 2018 - 2019                        *
 *                                                                      *
 -----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
 *
 *                              利用方法
 *
 *-----------------------------------------------------------------------
 *
 *  1. 環境の導入
 *  　　まず、C++とSDL2の開発環境を導入してください。
 *      動作確認環境は以下のとおりです。
 *  　　　・C++ 14
 *  　　　・SDL2 2.0.9
 *  　　　・SDL2 mixer 2.0.4
 *  　　　・SDL2 image 2.0.4
 * 　　　 ・SDL2 ttf 2.0.14
 * 　　　 ・SDL2 net 2.0.1
 * 　　　 ・CLion 2018.3（統合開発環境）
 * 　　　 ・Windows 10, macOS 10.13 High Sierra および 10.14 Mojave （いずれも64bit環境）
 *
 *  2. 利用する上での注意点
 *  　　macOSの10.14 Mojave以降で使用する際は、プログラムのmain関数冒頭に
 *  　　　SDL_PumpEvents();
 *  　　を記述してください。記述しないと何も表示されません。
 *  　　他のOSでは記述する必要はありませんが、記述しても正常に作動します。
 *  　　インターネット上に公開する際は、記述することをおすすめします。
 *
 *  3. ライセンス
 *     FakeHSPはMITライセンスです。
 *     改造、再配布OKですので、自作ソフトに組み込んで公開できます。
 *     また、FakeHSPを自分で改良・改造してもよいです。
 *     ただし、再配布物で問題が起きても、作者は責任を負いません。
 *
 *     Released under the MIT license
 *       Permission is hereby granted, free of charge, to any person obtaining a
 *       copy of this software and associated documentation files (the
 *       "Software"), to deal in the Software without restriction, including
 *       without limitation the rights to use, copy, modify, merge, publish,
 *       distribute, sublicense, and/or sell copies of the Software, and to
 *       permit persons to whom the Software is furnished to do so, subject to
 *       the following conditions:
 *
 *       The above copyright notice and this permission notice shall be
 *       included in all copies or substantial portions of the Software.
 *
 *       THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *       EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *       MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *       NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *       LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *       OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *       WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *  3. 各関数についての詳細
 *      こちらをご覧ください：
 *       http://yotiosoft.com/fakehsp-for-cpp/#i-4
 *
 */

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_net.h>
#include <SDL_mixer.h>
#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>

using namespace std;

#ifndef SDL_FakeHSP_H
#define SDL_FakeHSP_H

#endif //SDL_FakeHSP_H

int number_of_windows = 0;

/*------------------------------定数--------------------------------*/
//stop, wait, waitLoop関連
#define HSP_WINDOW_CLOSE                    (-1)    //ウィンドウを閉じるボタンが押された
#define HSP_WINODW_CHANGED_SIZE             (-2)    //ウィンドウの大きさが変更された
#define HSP_NOTHING                         (-3)    //特に何も起きていない

//処理の成功・失敗関連
#define HSP_FUNC_SUCCESED                  0       //処理に成功した
#define HSP_FUNC_FAILED                    (-1)    //処理に失敗した

//マウス関連
#define HSP_CURSOR_ON_RANGE_OF_BUTTON      1       //ボタンの範囲内（ボタンの上）にカーソルがある
#define HSP_CURSOR_NOT_ON_RANGE_OF_BUTTON  0       //ボタンの範囲内（ボタンの上）にカーソルがない

/*---------------------------HSPクラスの関数-------------------------*/
namespace hsp {
    //stringからchar*への変換
    char* convStringToCharP(string str) {
        char* cstr = new char[str.size() + 1];
        return char_traits<char>::copy(cstr, str.c_str(), str.size()+1);
    }

    //文字列に検索文字列が含まれるか判定
    template<class T> bool contain(const std::string& s, const T& v) {
        return s.find(v) != std::string::npos;
    }

    //ファイルの有無（SDL不使用, fstream, iostream使用）
    int exist(string filename) {
        ifstream ifs(filename, ifstream::ate);
        if (ifs.is_open() == false) {
            return HSP_FUNC_FAILED;
        }
        else {
            size_t filesize = static_cast<size_t>(ifs.tellg());
            return int(filesize);
        }
    }

    //乱数の不定処理
    void randomize() {
        //srand((unsigned int)time(NULL));
    }

    //文字列を区切る
    vector<string> split(string& input, string delimiter) {
        auto separator_length = delimiter.length();

        auto list = vector<string>();

        if (separator_length == 0) {
            list.push_back(input);
        } else {
            auto offset = string::size_type(0);
            while (1) {
                auto pos = input.find(delimiter, offset);
                if (pos == string::npos) {
                    list.push_back(input.substr(offset));
                    break;
                }
                list.push_back(input.substr(offset, pos - offset));
                offset = pos + separator_length;
            }
        }

        return list;
    }
    string split(string& input, string delimiter, int return_chapter_num) {
        vector<string> str = split(input, delimiter);

        if (return_chapter_num < 0) {
            return str[str.size()+return_chapter_num-1];
        }
        return str[return_chapter_num];
    }

    //待機
    void wait(int _t) {
        SDL_Delay(_t*10);
    }

    /*---------------------------mixerクラス-------------------------*/
    class mixer {
    public:
        typedef struct {
            int bufferNumber;
            string bufferMusicName;
            int mode;
            string type;
        } bufferMusicStruct;
    private:
        vector<Mix_Music*> musicBuffer;
        vector<Mix_Chunk*> chunkBuffer;
        int musicBufferingLastNumber = -1;
        vector<bufferMusicStruct> bufferMusicsData;
    public:
        mixer() {
            mmSet();
            int mixFlags = MIX_INIT_MID/*|MIX_INIT_MP3|MIX_INIT_FLAC|MIX_INIT_MOD|MIX_INIT_OGG*/;
            int mi = Mix_Init(mixFlags);
            if (mi < 0) {
                cout << "[HSP Error : I06 in init] SDL_mixerの初期化に失敗しました" << endl << " ->Mix_Error: " << Mix_GetError() << endl;
            }
            if (mi != mixFlags) {
                cout << "[FakeHSP Error : I07 in init] SDL_mixerのローダーの初期化に失敗しました" << endl << " ->Mix_Error: " << Mix_GetError() << endl;
            }
        }

        void closeMixer() {
            for (int n=0; n<musicBufferingLastNumber; n++) {
                Mix_FreeMusic(musicBuffer[n]);
            }
            for (int n=0; n<musicBufferingLastNumber; n++) {
                Mix_FreeChunk(chunkBuffer[n]);
            }
        }

        //音楽ファイル読み込み
        bool mmLoad(string filepath, int mID, int mode) {
            if (hsp::exist(filepath) == HSP_FUNC_FAILED) {cout << "[FakeHSP Error : F02 in mmLoad] 音楽ファイルが存在しません：" << filepath << endl;}

            bufferMusicStruct bms_tmp;
            if (mID > musicBufferingLastNumber) {
                for (int n = musicBufferingLastNumber; n < mID; n++) {
                    chunkBuffer.push_back(NULL);
                    musicBuffer.push_back(NULL);

                    bms_tmp.bufferNumber = -1;
                    bms_tmp.bufferMusicName = "";
                    bms_tmp.mode = -1;
                    bufferMusicsData.push_back(bms_tmp);
                }
                musicBufferingLastNumber = mID;
            }

            if (hsp::contain<string>(filepath, ".wav") == true) {
                chunkBuffer[mID] = Mix_LoadWAV(hsp::convStringToCharP(filepath));
                bms_tmp.type = "chunk";

                if (!chunkBuffer[mID]) {
                    cout << "[FakeHSP Error : M01 in mmLoad] 音楽ファイルの読み込みに失敗しました : " << filepath << endl << " -> Mix_Error : " << Mix_GetError() << endl;
                    return bool(HSP_FUNC_FAILED);
                }
            }
            else {
                musicBuffer[mID] = Mix_LoadMUS(hsp::convStringToCharP(filepath));
                bms_tmp.type = "music";

                if (!musicBuffer[mID]) {
                    cout << "[FakeHSP Error : M01 in mmLoad] 音楽ファイルの読み込みに失敗しました : " << filepath << endl << " -> Mix_Error : " << Mix_GetError() << endl;
                    return bool(HSP_FUNC_FAILED);
                }
            }

            bms_tmp.bufferNumber = mID;
            bms_tmp.bufferMusicName = filepath;
            bms_tmp.mode = mode;

            bufferMusicsData[mID] = bms_tmp;

            return bool(HSP_FUNC_SUCCESED);
        }
        bool mmLoad(string filepath, int mode) {
            int mID = musicBufferingLastNumber + 1;
            return mmLoad(filepath, mID, mode);
        }
        bool mmLoad(string filepath) {
            int mID = musicBufferingLastNumber + 1;
            int mode = 1;
            return mmLoad(filepath, mID, mode);
        }

        //音楽ファイルの再生
        bool mmPlay(int mID) {
            if (mID > musicBufferingLastNumber) {
                return HSP_FUNC_FAILED;
            }
            if (bufferMusicsData[mID].bufferNumber != mID) {
                return HSP_FUNC_FAILED;
            }

            int mpm;
            if (bufferMusicsData[mID].type == "chunk") {
                mpm = Mix_PlayChannel(-1, chunkBuffer[mID], bufferMusicsData[mID].mode);
            }
            else {
                mpm = Mix_PlayMusic(musicBuffer[mID], bufferMusicsData[mID].mode);
            }

            return bool(mpm);
        }
        bool mmPlay() {
            int mID = musicBufferingLastNumber;
            mmPlay(mID);
        }

        //音楽ファイル再生時の設定
        void mmSet() {
            Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024);
        }
    };

/*----------------------------netクラス--------------------------*/
    class net {
        IPaddress ip;
        TCPsocket sock;
        string url;
    public:
        net() {
            if (SDLNet_Init() < 0) {
                cout << "[FakeHSP Error : I05 in init] SDL_netの初期化に失敗しました" << endl << " ->SDLNet_Error: " << SDLNet_GetError() << endl;
            }
        }

        //ネットワークから文字列を取得
        string netGet(string filepath, int msgsize) {
            string sendmes = "GET "+filepath+" HTTP/1.1\r\nHost: "+url+" \r\n\r\n";
            SDLNet_TCP_Send(sock, hsp::convStringToCharP(sendmes), sendmes.length());

            char inmsg[msgsize];
            int reclen= SDLNet_TCP_Recv(sock, inmsg, sizeof(inmsg));

            if (reclen < 0) {
                cout << "[FakeHSP Error : N02 in netGet] ネットワークからの文字列の取得に失敗しました : " << SDLNet_GetError() << endl;
            }

            return hsp::convStringToCharP(inmsg);
        }
        string netGet(string filepath) {
            int msgsize = 1024;
            return netGet(filepath, msgsize);
        }

        //ネットワークの終了
        void netTerm() {
            SDLNet_TCP_Close(sock);
        }

        //ネットワークのURLの設定
        int netURL(string _url, int port) {
            url = _url;
            if(SDLNet_ResolveHost(&ip, hsp::convStringToCharP(_url), port) == -1) {
                cout << "[FakeHSP Error : N01 in netURL] URLまたはIPアドレスの指定に失敗しました : " << SDLNet_GetError() << endl;
                return HSP_FUNC_FAILED;
            }
            sock = SDLNet_TCP_Open(&ip);
            return HSP_FUNC_SUCCESED;
        }
        int netURL(string _url) {
            int port = 80;
            return netURL(_url, port);
        }
    };

/*----------------------------noteクラス-------------------------*/
    class note {
        string* noteP = nullptr;

    public:
        //ノートの指定
        void noteSel(string& note) {
            noteP = &note;
        }

        //テキストファイルの読み込み
        void noteLoad(string fileName) {
            if (noteP == nullptr) {
                cout << "[FakeHSP Error : T01 in noteLoad] noteSelで対象となるノート（string型）を指定してください" << endl;
            }

            ifstream ifs(fileName);
            stringstream sts;
            sts << ifs.rdbuf();
            ifs.close();

            *noteP = sts.str();
        }

        //テキストファイルから1行分を取得
        void noteGet(string& returnStr, int line) {
            if (noteP == nullptr) {
                cout << "[FakeHSP Error : T01 in noteGet] noteSelで対象となるノート（string型）を指定してください" << endl;
            }

            returnStr = hsp::split(*noteP, "\n", line);
        }

        //テキストファイルの保存
        void noteSave(string fileName) {
            if (noteP == nullptr) {
                cout << "[FakeHSP Error : T01 in noteSave] noteSelで対象となるノート（string型）を指定してください" << endl;
            }

            ofstream ofs;
            ofs.open(fileName, ios::out);
            ofs << *noteP;
        }
    };

/*---------------------------windowクラス------------------------*/
    class window {
    public:
        /*----------------------------構造体-----------------------------*/
        typedef struct {
            int buttonID;
            int buttonPositionX; int buttonPositionY;
            int buttonSizeX; int buttonSizeY;
            string buttonMessage;
            int buttonColorR; int buttonColorG; int buttonColorB;
            bool buttonEnable;
        } buttonStruct;

        typedef struct {
            int bufferNumber;
            string bufferImageName;
            int bufferSizeW; int bufferSizeH;
            int rewrites;
        } bufferImageStruct;

        typedef struct {
            int bufferNumber;
            vector<SDL_Texture*> bufferTexture;
            vector<int> bufferTexturePositionX; vector<int> bufferTexturePositionY;
            vector<int> bufferTextureSizeW; vector<int> bufferTextureSizeH;
            vector<SDL_Rect*> bufferRect;
            vector<SDL_Color> bufferColor;
            vector<string> bufferType;
        } bufferSyntheticImageStruct;

    private:
        SDL_Window* sdlWindow;
        SDL_Renderer* render;
        TTF_Font* ttfFont = NULL;
        SDL_Surface* surfaceMessage = NULL, * surfaceImage = NULL;
        SDL_Texture* textureMessage = NULL, * textureImage = NULL, * textureSyntheticImage = NULL;
        vector<SDL_Texture*> textureImageBuffer; int textureImageBufferMaxNumber = 0;
        vector<bufferImageStruct> bufferImagesData; int textureImageBufferingNumber = -1;
        vector<bufferSyntheticImageStruct> bufferSyntheticImagesData; int textureImageSyntheticImageBufferingMaxNumber = -1;
        SDL_PixelFormat* sdlPixelFormat;
        SDL_Rect sdlRect, sdlRect2;
        SDL_Point sdlPoint;
        SDL_Event ev;
        SDL_Cursor* sdlCursor;

        int positionX = 0, positionY = 0;
        Uint8 r = 0, g = 0, b = 0, a = 0;
        char* fontName = NULL; int font_size;

        vector<buttonStruct> button_IDs;
        int total_buttons = 0;
        int objsizeW = 0, objsizeH = 0;
        int objimage = -1;

    public:
        /*-------------------------ウィンドウ作成関連（必須）---------------------------*/

        window() {
            if (number_of_windows == 0){
                if (init() == HSP_FUNC_FAILED){
                    closeWindow();
                }
            }
        }

        //初期化
        int init() {
            if (SDL_Init(SDL_INIT_VIDEO) < 0){
                cout << "[FakeHSP Error : I01 in init] SDLの初期化に失敗しました" << endl << " ->SDL_Error: " << SDL_GetError() << endl;
                return HSP_FUNC_FAILED;    //SDL_Initは最低限必要なためHSP_FUNC_FAILEDを返す
            }
            if (TTF_Init() < 0) {
                cout << "[FakeHSP Error : I02 in init] SDL_TTFの初期化に失敗しました" << endl << " ->TTF_Error: " << TTF_GetError() << endl;
            }
            int imgFlags = IMG_INIT_JPG|IMG_INIT_PNG;
            int ii = IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG);
            if (ii < 0) {
                cout << "[FakeHSP Error : I03 in init] SDL_IMGの初期化に失敗しました" << endl << " ->IMG_Error: " << IMG_GetError() << endl;
            }
            if (ii != imgFlags) {
                cout << "[FakeHSP Error : I04 in init] SDL_IMGのローダーの初期化に失敗しました" << endl << " ->IMG_Error: " << IMG_GetError() << endl;
            }

            return HSP_FUNC_SUCCESED;
        }

        //ウィンドウ作成
        int screen(int w, int h, int mode) {
            if (mode == 0) sdlWindow = SDL_CreateWindow("FakeHSP Window", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
            if (mode == 1) sdlWindow = SDL_CreateWindow("FakeHSP Window", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN+SDL_WINDOW_RESIZABLE);
            render = SDL_CreateRenderer(sdlWindow, -1, 0);

            if (sdlWindow==NULL){
                cout << "[FaledHSP Error : W01 in screen] ウィンドウの作成に失敗しました : SDL_Error :" << SDL_GetError() << endl;
                return HSP_FUNC_FAILED;
            }

            number_of_windows ++;

            return HSP_FUNC_SUCCESED;
        }

        //ウィンドウタイトル保持
        void title(string title) {
            char* cstr = new char[title.size() + 1];
            SDL_SetWindowTitle(sdlWindow, char_traits<char>::copy(cstr, title.c_str(), title.size()+1));
        }

        //Rendererを返す
        SDL_Renderer* getRender() {
            return render;
        }

        //終了処理
        void closeWindow() {
            if (ttfFont != NULL)
                TTF_CloseFont(ttfFont);
            if (surfaceMessage != NULL)
                SDL_FreeSurface(surfaceMessage);
            if (surfaceImage != NULL)
                SDL_FreeSurface(surfaceImage);
            if (textureMessage != NULL)
                SDL_DestroyTexture(textureMessage);
            if (textureImage != NULL)
                SDL_DestroyTexture(textureImage);

            if (sdlWindow != NULL) {
                SDL_DestroyWindow(sdlWindow);
                number_of_windows--;
            }
            if (render != NULL)
                SDL_free(render);

            if (number_of_windows == 0) {
                Mix_CloseAudio(); Mix_Quit();
                TTF_Quit();
                IMG_Quit();
                SDLNet_Quit();
                SDL_Quit();
            }
        }

        /*-------------------------その他の関数---------------------------*/

        //塗りつぶし（四角形）
        int boxf(int x1, int y1, int x2, int y2) {
            if (x2 < 0) {
                x2 = gInfo(10);
            }
            if (y2 < 0) {
                y2 = gInfo(11);
            }

            SDL_Rect sdlr;
            if (x2>x1) {
                sdlr.x = x1;
                sdlr.y = y1;
            } else {
                sdlr.x = x2;
                sdlr.y = y2;
            }

            sdlr.w = abs(x2-x1); sdlr.h = abs(y2-y1);

            if (textureImageBufferingNumber > 0) {
                setRectOnBuffer(&sdlr, "normal");
                return 0;
            }

            int rfr = SDL_RenderFillRect(render, &sdlr);

            if (rfr < 0) {cout << "[FakeHSP Error : D01 in boxf] 図形の描写に失敗しました" << endl;}

            return rfr;
        }
        int boxf() {        // 画面全体塗りつぶし
            return boxf(0, 0, gInfo(10), gInfo(11));
        }

        //半透明色で塗りつぶし
        int boxfA(int x1, int y1, int x2, int y2) {
            if (x2 < 0) {
                x2 = gInfo(10);
            }
            if (y2 < 0) {
                y2 = gInfo(11);
            }

            SDL_Rect sdlr;
            if (x2>x1) {
                sdlr.x = x1;
                sdlr.y = y1;
            } else {
                sdlr.x = x2;
                sdlr.y = y2;
            }

            sdlr.w = abs(x2-x1); sdlr.h = abs(y2-y1);

            SDL_SetRenderDrawColor(render, r, g, b, a);
            SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);

            if (textureImageBufferingNumber > 0) {
                setRectOnBuffer(&sdlr, "alpha");
                return 0;
            }

            int srfr = SDL_RenderFillRect(render, &sdlr);
            if (srfr < 0) {cout << "[FakeHSP Error : D01 in boxfA] 図形の描写に失敗しました" << endl;}

            SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_NONE);

            return srfr;
        }
        int boxfA() {       // 画面全体塗りつぶし
            return boxfA(0, 0, gInfo(10), gInfo(11));
        }

        //バッファ番号の指定
        void buffer(int number) {
            textureImageBufferingNumber = number;
        }

        //ボタン（疑似オブジェクト）の表示
        int button(string message) {
            int bID = total_buttons;

            int objsizeW_tmp = objsizeW, objsizeH_tmp = objsizeH;

            if (objimage == -1) {
                boxf(positionX, positionY, positionX + objsizeW, positionY + objsizeH);
            }
            else if (objimage == -2) {
                objsizeW_tmp = getStringSize(message, 'w');
                objsizeH_tmp = getStringSize(message, 'h');
            }
            else {
                gCopy(objimage, 0, 0, -1, -1);
                objsizeW_tmp = bufferImagesData[objimage].bufferSizeW;
                objsizeH_tmp = bufferImagesData[objimage].bufferSizeH;
            }

            buttonStruct bs = {bID, positionX, positionY, objsizeW_tmp, objsizeH_tmp, message, r, g, b, true};
            button_IDs.push_back(bs);

            int font_size_tmp = font_size;
            font_size = objsizeH_tmp-5;
            int rtmp = r, gtmp = g, btmp = b;
            if (objimage != -2) {r = 0; g = 0; b = 0;}

            if (message != "") { cMes(message, objsizeW_tmp, objsizeH_tmp - 1, 0);}

            font_size = font_size_tmp;
            r = rtmp; g = gtmp; b = btmp;

            total_buttons ++;

            return bID;
        }

        //_buttonIDのボタンの状態を取得
        int buttonGetInfo(int _buttonID){
            if (button_IDs[_buttonID].buttonPositionX < gInfo(0) and
                gInfo(0) < (button_IDs[_buttonID].buttonPositionX+button_IDs[_buttonID].buttonSizeX)
                and button_IDs[_buttonID].buttonPositionY < gInfo(1) and
                gInfo(1) < (button_IDs[_buttonID].buttonPositionY+button_IDs[_buttonID].buttonSizeY)) {
                return HSP_CURSOR_ON_RANGE_OF_BUTTON;
            }
            else {
                return HSP_CURSOR_NOT_ON_RANGE_OF_BUTTON;
            }
        }

        //次に設置するボタンで使われるボタンID
        int buttonNextID() {
            return total_buttons;
        }

        //画像のバッファ
        void celLoad(string img_name, int num, int transp_r, int transp_g, int transp_b) {
            bufferImageStruct bs_tmp;

            if (num > textureImageBufferMaxNumber) {
                for (int n=textureImageBufferMaxNumber; n<num; n++) {
                    textureImageBuffer.push_back(SDL_CreateTextureFromSurface(render, NULL));

                    bs_tmp.bufferNumber = -1;
                    bs_tmp.bufferImageName = "";
                    bs_tmp.bufferSizeW = 0; bs_tmp.bufferSizeH = 0;
                    bufferImagesData.push_back(bs_tmp);
                }

                if (hsp::exist(img_name) == HSP_FUNC_FAILED) {
                    cout << "[FakeHSP Error : F01 in celLoad] 画像ファイルが存在しません：" << img_name << endl;
                    closeWindow();
                }

                surfaceImage = IMG_Load(hsp::convStringToCharP(img_name));
                if (transp_r != -1 and transp_g != -1 and transp_b != -1) {
                    SDL_SetColorKey(surfaceImage, SDL_TRUE, SDL_MapRGB(surfaceImage->format, transp_r, transp_g, transp_b));
                    surfaceImage = SDL_ConvertSurfaceFormat(surfaceImage, SDL_PIXELFORMAT_RGBA32, 0);
                }
                textureImageBuffer.push_back(SDL_CreateTextureFromSurface(render, surfaceImage));

                bs_tmp.bufferNumber = num;
                bs_tmp.bufferImageName = img_name;
                bs_tmp.bufferSizeW = surfaceImage->w; bs_tmp.bufferSizeH = surfaceImage->h;
                bs_tmp.rewrites = 0;
                bufferImagesData.push_back(bs_tmp);

                textureImageBufferMaxNumber = num;
            }
            else {
                surfaceImage = IMG_Load(hsp::convStringToCharP(img_name));
                if (transp_r != -1 and transp_g != -1 and transp_b != -1) {
                    SDL_SetColorKey(surfaceImage, SDL_TRUE, SDL_MapRGB(surfaceImage->format, transp_r, transp_g, transp_b));
                    surfaceImage = SDL_ConvertSurfaceFormat(surfaceImage, SDL_PIXELFORMAT_RGBA32, 0);
                }
                textureImageBuffer[num] = (SDL_CreateTextureFromSurface(render, surfaceImage));

                bs_tmp.bufferNumber = num;
                bs_tmp.bufferImageName = img_name;
                bs_tmp.bufferSizeW = surfaceImage->w; bs_tmp.bufferSizeH = surfaceImage->h;
                bs_tmp.rewrites = 0;
                bufferImagesData[num] = (bs_tmp);
            }
        }
        void celLoad(string img_name, int num) {
            celLoad(img_name, num, -1, -1, -1);
        }

        //円の描画
        void circleDrowOnce(int n_cx, int n_cy, int radius) {
            double error = (double)-radius;
            double x = (double)radius - 0.5;
            double y = (double)0.5;
            double cx = n_cx - 0.5;
            double cy = n_cy - 0.5;

            while (x >= y) {
                point((int) (cx + x), (int) (cy + y));
                point((int) (cx + y), (int) (cy + x));

                if (x != 0) {
                    point((int) (cx - x), (int) (cy + y));
                    point((int) (cx + y), (int) (cy - x));
                }

                if (y != 0) {
                    point((int) (cx + x), (int) (cy - y));
                    point((int) (cx - y), (int) (cy + x));
                }

                if (x != 0 && y != 0) {
                    point((int) (cx - x), (int) (cy - y));
                    point((int) (cx - y), (int) (cy - x));
                }

                error += y;
                y++;
                error += y;

                if (error >= 0) {
                    x--;
                    error -= x;
                    error -= x;
                }
            }
        }
        void circle(int n_cx, int n_cy, int radius) {
            while (radius >= 0) {
                circleDrowOnce(n_cx, n_cy, radius);
                radius -= 1;
            }
        }

        //ウィンドウ全体を塗りつぶし&疑似オブジェクト全撤去
        void cls() {
            SDL_RenderClear(render);
            objClear();
        }

        //文字を中央寄せで表示する
        int cMes(string message, int sizeX, int sizeY, int mode) {
            SDL_Color FontColor = {r, g, b};

            vector<string> vecstr = hsp::split(message, "\n");

            int vi = vecstr.size();
            if (vi == 0) {vi = 1;}
            int src;

            for (int mi = 0; mi < vi; mi++) {
                if (mode == 0) {        //描写速度は遅いが綺麗
                    surfaceMessage = TTF_RenderUTF8_Blended(ttfFont, hsp::convStringToCharP(vecstr[mi]), FontColor);
                } else if (mode == 1) {   //描写速度は早いが汚い
                    surfaceMessage = TTF_RenderUTF8_Solid(ttfFont, hsp::convStringToCharP(vecstr[mi]), FontColor);
                }

                textureMessage = SDL_CreateTextureFromSurface(render, surfaceMessage);

                SDL_Rect Message_rect;
                Message_rect.x = positionX + (sizeX / 2) - ((surfaceMessage->w) / 2);
                Message_rect.y = positionY + (sizeY / 2) - ((surfaceMessage->h) / 2) * vi + mi * surfaceMessage->h;
                Message_rect.w = surfaceMessage->w;
                Message_rect.h = surfaceMessage->h;

                src = SDL_RenderCopy(render, textureMessage, NULL, &Message_rect);
            }

            if (src < 0) {cout << "[FakeHSP Error : D02 in cMes] 文字の描写に失敗しました : " << message << endl; src = HSP_FUNC_FAILED;}

            if (src >= 0) {
                src = positionY + (sizeY / 2) - ((surfaceMessage->h) / 2) + vi * surfaceMessage->h;
            }

            return src;
        }
        void cMes(string message, int sizeX, int sizeY) {
            cMes(message, sizeX, sizeY, 0);
        }
        void cMes(string message, int sizeX) {
            cMes(message, sizeX, font_size, 0);
        }

        //描写色を指定する
        void color(int _r, int _g, int _b) {
            r = _r; g = _g; b = _b; a = 255;
            int srd = SDL_SetRenderDrawColor(render, r, g, b, a);
            if (srd < 0) {cout << "[FakeHSP Error : S01 in color] 描写色の指定に失敗しました" << endl;}
        }

        //描写色を指定する（α値つき）
        void colorA(int _r, int _g, int _b, int _a) {
            r = _r, g = _g, b = _b, a = _a;
            int srd = SDL_SetRenderDrawColor(render, r, g, b, a);
            if (srd < 0) {cout << "[FakeHSP Error : S01 in colorA] 描写色の指定に失敗しました" << endl;}
        }

        //ダイアログ表示（表示に成功すれば0, 失敗すれば-1を返す）
        int dialog(string message, int mode, string option) {
            char* cstr_message = new char[message.size() + 1];
            char* cstr_option = new char[option.size() + 1];
            int ssm;
            if (mode == 0) {
                ssm = SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, hsp::convStringToCharP(option),
                                               hsp::convStringToCharP(message), sdlWindow);
            }
            if (mode == 1) {
                ssm = SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, hsp::convStringToCharP(option),
                                               hsp::convStringToCharP(message), sdlWindow);
            }

            if (ssm < 0) {cout << "[FakeHSP Error : D03 in dialog] ダイアログの表示に失敗しました : " << message << ']' << endl; ssm = HSP_FUNC_FAILED;}

            delete cstr_message;
            delete cstr_option;

            return ssm;
        }

        //フォントを指定
        void font(string _font, int _font_size) {
            fontName = hsp::convStringToCharP(_font);
            font_size = _font_size;
            ttfFont = TTF_OpenFont(fontName, font_size);
        }

        //バッファ画像の表示
        int gCopy(int num, int TopLeftPositionX, int TopLeftPositionY, int SizeW, int SizeH) {
            int rc;
            int bsid_num = -1;
            if (bufferImagesData[num].rewrites > 0) {
                for (int i=0; i<bufferSyntheticImagesData.size(); i++) {
                    if (bufferSyntheticImagesData[i].bufferNumber == num) {
                        for (int j=0; j < bufferImagesData[num].rewrites; j++) {
                            sdlRect.x = positionX - TopLeftPositionX + bufferSyntheticImagesData[i].bufferTexturePositionX[j];
                            sdlRect.y = positionY - TopLeftPositionY + bufferSyntheticImagesData[i].bufferTexturePositionY[j];

                            if (TopLeftPositionX == 0)
                                sdlRect.w = bufferImagesData[num].bufferSizeW - bufferSyntheticImagesData[i].bufferTexturePositionX[j];
                            else
                                sdlRect.w = bufferImagesData[num].bufferSizeW - bufferSyntheticImagesData[i].bufferTexturePositionX[0];

                            if (TopLeftPositionY == 0)
                                sdlRect.h = bufferImagesData[num].bufferSizeH - bufferSyntheticImagesData[i].bufferTexturePositionY[j];
                            else
                                sdlRect.h = bufferImagesData[num].bufferSizeH - bufferSyntheticImagesData[i].bufferTexturePositionY[0];

                            SDL_RenderSetViewport(render, &sdlRect);

                            sdlRect.x = TopLeftPositionX;
                            sdlRect.y = TopLeftPositionY;
                            sdlRect.w = SizeW;
                            sdlRect.h = SizeH;
                            if (sdlRect.w < 0) {sdlRect.w = bufferImagesData[num].bufferSizeW;}
                            if (sdlRect.h < 0) {sdlRect.h = bufferImagesData[num].bufferSizeH;}

                            if (bufferSyntheticImagesData[i].bufferType[j] == "image") {
                                rc = SDL_RenderCopy(render, bufferSyntheticImagesData[i].bufferTexture[j], &sdlRect, &sdlRect);
                            }
                            if (bufferSyntheticImagesData[i].bufferType[j] == "rect-normal") {
                                int r_tmp = r, g_tmp = g, b_tmp = b;
                                r = bufferSyntheticImagesData[i].bufferColor[j].r;
                                g = bufferSyntheticImagesData[i].bufferColor[j].g;
                                b = bufferSyntheticImagesData[i].bufferColor[j].b;
                                color(r, g, b);
                                rc = boxf(bufferSyntheticImagesData[i].bufferTexturePositionX[j],
                                          bufferSyntheticImagesData[i].bufferTexturePositionY[j],
                                          bufferSyntheticImagesData[i].bufferRect[j]->w,
                                          bufferSyntheticImagesData[i].bufferRect[j]->h);
                                r = r_tmp; g = g_tmp; b = b_tmp;
                                color(r, g, b);
                            }
                            if (bufferSyntheticImagesData[i].bufferType[j] == "rect-alpha") {
                                int r_tmp = r, g_tmp = g, b_tmp = b, a_tmp = a;
                                r = bufferSyntheticImagesData[i].bufferColor[j].r;
                                g = bufferSyntheticImagesData[i].bufferColor[j].g;
                                b = bufferSyntheticImagesData[i].bufferColor[j].b;
                                a = bufferSyntheticImagesData[i].bufferColor[j].a;
                                colorA(r, g, b, a);
                                rc = boxfA(bufferSyntheticImagesData[i].bufferTexturePositionX[j],
                                           bufferSyntheticImagesData[i].bufferTexturePositionY[j],
                                           bufferSyntheticImagesData[i].bufferRect[j]->w,
                                           bufferSyntheticImagesData[i].bufferRect[j]->h);
                                r = r_tmp; g = g_tmp; b = b_tmp; a = a_tmp;
                                color(r, g, b);
                            }
                        }
                        break;
                    }
                }
            }
            else {
                sdlRect.x = positionX - TopLeftPositionX; sdlRect.y = positionY - TopLeftPositionY;
                sdlRect.w = bufferImagesData[num].bufferSizeW; sdlRect.h = bufferImagesData[num].bufferSizeH;
                SDL_RenderSetViewport(render, &sdlRect);

                sdlRect.x = TopLeftPositionX; sdlRect.y = TopLeftPositionY;
                sdlRect.w = SizeW; sdlRect.h = SizeH;
                if (sdlRect.w < 0) {sdlRect.w = bufferImagesData[num].bufferSizeW;}
                if (sdlRect.h < 0) {sdlRect.h = bufferImagesData[num].bufferSizeH;}

                rc = SDL_RenderCopy(render, textureImageBuffer[num], &sdlRect, &sdlRect);
            }

            if (rc < 0) {
                cout << "[FaledHSP Error : D06 in gCopy] バッファ画像の描画に失敗しました バッファ番号 : " << num << endl << " ->IMG_Error : " << IMG_GetError() << endl;
                rc = HSP_FUNC_FAILED;
            }

            sdlRect.x = 0; sdlRect.y = 0;
            sdlRect.w = gInfo(10); sdlRect.h = gInfo(11);
            SDL_RenderSetViewport(render, &sdlRect);

            return rc;
        }

        //SDL2のプライベートメンバevを返す
        SDL_Event getEvent() {
            return ev;
        }

        //文字を描写したときの描画に使う範囲を返す
        int getStringSize(string str, char mode) {
            SDL_Color sdlColor = {r, g, b};
            vector<string> vecstr = hsp::split(str, "\n");
            int vecstr_lines = vecstr.size();
            if (vecstr_lines == 0) {vecstr_lines = 1;}

            int line_w_longest = 0, tmp_long;
            for (int i=0; i<vecstr_lines; i++) {
                surfaceMessage = TTF_RenderUTF8_Blended(ttfFont, hsp::convStringToCharP(vecstr[i]), sdlColor);
                if (surfaceMessage->w > line_w_longest) {
                    line_w_longest = surfaceMessage->w;
                }
            }

            return mode == 'w' ? line_w_longest : surfaceMessage->h * vecstr_lines;
        }

        //ウィンドウ, マウスの情報を取得
        int gInfo(int mode) {
            if (mode == 0) {    //マウスのx座標
                SDL_GetMouseState(&sdlPoint.x, &sdlPoint.y);
                return sdlPoint.x;
            }
            if (mode == 1) {    //マウスのy座標
                SDL_GetMouseState(&sdlPoint.x, &sdlPoint.y);
                return sdlPoint.y;
            }
            if (mode == 3) {    //このウィンドウのウィンドウID
                return SDL_GetWindowID(sdlWindow);
            }
            if (mode == 4) {    //このウィンドウの左上のx座標
                SDL_GetWindowPosition(sdlWindow, &sdlPoint.x, &sdlPoint.y);
                return sdlPoint.x;
            }
            if (mode == 5) {    //このウィンドウの左上のy座標
                SDL_GetWindowPosition(sdlWindow, &sdlPoint.x, &sdlPoint.y);
                return sdlPoint.y;
            }
            if (mode == 6) {    //このウィンドウの右下のx座標
                SDL_GetWindowPosition(sdlWindow, &sdlRect.x, &sdlRect.y);
                SDL_GetWindowSize(sdlWindow, &sdlRect.w, &sdlRect.h);
                return (sdlRect.x+sdlRect.w);
            }
            if (mode == 7) {    //このウィンドウの右下のy座標
                SDL_GetWindowPosition(sdlWindow, &sdlRect.x, &sdlRect.y);
                SDL_GetWindowSize(sdlWindow, &sdlRect.w, &sdlRect.h);
                return (sdlRect.y+sdlRect.h);
            }
            if (mode == 10) {   //このウィンドウの大きさ（横）
                SDL_GetWindowSize(sdlWindow, &sdlRect.w, &sdlRect.h);
                return sdlRect.w;
            }
            if (mode == 11) {   //このウィンドウの大きさ（縦）
                SDL_GetWindowSize(sdlWindow, &sdlRect.w, &sdlRect.h);
                return sdlRect.h;
            }
            return -1;
        }

        //描画先の指定
        void gSel(int number) {
            if (number == 0)
                textureImageBufferingNumber = -1;
            else
                textureImageBufferingNumber = number;
        }

        //画像の伸縮表示
        int gZoom(int ZoomW, int ZoomH, int num, int TopLeftPositionX, int TopLeftPositionY, int SizeW, int SizeH) {
            int rc;
            int bsid_num = -1;
            if (bufferImagesData[num].rewrites > 0) {
                for (int i=0; i<bufferSyntheticImagesData.size(); i++) {
                    if (bufferSyntheticImagesData[i].bufferNumber == num) {
                        for (int j=0; j < bufferImagesData[num].rewrites; j++) {
                            sdlRect.x = bufferSyntheticImagesData[i].bufferTexturePositionX[j] - TopLeftPositionX;
                            sdlRect.y = bufferSyntheticImagesData[i].bufferTexturePositionY[j] - TopLeftPositionY;
                            sdlRect.w = ZoomW; sdlRect.h = ZoomH;
                            if (sdlRect.w < 0) {sdlRect.w = gInfo(10);}
                            if (sdlRect.h < 0) {sdlRect.h = gInfo(11);}
                            SDL_RenderSetViewport(render, &sdlRect);

                            sdlRect.x = TopLeftPositionX;
                            sdlRect.y = TopLeftPositionY;
                            //if (i==0) {     //背景画像であればSizeW, SizeHに合わせる
                            sdlRect.w = SizeW;
                            sdlRect.h = SizeH;
                            if (sdlRect.w < 0) {sdlRect.w = bufferSyntheticImagesData[i].bufferTextureSizeW[j];}
                            if (sdlRect.h < 0) {sdlRect.h = bufferSyntheticImagesData[i].bufferTextureSizeH[j];}
                            /*}
                            else {          //合成要素であれば比率で大きさを変える
                                sdlRect.w = bufferSyntheticImagesData[i].bufferTextureSizeW[j] / bufferImagesData[num].bufferSizeW * SizeW;
                                sdlRect.h = bufferSyntheticImagesData[i].bufferTextureSizeH[j] / bufferImagesData[num].bufferSizeH * SizeH;
                                if (SizeW < 0) {bufferSyntheticImagesData[i].bufferTextureSizeW[j] / bufferImagesData[num].bufferSizeW * Ginfo(10);}
                                if (SizeH < 0) {bufferSyntheticImagesData[i].bufferTextureSizeH[j] / bufferImagesData[num].bufferSizeH * gInfo(11);}
                            }*/

                            sdlRect2.x = TopLeftPositionX;
                            sdlRect2.y = TopLeftPositionY;
                            if (j==0) {     //背景画像であればSizeW, SizeHに合わせる
                                sdlRect2.w = ZoomW;
                                sdlRect2.h = ZoomH;
                                if (sdlRect2.w < 0) {sdlRect2.w = gInfo(10);}
                                if (sdlRect2.h < 0) {sdlRect2.h = gInfo(11);}
                            }
                            else {          //合成要素であれば比率で大きさを変える
                                sdlRect2.w = bufferSyntheticImagesData[i].bufferTextureSizeW[j] / bufferImagesData[num].bufferSizeW * ZoomW;
                                sdlRect2.h = bufferSyntheticImagesData[i].bufferTextureSizeH[j] / bufferImagesData[num].bufferSizeH * ZoomH;
                                if (SizeW < 0) {sdlRect2.w = int(float(bufferSyntheticImagesData[i].bufferTextureSizeW[j]) / bufferSyntheticImagesData[i].bufferTextureSizeW[0] *
                                                                 gInfo(10));}
                                if (SizeH < 0) {sdlRect2.h = int(float(bufferSyntheticImagesData[i].bufferTextureSizeH[j]) / bufferSyntheticImagesData[i].bufferTextureSizeH[0] *
                                                                 gInfo(11));}
                            }

                            rc = SDL_RenderCopy(render, bufferSyntheticImagesData[i].bufferTexture[j], &sdlRect, &sdlRect2);
                        }
                        break;
                    }
                }
            }
            else {
                sdlRect.x = positionX - TopLeftPositionX; sdlRect.y = positionY - TopLeftPositionY;
                sdlRect.w = ZoomW; sdlRect.h = ZoomH;
                if (sdlRect.w < 0) {sdlRect.w = gInfo(10);}
                if (sdlRect.h < 0) {sdlRect.h = gInfo(11);}
                SDL_RenderSetViewport(render, &sdlRect);

                sdlRect.x = TopLeftPositionX; sdlRect.y = TopLeftPositionY;
                sdlRect.w = SizeW; sdlRect.h = SizeH;
                if (sdlRect.w < 0) {sdlRect.w = bufferImagesData[num].bufferSizeW;}
                if (sdlRect.h < 0) {sdlRect.h = bufferImagesData[num].bufferSizeH;}

                sdlRect2.x = TopLeftPositionX; sdlRect2.y = TopLeftPositionY;
                sdlRect2.w = ZoomW; sdlRect2.h = ZoomH;
                if (sdlRect2.w < 0) {sdlRect2.w = gInfo(10);}
                if (sdlRect2.h < 0) {sdlRect2.h = gInfo(11);}

                int rc = SDL_RenderCopy(render, textureImageBuffer[num], &sdlRect, &sdlRect2);
            }

            if (rc < 0) {cout << "[FaledHSP Error : D06 in gCopy] バッファ画像の描画に失敗しました バッファ番号 : " << num << endl; rc = HSP_FUNC_FAILED;}

            sdlRect.x = 0; sdlRect.y = 0;
            sdlRect.w = gInfo(10); sdlRect.h = gInfo(11);
            SDL_RenderSetViewport(render, &sdlRect);

            return rc;
        }

        //線を引く
        int line(int x1, int y1, int x2, int y2) {
            int rdl = SDL_RenderDrawLine(render, x1, y1, x2, y2);
            if (rdl < 0) {cout << "[FakeHSP Error : D04 in line] 線の描画に失敗しました" << endl; rdl = HSP_FUNC_FAILED;}
            return rdl;
        }

        //文字を表示する
        int mes(string message, int mode) {
            SDL_Color FontColor = {r, g, b};

            vector<string> vecstr = hsp::split(message, "\n");

            int src;
            int vi = vecstr.size();
            if (vi == 0) {vi = 1;}

            for (int mi = 0; mi < vi; mi++) {
                if (mode == 0) {        //描写速度は遅いが綺麗
                    surfaceMessage = TTF_RenderUTF8_Blended(ttfFont, hsp::convStringToCharP(vecstr[mi]), FontColor);
                } else if (mode == 1) {   //描写速度は早いが汚い
                    surfaceMessage = TTF_RenderUTF8_Solid(ttfFont, hsp::convStringToCharP(vecstr[mi]), FontColor);
                }

                textureMessage = SDL_CreateTextureFromSurface(render, surfaceMessage);

                SDL_Rect Message_rect;
                Message_rect.x = positionX;
                Message_rect.y = positionY + surfaceMessage->h*mi;
                Message_rect.w = surfaceMessage->w;
                Message_rect.h = surfaceMessage->h;

                src = SDL_RenderCopy(render, textureMessage, NULL, &Message_rect);
            }

            if (src < 0) {cout << "[FakeHSP Error : D02 in mes] 文字の描写に失敗しました : " << message << endl; src = HSP_FUNC_FAILED;}

            if (src >= 0) {
                src = positionY + vi * surfaceMessage->h;
            }

            return src;
        }
        void mes(string message) {
            mes(message, 0);
        }

        //オブジェクト情報のクリア
        void objClear() {
            button_IDs.clear();
            total_buttons = 0;
        }

        //オブジェクトの無効化（buttonのみ対応）
        void objEnable(int bID, bool enable) {
            button_IDs[bID].buttonEnable = enable;
        }

        //オブジェクトに使う画像の指定（バッファ番号）
        void objImage(int num) {
            objimage = num;
        }

        //オブジェクトの大きさを指定
        void objSize(int _w, int _h) {
            objsizeW = _w;
            objsizeH = _h;
        }

        //画像を表示
        int picLoad(string img_name) {
            if (hsp::exist(img_name) == HSP_FUNC_FAILED) {cout << "[FakeHSP Error : F01 in picLoad] 画像ファイルが存在しません：" << img_name << endl;
                closeWindow();}

            surfaceImage = IMG_Load(hsp::convStringToCharP(img_name));

            sdlRect.x = positionX; sdlRect.y = positionY;
            sdlRect.w = surfaceImage->w; sdlRect.h = surfaceImage->h;
            SDL_RenderSetViewport(render, &sdlRect);

            if (textureImageBufferingNumber >= 0) {
                bufferImageStruct bs_tmp;
                if (textureImageBufferingNumber > textureImageBufferMaxNumber) {
                    for (int n=textureImageBufferMaxNumber; n<textureImageBufferingNumber; n++) {
                        textureImageBuffer.push_back(SDL_CreateTextureFromSurface(render, NULL));

                        bs_tmp.bufferNumber = -1;
                        bs_tmp.bufferImageName = "";
                        bs_tmp.bufferSizeW = 0; bs_tmp.bufferSizeH = 0;
                        bufferImagesData.push_back(bs_tmp);
                    }

                    surfaceImage = IMG_Load(hsp::convStringToCharP(img_name));
                    textureImageBuffer.push_back(SDL_CreateTextureFromSurface(render, surfaceImage));

                    bs_tmp.bufferNumber = textureImageBufferingNumber;
                    bs_tmp.bufferImageName = img_name;
                    bs_tmp.bufferSizeW = surfaceImage->w; bs_tmp.bufferSizeH = surfaceImage->h;
                    bs_tmp.rewrites = 0;
                    bufferImagesData.push_back(bs_tmp);

                    textureImageBufferMaxNumber = textureImageBufferingNumber;
                }
                else {
                    //既にバッファになにか書き込まれている場合（合成）
                    if (bufferImagesData[textureImageBufferingNumber].bufferNumber == textureImageBufferingNumber) {
                        surfaceImage = IMG_Load(hsp::convStringToCharP(img_name));
                        setSurfaceOnBuffer(surfaceImage);
                    }
                        //まだバッファに書き込まれていない場合（バッファ領域作成）
                    else {
                        surfaceImage = IMG_Load(hsp::convStringToCharP(img_name));
                        textureImageBuffer[textureImageBufferingNumber] = SDL_CreateTextureFromSurface(render, surfaceImage);

                        bs_tmp.bufferNumber = textureImageBufferingNumber;
                        bs_tmp.bufferImageName = img_name;
                        bs_tmp.bufferSizeW = surfaceImage->w; bs_tmp.bufferSizeH = surfaceImage->h;
                        bs_tmp.rewrites = 0;
                        bufferImagesData[textureImageBufferingNumber] = (bs_tmp);
                    }
                }

                return HSP_FUNC_SUCCESED;
            }

            textureImage = SDL_CreateTextureFromSurface(render, surfaceImage);

            int rc = SDL_RenderCopy(render, textureImage, NULL, NULL);

            if (rc < 0) {cout << "[FaledHSP Error : D05 in picLoad] 画像の描画に失敗しました : " << img_name << endl; rc = HSP_FUNC_FAILED;}

            sdlRect.x = 0; sdlRect.y = 0;
            sdlRect.w = gInfo(10); sdlRect.h = gInfo(11);
            SDL_RenderSetViewport(render, &sdlRect);

            return rc;
        }

        // 座標に点を描画
        void point(int x, int y) {
            SDL_SetRenderDrawColor(render, r, g, b, 0);
            SDL_RenderDrawPoint(render, x, y);
        }

        //位置指定
        void pos(int _x, int _y) {
            positionX = _x;
            positionY = _y;
        }

        //画面を更新/初期化
        void reDraw(int mode) {
            if (mode == 0) {
                int rcrp = SDL_RenderClear(render);
                if (rcrp < 0) {cout << "[FakeHSP Error : R01 in reDraw] 画面の初期化に失敗しました" << endl;}
            }
            if (mode == 1) {
                SDL_RenderPresent(render);
            }
        }

        //カーソルアイコンの変更
        void setCursor(int cursorID) {
            sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);    // デフォルト

            if (cursorID == 1)  sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
            if (cursorID == 2)  sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
            if (cursorID == 3)  sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
            if (cursorID == 4)  sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAITARROW);
            if (cursorID == 5)  sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
            if (cursorID == 6)  sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
            if (cursorID == 7)  sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
            if (cursorID == 8)  sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
            if (cursorID == 9)  sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
            if (cursorID == 10) sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO);
            if (cursorID == 11) sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

            SDL_SetCursor(sdlCursor);
        }

        //バッファに合成要素のサーフェスを追加（合成）
        void setSurfaceOnBuffer(SDL_Surface *surfaceImage) {
            bool struct_exist = false;
            SDL_Color nullColor;
            for (int n=0; n<bufferSyntheticImagesData.size(); n++) {
                if (bufferSyntheticImagesData[n].bufferNumber == textureImageBufferingNumber) {
                    struct_exist = true;

                    bufferImagesData[textureImageBufferingNumber].rewrites ++;

                    bufferSyntheticImagesData[n].bufferTexture.push_back(SDL_CreateTextureFromSurface(render, surfaceImage));
                    bufferSyntheticImagesData[n].bufferTexturePositionX.push_back(positionX);
                    bufferSyntheticImagesData[n].bufferTexturePositionY.push_back(positionY);
                    bufferSyntheticImagesData[n].bufferTextureSizeW.push_back(bufferImagesData[textureImageBufferingNumber].bufferSizeW);
                    bufferSyntheticImagesData[n].bufferTextureSizeH.push_back(bufferImagesData[textureImageBufferingNumber].bufferSizeH);
                    bufferSyntheticImagesData[n].bufferRect.push_back(NULL);
                    bufferSyntheticImagesData[n].bufferColor.push_back(nullColor);
                    bufferSyntheticImagesData[n].bufferType.push_back("image");

                    bufferImagesData[textureImageBufferingNumber].rewrites ++;
                }
            }

            if (struct_exist == false) {
                bufferSyntheticImageStruct bsic;

                bsic.bufferNumber = textureImageBufferingNumber;
                bsic.bufferTexture.push_back(textureImageBuffer[textureImageBufferingNumber]);
                bsic.bufferTexturePositionX.push_back(0);
                bsic.bufferTexturePositionY.push_back(0);
                bsic.bufferTextureSizeW.push_back(bufferImagesData[textureImageBufferingNumber].bufferSizeW);
                bsic.bufferTextureSizeH.push_back(bufferImagesData[textureImageBufferingNumber].bufferSizeH);
                bsic.bufferRect.push_back(NULL);
                bsic.bufferColor.push_back(nullColor);
                bsic.bufferType.push_back("image");
                bufferSyntheticImagesData.push_back(bsic);

                bufferSyntheticImagesData[bufferSyntheticImagesData.size()-1].bufferNumber = textureImageBufferingNumber;
                bufferSyntheticImagesData[bufferSyntheticImagesData.size()-1].bufferTexture.push_back(SDL_CreateTextureFromSurface(render, surfaceImage));
                bufferSyntheticImagesData[bufferSyntheticImagesData.size()-1].bufferTexturePositionX.push_back(positionX);
                bufferSyntheticImagesData[bufferSyntheticImagesData.size()-1].bufferTexturePositionY.push_back(positionY);
                bufferSyntheticImagesData[bufferSyntheticImagesData.size()-1].bufferTextureSizeW.push_back(surfaceImage->w);
                bufferSyntheticImagesData[bufferSyntheticImagesData.size()-1].bufferTextureSizeH.push_back(surfaceImage->h);
                bufferSyntheticImagesData[bufferSyntheticImagesData.size()-1].bufferRect.push_back(NULL);
                bufferSyntheticImagesData[bufferSyntheticImagesData.size()-1].bufferColor.push_back(nullColor);
                bufferSyntheticImagesData[bufferSyntheticImagesData.size()-1].bufferType.push_back("image");

                bufferImagesData[textureImageBufferingNumber].rewrites = 2;
            }
        }

        //バッファに合成要素の図形を追加（合成）
        void setRectOnBuffer(SDL_Rect *rect, string mode) {
            bool struct_exist = false;
            SDL_Color color = {Uint8(r), Uint8(g), Uint8(b), Uint8(a)}, nullColor;
            for (int n=0; n<bufferSyntheticImagesData.size(); n++) {
                if (bufferSyntheticImagesData[n].bufferNumber == textureImageBufferingNumber) {
                    struct_exist = true;

                    bufferImagesData[textureImageBufferingNumber].rewrites ++;

                    bufferSyntheticImagesData[n].bufferRect.push_back(rect);
                    bufferSyntheticImagesData[n].bufferTexture.push_back(NULL);
                    bufferSyntheticImagesData[n].bufferTextureSizeW.push_back(NULL);
                    bufferSyntheticImagesData[n].bufferTextureSizeH.push_back(NULL);
                    bufferSyntheticImagesData[n].bufferTexturePositionX.push_back(positionX);
                    bufferSyntheticImagesData[n].bufferTexturePositionY.push_back(positionY);
                    bufferSyntheticImagesData[n].bufferColor.push_back(color);

                    if (mode == "normal") {
                        bufferSyntheticImagesData[n].bufferType.push_back("rect-normal");
                    }
                    else if (mode == "alpha") {
                        bufferSyntheticImagesData[n].bufferType.push_back("rect-alpha");
                    }

                    bufferImagesData[textureImageBufferingNumber].rewrites ++;
                }
            }

            if (struct_exist == false) {
                bufferSyntheticImageStruct bsic;

                bsic.bufferNumber = textureImageBufferingNumber;
                bsic.bufferTexture.push_back(textureImageBuffer[textureImageBufferingNumber]);
                bsic.bufferTexturePositionX.push_back(0);
                bsic.bufferTexturePositionY.push_back(0);
                bsic.bufferTextureSizeW.push_back(bufferImagesData[textureImageBufferingNumber].bufferSizeW);
                bsic.bufferTextureSizeH.push_back(bufferImagesData[textureImageBufferingNumber].bufferSizeH);
                bsic.bufferRect.push_back(NULL);
                bsic.bufferColor.push_back(nullColor);
                bsic.bufferType.push_back("image");
                bufferSyntheticImagesData.push_back(bsic);

                bufferSyntheticImagesData[bufferSyntheticImagesData.size()-1].bufferRect.push_back(rect);
                bufferSyntheticImagesData[bufferSyntheticImagesData.size()-1].bufferTexturePositionX.push_back(positionX);
                bufferSyntheticImagesData[bufferSyntheticImagesData.size()-1].bufferTexturePositionY.push_back(positionY);
                bufferSyntheticImagesData[bufferSyntheticImagesData.size()-1].bufferColor.push_back(color);
                bufferSyntheticImagesData[bufferSyntheticImagesData.size()-1].bufferTexture.push_back(NULL);
                bufferSyntheticImagesData[bufferSyntheticImagesData.size()-1].bufferTextureSizeW.push_back(NULL);
                bufferSyntheticImagesData[bufferSyntheticImagesData.size()-1].bufferTextureSizeH.push_back(NULL);

                if (mode == "normal") {
                    bufferSyntheticImagesData[bufferSyntheticImagesData.size()-1].bufferType.push_back("rect-normal");
                }
                else if (mode == "alpha") {
                    bufferSyntheticImagesData[bufferSyntheticImagesData.size()-1].bufferType.push_back("rect-alpha");
                }

                bufferImagesData[textureImageBufferingNumber].rewrites = 2;
            }
        }

        //終端処理（ウィンドウを閉じるボタンの操作待ち）
        //ボタンが押されたらボタンのIDを返す
        int stop() {
            bool changed_cursor = true;
            int waitLoopID;

            while(true){
                waitLoopID = waitLoop(changed_cursor);

                if (waitLoopID != HSP_NOTHING) {
                    break;
                }
            }

            if (waitLoopID == HSP_WINDOW_CLOSE) {
                closeWindow();
                return HSP_WINDOW_CLOSE;
            }
            else {
                return waitLoopID;
            }
        }

        int windowWait(int waitTime) {
            bool changed_cursor = true;
            int waitLoopID;

            for (int i=0; i<waitTime; i++) {
                waitLoopID = waitLoop(changed_cursor);

                if (waitLoopID == HSP_WINDOW_CLOSE) {
                    closeWindow();
                    return HSP_WINDOW_CLOSE;
                }

                wait(1);
            }

            return waitLoopID;
        }

        // stop, window.waitのループ中の処理
        // ウィンドウの状態やボタンの状態の確認
        int waitLoop(bool changed_cursor) {
            //ボタンの状態の確認
            int mb = 0;
            for(int b=0; b<total_buttons; b++) {
                //有効なボタン上にカーソルがあるとき、カーソルのアイコンを変える
                if (buttonGetInfo(button_IDs[b].buttonID) == HSP_CURSOR_ON_RANGE_OF_BUTTON) {
                    if (button_IDs[b].buttonEnable) {
                        sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
                        SDL_SetCursor(sdlCursor);

                        /*
                        if (changed_cursor == false) {
                            int positionX_tmp = positionX, positionY_tmp = positionY;
                            Pos(button_IDs[b].buttonPositionX, button_IDs[b].buttonPositionY);
                            int r_tmp = r, g_tmp = g, b_tmp = b;
                            Color(button_IDs[b].buttonColorR+100, button_IDs[b].buttonColorG+100, button_IDs[b].buttonColorB+100);

                            button(button_IDs[b].buttonMessage);

                            color(r_tmp, g_tmp, b_tmp);
                            pos(positionX_tmp, positionY_tmp);
                        }
                         */

                        mb++;
                        changed_cursor = true;
                    }
                }
                //有効なボタン上でクリックされれば押されたボタンのIDを返す
                if (buttonGetInfo(button_IDs[b].buttonID) == HSP_CURSOR_ON_RANGE_OF_BUTTON and ev.type==SDL_MOUSEBUTTONDOWN) {
                    if (buttonGetInfo(button_IDs[b].buttonID) == 1) {
                        if (button_IDs[b].buttonEnable) {
                            ev.type = 0;        // ev.typeをリセット
                            return button_IDs[b].buttonID;
                        }
                    }
                }
            }
            //カーソルをもとに戻す
            if (changed_cursor and mb == 0) {
                sdlCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
                SDL_SetCursor(sdlCursor);
                changed_cursor = false;
            }

            //ウィンドウが操作されたときの処理
            while(SDL_PollEvent(&ev)) {
                //ウィンドウを閉じる
                if (ev.type==SDL_WINDOWEVENT and ev.window.event==SDL_WINDOWEVENT_CLOSE) {
                    return HSP_WINDOW_CLOSE;
                }
                if (ev.type==SDL_WINDOWEVENT and ev.window.event==SDL_WINDOWEVENT_SIZE_CHANGED) {
                    return HSP_WINODW_CHANGED_SIZE;
                }
            }

            return HSP_NOTHING;
        }

        //ウィンドウサイズ変更
        void width(int w, int h) {
            SDL_SetWindowSize(sdlWindow, w, h);
        }
    };
};