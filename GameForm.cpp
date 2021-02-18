#include "GameForm.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThreadAttribute]
void main(array<String^>^ args) {
	//������� ��������� ��� ������� �����
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	//��������� ��������� �����
	GameSnake::GameForm form;
	Application::Run(% form);
}

//���������, ������� ��� 2D ������������ 
struct Vector2 {
	int X, Y;
};

Vector2 direction;//����������� ������
Vector2 positionFruit;//������� ������
Vector2 gameArea;//������� ����

//����������� �����
GameSnake::GameForm::GameForm(void)
{
	InitializeComponent();

	//������ ������� ������� ����
	gameArea.X = 500;
	gameArea.Y = 500;

	firstLaunch = true;//������ ������
	NewGame();//����� ����
}

//------------------------
//���������� �������� ����
void GameSnake::GameForm::GeneratePositionFruit()
{
	//���������� ��������� ������� ������
	Random^ rand = gcnew Random();
	positionFruit.X = rand->Next(10, gameArea.X);//10 - �������� ������������ ����� �����
	positionFruit.Y = rand->Next(90, gameArea.Y);//90 - �������� ������� ������� ������������ ����� �����

	//��������, ����� ����� �� �������� �� ������
	for (int i = 0; i < score; i++) {
		if (positionFruit.X == Serpens[i]->Location.X &&
			positionFruit.Y == Serpens[i]->Location.Y)
			GeneratePositionFruit();
	}

	//����������� ��������, ����� ���� ������ ����
	int tempX = positionFruit.X % step;
	positionFruit.X -= tempX;

	int tempY = positionFruit.Y % step;
	positionFruit.Y -= tempY;

	//����������� ������� ������
	fruit->Location = Point(positionFruit.X, positionFruit.Y);

	//��������� ������ �� �����
	this->Controls->Add(fruit);
}

void GameSnake::GameForm::Eating()
{
	//�������� ������� ������ ���� � ������� ������
	if (Serpens[0]->Location.X == positionFruit.X && Serpens[0]->Location.Y == positionFruit.Y) {
		labelScore->Text = "����: " + ++score;

		//��������� ����� ������� ������
		Serpens[score] = gcnew PictureBox();
		Serpens[score]->Location = Point(Serpens[score - 1]->Location.X + step * direction.X, Serpens[score - 1]->Location.Y - step * direction.Y);
		Serpens[score]->BackColor = Color::Red;
		Serpens[score]->Width = step;
		Serpens[score]->Height = step;
		this->Controls->Add(Serpens[score]);

		GeneratePositionFruit();
	}
}

void GameSnake::GameForm::Movement()
{
	//������� ������ �������� ������
	for (int i = score; i >= 1; i--) {
		Serpens[i]->Location = Serpens[i - 1]->Location;
	}
	Serpens[0]->Location = Point(Serpens[0]->Location.X + direction.X * step, Serpens[0]->Location.Y + direction.Y * step);
}

void GameSnake::GameForm::EatYourself()
{
	//��������� ������� ������ ����� ������
	for (int i = 1; i < score; i++) {
		if (Serpens[0]->Location == Serpens[i]->Location) {
			GameOver();
		}
	}
}

void GameSnake::GameForm::GameOver()
{
	play = true;
	die = true;

	labelGameOver->Visible = true;
}

void GameSnake::GameForm::NewGame()
{
	//���� ������������� ����, �� ������� ������� � �����
	if (!firstLaunch) {
		this->Controls->Remove(fruit);

		for (int i = 0; i <= score; i++)
		{
			this->Controls->Remove(Serpens[i]);
		}

		score = 0;
	}
	else
		firstLaunch = false;

	//�������������� ������
	Serpens = gcnew array <PictureBox^, 1>(400);
	Serpens[0] = gcnew PictureBox();
	Serpens[0]->Location = Point(200, 200);
	Serpens[0]->BackColor = Color::Green;//������ ����� �������, ����� ���������� �� ����
	Serpens[0]->Width = step;
	Serpens[0]->Height = step;

	score = 0;
	this->Controls->Add(Serpens[0]);

	//�������������� �����
	fruit = gcnew PictureBox();
	fruit->BackColor = Color::Yellow;
	fruit->Width = step;
	fruit->Height = step;
	GeneratePositionFruit();

	//������ �������� ���������� � ��������� ������ ��� ����������
	timer->Interval = updateInterval;
	timer->Start();

	//������ ����������� �� �������
	direction.X = 1;
	direction.Y = 0;

	//����� �����
	play = true;
	die = false;

	labelScore->Text = "����: 0";

	//�������� �� ������ ���������� �� �����
	labelGameOver->Visible = false;
	groupBoxSettings->Visible = false;
}

