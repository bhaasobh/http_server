#include "httpd.h"
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define BUF_SIZE 1024


void save_credentials_login(const char *username, const char *password) {
      char fileName[256];  

   
    snprintf(fileName, sizeof(fileName), "%s.txt", username);
    FILE *userFile = fopen(fileName, "a");
    if (userFile == NULL) {
        perror("Unable to open user file");
        exit(EXIT_FAILURE);
    }
    FILE *file = fopen("password.txt", "a");
    if (file == NULL) {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "Username: %s, Password: %s\n", username, password);
    fprintf(userFile, "Hi : %s\n here is your info",username);
    fclose(userFile);
    fclose(file);
}
void save_user_info(const char *username, const char *info) {
    char fileName[256];  

   
    snprintf(fileName, sizeof(fileName), "%s.txt", username);

    
    FILE *file = fopen(fileName, "w");

    if (file == NULL) {
        printf("Could not open file %s for writing.\n", fileName);
        return;
    }

    
    fprintf(file, "%s", info);

    
    fclose(file);
}



int check_credentials(const char *username, const char *password) {
    FILE *file = fopen("password.txt", "r");
    if (file == NULL) {
        perror("Unable to open file");
        return 0; 
    }

    char stored_username[BUF_SIZE];
    char stored_password[BUF_SIZE];
    char line[BUF_SIZE];

    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "Username: %[^,], Password: %s\n", stored_username, stored_password);
        if (strcmp(stored_username, username) == 0 && strcmp(stored_password, password) == 0) {
            fclose(file);
            return 1; 
        }
    }


    fclose(file);
    return 0; 
}
char* read_file_content(const char *username) {
    char fileName[256];  // Buffer for the file name

    // Correctly use snprintf to limit the size of the written string
    snprintf(fileName, sizeof(fileName), "%s.txt", username);

    // Open the file for reading
    FILE *file = fopen(fileName, "r");

    // Check if the file was opened successfully
    if (file == NULL) {
        printf("Could not open file %s for reading.\n", fileName);
        return NULL; // Return NULL to indicate failure
    }

    // Move to the end of the file to find the file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);  // Move back to the start of the file

    // Allocate memory to store the content (+1 for null-terminator)
    char *content = (char *)malloc(fileSize + 1);
    if (content == NULL) {
        printf("Memory allocation failed.\n");
        fclose(file);
        return NULL;  // Return NULL to indicate failure
    }

    // Read the entire file into memory
    fread(content, 1, fileSize, file);
    content[fileSize] = '\0';  // Null-terminate the string

    // Close the file
    fclose(file);

    // Return the file content
    return content;
}
void serve_image(const char *image_path) {
    FILE *file = fopen(image_path, "rb");  // Open image in binary mode
    if (file == NULL) {
        printf("HTTP/1.1 404 Not Found\r\n");
        printf("Content-Type: text/html\r\n\r\n");
        printf("<html><body><h1>404 Not Found</h1></body></html>\n");
        return;
    }

    // Send headers for the image file
    printf("HTTP/1.1 200 OK\r\n");
    printf("Content-Type: image/jpeg\r\n\r\n");  // Change to the correct MIME type if needed (e.g., image/png)

    // Read and send the image data
    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        fwrite(buffer, 1, bytes_read, stdout);
    }

    // Close the image file
    fclose(file);
}

