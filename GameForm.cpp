#include "GameForm.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThreadAttribute]
void main(array<String^>^ args) {
	//Базовые настройки для запуска формы
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	//Запускаем указанную форму
	GameSnake::GameForm form;
	Application::Run(% form);
}

//Структура, удобная для 2D пространства 
struct Vector2 {
	int X, Y;
};

Vector2 direction;//направление змейки
Vector2 positionFruit;//позиция фрукта
Vector2 gameArea;//игровая зона

//Конструктор формы
GameSnake::GameForm::GameForm(void)
{
	InitializeComponent();

	//Задаем размеры игровой зоны
	gameArea.X = 500;
	gameArea.Y = 500;

	firstLaunch = true;//первый запуск
	NewGame();//новая игра
}

//------------------------
//Реализация действий игры
void GameSnake::GameForm::GeneratePositionFruit()
{
	//Генерируем случайную позицию фрукту
	Random^ rand = gcnew Random();
	positionFruit.X = rand->Next(10, gameArea.X);//10 - смещение относительно боков формы
	positionFruit.Y = rand->Next(90, gameArea.Y);//90 - смещение верхней граници относительно верха формы

	//Проверка, чтобы фрукт не создался на змейке
	for (int i = 0; i < score; i++) {
		if (positionFruit.X == Serpens[i]->Location.X &&
			positionFruit.Y == Serpens[i]->Location.Y)
			GeneratePositionFruit();
	}

	//Преобразуем значение, чтобы было кратно шагу
	int tempX = positionFruit.X % step;
	positionFruit.X -= tempX;

	int tempY = positionFruit.Y % step;
	positionFruit.Y -= tempY;

	//Присваеваем пизицию фрукту
	fruit->Location = Point(positionFruit.X, positionFruit.Y);

	//Добавляем объект на форму
	this->Controls->Add(fruit);
}

void GameSnake::GameForm::Eating()
{
	//Проверим позицию головы змеи и позицию фрукта
	if (Serpens[0]->Location.X == positionFruit.X && Serpens[0]->Location.Y == positionFruit.Y) {
		labelScore->Text = "Счет: " + ++score;

		//Добавляем новый элемент змейке
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
	//Двигаем каждый компонет змейки
	for (int i = score; i >= 1; i--) {
		Serpens[i]->Location = Serpens[i - 1]->Location;
	}
	Serpens[0]->Location = Point(Serpens[0]->Location.X + direction.X * step, Serpens[0]->Location.Y + direction.Y * step);
}

void GameSnake::GameForm::EatYourself()
{
	//Проверяем позицию каждой части змейки
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
	//Если инициализация была, то удаляем объекты с формы
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

	//Инициализируем змейку
	Serpens = gcnew array <PictureBox^, 1>(400);
	Serpens[0] = gcnew PictureBox();
	Serpens[0]->Location = Point(200, 200);
	Serpens[0]->BackColor = Color::Green;//голова будет зеленой, чтобы отличалась от всех
	Serpens[0]->Width = step;
	Serpens[0]->Height = step;

	score = 0;
	this->Controls->Add(Serpens[0]);

	//Инициализируем фрукт
	fruit = gcnew PictureBox();
	fruit->BackColor = Color::Yellow;
	fruit->Width = step;
	fruit->Height = step;
	GeneratePositionFruit();

	//Задаем интревал обнавления и запускаем таймер для обнавления
	timer->Interval = updateInterval;
	timer->Start();

	//Задаем направление на запуске
	direction.X = 1;
	direction.Y = 0;

	//Можно грать
	play = true;
	die = false;

	labelScore->Text = "Счет: 0";

	//Скрываем не нужные компоненты на форме
	labelGameOver->Visible = false;
	groupBoxSettings->Visible = false;
}

void GameSnake::GameForm::ChackBorders()
{
	if (Serpens[0]->Location.X >= RigtBorder->Location.X || Serpens[0]->Location.X <= LeftBorder->Location.X) {
		//direction.X *= -1;//развернуть в обратном направлении
		GameOver();
	}
	
	if (Serpens[0]->Location.Y <= UpperBorder->Location.Y || Serpens[0]->Location.Y >= LowerBorder->Location.Y) {
		//direction.Y *= -1;//развернуть в обратном направлении
		GameOver();
	}

	//Вывод координат змейки
	labelCoordinatesSnake->Text = "Кооринаты змейки: (" + Convert::ToString(Serpens[0]->Location.X) + ", " + Convert::ToString(Serpens[0]->Location.Y) +")" ;
}

//------------------
//Реализация событий
System::Void GameSnake::GameForm::выходToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
{
	Application::Exit();
}

System::Void GameSnake::GameForm::информацияОИгреToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
{
	MessageBox::Show("Правила игры:\n1. Для управления использовать стрелки\n2. Ешьте фрукты для набора счета и увеличения змейки\n3. Нельзя есть себя и врезаться в стенкуи\n4. И т.д.","Правила игры!");

	return System::Void();
}

System::Void GameSnake::GameForm::новаяИграToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
{
	NewGame();

	return System::Void();
}

System::Void GameSnake::GameForm::паузапродолжитьToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
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

System::Void GameSnake::GameForm::настройкиToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
{
	if (groupBoxSettings->Visible == false) {
		//Приостанавливаем игру
		play = false;

		buttonApplySpeed->Enabled = true;
		numericUpDownSpeedSnake->Enabled = true;
		groupBoxSettings->Visible = true;
	}
	else {
		//Запускаем игру
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

	//Запускаем игру
	play = true;
	timer->Start();

	return System::Void();
}

System::Void GameSnake::GameForm::GameForm_KeyDown(System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e)
{
	//Считываем нажатую клавишу
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
		//Задает движение змейки
		Movement();

		Eating();//проверка, что съели фрукт
		EatYourself();//проверка, что съел себя
		ChackBorders();//проверка на столкновение с стеной
	}
	else if (die && play) {
		timer->Stop();
		MessageBox::Show("Игра окончена!", "Внимание!");
	}
	else if (!play && !die) {
		timer->Stop();
		MessageBox::Show("Игра приостановлена!", "Внимание!");
	}
}
