#include <Siv3D.hpp>
#include "ReversibleTetris.hpp"
#include "../../ReversibleApplication.hpp"

Tetrimino::Tetrimino()
{
	debag=false;
	//TBlockNum�F�e�g���~�m�Ɋ܂܂��u���b�N�̐�
	for(int i=0; i<TBlockNum; ++i)
	{
		block[i].rect.w=BlockSize;
		block[i].rect.h=BlockSize;
	}
	flag=true;
	count=0;
	for(int i=0; i<TetriKind; ++i)
	{
		kind[i] = i;
	}
	TetriIni();
}

bool Tetrimino::RoopIni()
{
	if(GetTop()<=4)
	{
		return false;
	}
	else if(!flag)
	{
		TetriIni();
	}
	return true;
}

//�e�g���~�m���ڒn���邽�тɎ��s���鏉�����֐�
//�u���b�N�̎�ނ̌���⏉�����W�̐ݒ���s��
void Tetrimino::TetriIni()
{
	flag=true;
	x=BlockX;
	y=BlockY;
	int k = ++count % TetriKind;
	if(k==1)
	{
		//�e�g���~�m�̔������������_���Ƀ\�[�g����B10�͓K���B7�ȏ�Ȃ牽�ł������Ǝv��
		for(int j=0; j<10; ++j)
		{
			int ta = Random(0, 6);
			int tb = Random(0, 6);
			//int ta = blockkind(rng);
			//int tb = blockkind(rng);
			int tx = kind[ta];
			kind[ta] = kind[tb];
			kind[tb] = tx;
		}
	}
	
	BlockTypeInit(kind[k]);
	Tetrimino::LocalToGrobal();
	/*
	��]���͏�ɓ�ڂ̃u���b�N
	�����ɏ����z�u�A�e�g���~�m�̎�ނȂǂ����߂鏈���������B
	*/
}

//�e�g���~�m�̎�ނ̎��ʔԍ��������ɁA���[�J�����W�̏����l��ݒ肷��֐�
//������e�g���~�m���ڒn�������̂ݍs��
void Tetrimino::BlockTypeInit(int kind)
{
	for(int i=0; i<5; ++i)
	{
		for(int j=0; j<5; ++j)
		{
			local[i][j]=false;
		}
	}
	switch(kind)
	{//���[�J�����W(2,2)����]��
	case 0: local[2][0]=true; local[2][1]=true; local[2][2]=true; local[2][3]=true; break;   //I�^
	case 1:	local[2][2]=true; local[2][3]=true; local[3][2]=true; local[3][3]=true; break;  //O�^
	case 2:	local[2][1]=true; local[2][2]=true; local[3][2]=true; local[3][3]=true; break;   //S�^
	case 3:	local[2][1]=true; local[2][2]=true; local[1][2]=true; local[1][3]=true; break;   //Z�^
	case 4:	local[2][1]=true; local[2][2]=true; local[2][3]=true; local[1][3]=true; break;   //J�^
	case 5:	local[2][1]=true; local[2][2]=true; local[2][3]=true; local[3][3]=true; break;   //L�^
	case 6:	local[2][1]=true; local[2][2]=true; local[2][3]=true; local[3][2]=true; break;   //T�^
	default:	break;
	}
}

//���[�J�����W�̏����O���[�o�����W�֓n���֐�
//Rect�N���X�̍��W�ɑ������
void Tetrimino::LocalToGrobal()
{
	int n=0;
	//���S���W��(0,0)�Ȃ̂ŁA�����(-2,-2)
	for(int ly=-2; ly<=2; ++ly)
	{
		for(int lx=-2; lx<=2; ++lx)
		{
			if(local[lx+2][ly+2])
			{
				block[n].rect.x=(lx+x)*BlockSize;
				block[n].rect.y=(ly+y-5)*BlockSize;	//����5�͉�ʂ���ɂ�����W�̒���
				++n;
			}
		}
	}
}