void route() {
    ROUTE_START()

    ROUTE_GET("/")
    {
        printf("HTTP/1.1 200 OK\r\n\r\n");
        printf("Hello! You are using %s", request_header("User-Agent"));
    }

   
 

  
    ROUTE_POST("/register.html")
    {
        printf("HTTP/1.1 200 OK\r\n\r\n");

        char username[BUF_SIZE] = {0};
        char password[BUF_SIZE] = {0};

        
        char *body = payload;
        sscanf(body, "sername=%[^&]&password=%s", username, password);
      
        
        for (char *p = username; *p; p++) if (*p == '+') *p = ' ';
        for (char *p = password; *p; p++) if (*p == '+') *p = ' ';

      
        save_credentials_login(username, password);

      
        printf("<style>\n");
        printf("body \n{");
        printf("background-image: url(/lion_awakee.jpg);\n");
        printf("background-repeat: no-repeat;\n");
        printf("background-attachment: fixed;\n");
        printf("background-size:contain;\n");
        printf("}\n</style>\n");
        printf("<html><body>");
        printf("<h1>Registration Successful</h1>");
        printf("<p>Username: %s</p>", username);
        printf("</body></html>");
    }
    ROUTE_GET("/lion_awake.jpg")
    {   
    serve_image("lion_awake.jpg");  // Serve the image file
    }
    ROUTE_GET("/lion_sleeping.jpg")
    {   
    serve_image("lion_sleeping.jpg");  // Serve the image file
    }
    ROUTE_POST("/saveinfo.html")
{
    char username[BUF_SIZE] = {0};
    char user_info[BUF_SIZE] = {0};

    
    char *body = payload;
    
    
    sscanf(body, "sername=%[^&]&userinfo=%s", username, user_info);

   
    for (char *p = username; *p; p++) if (*p == '+') *p = ' ';
    for (char *p = user_info; *p; p++) if (*p == '+') *p = ' ';

    
    save_user_info(username, user_info);

   
    printf("HTTP/1.1 200 OK\r\n\r\n");
    printf("<style>\n");
        printf("body \n{");
        printf("background-image: url(/lion_awake.jpg);\n");
        printf("background-repeat: no-repeat;\n");
        printf("background-attachment: fixed;\n");
        printf("background-size:contain;\n");
        printf("}\n</style>\n");
    printf("<html>");
    printf("<body>");
    printf("<h1>Saved info for user: %s</h1>", username);
    printf("<p>Info: %s</p>", user_info);
    printf("<button onclick=\"history.back()\">Go Back</button>");
    printf("</body>");
    printf("</html>");
}

// Handle POST request for login
ROUTE_POST("/login")
{
    printf("HTTP/1.1 200 OK\r\n\r\n");

    char username[BUF_SIZE] = {0};
    char password[BUF_SIZE] = {0};
    char *file_content = NULL;

    
    char *body = payload;
    sscanf(body, "sername=%[^&]&password=%s", username, password);

    // Decode URL-encoded values (replace '+' with ' ', etc.)
    for (char *p = username; *p; p++) if (*p == '+') *p = ' ';
    for (char *p = password; *p; p++) if (*p == '+') *p = ' ';

    
    if (check_credentials(username, password)) 
    {
        
        file_content = read_file_content(username);

        printf("<style>\n");
        printf("body \n{");
        printf("background-image: url(/lion_awake.jpg);\n");
        printf("background-repeat: no-repeat;\n");
        printf("background-attachment: fixed;\n");
        printf("background-size:contain;\n");
        printf("}\n</style>\n");

        printf("<html><body>");
        printf("<h1>Login Successful</h1>");
        printf("<p><label>Welcome, %s!</label></p>", username);

   
        printf("<form action=\"/saveinfo.html\" method=\"POST\">");
        printf("<input type=\"hidden\" name=\"username\" value=\"%s\"><br><br>", username); // Use hidden input instead of disabled
        printf("<textarea id=\"userinfo\" name=\"userinfo\" rows=\"4\" cols=\"50\">%s</textarea><br><br>", file_content);
        printf("<br>");
        printf("<input type=\"submit\" value=\"Submit\">");
        printf("</form>");
        printf("</body></html>");
    } 
    else 
    {
        // Failed login
        printf("<style>\n");
        printf("body \n{");
        printf("background-image: url(/lion_sleeping.jpg);\n");
        printf("background-repeat: no-repeat;\n");
        printf("background-attachment: fixed;\n");
        printf("background-size:contain;\n");
        printf("}\n</style>\n");

        printf("<html><body>");
        printf("<h1>Login Failed</h1>");
        printf("<p>Invalid username or password.</p>");
        printf("</body></html>");
    }
}

    ROUTE_END()
}
