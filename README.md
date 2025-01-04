# Xplois Upload
Simple Image Upload Server using C

## Quick Start (Windows)

1. **Prerequisites:**
   - Download and install MSYS2 from https://www.msys2.org/
   - Open MSYS2 MinGW and install required packages:
     ```bash
     pacman -Syu
     pacman -S mingw-w64-x86_64-gcc make
     ```

2. **Installation:**
   - Download and extract the Xplois Upload files
   - Double-click `deploy.bat`
   - Follow the on-screen instructions

3. **Usage:**
   - The server will start automatically after installation
   - Access the upload page at http://localhost:8080
   - To restart the server later, use `start_server.bat`

## Features
- Image upload with expiry times (1 hour, 30 minutes, 15 minutes, 1 minute)
- Supports JPG, PNG, and GIF formats
- Automatic file cleanup
- Simple and lightweight

## Configuration
Edit `config.json` to customize:
- Port number (default: 8080)
- Maximum file size (default: 32MB)
- Allowed file types
- Cleanup interval

## Troubleshooting
If you encounter issues:
1. Check if port 8080 is available
2. Ensure MSYS2 is properly installed
3. Check the logs in the `logs` directory
4. Try running as administrator

## Files
- `start_server.bat` - Starts the upload server
- `cleanup.bat` - Automatically removes expired files
- `config.json` - Server configuration
- `uploads/` - Directory for uploaded files
- `logs/` - Server logs

## Deployment to Fly.io

1. Install Fly CLI:
   ```bash
   curl -L https://fly.io/install.sh | sh
   ```

2. Login to Fly:
   ```bash
   fly auth login
   ```

3. Deploy the app:
   ```bash
   fly apps create xplois-upload
   fly volumes create xplois_upload_data --size 1
   fly deploy
   ```

Your app will be available at: `https://xplois-upload.fly.dev`