void Tetrimino::HardDrop()
{
	//std::ofstream Tdebag( "TetriminoDebag.txt" );
	//Tdebag << "y:" << y << std::endl;
	debag=true;
	//int tx[4]={0},ty[4]={0};
	std::array<int, 4> tx, ty;
	int k=0;
	for(int lx=-2; lx<=2; ++lx)
	{
		for(int ly=-2; ly<=2; ++ly)
		{
			if(local[lx+2][ly+2])
			{
				tx[k]=x+lx;
				ty[k]=y+ly;
				++k;
			}
		}
	}
	int l = GetProjection(tx, ty);//���̂��̓u���b�N�̒��S�Ԃ̋����Ȃ̂ŁA���̂܂ܑ����Əd�Ȃ��Ă��܂�
	if(l==0)
	{
		++l;
	}
	
	//Tdebag << "y:" << y << std::endl;
	y += (l-1);//���������āA-1���Ă��瑫��
	//Tdebag << "y:" << y  << "\nl:" << l << std::endl;
	//Tdebag.close();
	Earth();
}

void Tetrimino::SoftDrop()
{
	if(MoveJudge(x,y+1,local)==0)
	{
		++y;
	}
	else
	{
		Earth();
	}
}

void Tetrimino::Move(bool xplus)
{
	int mx=x+xplus*2-1;
	//�����̍��E�ړ��̓N�����v���Ȃ�
	if(MoveJudge(mx,y,local)==0)
	{
		x=mx;
	}
}

void Tetrimino::Spin(bool clockwise)
{
	if(GetNowKind() != 1)//O�^�͉�]�����Ă��`���ς��Ȃ��̂ŏȂ��i�ǂ݂̂���]�����ݒ�ł��Ȃ��j
	{
		//bool temp[5][5]={0};
		std::array<std::array<char, 5>, 5> temp;
		
		if(clockwise)
		{
			for(int i=0; i<5; ++i)
			{
				for(int j=0; j<5; ++j)
				{
					temp[4-j][i]=local[i][j];
				}
			}
		}
		else
		{
			for(int i=0; i<5; ++i)
			{
				for(int j=0; j<5; ++j)
				{
					temp[j][4-i]=local[i][j];
				}
			}
		}

		int coordclamp=MoveJudge(x,y,temp);
		if(coordclamp!=-1)
		{
			const int dx[9]={0, -1, 1, 0, 0, -1, 1, -1, 1};
			const int dy[9]={0, 0, 0, -1, 1, -1, -1, 1, 1};
			for(int i=0; i<5; ++i)
			{
				for(int j=0; j<5; ++j)
				{
					local[i][j]=temp[i][j];
				}
			}
			x+=dx[coordclamp];
			y+=dy[coordclamp];
		}

	}
}

//int Tetrimino::MoveJudge(int mx, int my, bool mlocal[5][5], int i)
int Tetrimino::MoveJudge(int mx, int my, const std::array<std::array<char, 5>, 5>& mlocal, int i)
{
	const int dx[9]={0, -1, 1, 0, 0, -1, 1, -1, 1};
	const int dy[9]={0, 0, 0, -1, 1, -1, -1, 1, 1};
	for(int lx=-2; lx<=2; ++lx)
	{
		for(int ly=-2; ly<=2; ++ly)
		{
			if(mlocal[lx+2][ly+2] & grd.GetBlockExist(lx+mx+dx[i], ly+my+dy[i]) )
			{
				if(i==8)//�X���C�h���Ă��������Ȃ�������
					return -1;
				else
					return MoveJudge(mx, my, mlocal, i+1);
			}
		}
	}
	return i;
}

