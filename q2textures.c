#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <physfs.h>

#define MAGIC 	(('P' << 24) + ('S' << 16) + ('B' << 8) + 'I')
#define HEADERLEN 4 * 40
#define TEXTURE 5
#define TEXTURE_LEN 76	

#define true 	1
#define false	0

uint32_t pos;
_Bool pak = false;

static _Bool usage(int32_t argc, char** argv) {
	if (argc < 2) {
		printf("Usage: %s [-p pakfile] <bspfile> [<bspfile>...]\n", argv[0]);
		return false;
	}

	if (argv[1][0] == '-' && argv[1][1] == 'p') {
		pak = true;
	}

	return true;
}

uint32_t ReadInt(char *data) {
	uint32_t i;
	i = ((data[pos+3] & 0xff) << 24) + 
		((data[pos+2] & 0xff) << 16) + 
		((data[pos+1] & 0xff) << 8) + 
		(data[pos] & 0xff);
	pos += 4;
	
	return i;
}	

void readData(char *data, size_t len, char *buf) {
	int32_t i;
	for (i = 0; i < len; i++) {
		buf[i] = data[pos];
		pos++;
	}
}

void printTextures(char *bspfile) {
	char header[160];

	uint32_t i;
	pos = 0;
	uint32_t offsets[19];
	uint32_t lengths[19];
	FILE *fp;
	PHYSFS_file *pfp;
	uint32_t check, version;
	char texture_name[32];

	memset(&offsets, 0, sizeof(offsets));
	memset(&lengths, 0, sizeof(lengths));
	
	if (pak) {
		if (!PHYSFS_exists(bspfile)) {
			printf("%s not found in pak\n", bspfile);
			return;
		}

		pfp = PHYSFS_openRead(bspfile);
		PHYSFS_read (pfp, header, sizeof(header[0]), HEADERLEN);
	} else {
		fp = fopen(bspfile, "r");
		fread(header, sizeof(header[0]), HEADERLEN, fp);
	}
	
	check = ReadInt(header);
	version = ReadInt(header);
	
	if (check != MAGIC) {
		printf("Invalid BPS file: %s\n", bspfile);
		return;
	}
	
	for (i=0; i<19; i++) {
		offsets[i] = ReadInt(header) - HEADERLEN;
		lengths[i] = ReadInt(header);
	}	
	
	char texture_lump[(lengths[TEXTURE])];
	
	// move to the texture lump
	if (pak) {
		PHYSFS_seek(pfp, offsets[TEXTURE] + HEADERLEN);
		PHYSFS_read(pfp, texture_lump, 1, lengths[TEXTURE]);
	} else {
		fseek(fp, offsets[TEXTURE] + HEADERLEN, SEEK_SET);
		fread(texture_lump, sizeof(char), lengths[TEXTURE], fp);
	}
	
	pos = 0;
	size_t texture_count = lengths[TEXTURE] / TEXTURE_LEN;
	
	for (i=0; i<texture_count; i++) {
		memset(&texture_name, 0, sizeof(texture_name));
		
		pos += 10 * sizeof(uint32_t); // skip origin and flags stuff, 10 integers
		readData(texture_lump, 32, texture_name);
		pos += 4;
		printf("%s\n", texture_name);
	}
	
	if (pak) {
		PHYSFS_close(pfp);
	} else {
		fclose(fp);
	}
}

int32_t main(int32_t argc, char** argv) {
	
	int32_t i;
	
	if (!usage(argc, argv)){
		return EXIT_FAILURE;
	}

	// init physfs if we're dealing with a pak file
	if (pak) {
		PHYSFS_init(NULL);
		PHYSFS_permitSymbolicLinks(true);
		PHYSFS_addToSearchPath(argv[2], true);
	}

	// loop through cli args
	for (i = pak ? 3 : 1; i<argc; i++) {
		printTextures(argv[i]);
	}
	
	return EXIT_SUCCESS;
}
