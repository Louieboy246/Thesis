#include <iostream>
#include <graphics>
#include <conio>
#include <math>

using namespace std;

void show_screen();

void Polygon(const int, const int[]);

void Line(const int, const int, const int, const int);

int main()
{
	int driver = VGA;
	int mode = VGAHI;

	int n = 0;

	do
	{
		show_screen();

		gotoxy(12, 13);
		cout << "Enter the number of points : ";
		cin >> n;

		int *coordinates = new int[(n * 2)];

		for (int count = 0; count<n; count++)>
		{
			gotoxy(8, 18);
			cout << "Coordinates of Point-" << (count + 1) << " (x" << (count + 1) << ",y" << (count + 1) << ") :";


			gotoxy(12, 21);
			cout << "Enter the value of x" << (count + 1) << " = ";
			cin >> coordinates[(count * 2)];

			gotoxy(12, 22);
			cout << "Enter the value of y" << (count + 1) << " = ";
			cin >> coordinates[((count * 2) + 1)];

			gotoxy(8, 18);
			cout << "                                            ";

			gotoxy(12, 21);
			cout << "                                            ";

			gotoxy(12, 22);
			cout << "                                            ";
		}

		initgraph(&driver, &mode, "c:\\tc\\Bgi");

		setcolor(15);
		Polygon(n, coordinates);

		delete coordinates;

		setcolor(15);
		outtextxy(110, 460, "Press <enter> to continue or any other key to exit.");

		int key = int(getch());

		if (key != 13)
			break;
	} while (1);

	return 0;
}

void Polygon(const int n, const int coordinates[])
{
	if (n >= 2)
	{
		Line(coordinates[0], coordinates[1],
			coordinates[2], coordinates[3]);

		for (int count = 1; count<(n - 1); count++)
			Line(coordinates[(count * 2)], coordinates[((count * 2) + 1)],
			coordinates[((count + 1) * 2)],
			coordinates[(((count + 1) * 2) + 1)]);
	}
}

void Line(const int x_1, const int y_1, const int x_2, const int y_2)
{
	int color = getcolor();

	int x1 = x_1;
	int y1 = y_1;

	int x2 = x_2;
	int y2 = y_2;

	if (x_1>x_2)
	{
		x1 = x_2;
		y1 = y_2;

		x2 = x_1;
		y2 = y_1;
	}

	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	int inc_dec = ((y2 >= y1) ? 1 : -1);

	if (dx>dy)
	{
		int two_dy = (2 * dy);
		int two_dy_dx = (2 * (dy - dx));
		int p = ((2 * dy) - dx);

		int x = x1;
		int y = y1;

		putpixel(x, y, color);

		while (x<x2)>
		{
			x++;

			if (p<0)
				p += two_dy;

			else
			{
				y += inc_dec;
				p += two_dy_dx;
			}

			putpixel(x, y, color);
		}
	}

	else
	{
		int two_dx = (2 * dx);
		int two_dx_dy = (2 * (dx - dy));
		int p = ((2 * dx) - dy);

		int x = x1;
		int y = y1;

		putpixel(x, y, color);

		while (y != y2)
		{
			y += inc_dec;

			if (p<0)
				p += two_dx;

			else
			{
				x++;
				p += two_dx_dy;
			}

			putpixel(x, y, color);
		}
	}
}

void show_screen()
{
	restorecrtmode();
	textmode(C4350);

	textbackground(1);
	cprintf(" Polygon ");
	textbackground(8);


	gotoxy(1, 2);
}

