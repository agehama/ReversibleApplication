#pragma once
#include <array>
#include <Siv3D.hpp>

const int WWnd = 640;
const int HWnd = 480;

const int HBlockNum = 10;	//�������̃u���b�N�̌�
const int VBlockNum = 14;	//�c�����̃u���b�N�̌�
const int BlockSize = 32;
const int FrameSize = BlockSize;//�t���[���i�ǁj�������蔻��̎��u���b�N�Ƃ��Ĉ����̂Ńt���[���T�C�Y�͏�Ƀu���b�N�Ɠ���

const int GroundHNum = HBlockNum+2;
const int GroundVNum = VBlockNum+6;
const int GroundNum = GroundHNum*GroundVNum;

//���W��(0,0)����n�܂�̂ŁA��ԉE��̃u���b�N��(0,0)
//(GroundHNum - HBlockNum)�F�t�B�[���h�O�̃u���b�N�̐��i���������j
const int GroundFXS = (GroundHNum - HBlockNum)/2;
//const int GroundFYS = 0;
const int GroundFXE = GroundHNum-1 - (GroundHNum - HBlockNum)/2;
const int GroundFYE = GroundVNum-1 - 1;

const int BlockX = GroundHNum/2;		//�e�g���~�m�̏���x���W
const int BlockY = 2;			//�e�g���~�m�̏���y���W
const int TBlockNum = 4;		//�e�g���~�m�����u���b�N�̌�
const int TetriKind = 7;			//�e�g���~�m�̎��

const int DropVelocity = 10;//DropVelocity�t���[�����ƂɂP�}�X������

const int KeyNum = 10;

const Color BlockColor[7] = 
{
	Color(0x00, 0xff, 0xff),		//  ��
	Color(0xff, 0xff, 0x00),		//  ��
	Color(0x00, 0xff, 0x00),	//  ��
	Color(0xff, 0x00, 0x00),	//  ��
	Color(0x00, 0x00, 0xff),	//  ��
	Color(0xeb, 0xb6, 0x00),	//  ��
	Color(0x9b, 0x01, 0xeb)	//  ��
};

const Color FrameColor(0x2f, 0xa5, 0xba);
const Color BoardColor(0x1c, 0xd4, 0xf5);;

struct block;
class Tetrimino;
class Ground;

struct block
{
	Rect rect;
	bool flag;
	int col;

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(rect, flag, col);
	}
};

class Ground
{
	void GraphCoordSet();
public:
	//block b[GroundVNum][GroundHNum];
	std::array<std::array<block, GroundHNum>, GroundVNum> b;
	Ground();
	void Graph(Point pos);
	void Debag();
	inline bool GetBlockExist(int x, int y);

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(b);
	}
};

//�e�g���~�m�N���X�͍ŏ��Ɏn�܂鎞�Ɉ�x�錾���邾��
//�ڒn���邽�т�count���C���N�������g���A�t���O��|���B�I�u�W�F�N�g�̔j���͂��Ȃ��B
class Tetrimino
{
	bool debag;
	//block block[TBlockNum];	//�u���b�N�\���̂S����Ȃ�
	std::array<block, TBlockNum> block;
	Ground grd;
	bool flag;			//0:�Ȃ��A�ڒn�@1:����
	int count;			//���݂̃��x�����n�܂��Ă��牽�ڂ̃e�g���~�m��
	//int kind[TetriKind];		//�e�g���~�m�̔����������߂郉���_���z��
	std::array<int, TetriKind> kind;
	//bool local[5][5];//���[�J�����W�z��	(BlockX, BlockY)��local[0][0]�ɑΉ�
	std::array<std::array<char, 5>, 5> local;
	int x,y;				//�e�g���~�m�̒��S���W
	void TetriIni();
	void BlockTypeInit(int k);
	void LocalToGrobal();
	//int MoveJudge(int mx, int my, bool mlocal[5][5], int i=0);
	int MoveJudge(int mx, int my, const std::array<std::array<char, 5>, 5>& mlocal, int i = 0);
	inline int GetNowKind(){ return kind[count%TetriKind]; }
	inline bool GetBlockExist(int tx, int ty){ return local[tx-GroundFXS-x][ty-y]; }
	void DeleteLine(int y);
	void PushLine(int k, int y[]);
	//int GetProjection(int tx[4], int ty[4]);
	int GetProjection(const std::array<int,4>& tx, const std::array<int, 4>& ty);
	int GetTop();
public:
	Tetrimino();
	bool RoopIni();		//���[�v���Ƃ̏����������@�����[�v���s����@false���Ԃ�����I������
	void HardDrop();	//��C�ɗ���������
	void SoftDrop();	//�P�}�X����
	void Move(bool xplus);	//x�������̈ړ���
	void Spin(bool clockwise);//���v��肩
	void Earth();
	void Graph(Point pos);

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(debag, block, grd, flag, count, kind, local, x, y);
	}
};

class Tetris
{
	Rect LeftFrame;
	Rect RightFrame;
	Rect UnderFrame;
	std::shared_ptr<Tetrimino> tetri;
	int graphflag;	//����\�����邩�̕���Ɏg��
	int count;
	Optional<String> m_newAction;
	bool gameover;
	void Option();
	void RoopIni();
	void KeyProcess();
public:
	Tetris();

	void update();
	void draw();

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(LeftFrame, RightFrame, UnderFrame, tetri, graphflag, count, gameover);
	}

	Optional<String> popAction()
	{
		const Optional<String> result = m_newAction;
		m_newAction = none;
		return result;
	}
};