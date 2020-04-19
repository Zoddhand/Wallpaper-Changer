#include <windows.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <time.h>
#include <chrono>
//#include <thread>
#include <direct.h>
#include <stdlib.h>
#include <locale>


using namespace cv;
using namespace std;

int MonitorCount();
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
bool changeWallpaper(const wchar_t* wall1);
void combineImage(const char* wallpaper1, const char* wallpaper2);
string getfile(const char* file);
int loadSettings(const char*);
int settings[20];
string ssettings;
struct Dir
{
	string settings;
	string directory;
	string batDir;
	string imgDir;
};
Dir dir[4];
enum { mNum, rMonX, rMonY, cMonX, cMonY, lMonX, lMonY, imgY };

void HideConsole()
{
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
}
void ShowConsole()
{
	::ShowWindow(::GetConsoleWindow(), SW_SHOW);
}

void CreateFolder(const char* path)
{
	if (_mkdir(path) == 0)
	{
		cout << "Directory " << path << " was successfully created\n";
		//system("dir \\testtmp");
	}
	else
		cout << "Problem Creating Directory " << path << ". It may already exist\n";
}

struct display
{
	string image;
	string conc;
	const char* c;
};
int numMonitors = 0;
bool a = false;
int main()
{


	CreateFolder(dir[3].directory.c_str());
	loadSettings("config.txt");


	std::wstring_convert< std::codecvt<wchar_t, char, std::mbstate_t> > conv;
	std::wstring wstr = conv.from_bytes(dir[3].directory);
	std::wcout << wstr << L'\n';

	display dis[3];
	std::chrono::seconds interval(settings[8]);
	if (settings[9] == 1)
		ShowConsole();
	else HideConsole();
	while (1)
	{
		for (int i = 0; i < 10; ++i)
		{
			if (settings[0] == 0)
				numMonitors = MonitorCount();
			else numMonitors = settings[0];

			cout << "Monitors:" << numMonitors << endl;
			std::cout << "tick!\n" << std::flush;

			for (int i = 0; i < numMonitors; i++)
			{
				dir[i].batDir = "cd /d";
				dir[i].batDir += dir[i].directory;
				dir[i].batDir += " && dir /b *.jpg *.png *.jpeg > filenames.txt";
				system(dir[i].batDir.c_str());
				dir[i].imgDir = dir[i].directory;
				dir[i].imgDir += "/filenames.txt";
				dis[i].image = getfile(dir[i].imgDir.c_str());
				dis[i].conc = dir[i].directory;
				dis[i].conc += "/";
				dis[i].conc += dis[i].image;
				dis[i].c = dis[i].conc.c_str();
				cout << "wallpaper " << i << ":" << dis[i].conc << endl;
			}

			if (numMonitors == 1)
			{
				string sstring = dir[0].directory;
				sstring += "\\";
				sstring += dis[0].image;
				Mat3b tmp1 = imread(sstring.c_str());
				string tmp = dir[3].directory;
				tmp += "Wall.jpg";
				imwrite(tmp, tmp1);
				wstring wtmp = wstr;
				wtmp += L"Wall.jpg";
				changeWallpaper(wtmp.c_str());
			}
			else if (numMonitors == 3)
			{
				a = false;
				combineImage(dis[1].c, dis[0].c);
				a = true;
				string tmp = dir[3].directory;
				tmp += "Wallpaper.jpg";
				combineImage(dis[2].c, tmp.c_str());
				wstring wtmp = wstr;
				wtmp += L"Wallpaper2.jpg";
				changeWallpaper(wtmp.c_str());
			}
			else
			{
				a = false;
				combineImage(dis[1].c, dis[0].c);
				wstring wtmp = wstr;
				wtmp += L"Wallpaper.jpg";
				changeWallpaper(wtmp.c_str());
			}
			std::this_thread::sleep_for(interval);
		}
	}
	return 0;
}

bool changeWallpaper(const wchar_t* wall1)
{
	wcout << wall1;
	const wchar_t* filenm = wall1; //ADDRESS of first image
	bool isWallSet = SystemParametersInfoW(SPI_SETDESKWALLPAPER, 1, (void*)filenm, SPIF_UPDATEINIFILE);
	std::cout << "Changed\n";
	return false;
}

