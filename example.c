#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include "httplibrary.h"

#define CONFIG_FILE "config.json"
#define UPLOADS_DIR "uploads"

void ensure_upload_directory() {
    #ifdef _WIN32
    mkdir(UPLOADS_DIR);
    #else
    mkdir(UPLOADS_DIR, 0755);
    #endif
}

void save_file(const char* filename, const char* data, size_t size) {
    FILE* file = fopen(filename, "wb");
    if (file) {
        fwrite(data, 1, size, file);
        fclose(file);
        printf("File saved: %s\n", filename);
    } else {
        printf("Failed to save file: %s\n", filename);
    }
}

void http_handler(http_client* client) {
    printf("Client connected!\n");

    if (strcmp(client->method.val, "POST") == 0 && strstr(client->path.val, "/upload")) {
        ensure_upload_directory();

        // Get file information from headers
        const char* content_type = http_get_header(client, "Content-Type");
        const char* file_name = http_get_header(client, "X-File-Name");

        // Validate content type
        if (!content_type || !strstr(content_type, "image/")) {
            const char* error_response = 
                "HTTP/1.1 400 Bad Request\r\n"
                "Content-Type: application/json\r\n"
                "\r\n"
                "{\"status\":\"error\",\"message\":\"Invalid file type\"}";
            send(client->socket, error_response, strlen(error_response), 0);
            return;
        }

        // Get file extension
        const char* ext = ".jpg";  // default extension
        if (file_name) {
            const char* dot = strrchr(file_name, '.');
            if (dot) ext = dot;
        }

        // Generate unique filename
        time_t now = time(NULL);
        char filename[256];
        snprintf(filename, sizeof(filename), "uploads/image_%lld%s", (long long)now, ext);

        // Save the file
        save_file(filename, client->body.val, client->body.len);
        printf("Saved file: %s (size: %zu bytes)\n", filename, client->body.len);

        // Generate relative URL
        char url[256];
        snprintf(url, sizeof(url), "/uploads/image_%lld%s", (long long)now, ext);

        // Send JSON response
        char response[512];
        snprintf(response, sizeof(response), 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "\r\n"
            "{\"status\":\"success\",\"message\":\"File uploaded successfully\",\"url\":\"%s\"}", 
            url);

        send(client->socket, response, strlen(response), 0);
    } else if (strstr(client->path.val, "/uploads/")) {
        // Serve uploaded files
        char filepath[256];
        snprintf(filepath, sizeof(filepath), ".%s", client->path.val);
        http_send_file(client->socket, filepath, 0);
    } else {
        // Serve the modern upload form
        const char* response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "\r\n"
            "<!DOCTYPE html>\n"
            "<html lang=\"en\">\n"
            "<head>\n"
            "    <meta charset=\"UTF-8\">\n"
            "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
            "    <title>Xplois Upload</title>\n"
            "    <link href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/css/all.min.css\" rel=\"stylesheet\">\n"
            "    <style>\n"
            "        @import url('https://fonts.googleapis.com/css2?family=Poppins:wght@400;500;600&display=swap');\n"
            "        :root { --primary: #6C63FF; --secondary: #4CAF50; --dark: #2C3E50; --light: #ECF0F1; }\n"
            "        * { margin: 0; padding: 0; box-sizing: border-box; font-family: 'Poppins', sans-serif; }\n"
            "        body { background: var(--light); min-height: 100vh; display: flex; align-items: center; justify-content: center; }\n"
            "        .container { max-width: 800px; width: 90%; padding: 2rem; background: white; border-radius: 20px; box-shadow: 0 10px 30px rgba(0,0,0,0.1); animation: slideUp 0.8s ease; }\n"
            "        .upload-area { border: 3px dashed var(--primary); border-radius: 15px; padding: 2rem; text-align: center; transition: all 0.3s ease; cursor: pointer; }\n"
            "        .upload-area:hover { background: rgba(108, 99, 255, 0.1); }\n"
            "        .upload-area.dragging { background: rgba(108, 99, 255, 0.2); transform: scale(1.02); }\n"
            "        .title { color: var(--dark); margin-bottom: 1.5rem; font-size: 2rem; animation: fadeIn 1s ease; }\n"
            "        .upload-icon { font-size: 4rem; color: var(--primary); margin-bottom: 1rem; animation: bounce 2s infinite; }\n"
            "        .expiry-select { margin: 1rem 0; padding: 0.8rem; border: 2px solid var(--primary); border-radius: 10px; width: 200px; outline: none; transition: all 0.3s ease; }\n"
            "        .upload-btn { background: var(--primary); color: white; padding: 1rem 2rem; border: none; border-radius: 10px; cursor: pointer; font-size: 1.1rem; transition: all 0.3s ease; }\n"
            "        .upload-btn:hover { transform: translateY(-2px); box-shadow: 0 5px 15px rgba(108, 99, 255, 0.3); }\n"
            "        .progress-bar { width: 100%; height: 10px; background: #eee; border-radius: 5px; margin: 1rem 0; overflow: hidden; display: none; }\n"
            "        .progress { width: 0%; height: 100%; background: var(--primary); transition: width 0.3s ease; }\n"
            "        .result { margin-top: 1rem; padding: 1rem; border-radius: 10px; background: #f8f9fa; display: none; }\n"
            "        .success { background: #d4edda; color: #155724; padding: 1rem; border-radius: 10px; margin-top: 1rem; }\n"
            "        .error { background: #f8d7da; color: #721c24; padding: 1rem; border-radius: 10px; margin-top: 1rem; }\n"
            "        code { background: rgba(0,0,0,0.1); padding: 0.2rem 0.4rem; border-radius: 4px; font-family: monospace; }\n"
            "        a { color: var(--primary); text-decoration: none; }\n"
            "        a:hover { text-decoration: underline; }\n"
            "        @keyframes slideUp { from { opacity: 0; transform: translateY(50px); } to { opacity: 1; transform: translateY(0); } }\n"
            "        @keyframes fadeIn { from { opacity: 0; } to { opacity: 1; } }\n"
            "        @keyframes bounce { 0%, 100% { transform: translateY(0); } 50% { transform: translateY(-10px); } }\n"
            "    </style>\n"
            "</head>\n"
            "<body>\n"
            "    <div class=\"container\">\n"
            "        <h1 class=\"title\">Xplois Upload</h1>\n"
            "        <div class=\"upload-area\" id=\"dropZone\">\n"
            "            <i class=\"fas fa-cloud-upload-alt upload-icon\"></i>\n"
            "            <h3>Drag & Drop your images here</h3>\n"
            "            <p>or</p>\n"
            "            <input type=\"file\" id=\"fileInput\" hidden accept=\"image/*\">\n"
            "            <button class=\"upload-btn\" onclick=\"document.getElementById('fileInput').click()\">\n"
            "                Choose File\n"
            "            </button>\n"
            "            <select class=\"expiry-select\">\n"
            "                <option value=\"3600\">1 hour</option>\n"
            "                <option value=\"1800\">30 minutes</option>\n"
            "                <option value=\"900\">15 minutes</option>\n"
            "                <option value=\"60\">1 minute</option>\n"
            "            </select>\n"
            "        </div>\n"
            "        <div class=\"progress-bar\">\n"
            "            <div class=\"progress\"></div>\n"
            "        </div>\n"
            "        <div class=\"result\"></div>\n"
            "    </div>\n"
            "    <script>\n"
            "        const dropZone = document.getElementById('dropZone');\n"
            "        const fileInput = document.getElementById('fileInput');\n"
            "        \n"
            "        ['dragenter', 'dragover', 'dragleave', 'drop'].forEach(eventName => {\n"
            "            dropZone.addEventListener(eventName, preventDefaults, false);\n"
            "        });\n"
            "        \n"
            "        function preventDefaults (e) {\n"
            "            e.preventDefault();\n"
            "            e.stopPropagation();\n"
            "        }\n"
            "        \n"
            "        ['dragenter', 'dragover'].forEach(eventName => {\n"
            "            dropZone.addEventListener(eventName, highlight, false);\n"
            "        });\n"
            "        \n"
            "        ['dragleave', 'drop'].forEach(eventName => {\n"
            "            dropZone.addEventListener(eventName, unhighlight, false);\n"
            "        });\n"
            "        \n"
            "        function highlight(e) {\n"
            "            dropZone.classList.add('dragging');\n"
            "        }\n"
            "        \n"
            "        function unhighlight(e) {\n"
            "            dropZone.classList.remove('dragging');\n"
            "        }\n"
            "        \n"
            "        dropZone.addEventListener('drop', handleDrop, false);\n"
            "        fileInput.addEventListener('change', handleFiles, false);\n"
            "        \n"
            "        function handleDrop(e) {\n"
            "            const dt = e.dataTransfer;\n"
            "            const files = dt.files;\n"
            "            handleFiles({target: {files: files}});\n"
            "        }\n"
            "        \n"
            "        function handleFiles(e) {\n"
            "            const files = e.target.files;\n"
            "            uploadFile(files[0]);\n"
            "        }\n"
            "        \n"
            "        function uploadFile(file) {\n"
            "            const progressBar = document.querySelector('.progress-bar');\n"
            "            const progress = document.querySelector('.progress');\n"
            "            const result = document.querySelector('.result');\n"
            "            \n"
            "            if (!file || !file.type.startsWith('image/')) {\n"
            "                result.style.display = 'block';\n"
            "                result.innerHTML = `<div class='error'>Please select a valid image file</div>`;\n"
            "                return;\n"
            "            }\n"
            "            \n"
            "            progressBar.style.display = 'block';\n"
            "            progress.style.width = '0%';\n"
            "            \n"
            "            // Create binary chunks\n"
            "            const reader = new FileReader();\n"
            "            reader.onload = function(e) {\n"
            "                const arrayBuffer = e.target.result;\n"
            "                \n"
            "                // Send the binary data directly\n"
            "                fetch('/upload', {\n"
            "                    method: 'POST',\n"
            "                    headers: {\n"
            "                        'Content-Type': file.type,\n"
            "                        'X-File-Name': file.name\n"
            "                    },\n"
            "                    body: arrayBuffer\n"
            "                })\n"
            "                .then(response => response.json())\n"
            "                .then(data => {\n"
            "                    progress.style.width = '100%';\n"
            "                    result.style.display = 'block';\n"
            "                    const imageUrl = window.location.origin + data.url;\n"
            "                    result.innerHTML = `\n"
            "                        <div class='success'>\n"
            "                            <h3>Upload Complete!</h3>\n"
            "                            <img src='${imageUrl}' style='max-width: 300px; margin: 10px 0;' />\n"
            "                            <p>Direct Link: <a href='${imageUrl}' target='_blank'>${imageUrl}</a></p>\n"
            "                            <p>HTML: <code>&lt;img src='${imageUrl}' /&gt;</code></p>\n"
            "                            <p>BBCode: <code>[img]${imageUrl}[/img]</code></p>\n"
            "                        </div>`;\n"
            "                })\n"
            "                .catch(error => {\n"
            "                    progress.style.width = '100%';\n"
            "                    result.style.display = 'block';\n"
            "                    result.innerHTML = `<div class='error'>Upload failed: ${error.message}</div>`;\n"
            "                });\n"
            "            };\n"
            "            \n"
            "            reader.readAsArrayBuffer(file);\n"
            "        }\n"
            "    </script>\n"
            "</body>\n"
            "</html>";
        
        send(client->socket, response, strlen(response), 0);
    }
}

int main() {
    printf("Starting server...\n");
    SOCKET s_socket = http_init_socket("0.0.0.0", 8080);
    
    if (s_socket <= 0) {
        printf("Failed to initialize socket!\n");
        return 1;
    }
    
    printf("Server listening on port 8080...\n");
    http_start(s_socket, http_handler);

    http_close_socket(s_socket);
    return 0;
}