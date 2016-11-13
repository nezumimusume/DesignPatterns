// PackmanConsol.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "Player.h"
#include "Enemy.h"



//�}�b�v�̒�`�B1�͕ǁB0�͉a�B
int g_map[MAP_HEIGHT][MAP_WIDTH] = {
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1 },
	{ 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1 },
	{ 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 2, 0, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1 },
	{ 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1 },
	{ 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1 },
	{ 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1 },
	{ 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1 },
	{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
};

//�t���[���o�b�t�@�E�E�E�̂悤�Ȃ��́B
char sFrameBuffer[MAP_HEIGHT][MAP_WIDTH];

CPathFinding g_pathFinding;

void EndFrame()
{
	Sleep(100);	//100�~���b����B
	HANDLE hCons = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos;
	pos.X = 0;
	pos.Y = 0;

	SetConsoleCursorPosition(hCons, pos);
}



/*!
*@brief	�H�ו��N���X�B
*/
class CFood {
private:
	CVector2 m_position;	//!<���W�B
	bool m_isDead;			//!<���S�t���O�B
public:
	/*!
	*@brief	�R���X�g���N�^�B
	*/
	CFood() :
		m_isDead(false)
	{
		m_position.Set(0, 0);
	}
	/*!
	*@brief	�ǂ̕`�揈���B
	*/
	void Draw()
	{
		if (!m_isDead) {
			//�t���[���o�b�t�@�Ƀh���[�B
			sFrameBuffer[m_position.GetY()][m_position.GetX()] = '.';
		}
	}
	/*!
	*@brief	���W��ݒ�B
	*/
	void SetPosition(int x, int y)
	{
		m_position.Set(x, y);
	}
	/*!
	*@brief	�X�V�����B
	*/
	void Update()
	{
		//�v���C���[�ƐG�ꂽ��H�ו���������悤�ɂ��Ă݂悤�E�E�E
		//�v���C���[��g_player�ŃA�N�Z�X�ł���B
		//�v���C���[��CFood�̍��W�������ɂȂ������ɁA������悤�ɂ���΂����͂��B
		//�v���C���[�̍��W��g_player.GetPositionX()��g_player.GetPositionY()�Ŏ擾�ł���B
		//�Ⴆ�΃v���C���[��X���W���~�����Ȃ炱��Ȋ����B
		//int pos_x = g_player.GetPositionX();
		if (m_position.GetX() == g_player.GetPositionX()
			&& m_position.GetY() == g_player.GetPositionY()
			) {
			m_isDead = true;
		}
	}
};
/*!
*@brief	�ǃN���X�B
*/
class CWall {
private:
	CVector2 m_position;		//!<���W
public:
	/*!
	*@brief	�R���X�g���N�^�B
	*/
	CWall()
	{
		m_position.Set(0, 0);
	}
	/*!
	*@brief	�ǂ̕`�揈���B
	*/
	void Draw()
	{
		//�t���[���o�b�t�@�Ƀh���[�B
		sFrameBuffer[m_position.GetY()][m_position.GetX()] = 'W';
	}
	/*!
	*@brief	���W��ݒ�B
	*/
	void SetPosition(int x, int y)
	{
		m_position.Set(x, y);
	}
};

/*!
 *@brief	���C���֐��B
 */
int main()
{
	//�v���C���[�̏����ʒu������B
	g_player.SetPosition(7, 7);
	//��
	int numWall = 0;	//�ǂ̐��B
	CWall walls[MAP_HEIGHT*MAP_WIDTH];
	//�H�ו�
	int numFood = 0;
	CFood foods[MAP_HEIGHT*MAP_WIDTH];
	//�G�B
	int numEnemy = 0;
	Enemy enemy[MAP_HEIGHT*MAP_WIDTH];
	g_pathFinding.BuildNodes();
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (g_map[i][j] == 1) {
				//�ǁB
				walls[numWall].SetPosition(j, i);
				//�ǂ��������̂ŃJ�E���g�A�b�v�B
				numWall++;
			}
			else if (g_map[i][j] == 2) {
				//�G�B
				enemy[numEnemy].SetPosition(j, i);
				numEnemy++;
			}
			if (g_map[i][j] != 1) {
				//�H�ו�
				foods[numFood].SetPosition(j, i);
				//�H�ו����������̂ŃJ�E���g�A�b�v�B
				numFood++;
			}
		}
	}
	//�Q�[�����[�v�B
	while (true) {
		//�t���[���o�b�t�@���N���A�B
		memset(sFrameBuffer, 0, sizeof(sFrameBuffer));
		
		//�H�ו��̏����B
		for (int i = 0; i < numFood; i++) {
			foods[i].Update();
			foods[i].Draw();
		}
		//�ǂ̏����B
		for (int i = 0; i < numWall; i++) {
			walls[i].Draw();
		}
		//�G�̏����B
		for (int i = 0; i < numEnemy; i++) {
			enemy[i].Update();
			enemy[i].Draw();
		}
		//�v���C���[�̏����B
		g_player.Update();
		g_player.Draw();

		//�t���[���o�b�t�@�̓��e����ʂɕ\������B
		for (int i = 0; i < MAP_HEIGHT; i++) {
			for (int j = 0; j < MAP_WIDTH; j++) {
				std::cout << sFrameBuffer[i][j];
			}
			std::cout << "\n";
		}
		EndFrame();	//�t���[���̍Ō�ɌĂ�łˁB���܂��Ȃ��B
	}
    return 0;
}
