//======================================================================================
// 
// 3Dアクションゲーム_MASTER KEY[input.cpp]
//
//======================================================================================
#include "main.h"
#include "input.h"

// マクロ定義
#define NUM_KEY_MAX			(256)	// キーの最大数
#define REPEAT_INTERVAL		(20)	// リピートに入るまでのの間隔
#define TRIGGER_INTERVAL	(-15)	// トリガーからの間隔

// グローバル変数
LPDIRECTINPUT8			g_pInput = NULL;		// DirectInputオブジェクトへのポインタ
LPDIRECTINPUTDEVICE8	g_pDevKeyboard = NULL;	// 入力デバイス(キーボード)へのポインタ
BYTE g_aKeyState[NUM_KEY_MAX];					// キーボードのプレス情報
BYTE g_aKeyStateTrigger[NUM_KEY_MAX];			// キーボードのトリガー情報
BYTE g_aKeyStateRelease[NUM_KEY_MAX];			// キーボードのリリース情報
BYTE g_aKeyStateRepeat[NUM_KEY_MAX];			// キーボードのリピート情報
XINPUT_STATE	g_joyKeyState;					// ジョイパッドのプレス情報
XINPUT_STATE	g_joyKeyStateOld;				// 1フレーム前のジョイパッドのプレス情報
XINPUT_STATE	g_joyKeyStateTrigger;			// ジョイパッドのトリガー情報
XINPUT_STATE	g_joyKeyStateRelease;			// ジョイパッドのリリース情報
XINPUT_STATE	g_joyKeyStateRepeat;			// ジョイパッドのリピート情報
XINPUT_STATE	g_joyKeyStateLstickRepeat;		// ジョイパッドのリピート情報[LSTICK]
XINPUT_VIBRATION	g_joyKeyVibration;			// ジョイパッドのバイブレーション情報
int g_nVibrationTime = RESET_DATA;				// バイブレーションの継続時間を計測
int g_aCountRepeat[NUM_KEY_MAX];				// キーボードのリピート情報を計測

//======================================================================================
// キーボードの初期化処理
//======================================================================================
HRESULT InitKeyboard(HINSTANCE hInstance, HWND hWnd)
{
	// DirectInputオブジェクトの生成
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_pInput, NULL)))
	{
		return E_FAIL;
	}
	// 入力デバイス(キーボード)の生成
	if (FAILED(g_pInput->CreateDevice(GUID_SysKeyboard, &g_pDevKeyboard, NULL)))
	{
		return E_FAIL;
	}
	// データフォーマットを設定
	if (FAILED(g_pDevKeyboard->SetDataFormat(&c_dfDIKeyboard)))
	{
		return E_FAIL;
	}
	// 協調モードを設定
	if (FAILED(g_pDevKeyboard->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		return E_FAIL;
	}

	// キーボードへのアクセス権を取得
	g_pDevKeyboard->Acquire();

	// 計測時間の初期化
	for (int nCntKey = RESET_DATA; nCntKey < NUM_KEY_MAX; nCntKey++)
	{
		g_aCountRepeat[nCntKey] = RESET_DATA;
	}

	return S_OK;
}

//======================================================================================
// キーボードの終了処理
//======================================================================================
void UninitKeyboard(void)
{
	// 入力デバイス(キーボード)の破棄
	if (g_pDevKeyboard != NULL)
	{
		g_pDevKeyboard->Unacquire();	// キーボードのアクセス権を放棄
		g_pDevKeyboard->Release();
		g_pDevKeyboard = NULL;
	}
	// DirectInputオブジェクトの破棄
	if (g_pInput != NULL)
	{
		g_pInput->Release();
		g_pInput = NULL;
	}
}

//======================================================================================
// キーボードの更新処理
//======================================================================================
void UpdateKeyboard(void)
{
	BYTE aKeyState[NUM_KEY_MAX];	// キーボードの入力情報

	// 入力デバイスからデータを取得
	if (SUCCEEDED(g_pDevKeyboard->GetDeviceState(sizeof(aKeyState), &aKeyState[0])))
	{
		for (int nCntKey = 0; nCntKey < NUM_KEY_MAX; nCntKey++)
		{
			g_aKeyStateTrigger[nCntKey] = (~g_aKeyState[nCntKey]) & (aKeyState[nCntKey]);
			g_aKeyStateRelease[nCntKey] = (g_aKeyState[nCntKey] | aKeyState[nCntKey]) ^ aKeyState[nCntKey];
			if (GetKeyboardTrigger(nCntKey) == true)
			{ // トリガーからリピートに入るまで若干の余裕を持たせる
				g_aCountRepeat[nCntKey] = TRIGGER_INTERVAL;
			}
			if (GetKeyboardPress(nCntKey) == true)
			{ // キーが押されている場合、時間を計測
				g_aCountRepeat[nCntKey]++;
				
				if (g_aCountRepeat[nCntKey] >= REPEAT_INTERVAL)
				{ // 一定時間経過するとリピート処理に移行
					g_aKeyStateRepeat[nCntKey] = aKeyState[nCntKey];
					g_aCountRepeat[nCntKey] = RESET_DATA;
				}
				else
				{ // なにも情報を渡さない
					g_aKeyStateRepeat[nCntKey] = NULL;
				}
				
			}
			if (GetKeyboardRelease(nCntKey) == true)	// キーが話された場合カウンターを初期化
			{
				g_aCountRepeat[nCntKey] = RESET_DATA;
			}
			g_aKeyState[nCntKey] = aKeyState[nCntKey];	// キーボードのプレス情報を保存
		}
	}
	else
	{
		g_pDevKeyboard->Acquire();
	}
}

