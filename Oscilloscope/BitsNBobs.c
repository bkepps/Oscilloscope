//#include "BitsNBobs.h"

#pragma warning(disable : 4996)

/*ideal for getting path of a file, set orgin to SDL_GetBasePath(), and for toAppend give hard coded string*/
const char* BitsNBobs_append(const char* orgin, char* toAppend) {
	int orginSize = strlen(orgin);
	int toAppendSize = strlen(toAppend);
	char* dest;
	if (dest = malloc(sizeof(char) * (orginSize + toAppendSize + 1))) {
		int i = 0;
		dest[0] = '\0';
		strcat(dest, orgin);
		strcat(dest, toAppend);
		return (const char*)dest;
	}
	else {
		//malloc failed, but I don't have any proper logging happening in here, so it returns f
		return "f";
	}	
}

int BitsNBobs_mapTo(int x, int inMin, int inMax, int outMin, int outMax) {
	return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}