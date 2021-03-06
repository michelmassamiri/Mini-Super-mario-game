#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>


void usage(int err_msg)
{
	if(err_msg == 0)
		printf("usage : maputil <file> <options>\n");
	if(err_msg == 1)
		printf("usage : maputil <file> --setobjetcs { <filename> <frames> <solidity> <destructible> <collectible> <generator> }\n");
	if(err_msg == 2)
		printf("You must set at least as much object as in the save\n");
	exit(-1);
}

int main(int argc, char * argv[])
{
	if(argc <= 2)
		usage(0);

	int file = open(argv[1], O_RDONLY);
	int g_width = 1, g_height = 1, g_objects = 1;
	int nb_obj, height, width, objs, trashint;
	char trash, sdl = '\n';


	//cas ou on demande l'affichage de certaines parties de la sauvegarde


	if(strcmp(argv[2], "--getwidth") || strcmp(argv[2], "--getinfo"))
		g_width = 0;
	if(strcmp(argv[2], "--getheight") || strcmp(argv[2], "--getinfo"))
		g_height = 0;
	if(strcmp(argv[2], "--getobjects") || strcmp(argv[2], "--getinfo"))
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
		printf("objects : \n");
		lseek(file, width * height * sizeof(int) + sizeof(char), SEEK_CUR);
		char *name;
		int nb_char_name;
		for(int i = 0; i < nb_obj; ++i)
		{
			read(file, &nb_char_name, sizeof(int));
			name = malloc(nb_char_name * sizeof(char));
			read(file, name, nb_char_name*sizeof(char));
			printf("%s : ", name);
			read(file, &objs, sizeof(int));
			printf("frames : %d, ", objs);
			read(file, &objs, sizeof(int));
			printf("solidity : %d, ", objs);
			read(file, &objs, sizeof(int));
			printf("destructibility : %d, ", objs);
			read(file, &objs, sizeof(int));
			printf("collectability :%d, ", objs);
			read(file, &objs, sizeof(int));
			printf("generator : %d, ", objs);
			free(name);
		}
		printf("\n");
	}


	//cas ou on veut réécrire la width ou la height;

	if(strcmp(argv[2], "--setwidth") || strcmp(argv[2], "--setheight"))
	{
		int size = height * width;
		int oldw = width;
		int tmp;
		tmp = atoi(argv[3]);
		if(strcmp(argv[2], "--setwidth"))
			width = tmp;
		if(strcmp(argv[2], "--setheight"))
			height = tmp;

		int tmp_file =  open("tmp", O_RDWR | O_TRUNC | O_CREAT, 0777);

		write(tmp_file, &height, sizeof(int));
		write(tmp_file, &sdl, sizeof(char));
		write(tmp_file, &width, sizeof(int));
		write(tmp_file, &sdl, sizeof(char));
		write(tmp_file, &nb_obj, sizeof(int));
		write(tmp_file, &sdl, sizeof(char));

		for(int i  =0; i < height; ++i)
		{
			for(int j = 0; j < width; ++j)
			{
				if(j >= oldw)
				{
					for(int k = 0; k < (width-oldw); ++k)
						read(file, &objs, sizeof(int));
					continue;
				}
				if(i*j < size)
					read(file, &objs, sizeof(int));
				else
					objs = 0;
				write(tmp_file, &objs, sizeof(int));
			}
		}

		while(read(file, &trash, sizeof(char)))
		{
			write(tmp_file, &trash, sizeof(char));
		}

		close(file);
		close(tmp_file);

		if(fork() == 0)
		execl("rm", "rm", argv[1], NULL);
		else
		wait(NULL);
		execlp("mv", "mv", "tmp", argv[1]);

	}


	//gestion de la reinitialisation des objets
	if(strcmp(argv[2], "--setobjects"))
	{
		if(((argc - 3) % 6) != 0)
			usage(1);
		if(((argc-3)/6) < nb_obj)
			usage(2);

		int tmp_file =  open("tmp", O_RDWR | O_TRUNC | O_CREAT, 0777);
		char sdl = '\n';
		write(tmp_file, &height, sizeof(int));
		write(tmp_file, &sdl, sizeof(char));
		write(tmp_file, &width, sizeof(int));
		write(tmp_file, &sdl, sizeof(char));
		write(tmp_file, &nb_obj, sizeof(int));
		write(tmp_file, &sdl, sizeof(char));

		for(int i  =0; i < height*width; ++i)
		{
			read(file, &objs, sizeof(int));
			write(tmp_file, &objs, sizeof(int));
		}

		write(tmp_file, &sdl, sizeof(char));

		int len, nb_frames, tr = 1, fa = 0;
		for(int i = 3; i < argc; i += 6)
		{
			len = strlen(argv[i]);
			write(tmp_file, &len, sizeof(int));
			write(tmp_file, argv[i], len * sizeof(char));
			nb_frames = atoi(argv[i+1]);
			write(tmp_file, &nb_frames, sizeof(int));

			if(strcmp(argv[i+2], "solid"))
				write(tmp_file, &tr, sizeof(int));
			else if(strcmp(argv[i+2], "not-solid"))
				write(tmp_file, &fa, sizeof(int));

			if(strcmp(argv[i+3], "destructible"))
				write(tmp_file, &tr, sizeof(int));
			else if(strcmp(argv[i+3], "not-destructible"))
				write(tmp_file, &fa, sizeof(int));

			if(strcmp(argv[i+4], "collectible"))
				write(tmp_file, &tr, sizeof(int));
			else if(strcmp(argv[i+4], "not-collectible"))
				write(tmp_file, &fa, sizeof(int));

			if(strcmp(argv[i+5], "generator"))
				write(tmp_file, &tr, sizeof(int));
			else if(strcmp(argv[i+5], "not-generator"))
				write(tmp_file, &fa, sizeof(int));
		}


		close(file);
		close(tmp_file);

		if(fork() == 0)
		    execl("rm", "rm", argv[1], NULL);
		else
		    wait(NULL);
		execlp("mv", "mv", "tmp", argv[1]);
	}


	if(strcmp(argv[2], "--pruneobjects"))
	{
		int *areused = malloc(nb_obj * sizeof(int)), willuse = malloc(nb_obj * sizeof(int)), nb_obj_rly_used = 0;

		for(int i = 0; i < nb_obj; ++i)
		{
			areused[i] = 0;
			willuse[i] = i;
		}

		for(int i  =0; i < height*width; ++i)
		{
			read(file, &objs, sizeof(int));
			areused[objs] = 1;
		}

		for(int i = 0; i < nb_obj; ++i)
		{
			++nb_obj_rly_used;
			if(areused[i] == 0)
			{
				--nb_obj_rly_used;
				for(int j = i; j < nb_obj-1; ++j)
					willuse[j] = j+1;
					willuse[nb_obj - 1] = 0;
			}
		}

		lseek(file, 0, SEEK_SET);
		int tmp_file =  open("tmp", O_RDWR | O_TRUNC | O_CREAT, 0777);
		char sdl = '\n';
		write(tmp_file, &height, sizeof(int));
		write(tmp_file, &sdl, sizeof(char));
		write(tmp_file, &width, sizeof(int));
		write(tmp_file, &sdl, sizeof(char));
		write(tmp_file, &nb_obj, sizeof(int));
		write(tmp_file, &sdl, sizeof(char));

		for(int i  =0; i < height*width; ++i)
		{
			read(file, &objs, sizeof(int));
			write(tmp_file, &willuse[objs], sizeof(int));
		}
		write(tmp_file, &sdl, sizeof(char));

        int len, des, col, gen, sol, fra;
        char *name;
		for(int i = 0, j = 0; i < nb_obj; ++i)
		{
            read(file, &len, sizeof(int));
            name = malloc(len * sizeof(char));
            read(file, name, len*sizeof(char));

            read(file, &fra, sizeof(int));
            read(file, &sol, sizeof(int));
            read(file, &des, sizeof(int));
            read(file, &col, sizeof(int));
            read(file, &gen, sizeof(int));
            read(file, &trash, sizeof(char));

            if(willuse[j] != i)
            {
                free(name);
                continue;
            }

            write(tmp_file, &len, sizeof(int));
            write(tmp_file, name, len*sizeof(char));
            write(tmp_file, &fra, sizeof(int));
            write(tmp_file, &sol, sizeof(int));
            write(tmp_file, &des, sizeof(int));
            write(tmp_file, &col, sizeof(int));
            write(tmp_file, &gen, sizeof(int));
            write(tmp_file, &sdl, sizeof(char));

            ++j;
            free(name);
		}

        close(file);
        close(tmp_file);

        if(fork() == 0)
            execl("rm", "rm", argv[1], NULL);
        else
            wait(NULL);
        execlp("mv", "mv", "tmp", argv[1]);
	}



	close(file);

	return 0;
}