//Ground(x) = local(x) + x + GroundFXS
//Ground(y) = local(y) + y
//�ڒn�������Ɏ��s�����֐�
//�ڒn������������ꂽ�����n�[�h�h���b�v���s�����Ƃ��Ɏ��s�����
//���C�����������A�e�g���~�m�̏��������s��
void Tetrimino::Earth()
{
	flag=false;
	for(int lx=-2; lx<=2; ++lx)
	{
		for(int ly=-2; ly<=2; ++ly)
		{
			if(local[lx+2][ly+2])
			{
				grd.b[ly+y][lx+x].flag=true;
				grd.b[ly+y][lx+x].col=GetNowKind();
			}
		}
	}

	int delline[TBlockNum]={0};//�����郉�C����y���W�i�B�O���[�o���j
	int k=0;

	//GroundVNum-2�F�u���b�N���ˍ��W�ւ̕ϊ���-�P�A��ԉ��̍��W�̓t���[��������Ȃ��̂ł����-�P�B
	//(x,4)�Ƀu���b�N���������瑦�Q�[���I�[�o�[�Ȃ̂łS�ȉ��̃��C�������̗L���͔��肷��K�v���Ȃ�
	for(int i=GroundVNum-2; i>4; --i)
	{
		int bnum=0;//bnum�͈��ɑ��݂���u���b�N��
		for(int j=0; j<GroundHNum; ++j)
		{
			if(grd.b[i][j].flag)
				++bnum;
		}
		if(bnum==GroundHNum)
		{
			delline[k++]=i;
		}
		else if(bnum>GroundHNum)//���ŃJ�E���g���ꂽ�u���b�N�����ɓ���ő�l�𒴂��Ă�����
		{
			//Logger::Write(L"�G���[�iTetrimino::Earth()�j�F������W���Ƀu���b�N���d�Ȃ��Ă���B");
			LOG(L"�G���[�iTetrimino::Earth()�j�F������W���Ƀu���b�N���d�Ȃ��Ă���B");
			System::Exit();
		}
	}
	for(int i=0; i<k; ++i){
		DeleteLine(delline[i]);
	}
	PushLine(k, delline);
}

//int Tetrimino::GetProjection(int tx[4], int ty[4])
int Tetrimino::GetProjection(const std::array<int, 4>& tx, const std::array<int, 4>& ty)
{
	int length[4];
	int min=GroundVNum;//�e�g���~�m����u���b�N�܂ŉ����������̍ŒZ����
	int top[GroundHNum];//�e��ɂ�����u���b�N�̍ō��_�̔z��
	int ax[4]={0}, ay[4]={0};
	for(int i=0; i<4; ++i)
	{
		length[i] = GroundVNum-1 - ty[i];
	}
	for(int i=0; i<GroundHNum; ++i)
	{
		for(int j=GroundVNum-1; j>4; --j)//����j���S�ȉ��ɂȂ�����Q�[���I�[�o�[(���[��j�͏��0�ł��邱�Ƃɒ���)
		{
			if(i==tx[0])
			{
				if(j<=ty[0])
				{
					break;
				}
			}
			if(i==tx[1])
			{
				if(j<=ty[1])
				{
					break;
				}
			}
			if(i==tx[2])
			{
				if(j<=ty[2])
				{
					break;
				}
			}
			if(i==tx[3])
			{
				if(j<=ty[3])
				{
					break;
				}
			}
			if(grd.b[j][i].flag)
			{
				top[i]=j;
			}
		}
	}
	for(int i=0; i<4; ++i)
	{
		length[i]=top[tx[i]]-ty[i];//���쒆�̃u���b�N�̒��S�Ƃ��̐^���ɂ���u���b�N�̒��S�̋���
		if(length[i]<min)
		{
			min=length[i];
		}
	}
	return min;
}

void Tetrimino::DeleteLine(int y)
{
	for(int i=GroundVNum-2; i>0; --i)
	{
		for(int j=1; j<GroundHNum-1; ++j)
		{
			if(i==y)
			{
				grd.b[i][j].flag=false;
			}
		}
	}
}

void Tetrimino::PushLine(int k, int y[])
{
	int a[4]={GroundVNum+1, GroundVNum+1, GroundVNum+1, GroundVNum+1};
	for(int i=0; i<k; ++i)
	{
		a[i]=y[i];
	}
	for(int i=GroundVNum-2,m=GroundVNum-2;  i>0;  --i)
	{
		if(a[0] != i && a[1] != i && a[2] != i && a[3] != i)
		{
			for(int j=1; j<GroundHNum-1; ++j)
			{
				grd.b[m][j].flag=grd.b[i][j].flag;
				grd.b[m][j].col=grd.b[i][j].col;
			}
			--m;
		}
	}
}

