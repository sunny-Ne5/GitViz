#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <git2.h>

#define REPO ".git"
#define GIT_SUCCESS 0

/**
 * Almost all libgit2 functions return 0 on success or negative on error.
 * This is not production quality error checking, but should be sufficient
 * as an example.
 */
static void check_error(int error_code, const char *action)
{
	const git_error *error = git_error_last();
	if (!error_code)
		return;

	printf("Error %d %s - %s\n", error_code, action,
			(error && error->message) ? error->message : "???");

	exit(1);
}

void init()
{
	/**
	 * Initialize the library, this will set up any global state which libgit2 needs
	 * including threading and crypto
	 */
	git_libgit2_init();
}

int main(int argc, char* argv[]){

	// Parse CLI options
    char *branch;

    // Loop through the command-line arguments
    for (int i = 1; i < argc; i++) {
        // Check if the current argument is "--branch"
        if (strcmp(argv[i], "--branch") == 0) {
            // Get the branch name from the next argument
            if (i + 1 < argc) {
                branch = argv[i + 1];
            } else {
                printf("Error: branch name not specified\n");
                return 1;
            }
        }
    }

	init();
	int error;
    git_repository *repo;
	

	error = git_repository_open(&repo, REPO);
	check_error(error, "opening repo");


    // Read HEAD on master
    char head_filepath[512];
    FILE *head_fileptr;
    char head_rev[41];

    strcpy(head_filepath, REPO);

    if(strrchr(REPO, '/') != (REPO+strlen(REPO)))
        strcat(head_filepath, "/refs/heads/");
    else
        strcat(head_filepath, "refs/heads/");

	// append branch
	strcat(head_filepath, branch);

    if((head_fileptr = fopen(head_filepath, "r")) == NULL){
        fprintf(stderr, "Error opening '%s'\n", head_filepath);
        return 1;
    }

    if(fread(head_rev, 40, 1, head_fileptr) != 1){
        fprintf(stderr, "Error reading from '%s'\n", head_filepath);
        fclose(head_fileptr);
        return 1;
    }   

    fclose(head_fileptr);


    git_oid oid;
    git_revwalk *walker;
    git_commit *commit;

    if(git_oid_fromstr(&oid, head_rev) != GIT_SUCCESS){
        fprintf(stderr, "Invalid git object: '%s'\n", head_rev);
        return 1;
    }

    git_revwalk_new(&walker, repo);
    git_revwalk_sorting(walker, GIT_SORT_TIME);
    git_revwalk_push(walker, &oid);

    const char *commit_message;
    const git_signature *commit_author;
	int cnt =0;
    while(git_revwalk_next(&oid, walker) == GIT_SUCCESS) {
        if(git_commit_lookup(&commit, repo, &oid)){
            fprintf(stderr, "Failed to lookup the next object\n");
            return 1;
        }

        commit_message  = git_commit_message(commit);
        commit_author = git_commit_committer(commit);


        // Don't print the \n in the commit_message 
        //printf("'%.*s' by %s <%s> time: %I64u\n", strlen(commit_message)-1, commit_message, commit_author->name, commit_author->email, commit_author->when);
		//printf("%s\t\t'%.*s'\t\t%llu\n", commit_author->email,strlen(commit_message)-1, commit_message, commit_author->when );
		printf("%s\t\t%llu\n", commit_author->email, commit_author->when);
		//printf("%s\n", commit_message);

        git_commit_free(commit);
    }
	printf("commit count %d\n", cnt);

    git_revwalk_free(walker);

    return 0;

}