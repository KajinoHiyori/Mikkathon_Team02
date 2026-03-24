//========================================================
// 
// 3Dアクションゲーム_MASTER KEY[main.h]
// 
//========================================================
#ifndef _MAIN_H_	// もしマクロ定義されていなければ
#define _MAIN_H_	// 2重インクルード防止のマクロを定義する

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include "d3dx9.h"						// 描画処理に必要
#define DIRECTINPUT_VERSION	(0x0800)	// ビルド時の警告対処用マクロ
#include "dinput.h"						// 入力処理に必要
#include "xaudio2.h"					// サウンド処理に必要
#include "xinput.h"						// ジョイパッド処理に必要

// ライブラリのリンク
#pragma comment(lib, "d3d9.lib")	// 描画処理に必要
#pragma comment(lib, "d3dx9.lib")	// [d3d9.lib]の拡張ライブラリ
#pragma comment(lib, "dxguid.lib")	// DirectXコンポーネント(部品)使用に必要
#pragma comment(lib, "winmm.lib")	// システム時刻の取得に必要
#pragma comment(lib, "dinput8.lib")	// 入力処理に必要
#pragma comment(lib, "xinput.lib")	// ジョイパッド処理に必要

// 画面の種類
typedef enum
{
	MODE_TITLE = 0,	// タイトル画面
	MODE_TUTORIAL,	// チュートリアル画面
	MODE_GAME,		// ゲーム画面
	MODE_RESULT,	// リザルト画面
	MODE_RANKING,	// ランキング画面
	MODE_MAX
}MODE;

// マクロ定義
#define SCREEN_WIDTH	(1280)	// ウィンドウの幅
#define SCREEN_HEIGHT	(720)	// ウィンドウの高さ
						//  頂点座標2D		頂点カラー			テクスチャ座標
#define FVF_VERTEX_2D	(D3DFVF_XYZRHW	|	D3DFVF_DIFFUSE		| D3DFVF_TEX1)	// 頂点フォーマット[2D]
						//  頂点座標3D		法線					頂点カラー			テクスチャ座標
#define FVF_VERTEX_3D	(D3DFVF_XYZ		|	D3DFVF_NORMAL		| D3DFVF_DIFFUSE	|	D3DFVF_TEX1)	// 頂点フォーマット[3D]
#define RESET_DATA		(0)		// 初期化
#define XOR_ONE			(1)		// 排他的論理和
#define FOV_DEFAULT		(45.0f)	// 基本の視野角

// 頂点情報[2D]の構造体を定義
typedef struct
{
	D3DXVECTOR3 pos;	// 頂点座標(x, y, z)
	float rhw;			// 座標変換用係数(1.0fで固定)
	D3DCOLOR col;		// 頂点カラー(r, g, b, a)
	D3DXVECTOR2 tex;	// テクスチャ座標(x, y)
}VERTEX_2D;

// 頂点情報[3D]の構造体を定義
typedef struct
{
	D3DXVECTOR3 pos;	// 頂点座標(x, y, z)
	D3DXVECTOR3 nor;	// 法線ベクトル
	D3DCOLOR col;		// 頂点カラー(r, g, b, a)
	D3DXVECTOR2 tex;	// テクスチャ座標(x, y)
}VERTEX_3D;

// プロトタイプ宣言
LPDIRECT3DDEVICE9 GetDevice(void);
void SetMode(MODE mode);
MODE GetMode();
int GetFPS(void);
#endif