#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void usage()
{
	printf("usage : maputil <file> <options>\n");
	exit(-1);
}

int main(int argc, char * argv[])
{
	if(argc <= 2)
		usage();

	int file = open(argv[1], O_RDONLY);
	int g_width = 1, g_height = 1, g_objects = 1;
	int nb_obj, height, width, *objs, trashint;
	char trash;

	if(strcomp(argv[2], "--getwidth" || strcomp(argv[2], "--getinfo"))
		g_width = 0;
	if(strcomp(argv[2], "--getheight" || strcomp(argv[2], "--getinfo"))
		g_height = 0;
	if(strcomp(argv[2], "--getheight" || strcomp(argv[2], "--getinfo"))
		g_objects = 0;

	read(file, &height, sizeof(int));
	read(file, &trash, sizeof(char));
	read(file, &width, sizeof(int));
	read(file, &trash, sizeof(char));
	read(file, &nb_obj, sizeof(int));
	read(file, &trash, sizeof(char));

	if(g_height == 0)
		printf("Height : %d\n", height);
	if(g_width == 0)
		printf("Height : %d\n", width);
	if(g_objects == 0)
	{
		printf("objects : \n", );
		objs = malloc(nb_obj * sizeof(int));
		lseek(file, width * height * sizeof(int), SEEK_CUR
		for(int i = 0; i < nb_obj; ++i)
		{
			read(file, objs+i, sizeof(int));
			printf("%d, ", *(objs+i));
		}
		free(objs);
		printf("\n");
	}

	return 0;
}
