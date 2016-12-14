#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "map.h"
#include "error.h"

#ifdef PADAWAN

void map_new (unsigned width, unsigned height)
{
  map_allocate (width, height);

  for (int x = 0; x < width; x++)
    map_set (x, height - 1, 0); // Ground

  for (int y = 0; y < height - 1; y++) {
    map_set (0, y, 1); // Wall
    map_set (width - 1, y, 1); // Wall
  }

  map_object_begin (6);

  // Texture pour le sol
  map_object_add ("images/ground.png", 1, MAP_OBJECT_SOLID);
  // Mur
  map_object_add ("images/wall.png", 1, MAP_OBJECT_SOLID);
  // Gazon
  map_object_add ("images/grass.png", 1, MAP_OBJECT_SEMI_SOLID);
  // Marbre
  map_object_add ("images/marble.png", 1, MAP_OBJECT_SOLID | MAP_OBJECT_DESTRUCTIBLE);
	map_object_add("images/flower.png", 1, MAP_OBJECT_AIR | MAP_OBJECT_DESTRUCTIBLE);
	map_object_add("images/coin.png", 20, MAP_OBJECT_AIR | MAP_OBJECT_COLLECTIBLE); 
  map_object_end ();

}

void map_save (char *filename)
{
  	// TODO
	// Elements qu'on doit avoir en début de fichier
	// height/width
	// nombre d'objets maximum possible sur la map
	// contenu de chaque case de la carte
	// caractéristique de chaque objet : nom du fichier, nombre de sprite, propriétés
	int height = map_height(), width = map_width(), nb_obj = map_objects();

	int cur_obj;
	int file = open("save.map", O_CREAT | O_TRUNC | O_WRONLY, 0777);


	write(file, &height, sizeof(int));
	write(file, "\n", 1);
	write(file, &width, sizeof(int));
	write(file, "\n", 1);
	write(file, &nb_obj, sizeof(int));
	write(file, "\n", 1);
	
	for(int i = 0; i < width; ++i)
	{
		for(int j = 0; j < height; ++j)
		{
			//do some stuff with
			cur_obj = map_get(i, j);
			write(file, &cur_obj, sizeof(int));
		}
	}


	write(file, "\n", 1);
	int len = 0, frames, solidity, destr, collec, gen;

	for(int i = 0; i < nb_obj; ++i)
	{
		len = strlen(map_get_name(i));
		write(file, &len, sizeof(int));
		write(file, map_get_name(i), len);
		
		frames = map_get_frames(i);
		solidity = map_get_solidity(i);
		destr = map_is_destructible(i);
		collec = map_is_collectible(i);
		gen = map_is_generator(i);
		
		write(file, &frames, sizeof(int));		
		write(file, &solidity, sizeof(int));		
		write(file, &destr, sizeof(int));		
		write(file, &collec, sizeof(int));		
		write(file, &gen, sizeof(int));
		write(file, "\n", 1);	
	}

	close(file);

}

void map_load (char *filename)
{
  	// TODO
  	int file = open(filename, O_RDONLY);
	int height, width, nb_obj = 0;
	char n;
	read(file, &height, sizeof(int));
	read(file, &n, 1);	
	read(file, &width, sizeof(int));
	read(file, &n, 1);	
	read(file, &nb_obj, sizeof(int));
	read(file, &n, 1);
	printf("%d, %d\n", width, nb_obj);
	map_allocate(width, height);
		
	int obj;
	for(int i = 0; i < width; ++i)
	{
		for(int j = 0; j < height; ++j)
		{
			read(file, &obj, sizeof(int));
			map_set(i, j, obj);
		}
	}
	printf("hello\n");
	read(file, &n, 1);

	int len, frames, solidity, destr, collec, gen;
	char name[100];
	map_object_begin(nb_obj);
	printf("map_object_pass\n");
	for(int i = 0; i < nb_obj; ++i)
	{
		read(file, &len, sizeof(int));
		read(file, name, len);
		name[len] = '\0';
		read(file, &frames, sizeof(int));		
		read(file, &solidity, sizeof(int));		
		read(file, &destr, sizeof(int));		
		read(file, &collec, sizeof(int));		
		read(file, &gen, sizeof(int));
		printf("%s\n", name);		
		read(file, &n, 1);
		map_object_add(name, frames, solidity | destr | collec | gen);
	}
	map_object_end();

	close(file);
}

#endif
