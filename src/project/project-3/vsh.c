#include <stdio.h>
#include <stdbool.h>
#include <openssl/evp.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>

#define MD5LENGTH  16  /* MD5 Digest size in bytes */

/*=================================================================
    Get the MD5 hash.
  -----------------------------------------------------------------
    unsigned char *str points to the bytes to hash
    unsigned char *hash points to the hash

    Uses the envelope interface to OpenSSL.  "man evp" for more.
   =================================================================*/
int getmd5(unsigned char *str, unsigned char *hash, int size) {

    EVP_MD_CTX *hashctx;            /* Context for MD5 sum */
    int hashlen;             /* Length of MD5 sum */


    hashctx = EVP_MD_CTX_new();  /* Allocate a context */
    EVP_MD_CTX_init(hashctx);  /* Initialize it */
    EVP_DigestInit_ex(hashctx, EVP_md5(), NULL); /* Specify the algorithm */
    EVP_DigestUpdate(hashctx, str, size); /* Add data to hash */

    EVP_DigestFinal_ex(hashctx, hash, &hashlen); /* No more data */
    EVP_MD_CTX_destroy(hashctx);  /* Free the context */
    if (hashlen != MD5LENGTH) return (0);

#ifdef DEBUGMD5
    fprintf(stdout, "GETMD5: Digest of: <%s>", str);
    fprint_string_as_hex(stdout, hash, hashlen);
    fprintf(stdout, ">\n");
#endif

    return (hashlen);

}

void fprint_string_as_hex(FILE *f, unsigned char *s, int len) {
    /* Don't need zero termination */
    /* Notice that each byte results in two characters */
    int i;
    for (i = 0; i < len; i++) fprintf(f, "%02x", s[i]);
}

void bytesToHexString(unsigned char *in, char *out, int len) {
    for (int i = 0; i < len; i++) {
        out += sprintf(out, "%02x", in[i]);
    }
}

void handleInput(char *buf, char *prompt) {
    char *delim = " \n";
    char *input_token;
    char *input_save;

    input_token = strtok_r(buf, delim, &input_save);

    // No command given
    if (input_token == NULL) {
        printf("Please enter a command\n");
        return;
    }
    // Quit
    if (strcmp(input_token, "q") == 0) {
        free(buf);
        free(prompt);
        exit(0);
    }
    // Change prompt
    else if (strcmp(input_token, "prompt") == 0) {
        input_token = strtok_r(NULL, delim, &input_save);
        if (input_token == NULL) {
            printf("Usage: prompt <prompt>\n", prompt);
            return;
        }

        // Validate characters are in interval [33,126]
        int i = 0;
        while (input_token[i] != 0) {
            if (input_token[i] < 33 || input_token[i] > 126) {
                printf("Error processing prompt\n");
                return;
            }
            i++;
        }
        prompt = realloc(prompt, strlen(input_token) + 1);
        strcpy(prompt, input_token);
        printf("Successfully changed prompt to '%s'\n", input_token);
    }
    // Attempt to run binary
    else {
        unsigned char hash[MD5LENGTH];
        int binary_fd;
        int as_fd;
        char *as_token;
        unsigned char *contents;
        char *as_contents;
        struct stat binary_stats;
        struct stat as_stats;
        int nfilebytes;

        // Load binary into memory
        binary_fd = open(input_token, O_RDONLY);
        if (binary_fd < 0) {
            printf("Error opening '%s'. Does binary exist?\n", input_token);
            return;
        }

        // Calculate md5sum
        fstat(binary_fd, &binary_stats);
        contents = malloc(binary_stats.st_size);
        nfilebytes = read(binary_fd, contents, binary_stats.st_size);
        getmd5(contents, hash, nfilebytes);

        free(contents);

        // Check ./allowedsums
        if (lstat("./allowedsums", &as_stats) < 0) {
            printf("Cannot perform lstat on './allowedsums'. File may not exist\n");
            return;
        }
        if (S_ISREG(as_stats.st_mode)
            && (as_stats.st_mode & (S_IRWXG | S_IRWXO)) == 0
            && getuid() == as_stats.st_uid) {

            // Find binary name
            as_contents = malloc(as_stats.st_size);
            as_fd = open("./allowedsums", O_RDONLY);
            read(as_fd, as_contents, as_stats.st_size);

            as_token = strstr(as_contents, input_token);
            if (as_token == NULL) {
                printf("Could not find binary '%s' in './allowedsums'\n", input_token);
                free(as_contents);
                return;
            }

            // Check if we only found substring
            if ((*(as_token - 1) > 32 && *(as_token - 1) < 127 )) {
                printf("Could not find binary '%s' in './allowedsums'\n", input_token);
                free(as_contents);
                return;
            }
            // Find pointer for hash
            as_token += strlen(input_token);
            // printf("%s\n", as_token);
            while (*as_token == ' ' || *as_token == '\t') {
                // printf("%s\n", as_token);
                as_token++;
            }
            // printf("%s\n", as_token);
            char *testString = malloc(sizeof(char) * 33);
            strncpy(testString, as_token, 32);

            // Check if hash matches
            char testHash[MD5LENGTH * 2 + 1];
            testHash[MD5LENGTH * 2] = 0;
            bytesToHexString(hash, testHash, MD5LENGTH);
            free(as_contents);
            // printf("file: %s\n", testString);
            // printf("gen : %s\n", testHash);
            if (strcmp(testHash, testString) != 0) {
                free(testString);
                printf("Hash in ./allowedsums does not match hash generated\n");

                return;
            }
            free(testString);

            // Get rest of arguments, place into array
            if (fork() == 0) {
                int i = 0;
                char command[strlen(input_token) + 1];
                strcpy(command, input_token);
                int size = 24;
                char **arguments = malloc(sizeof(char *) * size);
                while (input_token != NULL) {
                    if (i >= size) {
                        arguments = realloc(arguments, sizeof(char*) * size * 2);
                    }
                    arguments[i] = malloc(strlen(input_token)+1);
                    strcpy(arguments[i], input_token);
                    // fprintf(stdout, "arg[%d]: %s\n", i, arguments[i]);
                    input_token = strtok_r(NULL, delim, &input_save);
                    i++;
                }
                arguments[i] = NULL;
                if (execvp(command, arguments) < 0) {
                    printf("Something went wrong when executing '%s'\n", command);
                    exit(1);
                }
            }
        }
        else {
            printf("'./allowedsums' does not meet criteria\n");
            return;
        }
    }

}



int main(int argc, char **argv) {
    if (argc != 1) {
        printf("Usage: vsh\n");
        exit(1);
    }
    char *prompt;
    char *buf;
    size_t len = 8;

    buf = malloc(sizeof(char) * len);
    prompt = malloc(sizeof(char) * len);
    strcpy(prompt, "vsh#");
    if (buf == NULL) {
        printf("Error allocating buffer\n");
        exit(1);
    }

    while (true) {
        wait(NULL);
        printf("%s ", prompt);
        fflush(stdout);
        getline(&buf, &len, stdin);
        while(strstr(buf, "\n") == NULL) {
            len *=2;
            getline(&buf, &len, stdin);
        }
        handleInput(buf, prompt);
    }
}
