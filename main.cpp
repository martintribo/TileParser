#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstring>
#include <sstream>

#include <FreeImage.h>

class ImageTile
{
	int size;
	RGBQUAD** pixels;
public:
	ImageTile(int s)
	{
		size = s;
		pixels = new RGBQUAD*[size];
		for (int i = 0; i < size; i++)
			pixels[i] = new RGBQUAD[size];
	}
	ImageTile(int s, FIBITMAP* bitmap, int x, int y)
	{
		size = s;
		pixels = new RGBQUAD*[size];
		for (int i = 0; i < size; i++)
			pixels[i] = new RGBQUAD[size];
		setupPixels(bitmap, x, y);

	}
	~ImageTile()
	{
		/*for (int i = 0; i < size; i++)
			delete[] pixels[i];
		delete[] pixels;*/
	}

	bool matches(ImageTile* it)
	{
		bool same = true;
		for (int x = 0; x < size && same; x++)
		{
			for (int y = 0; y < size && same; y++)
			{
				RGBQUAD c1 = pixels[x][y];
				RGBQUAD c2 = it->pixels[x][y];
				int dif1 = std::abs((int)c1.rgbBlue - (int)c2.rgbBlue);
				int dif2 = std::abs((int)c1.rgbGreen - (int)c2.rgbGreen);
				int dif3 = std::abs((int)c1.rgbRed - (int)c2.rgbRed);

				int dif = dif1 + dif2 + dif3;
				//std::cout << "dif: " << dif << "\n";
				if (dif > 100)
					same = false;
			}
		}

		return same;
	}

	RGBQUAD* getPixel(int x, int y)
	{
		return &pixels[x][y];
	}

	void setupPixels(FIBITMAP* bitmap, int xl, int yl)
	{
		int xStart = xl*size;
		int yStart = yl*size;

		RGBQUAD* pal = FreeImage_GetPalette(bitmap);

		for (int y = 0; y < size; y++)
		{
			for (int x = 0; x < size; x++)
			{
				BYTE tb;
				if (!FreeImage_GetPixelColor(bitmap, xStart + x, yStart + y, &pixels[x][y]))
				{
					FreeImage_GetPixelIndex(bitmap, xStart + x, yStart + y, &tb);
					pixels[x][y].rgbRed = (*(pal+tb)).rgbRed;
					pixels[x][y].rgbGreen = (*(pal+tb)).rgbGreen;
					pixels[x][y].rgbBlue = (*(pal+tb)).rgbBlue;
				}
				//std::cout << (int)(pixels[x][y].rgbGreen) << " ";
			}
			//std::cout << "\n";
		}
	}


};

int main(int argc, char* arg[])
{
	//FreeImage_Initialise();
	if (argc >= 3)
	{
		std::cout << "Using FreeImage version " << FreeImage_GetVersion() << "\n";
		//int i;
		//std::cin >> i;
		char* fileName = arg[1];
		int tileSize = atoi(arg[2]);
		FREE_IMAGE_FORMAT form = FreeImage_GetFileType(fileName);
		FIBITMAP* bitmap = FreeImage_Load(form, fileName);
		if (bitmap)
		{/*
			RGBQUAD* pal = FreeImage_GetPalette(bitmap);
			RGBQUAD tQ;
			BYTE tb;
			std::cout << "Colors: " << (int)(tQ.rgbRed) << " " << (int)(tQ.rgbGreen) << " " << (int)(tQ.rgbBlue) << "\n";
			bool win = FreeImage_GetPixelColor(bitmap, 10, 10, &tQ);
			bool win2 = FreeImage_GetPixelIndex(bitmap, 0, 0, &tb);
			std::cout << "Colors: " << (int)(tQ.rgbRed) << " " << (int)(tQ.rgbGreen) << " " << (int)(tQ.rgbBlue) << "\n";
			std::cout << win;// << " Colors: " << (int)((*(pal+tb)).rgbRed) << " " << (int)((*(pal+tb)).rgbGreen) << " " << (int)((*(pal+tb)).rgbBlue) << "\n";
			FreeImage_Save(form, bitmap, "outputImage.png");
			return 0;*/

			std::cout << "Sucessfuly loaded file!\n";
			int width = FreeImage_GetWidth(bitmap);
			int height = FreeImage_GetHeight(bitmap);
			width /= tileSize;
			height /= tileSize;
			std::cout << "Tile size is " << tileSize << ", map is " << width << "x" << height << " tiles.\n";

			std::vector<ImageTile*> tiles;
			std::ofstream file("outputMap.txt");
			for (int y = height - 1; y >= 0; y--)
			{
				for (int x = 0; x < width; x++)
				{
					//std::cout << "inner loop " << x << " " << y << " " << tiles.size() << "\n";
					int tileIndex = -1;
					ImageTile* tile = new ImageTile(tileSize, bitmap, x, y);
					//std::cout << "middle0 " << tile << "\n";
					/*for (int ry = 0; ry < 16; ry++)
					{
						for (int rx = 0; rx < 16; rx++)
						{
							std::cout << (int)(tile->getPixel(rx, ry)->rgbGreen) << " ";
						}
						std::cout << "\n";
					}*/
					//tile.setupPixels(bitmap, x, y);
					//std::cout << "middle1\n";
					for (unsigned int i = 0; (i < tiles.size()) && (tileIndex == -1); i++)
					{
					//	std::cout << "i " << i << "\n";
						if (tile->matches(tiles[i]))
							tileIndex = i;
					}
					//std::cout << "middle2 " << tileIndex << "\n";
					if (tileIndex == -1)
					{
						tileIndex = tiles.size();
						tiles.push_back(new ImageTile(tileSize, bitmap, x, y));
						int left = x*tileSize, bottom = (height-y)*tileSize, right = x*tileSize + 16, top = (height-y)*tileSize - 16;
						FIBITMAP* tileImage = FreeImage_Copy(bitmap, left, top, right, bottom);
						std::string tileName = "Tile";

						std::stringstream ss;
						ss << tileIndex;
						tileName.append(ss.str());
						std::cout << "New tile " << tileIndex << " " << left << " " << top << " " << right << " " << bottom << "\n";
						tileName = tileName.append(".png");
						FreeImage_Save(form, tileImage, tileName.c_str());
						//std::cin >> tileName;
							
						FreeImage_Unload(tileImage);
					}
					//std::cout << "middle3\n";
					file << tileIndex << " ";
					//delete tile;
				}
				file << "\n";
			}
			file.close();
			//FreeImage_Unload(bitmap);
		}
		std::cout << "out of this bracket";
		/*
		else
		{
			std::cout << "Could not open file!\n";
			FreeImage_Unload(bitmap);
		}*/
		std::cout << "what";
	}
	else
	{
		std::cout << "Not enough arguments!";
	}
	std::cout << "finish :)";
	//FreeImage_DeInitialise();
	return 0;
}