# FilesToGPTDesktop

**FilesToGPTDesktop** is a Qt-based desktop application that allows users to select and manage files and folders, integrate task descriptions, and generate prompts with an estimated token count for use with language models like GPT.

## Features

- **Drag-and-Drop**: Easily add individual files and folders via drag-and-drop.
- **File Management**: Check or uncheck files to include/exclude them from the final prompt. Remove files from the list as needed.
- **Task Description**: Enter a task description that integrates with your selected files.
- **Token Counting**: Estimate the number of tokens in your final prompt using OpenAI's `tiktoken` library.
- **Export Options**: Copy the final prompt to the clipboard or save it as a `.txt` file.

## Setup Instructions

### Prerequisites

- **C++ Compiler**: Ensure you have a C++17 compatible compiler.
- **Qt 6.8.0**: The project is configured for Qt 6.8.0 on macOS.
- **CMake**: Version 3.16 or higher.
- **Python 3**: Required for token counting.

### Building the Project

1. **Clone the Repository** (if applicable):

   ```bash
   git clone <repository_url>
   cd FilesToGPTDesktop