int Tetrimino::GetTop()
{
	int ytop=GroundVNum;
	for(int i=1; i<GroundHNum-1; ++i)
	{
		for(int j=GroundVNum-1; j>0; --j)//����j���S�ȉ��ɂȂ�����Q�[���I�[�o�[(���[��j�͏��0�ł��邱�Ƃɒ���)
		{
			if(grd.b[j][i].flag)
			{
				if(ytop>j)
					ytop=j;
			}
		}
	}
	return ytop;
}

//�e�g���~�m��`�悷��֐�
void Tetrimino::Graph(Point pos)
{
	/*
	if(debag)
	{
		grd.Debag();
	}
	*/
	LocalToGrobal();
	for(int i=0; i<4; ++i)
	{
		Rect( block[i].rect.pos+pos, block[i].rect.size ).draw(BlockColor[kind[count%7]]);
		RectF( block[i].rect.pos+pos, block[i].rect.size ).drawFrame( 0.5, 0, Palette::White );
		//Rect( block[i].rect.pos+pos, block[i].rect.size ).drawFrame( 1, 0, Palette::Lightgrey );
		//block[i].rect.draw(BlockColor[kind[count%7]]);
	}
	grd.Graph(pos);
}

Ground::Ground()
{
	for(int i=0; i<GroundVNum; ++i)
	{
		for(int j=0; j<GroundHNum; ++j)
		{
			b[i][j].rect.w=BlockSize;
			b[i][j].rect.h=BlockSize;
			b[i][j].col=0;
			if(j < GroundFXS || GroundFXE < j || GroundFYE < i)
			{
				b[i][j].flag=true;
			}
			else
			{
				b[i][j].flag=false;
			}
		}
	}
	GraphCoordSet();
}

void Ground::GraphCoordSet()
{
	for(int ly=5; ly<GroundVNum-1; ++ly)
	{
		for(int lx=1; lx<GroundHNum-1; ++lx)
		{
			if(b[ly][lx].flag)
			{
				b[ly][lx].rect.x=lx*BlockSize;
				b[ly][lx].rect.y=(ly-5)*BlockSize;
			}
		}
	}
}

void Ground::Debag()
{
	/*
	std::ofstream Gdebag( "GroundDebag.txt" );
	for(int i=0; i<GroundVNum; ++i)
	{
		for(int j=0; j<GroundHNum; ++j)
		{
			if(b[j][i].flag)
			{
				Gdebag << b[j][i].col << ',' ;
			}
			else
			{
				Gdebag << "0,";
			}
		}
		Gdebag << "\n" << std::endl;
	}
   
   Gdebag.close();
   */
}

void Ground::Graph(Point pos)
{
	GraphCoordSet();
	for(int lx=1; lx<GroundHNum-1; ++lx)
	{
		for(int ly=5; ly<GroundVNum-1; ++ly)
		{
			//if(!(lx <= GroundFXS || GroundFXE <= lx || GroundFYE <= ly))
			//{
				if(b[ly][lx].flag)
				{
					Rect( b[ly][lx].rect.pos+pos, b[ly][lx].rect.size ).draw( BlockColor[ b[ly][lx].col ] );
					RectF( b[ly][lx].rect.pos+pos, b[ly][lx].rect.size ).drawFrame( 0.5, 0, Palette::Gray );
				}
			//}
		}
	}
}

bool Ground::GetBlockExist(int x, int y)
{
	if(x<0 || GroundHNum-1<x || y<0 || GroundVNum-1<y)
	{
		return true;
	}
	else
		return b[y][x].flag;
}

Tetris::Tetris()
{
	gameover=false;
	count=1;
	graphflag=0;
	tetri.reset( new Tetrimino );

	LeftFrame.x=0;
	LeftFrame.y=0;
	LeftFrame.w=FrameSize;
	LeftFrame.h=VBlockNum*BlockSize + FrameSize;

	RightFrame.x=HBlockNum*BlockSize + FrameSize;
	RightFrame.y=0;
	RightFrame.w=FrameSize;
	RightFrame.h=VBlockNum*BlockSize + FrameSize;

	UnderFrame.x=FrameSize;
	UnderFrame.y=VBlockNum*BlockSize;
	UnderFrame.w=HBlockNum*BlockSize;
	UnderFrame.h=FrameSize;
}

