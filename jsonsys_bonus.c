

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


cJSON *global_tree = NULL;						//GLOBAL JSON TREE OBJECT

int global_file_counter;

char global_all_paths[100][1000] = {""};			// HOLD ALL POSSIBLE PATHS
int all_paths_counter = 0;							// HOLD INDEX FOR PATHS (TOTAL NUMBER OF PATHS)

char recursive_file_paths[50][1000] = {""};				// HOLD PATH FOR  WHICH ARE SPECIFIED AS FILE
char recursive_file_values[50][1000] = {""};			// HOLD VALUES FOR  WHICH ARE SPECIFIED AS FILE
int recursive_file_counter=0;							// HOLD INDEX FOR FILES (TOTAL NUMBER OF FILES)


char global_subvalues[100][1000] = {""};			// HOLD SUBCONTENTS FOR PATHS
int global_subvalues_counter = 0;					// HOLD INDEX FOR SUBCONTENTS



/*
	PARSE JSON FROM FILE
*/
static cJSON *parse_file(const char *filename)
{
    cJSON *parsed_json = NULL;
    char *content = read_file(filename);
    parsed_json = cJSON_Parse(content);

    if (content != NULL)
    {
        free(content);
    }

    return parsed_json;
}


/*
	RECURSIVELY TRAVERSE JSON AND FILL FILES ARRAY FOR GETTING FILE CONTENTS AND FILENAMES
*/
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


/*
	RECURSIVELY TRAVERSE JSON AND FILL SUBVALUES ARRAY FOR DIRECTORY LISTING
		i.e. FOR EVERY DIRECTORY, GET DIRECTORY CONTENTS INSIDE THAT DIRECTORY
*/
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

/*
	RECURSIVELY TRAVERSE JSON AND DELETE SPECIFIED
*/
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
			 // printf("MATCHH ____\n, ");
			 cJSON_DeleteItemFromObjectCaseSensitive(tree, parameter->string);	

		}
		
		if(parameter->valuestring != NULL){
			strcpy(path_temp, "");		
		}

		delete_recursive(parameter, path_temp, path_to_delete);
		strcpy(path_temp, "");
	}

}

/*
	RECURSIVELY TRAVERSE JSON AND CHANGE A NODE CONTENT (FILE CONTENT) BY ANOTHER VALUE
*/
void set_value_recursive(cJSON *tree, char *previous, char *filepath, char *value_to_set)
{
	
	cJSON *parameter;
	// char path_base[1000] = "";			# defined below

	char path_temp[1000] = "";
	cJSON_ArrayForEach(parameter, tree) {

		strcat(path_temp, previous);
		strcat(path_temp, "/");
		strcat(path_temp, parameter->string);
		// strcat(path_temp, "/");
				
		if(parameter->valuestring != NULL){
			// SET NEW VALUE EXAMPLE
			// if(strcmp(path_temp, "/class/systemprogramming/students/student4") == 0){
			if(strcmp(path_temp, filepath) == 0){
				// printf("NAME MATCH ... : %s \n, ", parameter->string);
				// parameter->valuestring = "999999999";
				parameter->valuestring = value_to_set;
			}
		}

		set_value_recursive(parameter, path_temp, filepath, value_to_set);
		strcpy(path_temp, "");
	}
	
}



/*
	RECURSIVELY TRAVERSE JSON AND INSERT A NODE INTO APPROPRIATE LOCATION
*/
void create_node_recursive(cJSON *tree, char *previous, char *filepath, char *value_to_set)
{
	
	cJSON *parameter;
	cJSON *parameter_new = cJSON_CreateString(value_to_set);
	// char path_base[1000] = "";			# defined below

	char path_temp[1000] = "";
	cJSON_ArrayForEach(parameter, tree) {

		strcat(path_temp, previous);
		strcat(path_temp, "/");
		strcat(path_temp, parameter->string);
		// strcat(path_temp, "/");
		
		char path_temp_slashed[1000] = "";
		strcat(path_temp_slashed, path_temp);
		strcat(path_temp_slashed, "/");
		// printf("COMPARE ... : %s WITH %s \n, ",path_temp,  filepath);
		printf("COMPARE ... : %s WITH %s \n, ",path_temp_slashed,  filepath);
		// if(strcmp(path_temp, filepath) == 0){
		if(strcmp(path_temp_slashed, filepath) == 0){
			printf("NAME MATCH ... : %s \n, ", parameter->string);
			// cJSON *parameter_new = cJSON_AddObjectToObject(parameter, value_to_set);
			cJSON_AddItemToObject(parameter, value_to_set, parameter_new);
			parameter_new->valuestring = "empty";

		}				


		create_node_recursive(parameter, path_temp, filepath, value_to_set);
		strcpy(path_temp, "");
	}
	
}


