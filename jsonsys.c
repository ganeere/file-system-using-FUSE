

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include "cJSON.h"
//#include <cjson/cJSON.h>
//"../json-c/json.h"
//#include "./cJSON-master/cJSON.h"
#include <stdio.h>

#include "unity/examples/unity_config.h"
#include "unity/src/unity.h"
#include "common.h"





// char global_file_paths[10][1000];
// char global_file_values[10][1000] = {""};
// int global_file_counter;
cJSON *global_tree = NULL;

char global_file_paths[50][1000];
char global_file_values[50][1000] = {""};
int global_file_counter;

char global_all_paths[100][1000] = {""};
int all_paths_counter = 0;

char recursive_file_paths[50][1000] = {""};
char recursive_file_values[50][1000] = {""};
int recursive_file_counter=0;


char global_subvalues[100][1000] = {""};
int global_subvalues_counter = 0;




static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";



static cJSON *parse_file(const char *filename)
{
    cJSON *parsed = NULL;
    char *content = read_file(filename);

    parsed = cJSON_Parse(content);

    if (content != NULL)
    {
        free(content);
    }

    return parsed;
}


void get_json_file_content_recursive(cJSON *tree, char *previous)
{

	cJSON *parameter;
	
	char path_temp[1000] = "";
	cJSON_ArrayForEach(parameter, tree) {
		
		strcat(path_temp, previous);
		strcat(path_temp, "/");
		strcat(path_temp, parameter->string);
		// strcat(path_temp, "/");
		// printf("Path : %s\n", path_temp);
		strcpy(global_all_paths[all_paths_counter++], path_temp);
				
		if(parameter->valuestring != NULL){

			strcpy(recursive_file_paths[recursive_file_counter], path_temp);
			strcpy(recursive_file_values[recursive_file_counter], parameter->valuestring);
			// strcpy(recursive_file_values[recursive_file_counter++], path_temp);
			recursive_file_counter += 1;
			strcpy(path_temp, "");
			
		}

		get_json_file_content_recursive(parameter, path_temp);
		strcpy(path_temp, "");
	}
	
}
void get_subvalues_recursive(cJSON *tree, char *previous)
{
	// getsubitems(tree, 0);
	cJSON *parameter;
	cJSON *parameter2;
	char path_temp[1000] = "";
	cJSON_ArrayForEach(parameter, tree) {

		strcat(path_temp, previous);	
		if(strcmp(path_temp, "") == 0){
			strcpy(global_subvalues[global_subvalues_counter++], "/");
			// strcpy(global_subvalues[subvalues_counter++], path_temp);
		}
		else{
			strcpy(global_subvalues[global_subvalues_counter++], path_temp);		
		}


		strcpy(global_subvalues[global_subvalues_counter++], parameter->string);
		strcat(path_temp, "/");
		strcat(path_temp, parameter->string);

		get_subvalues_recursive(parameter, path_temp);
		strcpy(path_temp, "");
	}
	
}


void delete_recursive(cJSON *tree, char *previous, char *path_to_delete)
{

	cJSON *parameter;

	char path_temp[1000] = "";
	cJSON_ArrayForEach(parameter, tree) {

		strcat(path_temp, previous);
		strcat(path_temp, "/");
		strcat(path_temp, parameter->string);
		strcpy(global_all_paths[all_paths_counter++], path_temp);
		
		// DELETE EXAMPLE
		// if(strcmp(path_temp, "/class/systemprogramming/students/student4") == 0){
		if(strcmp(path_temp, path_to_delete) == 0){								 
			 printf("MATCHH ____\n, ");
			 cJSON_DeleteItemFromObjectCaseSensitive(tree, parameter->string);	

		}
		
		if(parameter->valuestring != NULL){
			strcpy(path_temp, "");		
		}

		delete_recursive(parameter, path_temp, path_to_delete);
		strcpy(path_temp, "");
	}

}