//======================================================================================
// キーボードのプレス情報を取得
//======================================================================================
bool GetKeyboardPress(int nKey)
{
	return (g_aKeyState[nKey] & 0x80) ? true : false;	// 3項演算子
}

//======================================================================================
// キーボードのトリガー情報を取得
//======================================================================================
bool GetKeyboardTrigger(int nKey)
{
	return (g_aKeyStateTrigger[nKey] & 0x80) ? true : false;
}

//======================================================================================
// キーボードのリリース情報を取得
//======================================================================================
bool GetKeyboardRelease(int nKey)
{
	return (g_aKeyStateRelease[nKey] & 0x80) ? true : false;
}

//======================================================================================
// キーボードのリピート情報を取得
//======================================================================================
bool GetKeyboardRepeat(int nKey)
{
	return (g_aKeyStateRepeat[nKey] & 0x80) ? true : false;
}

//======================================================================================
// ジョイパッドの初期化処理
//======================================================================================
HRESULT InitJoypad(void)
{
	// メモリのクリア
	memset(&g_joyKeyState, 0, sizeof(XINPUT_STATE));
	memset(&g_joyKeyStateOld, 0, sizeof(XINPUT_STATE));
	memset(&g_joyKeyStateTrigger, 0, sizeof(XINPUT_STATE));
	memset(&g_joyKeyStateRelease, 0, sizeof(XINPUT_STATE));
	memset(&g_joyKeyStateRepeat, 0, sizeof(XINPUT_STATE));
	memset(&g_joyKeyStateLstickRepeat, 0, sizeof(XINPUT_STATE));
	ZeroMemory(&g_joyKeyVibration, sizeof(XINPUT_VIBRATION));
	// XInputのステートを設定(有効にする)
	XInputEnable(true);

	// バイブレーションの継続時間の初期化
	g_nVibrationTime = RESET_DATA;

	return S_OK;
}

//======================================================================================
// ジョイパッドの終了処理
//======================================================================================
void UninitJoypad(void)
{
	// XInputのステートを設定(無効にする)
	XInputEnable(false);
}

//======================================================================================
// ジョイパッドの更新処理
//======================================================================================
void UpdateJoypad(void)
{
	XINPUT_STATE joyKeyState;	// ジョイパッドの入力情報
	static int nCntPressButton = RESET_DATA;	// リピート情報に入るための計測

	g_joyKeyStateOld = g_joyKeyState;	// 過去の入力情報を保存

	// ジョイパッドの状態を取得
	if (XInputGetState(0, &joyKeyState) == ERROR_SUCCESS)
	{
		g_joyKeyStateTrigger.Gamepad.wButtons = (~g_joyKeyState.Gamepad.wButtons & joyKeyState.Gamepad.wButtons);										// ジョイパッドのトリガー情報
		g_joyKeyStateRelease.Gamepad.wButtons = (g_joyKeyState.Gamepad.wButtons | joyKeyState.Gamepad.wButtons) ^ joyKeyState.Gamepad.wButtons;			// ジョイパッドのリリース情報
		g_joyKeyStateLstickRepeat.Gamepad.sThumbLX = (g_joyKeyState.Gamepad.sThumbLX | joyKeyState.Gamepad.sThumbLX) ^ joyKeyState.Gamepad.sThumbLX;	// ジョイパッドのリリース情報[Lstick_X]
		
		if ((joyKeyState.Gamepad.wButtons == g_joyKeyStateTrigger.Gamepad.wButtons) && (joyKeyState.Gamepad.wButtons != g_joyKeyState.Gamepad.wButtons))
		{ // トリガーからリピートに入るまで若干の余裕を持たせる
			nCntPressButton = TRIGGER_INTERVAL;
		}
		if (joyKeyState.Gamepad.wButtons == g_joyKeyState.Gamepad.wButtons)
		{ // キーが押されている場合、時間を計測(今回の入力情報と前回の入力情報が同じ場合、ここに入る)
			nCntPressButton++;
			if (nCntPressButton >= REPEAT_INTERVAL)
			{ // 一定時間経過するとリピート処理に移行
				g_joyKeyStateRepeat = joyKeyState;
				nCntPressButton = RESET_DATA;
			}
			else
			{ // なにも情報を渡さない
				g_joyKeyStateRepeat.Gamepad.wButtons = RESET_DATA;
			}
		}
		if ((joyKeyState.Gamepad.wButtons != g_joyKeyStateTrigger.Gamepad.wButtons) && (joyKeyState.Gamepad.wButtons != g_joyKeyState.Gamepad.wButtons))
		{ // ボタンが離された場合、カウンターを初期化する
			nCntPressButton = RESET_DATA;
		}
		g_joyKeyState = joyKeyState;	// ジョイパッドのプレス状態を保存
	}
	g_nVibrationTime--;
	if (g_nVibrationTime <= RESET_DATA)
	{
		StopVibration();
	}
}

