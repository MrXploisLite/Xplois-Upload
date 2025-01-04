FROM gcc:latest

WORKDIR /app

# Copy source files
COPY . .

# Install required libraries
RUN apt-get update && \
    apt-get install -y libssl-dev

# Create uploads directory with proper permissions
RUN mkdir -p uploads && \
    chmod 777 uploads

# Compile the application
RUN gcc -o xplois_upload example.c httplibrary.c string_lib.c -Wall

# Expose port
EXPOSE 8080

# Start the server
CMD ["./xplois_upload"] 