void combineImage(const char* wallpaper1, const char* wallpaper2)
{
	// Load images
	Mat3b tmp1 = imread(wallpaper1);
	Mat3b tmp2 = imread(wallpaper2);
	Size size(1920, 1080);
	Mat new_tmp1;
	Mat new_tmp2;

	if (a == true)
	{
		resize(tmp1, new_tmp1, Size(settings[lMonX], settings[lMonY]), 0, 0, INTER_NEAREST);//resize image
		resize(tmp2, new_tmp2, Size(settings[rMonX] + settings[cMonX] + 1, settings[imgY]), 0, 0, INTER_NEAREST);//resize image
	}
	else
	{
		resize(tmp1, new_tmp1, Size(settings[cMonX], settings[cMonY]), 0, 0, INTER_NEAREST);//resize image
		resize(tmp2, new_tmp2, Size(settings[rMonX] + 1, settings[rMonY]), 0, 0, INTER_NEAREST);//resize image
	}

	imwrite("tmp1.jpg", new_tmp1);
	imwrite("tmp2.jpg", new_tmp2);

	Mat3b img1 = imread("tmp1.jpg");
	Mat3b img2 = imread("tmp2.jpg");

	// Get dimension of final image
	int rows = max(img1.rows, img2.rows);
	int cols = img1.cols + img2.cols;

	// Create a black image
	Mat3b res(rows, cols, Vec3b(0, 0, 0));

	if (a == true)
	{
		// Copy images in correct position
		img1.copyTo(res(Rect(0, settings[11], img1.cols, img1.rows)));
		img2.copyTo(res(Rect(img1.cols, 0, img2.cols, img2.rows)));
		string tmp = dir[3].directory;
		tmp += "Wallpaper2.jpg";
		imwrite(tmp, res);
	}
	else
	{
		// Copy images in correct position
		img1.copyTo(res(Rect(0, 0, img1.cols, img1.rows)));
		img2.copyTo(res(Rect(img1.cols, settings[10], img2.cols, img2.rows)));
		string tmp = dir[3].directory;
		tmp += "Wallpaper.jpg";
		imwrite(tmp, res);
	}
}

const wchar_t* GetWC(const char* c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	//mbstowcs(wc, c, cSize);

	return wc;
}

string Clist(const char* f)
{
	string line;
	vector<string> lines;

	srand(time(0));

	//input file stream
	ifstream file(f);

	//count number of total lines in the file and store the lines in the string vector
	int total_lines = 0;
	while (getline(file, line))
	{
		total_lines++;
		lines.push_back(line);
	}

	//generate a random number between 0 and count of total lines
	int random_number = rand() % total_lines;



	//fetch the line where line index (starting from 0) matches with the random number

	cout << lines[random_number];

	return lines[random_number];
}

string getfile(const char* file)
{
	string sfile = Clist(file);
	return sfile;
}

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	int* Count = (int*)dwData;
	(*Count)++;
	return TRUE;
}

int MonitorCount()
{
	int Count = 0;
	if (EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&Count))
		return Count;
	return -1;//signals an error
}

int loadSettings(const char* f)
{
	int count = 0;
	fstream fin(f);
	string line;
	while (getline(fin, line))
	{
		//the following line trims white space from the beginning of the string
		line.erase(line.begin(), find_if(line.begin(), line.end(), not1(ptr_fun<int, int>(isspace))));
		if (line[0] == '!')
		{
			dir[0].directory += line;
			dir[0].directory.erase(0, 1);
		}
		if (line[0] == '@')
		{
			dir[1].directory += line;
			dir[1].directory.erase(0, 1);
		}
		if (line[0] == '$')
		{
			dir[2].directory += line;
			dir[2].directory.erase(0, 1);
		}
		if (line[0] == '%')
		{
			dir[3].directory += line;
			dir[3].directory.erase(0, 1);
		}
		if (line[0] == '#' || line[0] == '!' || line[0] == '@' || line[0] == '$') continue;

		int data;
		stringstream(line) >> settings[count];
		count++;
	}
	return 0;
}


/* EXAMPLE CONFIG FILE. CONFIG.TXT

#Number of Monitors. (Leave this blank for auto detection. or change from 0 to override)
0
#right/MAIN monitors resolution.
1920
1080
#left/center Monitors resolution.
1080
1920
#even further left monitors resolution.
3840
1620
#The Tallest Monitors Y resolution. if your best monitors resolution is 1920/1080 you would enter 1080.
#but if its a 4k monitor 3840 x 2160 you would enter 2160.
1920
#Time between Wallpaper Changes in seconds. Example: 120 is 2 minutes.
60
#Enable Debut Console Output. 0 for off 1 for on.
0
#Monitor Offset Y. You may need to offset your monitor based on its position in display settings.
575
#Monitor Offset Y. This is for your 3rd monitor.
0
#Destination Folder for right monitors Wallpapers dont forget the leading '!' . ### Example: !cd C:/Wallpapers && dir /b *.jpg *.png > filenames.txt
!D:\Photos\WallpaperChanger\MonitorRight
#Destination Folder for center monitors Wallpapers dont forget the leading '@' .
@D:\Photos\WallpaperChanger\MonitorLeft
#Destination Folder for left monitors Wallpapers dont forget the leading '$' .
$D:\Photos\WallpaperChanger\Projector
#Destination Folder Working Directory. Needs to be full path. following this format. ### Example %C:\\Users\\Public\\Wallpapers
%C:\\Users\\Public\\WallpaperChanger\\

*/