static int jsonsys_getattr(const char *path, struct stat *stbuf)
{
	printf("\n\n\n\nINSIDE jsonsys_getattr FUNCTION . path: %s\n\n\n\n", path);
    int res = 0;
	int matched=0;
    memset(stbuf, 0, sizeof(struct stat));
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
		matched = 1;
    }
	
	
	// memmove(path, path+1, strlen(path));
	
	// bool stop = false;
	int i,j;
	int is_file = 0;
	int size_file = 0;				// if path matches as file
	
	for(i=0; i< all_paths_counter; i++){				//CHANGED with all_paths_counter

		char temp[1000] = "";
		// strcpy(temp, base_);
		strcat(temp, global_all_paths[i]);	
		// printf("\n jsonsys_getattr: CHECKING: %s , with path: %s, ... \n\n", temp, path );

		// CHECK IF FILE
		for(j=0; j<recursive_file_counter; j++){						//CHANGED
		// for(j=0; j<global_file_counter; j++){
			char temp2[1000] = "";
			// strcpy(temp2, base_);
			strcat(temp2, recursive_file_paths[j]);						//CHANGED
			// printf("\n jsonsys_getattr: Compare %s with %s \n", path, temp2 );
			
			// strcat(temp2, global_file_paths[j]);			
			if (strcmp(path, temp2) == 0) {
				// printf("\n jsonsys_getattr: ATTRIBUTE MATCHED AS FILE ... \n\n" );
				is_file = 1;
				// size_file = strlen(global_file_values[j]);
				size_file = strlen(recursive_file_values[j]);
				break;
			}
		}

		// IF NOT FILE
		if( (strcmp(path, temp) == 0) && (is_file == 0)  ) {
			printf("\n jsonsys_getattr: ATTRIBUTE MATCHED AS FOLDER ... \n\n" );
			stbuf->st_mode = S_IFDIR | 0755;
			stbuf->st_nlink = 2;
			// stbuf->st_size = 4096;		
			matched = 1;
			break;
		}
		//IF FILE
		if( (strcmp(path, temp) == 0) && (is_file == 1)  ) {
			printf("\n jsonsys_getattr: ATTRIBUTE MATCHED AS FILE ... \n\n" );
			stbuf->st_mode = S_IFREG | 0777;					// ALL PERMISSIONS ALLOWED
			stbuf->st_nlink = 1;
			// stbuf->st_size = 4096;		
			stbuf->st_size = size_file;		
			matched = 1;
			break;
			
		}				


		
		
	}
	
	if(matched == 0)
		return -ENOENT;
	
    return res;
}

static int jsonsys_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{
	printf("\n\n\n\nINSIDE jsonsys_readdir FUNCTION .\n\n\n\n");
	
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

	/*
		MY CODE AFTER HERE
	*/
	int i, j;

	
	// GET LISTING INSIDE A SPECIFIC DIRECTORY
	for(i=0; i<global_subvalues_counter; i++){
		if(strcmp(global_subvalues[i], path) == 0){

			// printf("global_subvalues[i+1] :  %s \n", global_subvalues[i+1]);
			filler(buf, global_subvalues[i+1], NULL, 0);
		}	
		else{
			printf("\n\n\n jsonsys_readdir: _______ DOES NOT MATCH _____\n\n\n", path);
		}
			
	}	
	// memmove(path, path+1, strlen(path));

	
	
    return 0;
}

static int jsonsys_open(const char *path, struct fuse_file_info *fi)
{
	printf("\n\n\n\nINSIDE jsonsys_open FUNCTION .\n\n\n\n");


    return 0;
}

static int jsonsys_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi)
{
	printf("\n\n\nINSIDE jsonsys_read FUNCTION .\n\n\n");
	/*
		MY CODE HERE
	*/
	
	int ret_val = 0;
    size_t len;

	int i,j;
	// memmove(path, path+1, strlen(path));							//CHANGED
	for(i=0; i<recursive_file_counter; i++){						//CHANGED
		printf("comparing recursive_file_paths[i] : %s from path : %s\n", path, recursive_file_paths[i]);		//CHANGED
		if( strcmp(path, recursive_file_paths[i])  == 0){					//CHANGED
			memcpy(buf, recursive_file_values[i], strlen(recursive_file_values[i]));
			
			strcpy(buf, recursive_file_values[i]);				//CHANGED
			// strcpy(buf, global_file_values[i]);
			// size = strlen(global_file_values[i]);
			size = strlen(recursive_file_values[i]);			//CHANGED
			ret_val = size;
			break;
		}
		
		
	}	


	////////	

    // return size;
    return ret_val;
}




/*
	DELETE A FILE
*/
static int jsonsys_unlink(const char *path)
{
	printf("CALLED : jsonsys_unlink  with path : %s \n", path);
    // (void)path;
    // return -EROFS;
	
	// delete_recursive(global_tree, "", "/class/systemprogramming/students/student4");			//delete file from filesystem
	delete_recursive(global_tree, "", path);			//delete file from filesystem
	char *actual = NULL;
	actual = cJSON_Print(global_tree);
    printf("\nDELETED..\nNEW JSON CONTENT: \n %s   n\n\n", actual);		// print new json content for debugging
	//reinitialize data
	all_paths_counter = 0;
	recursive_file_counter=0;
	global_subvalues_counter = 0;
	
	get_json_file_content_recursive(global_tree, "");			//fill all paths and files with corresponding values
    get_subvalues_recursive(global_tree, "");	
}




