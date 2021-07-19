#include "DxLib.h"
#include<math.h>
#define BLOCK_SIZE	1000.0f

#define BLOCK_NUM_X	16
#define BLOCK_NUM_Z 16

#define CAMERA_Y	500.0f

#define MOVE_FRAME		30		// �ړ������Ɋ|����t���[����

//�}�b�v(	1:��		0:�� )
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

	//�ʒu�ƌ����Ɠ��͏�Ԃ̏�����
	x = 1;
	z = 1;
	direction = 0;

	//�J�����̍��W�ƌ����ƒ����_���Z�b�g����
	CamPos = VGet(x * BLOCK_SIZE, CAMERA_Y, z * BLOCK_SIZE);
	CamDir = VGet(1.0f, 0.0f, 0.0f);
	CamTarg = VAdd(CamPos, CamDir);
	SetCameraPositionAndTarget_UpVecY(CamPos, CamTarg);

	//��Ԃ̏�����
	State = 0;

	//�`���𗠔g�ʂɂ���
	SetDrawScreen(DX_SCREEN_BACK);

	//���C�����[�v
	//�G�X�P�[�v�L�[���������܂ł̃��[�v
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
	{
		//��ʂ��N���A
		ClearDrawScreen();

		//���݂̓��͂��擾����
		NowInput = GetJoypadInputState(DX_INPUT_KEY_PAD1);

		switch (State)
		{
		case 0:
		{
			//�オ�����ꂽ������Ă�������Ɉړ������ԂɈڍs����
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
					//�ړ���̃}�X������������ړ�
					State = 1;
					Count = 0;
				}
			}
			//���������ꂽ������Ă�������Ƌt�����Ɉړ�����
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
			//����������Ă���������Ă�����������ɂX�O�x�ύX����	
			if ((NowInput & PAD_INPUT_LEFT) != 0)
			{
				State = 3;
				Count = 0;
			}
			// �E��������Ă���������Ă���������E�ɂX�O�x�ύX����
			if ((NowInput & PAD_INPUT_RIGHT) != 0)
			{
				// ��Ԃ��E���񒆂ɂ���
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
			//�J�E���g���ړ����ԂɒB����������W���ړ����ē��͑҂���Ԃɖ߂�
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

			// �J�����̍��W���ړ��r���̍��W�ɂ���
			CamPos = VGet(x * BLOCK_SIZE, CAMERA_Y, z * BLOCK_SIZE);
			CamPos = VSub(CamPos, VScale(CamDir, BLOCK_SIZE * Count / MOVE_FRAME));
			CamTarg = VAdd(CamPos, CamDir);

			//Model�̍��W����
			MV1SetPosition(model, VGet(x * BLOCK_SIZE, 0.0, z * BLOCK_SIZE));

			// �J�E���g���ړ����ԂɒB����������W���ړ����ē��͑҂���Ԃɖ߂�
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

			// �����Ă�����������r���̕����ɂ���
			switch (direction)
			{
			case 0: f = 0.0f; break;		// �w�v���X����
			case 1: f = -DX_PI_F / 2.0f; break;		// �y�}�C�i�X����
			case 2: f = DX_PI_F; break;		// �w�}�C�i�X����
			case 3: f = DX_PI_F / 2.0f; break;		// �y�v���X����
			}
			f += DX_PI_F / 2.0f * Count / MOVE_FRAME;
			CamDir.x = cos(f);
			CamDir.z = sin(f);
			CamTarg = VAdd(CamPos, CamDir);

			// �J�E���g�����ڎ��ԂɒB�������������ύX���ē��͑҂���Ԃɖ߂�
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

			// �����Ă�����������r���̕����ɂ���
			switch (direction)
			{
			case 0: f = 0.0f; break;		// �w�v���X����
			case 1: f = -DX_PI_F / 2.0f; break;		// �y�}�C�i�X����
			case 2: f = DX_PI_F; break;		// �w�}�C�i�X����
			case 3: f = DX_PI_F / 2.0f; break;		// �y�v���X����
			}
			f -= DX_PI_F / 2.0f * Count / MOVE_FRAME;
			CamDir.x = cos(f);
			CamDir.z = sin(f);
			CamTarg = VAdd(CamPos, CamDir);

			// �J�E���g�����ڎ��ԂɒB�������������ύX���ē��͑҂���Ԃɖ߂�
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

		//�}�b�v��`�悷��
		for (i = 0; i < BLOCK_NUM_Z; i++)
		{
			for (j = 0; j < BLOCK_NUM_X; j++)
			{
				if (Map[i][j] == 0)continue;

				//�ǃ��f���̍��W��ύX����
				MV1SetPosition(KabeModel, VGet(j * BLOCK_SIZE, 0.0f, i * BLOCK_SIZE));

				//4���̕ǂ̏�Ԃ�`�悷��t���[���ԍ���ύX����	
				FrameNom = 0;
				if (Map[i][j + 1] == 0)FrameNom += 1;
				if (Map[i][j - 1] == 0)FrameNom += 2;
				if (Map[i + 1][j] == 0)FrameNom += 4;
				if (Map[i - 1][j] == 0)FrameNom += 8;


				//����o�����ԍ��̃t���[����`�悷��
				MV1DrawFrame(KabeModel, FrameNom);
				MV1DrawFrame(model, 0);
			}
		}

		//����ʂ̓��e��\��ʂɔ��f����
		ScreenFlip();
	}
	//DX���C�u�����̌�n��
	DxLib_End();

	//�\�t�g�̏I��
	return 0;

}