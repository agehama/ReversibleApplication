#pragma once
#include <array>
#include <Siv3D.hpp>

const int WWnd = 640;
const int HWnd = 480;

const int HBlockNum = 10;	//横方向のブロックの個数
const int VBlockNum = 14;	//縦方向のブロックの個数
const int BlockSize = 32;
const int FrameSize = BlockSize;//フレーム（壁）も当たり判定の時ブロックとして扱うのでフレームサイズは常にブロックと同じ

const int GroundHNum = HBlockNum+2;
const int GroundVNum = VBlockNum+6;
const int GroundNum = GroundHNum*GroundVNum;

//座標は(0,0)から始まるので、一番右上のブロックは(0,0)
//(GroundHNum - HBlockNum)：フィールド外のブロックの数（水平方向）
const int GroundFXS = (GroundHNum - HBlockNum)/2;
//const int GroundFYS = 0;
const int GroundFXE = GroundHNum-1 - (GroundHNum - HBlockNum)/2;
const int GroundFYE = GroundVNum-1 - 1;

const int BlockX = GroundHNum/2;		//テトリミノの初期x座標
const int BlockY = 2;			//テトリミノの初期y座標
const int TBlockNum = 4;		//テトリミノが持つブロックの個数
const int TetriKind = 7;			//テトリミノの種類

const int DropVelocity = 10;//DropVelocityフレームごとに１マス落ちる

const int KeyNum = 10;

const Color BlockColor[7] = 
{
	Color(0x00, 0xff, 0xff),		//  水
	Color(0xff, 0xff, 0x00),		//  黄
	Color(0x00, 0xff, 0x00),	//  緑
	Color(0xff, 0x00, 0x00),	//  赤
	Color(0x00, 0x00, 0xff),	//  青
	Color(0xeb, 0xb6, 0x00),	//  茶
	Color(0x9b, 0x01, 0xeb)	//  紫
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

//テトリミノクラスは最初に始まる時に一度宣言するだけ
//接地するたびにcountをインクリメントし、フラグを倒す。オブジェクトの破棄はしない。
class Tetrimino
{
	bool debag;
	//block block[TBlockNum];	//ブロック構造体４つからなる
	std::array<block, TBlockNum> block;
	Ground grd;
	bool flag;			//0:なし、接地　1:存在
	int count;			//現在のレベルが始まってから何個目のテトリミノか
	//int kind[TetriKind];		//テトリミノの発生順を決めるランダム配列
	std::array<int, TetriKind> kind;
	//bool local[5][5];//ローカル座標配列	(BlockX, BlockY)がlocal[0][0]に対応
	std::array<std::array<char, 5>, 5> local;
	int x,y;				//テトリミノの中心座標
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
	bool RoopIni();		//ループごとの初期化処理　毎ループ実行する　falseが返ったら終了処理
	void HardDrop();	//一気に落下させる
	void SoftDrop();	//１マス下へ
	void Move(bool xplus);	//x軸方向の移動か
	void Spin(bool clockwise);//時計回りか
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
	int graphflag;	//何を表示するかの分岐に使う
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