static int jsonsys_getattr(const char *path, struct stat *stbuf)
{
	printf("\n\n\n\nINSIDE jsonsys_getattr FUNCTION .\n\n\n\n");
    int res = 0;
	
    memset(stbuf, 0, sizeof(struct stat));
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    }
    else if (strcmp(path, hello_path) == 0) {
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = strlen(hello_str);
        // stbuf->st_size = strlen(hello_str2);
    }
	
	/*
		MY CODE AFTER HERE
	*/
    // else
        // res = -ENOENT;
	else{
		printf("\n rofs_getattr: CHECKING OTHERS: ... \n\n" );
	}	
	
	// memmove(path, path+1, strlen(path));
	
	// bool stop = false;
	int i,j;
	int is_file = 0;
	int size_file = 0;				// if path matches as file
	char base_[1000] = "testfolder/";

	for(i=0; i< all_paths_counter; i++){				//CHANGED with all_paths_counter

		char temp[1000] = "";
		// strcpy(temp, base_);
		strcat(temp, global_all_paths[i]);
		
		// printf("\n rofs_getattr: CHECKING: %s , with path: %s, ... \n\n", temp, path );


		// IF FILE
		for(j=0; j<recursive_file_counter; j++){						//CHANGED
		// for(j=0; j<global_file_counter; j++){
			char temp2[1000] = "";
			// strcpy(temp2, base_);
			strcat(temp2, recursive_file_paths[j]);						//CHANGED
			// strcat(temp2, global_file_paths[j]);			
			if (strcmp(path, temp2) == 0) {
				// printf("\n rofs_getattr: ATTRIBUTE MATCHED AS FILE ... \n\n" );
				is_file = 1;
				// size_file = strlen(global_file_values[j]);
				size_file = strlen(recursive_file_values[j]);
				break;
			}
		}

		// IF NOT FILE
		if( (strcmp(path, temp) == 0) && (is_file == 0)  ) {
			// printf("\n rofs_getattr: ATTRIBUTE MATCHED AS FOLDER ... \n\n" );
			stbuf->st_mode = S_IFDIR | 0755;
			stbuf->st_nlink = 2;
			// stbuf->st_size = 4096;		
			break;
		}
		//IF FILE
		if( (strcmp(path, temp) == 0) && (is_file == 1)  ) {
			// printf("\n rofs_getattr: ATTRIBUTE MATCHED AS FILE ... \n\n" );
			stbuf->st_mode = S_IFREG | 0444;
			stbuf->st_nlink = 1;
			// stbuf->st_size = 4096;		
			stbuf->st_size = size_file;		
		}				


		
		
	}
	
	
    return res;
}

static int jsonsys_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{
	printf("\n\n\n\nINSIDE jsonsys_readdir FUNCTION .\n\n\n\n");
	
    (void) offset;
    (void) fi;

    // if (strcmp(path, "/") != 0)
        // return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    // filler(buf, hello_path + 1, NULL, 0);
	
	/*
		MY CODE AFTER HERE
	*/
	int i, j;
	// for(i=0; i<global_file_counter; i++){
		// printf("global_file_paths[i]  : %s\n", global_file_paths[i]);
		// filler(buf, global_file_paths[i] , NULL, 0);
	// }
	
	
	// GET LISTING INSIDE A SPECIFIC DIRECTORY
	for(i=0; i<global_subvalues_counter; i++){
		if(strcmp(global_subvalues[i], path) == 0){

			printf("global_subvalues[i+1] :  %s \n", global_subvalues[i+1]);
			filler(buf, global_subvalues[i+1], NULL, 0);
		}	
		else{
			printf("\n\n\n rofs_readdir: _______ DOES NOT MATCH _____\n\n\n", path);
		}
			
	}	
	// memmove(path, path+1, strlen(path));
	// if (strcmp(path, "testfolder") == 0) {
		
		// filler(buf, "class", NULL, 0);
		// filler(buf, "club", NULL, 0);

	// }			
	// else if (strcmp(path, "class") == 0) {
		
		// filler(buf, "operatingsystem", NULL, 0);
		// filler(buf, "systemprogramming", NULL, 0);

	// }
	// else if (strcmp(path, "club") == 0) {
		
		// filler(buf, "basketball", NULL, 0);

	// }	
	// else if (strcmp(path, "class/systemprogramming") == 0) {
		
		// filler(buf, "students", NULL, 0);
		// filler(buf, "teachers", NULL, 0);
		// printf("\n\n\n rofs_readdir: _CONTENT FILLED _____\n\n\n", path);
	// }
	// else if (strcmp(path, "class/systemprogramming/teachers") == 0) {
		
		// filler(buf, "gokhansecinti", NULL, 0);
		// filler(buf, "turgutuyar", NULL, 0);
		// printf("\n\n\n rofs_readdir: _CONTENT FILLED _____\n\n\n", path);
	// }	

	// else{
		// printf("\n\n\n rofs_readdir: _______ DOES NOT MATCH _____\n\n\n", path);
	// }
	
	
    return 0;
}

static int jsonsys_open(const char *path, struct fuse_file_info *fi)
{
	printf("\n\n\n\nINSIDE jsonsys_open FUNCTION .\n\n\n\n");
	/*
		MY CODE AFTER HERE
	*/
    // if (strcmp(path, hello_path) != 0)
        // return -ENOENT;

    // if ((fi->flags & 3) != O_RDONLY)
        // return -EACCES;

    return 0;
}