void GameSnake::GameForm::ChackBorders()
{
	if (Serpens[0]->Location.X >= RigtBorder->Location.X || Serpens[0]->Location.X <= LeftBorder->Location.X) {
		//direction.X *= -1;//���������� � �������� �����������
		GameOver();
	}
	
	if (Serpens[0]->Location.Y <= UpperBorder->Location.Y || Serpens[0]->Location.Y >= LowerBorder->Location.Y) {
		//direction.Y *= -1;//���������� � �������� �����������
		GameOver();
	}

	//����� ��������� ������
	labelCoordinatesSnake->Text = "��������� ������: (" + Convert::ToString(Serpens[0]->Location.X) + ", " + Convert::ToString(Serpens[0]->Location.Y) +")" ;
}

//------------------
//���������� �������
System::Void GameSnake::GameForm::�����ToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
{
	Application::Exit();
}

System::Void GameSnake::GameForm::���������������ToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
{
	MessageBox::Show("������� ����:\n1. ��� ���������� ������������ �������\n2. ����� ������ ��� ������ ����� � ���������� ������\n3. ������ ���� ���� � ��������� � �������\n4. � �.�.","������� ����!");

	return System::Void();
}

System::Void GameSnake::GameForm::���������ToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
{
	NewGame();

	return System::Void();
}

System::Void GameSnake::GameForm::���������������ToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
{
	if (play) {
		play = false;
	}
	else
	{
		play = true;
		timer->Start();
	}

	return System::Void();
}

System::Void GameSnake::GameForm::���������ToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
{
	if (groupBoxSettings->Visible == false) {
		//���������������� ����
		play = false;

		buttonApplySpeed->Enabled = true;
		numericUpDownSpeedSnake->Enabled = true;
		groupBoxSettings->Visible = true;
	}
	else {
		//��������� ����
		play = true;
		timer->Start();

		buttonApplySpeed->Enabled = false;
		numericUpDownSpeedSnake->Enabled = false;
		groupBoxSettings->Visible = false;
	}
}

System::Void GameSnake::GameForm::buttonApplySpeed_Click(System::Object^ sender, System::EventArgs^ e)
{
	updateInterval = Convert::ToSingle(numericUpDownSpeedSnake->Value);
	timer->Interval = updateInterval;

	buttonApplySpeed->Enabled = false;
	numericUpDownSpeedSnake->Enabled = false;
	groupBoxSettings->Visible = false;

	//��������� ����
	play = true;
	timer->Start();

	return System::Void();
}

System::Void GameSnake::GameForm::GameForm_KeyDown(System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e)
{
	//��������� ������� �������
	if (e->KeyCode.ToString() == "Right") {
		direction.X = 1;
		direction.Y = 0;
	}
	else if (e->KeyCode.ToString() == "Left") {
		direction.X = -1;
		direction.Y = 0;
	}
	else if (e->KeyCode.ToString() == "Up") {
		direction.Y = -1;
		direction.X = 0;
	}
	else if (e->KeyCode.ToString() == "Down") {
		direction.Y = 1;
		direction.X = 0;
	}

	return System::Void();
}

void GameSnake::GameForm::GameForm_Update(Object^ obgect, EventArgs^ e)
{
	if (!die && play) {
		//������ �������� ������
		Movement();

		Eating();//��������, ��� ����� �����
		EatYourself();//��������, ��� ���� ����
		ChackBorders();//�������� �� ������������ � ������
	}
	else if (die && play) {
		timer->Stop();
		MessageBox::Show("���� ��������!", "��������!");
	}
	else if (!play && !die) {
		timer->Stop();
		MessageBox::Show("���� ��������������!", "��������!");
	}
}
