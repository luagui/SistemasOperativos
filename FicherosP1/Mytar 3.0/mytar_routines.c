#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "limits.h"
#include "mytar.h"

extern char *use;

void freeStrings(stHeaderEntry *array, int num){
	int i;
	for (i = 0; i < num; i++) {
		free(array[i].name);
	}
}

/** Copy nBytes bytes from the origin file to the destination file.
 *
 * origin: pointer to the FILE descriptor associated with the origin file
 * destination:  pointer to the FILE descriptor associated with the destination file
 * nBytes: number of bytes to copy
 *
 * Returns the number of bytes actually copied or -1 if an error occured.
 */
int copynFile(FILE * origin, FILE * destination, int nBytes)
{	
	// Complete the functions

	char c;
	int count = 0;
	while (count < nBytes) {
		c = getc(origin);
		if (ferror(origin)) {
			return -1;
		}
		if (feof(origin)) {
			return count;
		}
		count += 1;
		putc(c, destination);

		if (ferror(destination)) {
			return -1;
		}
	}

	return count;
}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor 
 * 
 * The loadstr() function must allocate memory from the heap to store 
 * the contents of the string read from the FILE. 
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc()) 
 * 
 * Returns: !=NULL if success, NULL if error
 */
char* loadstr(FILE *file)
{
	// Complete the function
	char c;
	int count = 0;
	c = getc(file);
	count += 1;
	if ( ferror(file) || c == feof(file)) { //Archivo vacio
		return NULL;
	}
	while (c != '\0' ) {
		c = getc(file);
		count += 1;
		if ( ferror(file)){
			return NULL;
		}
	}
	
	char *buffer = malloc(sizeof(char)*(count));
	fseek(file, -count, SEEK_CUR);
	
	int i;
	for (i = 0; i < count; i++) {
		buffer[i] = getc(file);
		if (ferror(file)) {
			free(buffer);
			return NULL;
		}
	}
	 return buffer;
}

/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor 
 * nFiles: output parameter. Used to return the number 
 * of files stored in the tarball archive (first 4 bytes of the header).
 *
 * On success it returns the starting memory address of an array that stores 
 * the (name,size) pairs read from the tar file. Upon failure, the function returns NULL.
 */
stHeaderEntry* readHeader(FILE * tarFile, int *nFiles)
{
	// Complete the function
	stHeaderEntry* array=NULL;

	fread(nFiles,sizeof(int), 1, tarFile);

	if (ferror(tarFile)) {  
		fclose(tarFile);
		return NULL;
	} 

	array = malloc(sizeof(stHeaderEntry) * (*nFiles));

	int i;

	for (i = 0; i < *nFiles; i++) {
		array[i].name = loadstr(tarFile);

		if (array[i].name == NULL) {
			fclose(tarFile);
			freeStrings(array, *nFiles); //Borramos primero los strings
			free(array); //Una vez eliminados los strings, borramos la memoria de esta funcion
			return NULL;
		}

		fread(&array[i].size, sizeof(unsigned int), 1, tarFile);

		if (ferror(tarFile)) {  
			fclose(tarFile);
			freeStrings(array,*nFiles); //Borramos primero los strings
			free(array); //Una vez eliminados los strings, borramos la memoria de esta funcion
			return NULL;
		} 
	}

	return array;
}

/** Creates a tarball archive 
 *
 * nfiles: number of files to be stored in the tarball
 * filenames: array with the path names of the files to be included in the tarball
 * tarname: name of the tarball archive
 * 
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First reserve room in the file to store the tarball header.
 * Move the file's position indicator to the data section (skip the header)
 * and dump the contents of the source files (one by one) in the tarball archive. 
 * At the same time, build the representation of the tarball header in memory.
 * Finally, rewind the file's position indicator, write the number of files as well as 
 * the (file name,file size) pairs in the tar archive.
 *
 * Important reminder: to calculate the room needed for the header, a simple sizeof 
 * of stHeaderEntry will not work. Bear in mind that, on disk, file names found in (name,size) 
 * pairs occupy strlen(name)+1 bytes.
 *
 */
int
createTar(int nFiles, char *fileNames[], char tarName[])
{
	// Complete the function
	int space = sizeof(int) + nFiles * sizeof(unsigned int);
	int i;

	stHeaderEntry* aux =NULL;

	for (i= 0; i < nFiles; i++) {
		space += strlen(fileNames[i]) + 1;
	}

	FILE *tar = fopen(tarName, "w");
	if (tar == NULL) {
		return EXIT_FAILURE;
	}

	if (fseek(tar, space, SEEK_CUR) != 0) { //Dejamos hueco para la cabecera y los pares Nombre/Tamaño. SI da distito de 0, error
		fclose(tar);
		return EXIT_FAILURE;
	} 

	aux = malloc(sizeof(stHeaderEntry) * nFiles);
	for (i=0; i < nFiles; i++) {
			FILE *read = fopen(fileNames[i], "r");
			if (read == NULL) {
				fclose(tar);
				fclose(read);
				free(aux);
				return EXIT_FAILURE;
			}

			aux[i].size = copynFile(read, tar, INT_MAX);
			if (aux[i].size == -1) {
				fclose(tar);
				fclose(read);
				free(aux);
				return EXIT_FAILURE;
			}
			fclose(read);
			aux[i].name = fileNames[i];
	}

	rewind(tar);
	fwrite(&nFiles, sizeof(int), 1, tar);
	if (ferror(tar)) {
		fclose(tar);
		free(aux);
		return EXIT_FAILURE;
	}

	for (i=0; i < nFiles; i++) {
			fwrite(aux[i].name, strlen(fileNames[i]) + 1, 1, tar);
			if (ferror(tar)) {
				fclose(tar);
				free(aux);
				return EXIT_FAILURE;
			}
			fwrite(&aux[i].size, sizeof(unsigned int), 1, tar);
			if (ferror(tar)) {
				fclose(tar);
				free(aux);
				return EXIT_FAILURE;
			}
	}

	fclose(tar);
	free(aux);
	return EXIT_SUCCESS;
}

/** Extract files stored in a tarball archive
 *
 * tarName: tarball's pathname
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First load the tarball's header into memory.
 * After reading the header, the file position indicator will be located at the 
 * tarball's data section. By using information from the 
 * header --number of files and (file name, file size) pairs--, extract files 
 * stored in the data section of the tarball.
 *
 */
int
extractTar(char tarName[])
{
	int numFiles;
	stHeaderEntry* aux = NULL;
	FILE *tar = fopen(tarName,"r");

	if (tar == NULL) {
		return EXIT_FAILURE;
	}

	aux = readHeader(tar, &numFiles);

	if (aux == NULL) {
		freeStrings(aux, numFiles);
		fclose(tar);
		return EXIT_FAILURE;
	}

	int i;

	for (i = 0; i < numFiles; i++) {
		FILE *write = fopen(aux[i].name, "w");
		if (write == NULL) {
			freeStrings(aux, numFiles);
			fclose(tar);
			fclose (write);
			return EXIT_FAILURE;
		}
		int k = copynFile(tar, write, aux[i].size);
		if ( k == -1) {
			freeStrings(aux, numFiles);
			fclose(tar);
			fclose (write);
			return EXIT_FAILURE;
		}
		fclose(write);
	}

	freeStrings(aux, numFiles);
	fclose(tar);
	// Complete the function
	return EXIT_SUCCESS;
}