static int jsonsys_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi)
{
	printf("\n\n\nINSIDE jsonsys_read FUNCTION .\n\n\n");
	/*
		MY CODE HERE
	*/
	
    size_t len;
    (void) fi;
    // if (strcmp(path, hello_path) != 0)
        // return -ENOENT;
	int i,j;
	// memmove(path, path+1, strlen(path));							//CHANGED
	for(i=0; i<recursive_file_counter; i++){						//CHANGED
	// for(i=0; i<global_file_counter; i++){
		printf("writing recursive_file_paths[i] : %s from path : %s\n", path, recursive_file_paths[i]);		//CHANGED
		// printf("writing global_file_paths[i] : %s from path : %s\n", path, global_file_paths[i]);
		if( strcmp(path, recursive_file_paths[i])  == 0){
		// if( strcmp(path, global_file_paths[i])  == 0){						//CHANGED
			// memcpy(buf, global_file_values[i], strlen(global_file_values[i]));
			
			strcpy(buf, recursive_file_values[i]);				//CHANGED
			// strcpy(buf, global_file_values[i]);
			// size = strlen(global_file_values[i]);
			size = strlen(recursive_file_values[i]);			//CHANGED
			break;
		}
		
		
	}	

    // len = strlen(hello_str);
    // // len = strlen(hello_str2);
    // if (offset < len) {
        // if (offset + size > len)
            // size = len - offset;
        // memcpy(buf, hello_str + offset, size);
        // // memcpy(buf, hello_str2 + offset, size);
    // } else
        // size = 0;

	////////	
	
	


    return size;
}




static int jsonsys_unlink(const char *path)
{
	printf("CALLED : rofs_unlink  with path : %s \n", path);
    // (void)path;
    // return -EROFS;
	
	
	// delete_recursive(global_tree, "", "/class/systemprogramming/students/student4");			//delete file from filesystem
	delete_recursive(global_tree, "", path);			//delete file from filesystem
	
	char *actual = NULL;
	actual = cJSON_Print(global_tree);
    printf("\nDELETED..\nNEW JSON CONTENT: \n %s   n\n\n", actual);	
    // printf("\n %s  \n" , actual);	
	//reinitialize data
	global_file_counter = 0;
	all_paths_counter = 0;
	recursive_file_counter=0;
	global_subvalues_counter = 0;
	
	get_json_file_content_recursive(global_tree, "");			//fill all paths and files with corresponding values
    get_subvalues_recursive(global_tree, "");	
}





// static int jsonsys_write(const char *path)
static int jsonsys_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *finfo)
{
	/*
		MODIFICATION TO FILE IS NOT ALLOWED
		
		BUT IT CAN BE IMPLEMENTED VIA
	*/
	printf("\n\n WRITE FUNCTION IS CALLED \n\n");

    // (void)path;
    // (void)buf;
    // (void)size;
    // (void)offset;
    // (void)finfo;
	
	
	// MODIFICATION OF FILE WITH JSON CONTENT
	// set_value_recursive(tree, "", "/class/systemprogramming/students/student1", "_");
	// set_value_recursive(global_tree, "", path, buf);
	
	
    return -EROFS;
}

static int jsonsys_truncate(const char *path)
// static int jsonsys_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *finfo)
{
	
	printf("\n\n jsonsys_truncate FUNCTION IS CALLED \n\n");

    (void)path;

    return -EROFS;
}

// static int jsonsys_mknod(const char *path, mode_t mode, dev_t rdev)
static int jsonsys_mknod(const char *path)
{
	/*
		FILE CREATION IS NORMALLY NOT ALLOWED BUT FOR BONUS IT IS IMPLEMENTED
	*/
	printf("\n\n jsonsys_truncate FUNCTION IS CALLED \n\n");
	// printf("\n\n jsonsys_truncate PARAMETERS: %s, %s, %s \n\n", path, mode, rdev);
	printf("\n\n jsonsys_truncate PARAMETERS: %s, %s, %s \n\n", path);
    // (void)path;
    // (void)mode;
    // (void)rdev;
	
    // return -EROFS;			// IMPLEMENT THIS IF FILE CREATION WOULD NOT BE ALLOWED
	
	//call function: add_new_json_content(path)
	
	
	return -EROFS;
}



static struct fuse_operations jsonsys_oper = {
    // .getattr	= hello_getattr,		//get attributes
    // .readdir	= hello_readdir,		//directory listing
    // .open	= hello_open,				//for opening
    // .read	= hello_read,				//read from file
	
	// .truncate   = hello_truncate,	// truncating file, for echo jksdnfjsdknf > test
	// .write  = hello_write,			// write file, for echo jksdnfjsdknf >> test
    // .unlink = hello_unlink,			// delete file
	// .mknod       = hello_mknod,		// create new file



    .getattr	= jsonsys_getattr,		//get attributes
    .readdir	= jsonsys_readdir,		//directory listing
    .open	= jsonsys_open,				//for opening
    .read	= jsonsys_read,				//read from file
	
	.truncate   = jsonsys_truncate,	// truncating file, for echo something > test
	.write  = jsonsys_write,			// write file, for echo something >> test
    .unlink = jsonsys_unlink,			// delete file
	.mknod       = jsonsys_mknod,		// create new file	
	
};

int main(int argc, char *argv[])
{
	
	// global_tree = get_json_file_content("example.json");
	global_tree = parse_file("example.json");
	get_json_file_content_recursive(global_tree, "");			//fill all paths and files with corresponding values
    get_subvalues_recursive(global_tree, "");					//fill directory contents
	
	// set_value_recursive(tree, "", "/class/systemprogramming/students/student1", "_");		//for json modification
	
	return fuse_main(argc, argv, &jsonsys_oper, NULL);
}


