void Tetris::KeyProcess()
{
	if(gameover)
	{
		return;
	}

	bool keyin[KeyNum]={};

	//keyin[0]=Input::MouseM.clicked;
	/*keyin[0]=Input::MouseX1.clicked;
	keyin[1]=false;
	keyin[2]=false;
	keyin[3]=Input::MouseL.clicked;
	keyin[4]=Input::MouseR.clicked;
	keyin[5]=Mouse::Wheel()<0;
	keyin[6]=0<Mouse::Wheel();
	keyin[7]=false;*/
	keyin[0] = Input::KeyUp.clicked;
	keyin[1] = false;
	keyin[2] = Input::KeyDown.clicked;
	keyin[3] = Input::KeyLeft.clicked;
	keyin[4] = Input::KeyRight.clicked;
	keyin[5] = Input::KeyA.clicked;
	keyin[6] = Input::KeyS.clicked;
	keyin[7] = false;
	
	for(int i=0; i<KeyNum; ++i)
	{
		if(keyin[i])
		{
			switch(i)
			{
			case 0:
				tetri->HardDrop();
				m_newAction = String(L"�n�[�h�h���b�v");
				break;
			case 1: break;//tetri.HardDrop(); break;
			case 2:
				tetri->SoftDrop();
				m_newAction = String(L"�\�t�g�h���b�v");
				break;
			case 3:
				tetri->Move(false);
				m_newAction = String(L"���ړ�");
				break;
			case 4:
				tetri->Move(true);
				m_newAction = String(L"�E�ړ�");
				break;
			case 5:
				tetri->Spin(false);
				m_newAction = String(L"����]");
				break;
			case 6:
				tetri->Spin(true);
				m_newAction = String(L"�E��]");
				break;
			case 7: Option(); break;
			case 8: break;
			case 9: break;
			}
		}
	}
}

void Tetris::Option()
{

}

void Tetris::RoopIni()
{
	if(++count%DropVelocity==0)
	{
		tetri->SoftDrop();
	}
	if(!tetri->RoopIni())
		gameover=true;
}

void Tetris::draw()
{
	const Point pos(0, 0);
	if(!gameover)
	{
		Rect( LeftFrame.pos+pos, LeftFrame.size ).draw(FrameColor);
		Rect( RightFrame.pos+pos, RightFrame.size ).draw(FrameColor);
		Rect( UnderFrame.pos+pos, UnderFrame.size ).draw(FrameColor);

		/*Rect( LeftFrame.pos+pos, LeftFrame.size ).drawFrame(1,0,Palette::Gray);
		Rect( RightFrame.pos+pos, RightFrame.size ).drawFrame(1,0,Palette::Gray);
		Rect( UnderFrame.pos+pos, UnderFrame.size ).drawFrame(1,0,Palette::Gray);*/

		tetri->Graph(pos);
	}
	else
	{
		Rect( LeftFrame.pos+pos, LeftFrame.size ).draw(FrameColor);
		Rect( RightFrame.pos+pos, RightFrame.size ).draw(FrameColor);
		Rect( UnderFrame.pos+pos, UnderFrame.size ).draw(FrameColor);

		/*Rect( LeftFrame.pos+pos, LeftFrame.size ).drawFrame(1,0,Palette::Gray);
		Rect( RightFrame.pos+pos, RightFrame.size ).drawFrame(1,0,Palette::Gray);
		Rect( UnderFrame.pos+pos, UnderFrame.size ).drawFrame(1,0,Palette::Gray);*/

		gameover=false;
		count=1;
		graphflag=0;
		tetri.reset( new Tetrimino );
		//Font a(Typeface::Default, 64);
		//a.drawCenter(L"GameOver!",150,Palette::Orange);
	}
}

void Tetris::update()
{
	RoopIni();
	KeyProcess();
}

void Main()
{
	ReversibleApplicationGUI<Tetris> tetris(Tetris{}, 50u, 200);
	
	while (System::Update())
	{
		tetris.update();
		tetris.draw();
	}
}