//======================================================================================
// ジョイパッドのプレス情報を取得
//======================================================================================
bool GetJoypadPress(JOYKEY key)
{
	return (g_joyKeyState.Gamepad.wButtons & (0x01 << key)) ? true : false;
}

//======================================================================================
// ジョイパッドのトリガー情報を取得
//======================================================================================
bool GetJoypadTrigger(JOYKEY key)
{
	return (g_joyKeyStateTrigger.Gamepad.wButtons & (0x01 << key)) ? true : false;
}

//======================================================================================
// ジョイパッドのリリース情報を取得
//======================================================================================
bool GetJoypadRelease(JOYKEY key)
{
	return (g_joyKeyStateRelease.Gamepad.wButtons & (0x01 << key)) ? true : false;
}

//======================================================================================
// ジョイパッドのリピート情報を取得
//======================================================================================
bool GetJoypadRepeat(JOYKEY key)
{
	return (g_joyKeyStateRepeat.Gamepad.wButtons & (0x01 << key)) ? true : false;
}

//======================================================================================
// ジョイパッドの左スティックの情報を取得
//======================================================================================
bool GetJoypadLeftStick(JOYKEY key)
{
	if (key == JOYKEY_LSTICK_UP)	// 上
	{
		return (g_joyKeyState.Gamepad.sThumbLY >= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ? true : false;
	}
	else if (key == JOYKEY_LSTICK_DOWN)	// 下
	{
		return (g_joyKeyState.Gamepad.sThumbLY <= -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ? true : false;
	}
	if (key == JOYKEY_LSTICK_LEFT)	// 左
	{
		return (g_joyKeyState.Gamepad.sThumbLX <= -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ? true : false;
	}
	else if (key == JOYKEY_LSTICK_RIGHT)	// 右
	{
		return (g_joyKeyState.Gamepad.sThumbLX >= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ? true : false;
	}
	else
	{
		return false;
	}
}

//======================================================================================
// ジョイパッドの左スティックの情報を数値ごと取得
//======================================================================================
bool GetJoypadLeftStickValue(int* pValueH, int* pValueV)
{
	if (pValueH != NULL)
	{ // 水平方向の設定
		*pValueH = g_joyKeyState.Gamepad.sThumbLX;
	}
	if (pValueV != NULL)
	{ // 水平方向の設定
		*pValueV = g_joyKeyState.Gamepad.sThumbLY;
	}
	if (g_joyKeyState.Gamepad.sThumbLX >= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||		// 水平方向のデッドゾーンを越えた
		g_joyKeyState.Gamepad.sThumbLX <= -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
		g_joyKeyState.Gamepad.sThumbLY >= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||		// 垂直方向のデッドゾーンを越えた
		g_joyKeyState.Gamepad.sThumbLY <= -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//======================================================================================
// ジョイパッドの右スティックの情報を取得
//======================================================================================
bool GetJoypadRightStick(JOYKEY key)
{
	if (key == JOYKEY_RSTICK_UP)	// 上
	{
		return (g_joyKeyState.Gamepad.sThumbRY >= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) ? true : false;
	}
	else if (key == JOYKEY_RSTICK_DOWN)	// 下
	{
		return (g_joyKeyState.Gamepad.sThumbRY <= -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) ? true : false;
	}
	if (key == JOYKEY_RSTICK_LEFT)	// 左
	{
		return (g_joyKeyState.Gamepad.sThumbRX <= -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) ? true : false;
	}
	else if (key == JOYKEY_RSTICK_RIGHT)	// 右
	{
		return (g_joyKeyState.Gamepad.sThumbRX >= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) ? true : false;
	}
	else
	{
		return false;
	}
}

//======================================================================================
// ジョイパッドの左スティックのトリガー情報を取得
//======================================================================================
bool GetJoypadLeftStickTrigger(JOYKEY key)
{
	// 前回の入力情報を保存
	bool bLStickLeft = (g_joyKeyStateOld.Gamepad.sThumbLX <= -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ? true : false;
	bool bLStickRight = (g_joyKeyStateOld.Gamepad.sThumbLX >= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ? true : false;
	bool bLStickUp = (g_joyKeyStateOld.Gamepad.sThumbLY >= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ? true : false;
	bool bLStickDown		= (g_joyKeyStateOld.Gamepad.sThumbLY <= -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ? true : false;

	if (bLStickLeft == false && (key == JOYKEY_LSTICK_LEFT))
	{ // 前回入力されていない && 左が入力された && deadzoneを越えている
		return (g_joyKeyState.Gamepad.sThumbLX <= -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ? true : false;
	}
	if (bLStickRight == false && (key == JOYKEY_LSTICK_RIGHT))
	{ // 前回入力されていない && 右が入力された && deadzoneを越えている
		return (g_joyKeyState.Gamepad.sThumbLX >= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ? true : false;
	}
	if (bLStickUp == false && (key == JOYKEY_LSTICK_UP))
	{ // 前回入力されていない && 上が入力された && deadzoneを越えている
		return (g_joyKeyState.Gamepad.sThumbLY >= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ? true : false;
	}
	if (bLStickDown == false && (key == JOYKEY_LSTICK_DOWN))
	{ // 前回入力されていない && 下が入力された && deadzoneを越えている
		return (g_joyKeyState.Gamepad.sThumbLY <= -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ? true : false;
	}
	return false;
}

//======================================================================================
// ジョイパッドの右スティックのトリガー情報を取得
//======================================================================================
bool GetJoypadRightStickTrigger(JOYKEY key)
{
	// 前回の入力情報を保存
	bool bRStickLeft = (g_joyKeyStateOld.Gamepad.sThumbRX <= -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) ? true : false;
	bool bRStickRight = (g_joyKeyStateOld.Gamepad.sThumbRX >= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) ? true : false;
	bool bRStickUp = (g_joyKeyStateOld.Gamepad.sThumbRY >= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) ? true : false;
	bool bRStickDown = (g_joyKeyStateOld.Gamepad.sThumbRY <= -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) ? true : false;

	if (bRStickLeft == false && (key == JOYKEY_RSTICK_LEFT))
	{ // 前回入力されていない && 左が入力された && deadzoneを越えている
		return (g_joyKeyState.Gamepad.sThumbRX <= -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) ? true : false;
	}
	if (bRStickRight == false && (key == JOYKEY_RSTICK_RIGHT))
	{ // 前回入力されていない && 右が入力された && deadzoneを越えている
		return (g_joyKeyState.Gamepad.sThumbRX >= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) ? true : false;
	}
	if (bRStickUp == false && (key == JOYKEY_RSTICK_UP))
	{ // 前回入力されていない && 上が入力された && deadzoneを越えている
		return (g_joyKeyState.Gamepad.sThumbRY >= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) ? true : false;
	}
	if (bRStickDown == false && (key == JOYKEY_RSTICK_DOWN))
	{ // 前回入力されていない && 下が入力された && deadzoneを越えている
		return (g_joyKeyState.Gamepad.sThumbRY <= -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) ? true : false;
	}
	return false;

}

//======================================================================================
// ジョイパッドの左スティックのリリース情報を取得
//======================================================================================
bool GetJoypadLeftStickRelease(JOYKEY key)
{
	return false;
}

//======================================================================================
// ジョイパッドのバイブレーションを設定
//======================================================================================
void SetVibration(int left, int right, int time)
{
	g_joyKeyVibration.wLeftMotorSpeed = left;
	g_joyKeyVibration.wRightMotorSpeed = right;
	g_nVibrationTime = time;
	XInputSetState(RESET_DATA, &g_joyKeyVibration);
}

//======================================================================================
// ジョイパッドのバイブレーションを停止
//======================================================================================
void StopVibration(void)
{
	g_joyKeyVibration.wLeftMotorSpeed = RESET_DATA;
	g_joyKeyVibration.wRightMotorSpeed = RESET_DATA;
	g_nVibrationTime = RESET_DATA;
	XInputSetState(RESET_DATA, &g_joyKeyVibration);
}

//===========================================================================================
// ジョイパッドの情報を取得
//===========================================================================================
XINPUT_STATE GetJoypadState(void)
{
	return g_joyKeyState;
}