/*
	WRITE SOMETHING TO A FILE
*/
static int jsonsys_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *finfo)
{
	/*
		MODIFICATION TO FILE IS NOT ALLOWED
		
		BUT IT CAN BE IMPLEMENTED VIA WRITE
	*/
	printf("\n\n WRITE FUNCTION IS CALLED, path: %s, buf: %s \n\n", path, buf);

	set_value_recursive(global_tree, "", path, buf);	
	
	//REINITIALIZE CONTENT OF JSON
	all_paths_counter = 0;
	recursive_file_counter=0;
	global_subvalues_counter = 0;
	
	get_json_file_content_recursive(global_tree, "");			//fill all paths and files with corresponding values
    get_subvalues_recursive(global_tree, "");	
	
	char *actual = NULL;
	actual = cJSON_Print(global_tree);
	printf("\n jsonsys_write..\nNEW JSON CONTENT: \n %s   n\n\n", actual);		// print new json content for debugging

	

    // return -EROFS;
    return size;
}


//neccessary for truncating file
static int jsonsys_truncate(const char *path)
{
	
	printf("\n\n jsonsys_truncate FUNCTION IS CALLED \n\n");



    // return -EROFS;
    return 0;
}

static int jsonsys_mknod(const char *path, mode_t mode, dev_t rdev)
// static int jsonsys_mknod(const char *path)
{
	/*
		FILE CREATION IS NORMALLY NOT ALLOWED BUT FOR BONUS IT IS IMPLEMENTED
	*/
	printf("\n\n jsonsys_mknod FUNCTION IS CALLED \n\n");
	// printf("\n\n jsonsys_truncate PARAMETERS: %s, %s, %s \n\n", path, mode, rdev);
	printf("\n\n jsonsys_mknod PARAMETERS: %s, %s, %s \n\n", path);


	return 0;
}


/*
	CREATE A FILE
*/
static int jsonsys_create(const char *path, mode_t mode, dev_t rdev)
// static int jsonsys_mknod(const char *path)
{
	/*
		FILE CREATION IS NORMALLY NOT ALLOWED BUT FOR BONUS IT IS IMPLEMENTED
	*/
	printf("\n\n jsonsys_create FUNCTION IS CALLED \n\n");
	// printf("\n\n jsonsys_truncate PARAMETERS: %s, %s, %s \n\n", path, mode, rdev);
	
	int i,j;
	int size_init = strlen(path);
	int path_size = size_init;
	for(i=size_init; i>0; i--){
		printf("\n\n path[i] %c\n\n", path[i]);
		if(path[i] == '/')
			break;
		else
			path_size -= 1;
		
	}
	
	// Extract real path to file and filename from path value
	char path_new[50] = "";
	char value_new[50] = "";
	// memcpy(path_new, path, path_size+1);
	for(i= 0; i<path_size+1;i++){
		printf("writingggg : %c \n", path[i]);
		path_new[i] = path[i];

	}	
	for(i= 0; i<size_init;i++){
		value_new[i] = path[i+path_size+1];
		printf("wrote: %c \n", value_new[i]);
	}
		
	
	printf("\n\n jsonsys_create path:  %s  path_size: %d, val_new: %s\n\n", path_new, path_size, value_new);
	//create new json node with nodename: path_new and nodevalue: value_new
	create_node_recursive(global_tree, "", path_new, value_new);
	printf("\n\n jsonsys_create recursion called ... \n");

	// UPDATE TREE CONTENTS
	all_paths_counter = 0;
	recursive_file_counter=0;
	global_subvalues_counter = 0;
	
	get_json_file_content_recursive(global_tree, "");			//fill all paths and files with corresponding values
    get_subvalues_recursive(global_tree, "");	
	
	// char *actual = NULL;
	// actual = cJSON_Print(global_tree);
	// printf("\n jsonsys_write..\nNEW JSON CONTENT: \n %s   n\n\n", actual);		// print new json content for debugging

		
	//call function: add_new_json_content(path)
	
	
	// return -EROFS;
	return 0;
}

static struct fuse_operations jsonsys_oper = {



    .getattr	= jsonsys_getattr,		//get attributes
    .readdir	= jsonsys_readdir,		//directory listing
    .open	= jsonsys_open,				//for opening
    .read	= jsonsys_read,				//read from file
	
	// .truncate   = jsonsys_truncate,	// truncating file, for echo something > test
	.write  = jsonsys_write,			// write file, for echo something >> test
    .unlink = jsonsys_unlink,			// delete file
	.mknod       = jsonsys_mknod,		// create new node	(special files included)
	.create       = jsonsys_create,		// create new file	
	
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

















