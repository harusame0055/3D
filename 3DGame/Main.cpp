#include "DxLib.h"
#include<math.h>
#define BLOCK_SIZE	1000.0f

#define BLOCK_NUM_X	16
#define BLOCK_NUM_Z 16

#define CAMERA_Y	500.0f

#define MOVE_FRAME		30		// 移動や旋回に掛けるフレーム数

//マップ(	1:道		0:壁 )
char Map[BLOCK_NUM_X][BLOCK_NUM_Z] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,1,1,1,1,0,1,1,1,0,1,1,0,1,1,0,
	0,0,0,1,0,0,1,0,1,0,0,1,0,0,1,0,
	0,1,1,1,1,1,1,0,1,0,0,1,1,1,1,0,
	0,1,0,1,0,0,0,0,1,0,0,1,0,0,0,0,
	0,0,0,0,0,0,0,0,1,0,0,1,1,1,0,0,
	0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,0,
	0,0,0,1,1,1,0,1,0,0,0,1,0,0,1,0,
	0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,
	0,0,0,1,0,0,0,1,0,0,0,1,0,0,1,0,
	0,0,0,1,1,1,1,1,0,0,0,1,0,0,1,0,
	0,0,0,0,0,1,0,0,0,0,1,1,1,1,1,0,
	0,1,1,1,0,1,0,0,0,0,1,0,1,0,1,0,
	0,1,0,1,1,1,0,0,0,1,1,0,1,0,0,0,
	0,1,0,1,0,0,0,1,1,1,0,0,1,1,1,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int KabeModel;
	int model;
	int x, z;
	int moveX, moveZ;
	int direction;
	int NowInput;
	int FrameNom;
	int State;
	int Count;
	int i, j;
	float f;
	VECTOR CamPos;
	VECTOR CamDir;
	VECTOR CamTarg;

	ChangeWindowMode(TRUE);

	if (DxLib_Init() < 0)	return	-1;

	KabeModel = MV1LoadModel("Kabe.mqo");
	model = MV1LoadModel("Player.mv1");

	//位置と向きと入力状態の初期化
	x = 1;
	z = 1;
	direction = 0;

	//カメラの座標と向きと注視点をセットする
	CamPos = VGet(x * BLOCK_SIZE, CAMERA_Y, z * BLOCK_SIZE);
	CamDir = VGet(1.0f, 0.0f, 0.0f);
	CamTarg = VAdd(CamPos, CamDir);
	SetCameraPositionAndTarget_UpVecY(CamPos, CamTarg);

	//状態の初期化
	State = 0;

	//描画先を裏波面にする
	SetDrawScreen(DX_SCREEN_BACK);

	//メインループ
	//エスケープキーが押されるまでのループ
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
	{
		//画面をクリア
		ClearDrawScreen();

		//現在の入力を取得する
		NowInput = GetJoypadInputState(DX_INPUT_KEY_PAD1);

		switch (State)
		{
		case 0:
		{
			//上が押されたら向いている方向に移動する状態に移行する
			if ((NowInput & PAD_INPUT_UP) != 0)
			{
				switch (direction)
				{
				case 0:moveX = 1; moveZ = 0; break;
				case 1:moveX = 0; moveZ = -1; break;
				case 2:moveX = -1; moveZ = 0; break;
				case 3:moveX = 0; moveZ = 1; break;
				}
				if (Map[z + moveZ][x + moveX] == 1)
				{
					//移動先のマスが道だったら移動
					State = 1;
					Count = 0;
				}
			}
			//下が押されたら向いている方向と逆方向に移動する
			if ((NowInput & PAD_INPUT_DOWN) != 0)
			{
				switch (direction)
				{
				case 0: moveX = -1; moveZ = 0; break;
				case 1:moveX = 0; moveZ = 1; break;
				case 2:moveX = -1; moveZ = 0; break;
				case 3:moveX = 0; moveZ = 1; break;
				}
				if (Map[z + moveZ][x + moveX] == 1)
				{
					State = 2;
					Count = 0;
				}
			}
			//左が押されていたら向いている方向を左に９０度変更する	
			if ((NowInput & PAD_INPUT_LEFT) != 0)
			{
				State = 3;
				Count = 0;
			}
			// 右が押されていたら向いている方向を右に９０度変更する
			if ((NowInput & PAD_INPUT_RIGHT) != 0)
			{
				// 状態を右旋回中にする
				State = 4;
				Count = 0;
			}
		}
		break;
		case 1:
		{
			Count++;
			CamPos = VGet(x * BLOCK_SIZE, CAMERA_Y, z * BLOCK_SIZE);
			CamPos = VAdd(CamPos, VScale(CamDir, BLOCK_SIZE * Count / MOVE_FRAME));
			CamTarg = VAdd(CamPos, CamDir);
			//カウントが移動時間に達したら実座標を移動して入力待ち状態に戻る
			if (Count == MOVE_FRAME)
			{
				x += moveX;
				z += moveZ;

				State = 0;
				Count = 0;
			}
		}
		break;
		case 2:
		{
			Count++;

			// カメラの座標を移動途中の座標にする
			CamPos = VGet(x * BLOCK_SIZE, CAMERA_Y, z * BLOCK_SIZE);
			CamPos = VSub(CamPos, VScale(CamDir, BLOCK_SIZE * Count / MOVE_FRAME));
			CamTarg = VAdd(CamPos, CamDir);

			//Modelの座標を代入
			MV1SetPosition(model, VGet(x * BLOCK_SIZE, 0.0, z * BLOCK_SIZE));

			// カウントが移動時間に達したら実座標を移動して入力待ち状態に戻る
			if (Count == MOVE_FRAME)
			{
				x += moveX;
				z += moveZ;

				State = 0;
				Count = 0;
			}

		}
		break;
		case 3:
		{
			Count++;

			// 向いている方向を旋回途中の方向にする
			switch (direction)
			{
			case 0: f = 0.0f; break;		// Ｘプラス方向
			case 1: f = -DX_PI_F / 2.0f; break;		// Ｚマイナス方向
			case 2: f = DX_PI_F; break;		// Ｘマイナス方向
			case 3: f = DX_PI_F / 2.0f; break;		// Ｚプラス方向
			}
			f += DX_PI_F / 2.0f * Count / MOVE_FRAME;
			CamDir.x = cos(f);
			CamDir.z = sin(f);
			CamTarg = VAdd(CamPos, CamDir);

			// カウントが推移時間に達したら実方向を変更して入力待ち状態に戻る
			if (Count == MOVE_FRAME)
			{
				if (direction == 0)
				{
					direction = 3;
				}
				else
				{
					direction--;
				}

				State = 0;
				Count = 0;
			}
		}
		break;
		case 4:
		{
			Count++;

			// 向いている方向を旋回途中の方向にする
			switch (direction)
			{
			case 0: f = 0.0f; break;		// Ｘプラス方向
			case 1: f = -DX_PI_F / 2.0f; break;		// Ｚマイナス方向
			case 2: f = DX_PI_F; break;		// Ｘマイナス方向
			case 3: f = DX_PI_F / 2.0f; break;		// Ｚプラス方向
			}
			f -= DX_PI_F / 2.0f * Count / MOVE_FRAME;
			CamDir.x = cos(f);
			CamDir.z = sin(f);
			CamTarg = VAdd(CamPos, CamDir);

			// カウントが推移時間に達したら実方向を変更して入力待ち状態に戻る
			if (Count == MOVE_FRAME)
			{
				if (direction == 3)
				{
					direction = 0;
				}
				else
				{
					direction++;
				}

				State = 0;
				Count = 0;
			}
		}
		break;
		}

		SetCameraPositionAndTarget_UpVecY(CamPos, CamTarg);

		//マップを描画する
		for (i = 0; i < BLOCK_NUM_Z; i++)
		{
			for (j = 0; j < BLOCK_NUM_X; j++)
			{
				if (Map[i][j] == 0)continue;

				//壁モデルの座標を変更する
				MV1SetPosition(KabeModel, VGet(j * BLOCK_SIZE, 0.0f, i * BLOCK_SIZE));

				//4方の壁の状態を描画するフレーム番号を変更する	
				FrameNom = 0;
				if (Map[i][j + 1] == 0)FrameNom += 1;
				if (Map[i][j - 1] == 0)FrameNom += 2;
				if (Map[i + 1][j] == 0)FrameNom += 4;
				if (Map[i - 1][j] == 0)FrameNom += 8;


				//割り出した番号のフレームを描画する
				MV1DrawFrame(KabeModel, FrameNom);
				MV1DrawFrame(model, 0);
			}
		}

		//裏画面の内容を表画面に反映する
		ScreenFlip();
	}
	//DXライブラリの後始末
	DxLib_End();

	//ソフトの終了
	return 0;

}