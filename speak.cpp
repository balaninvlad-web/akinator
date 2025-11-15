#include <TXLib.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "tree_functions.h"

const int MAX_SPEECH_BUFFER_SIZE = 2000;

static char speech_buffer[MAX_SPEECH_BUFFER_SIZE] = {0};
static size_t bufferPosition = 0;

/*void speakFlush()
{
    if (bufferPosition == 0) return;

    char command[MAX_SPEECH_BUFFER_SIZE * 2] = {};
    snprintf(command, sizeof(command),
        "PowerShell -Command \""
        "Add-Type -AssemblyName System.Speech; "
        "$speech = New-Object System.Speech.Synthesis.SpeechSynthesizer; "
        "$speech.Speak('%s');\"",
        speech_buffer);

    system(command);

    bufferPosition = 0;
    speech_buffer[0] = '\0';
}

void speak(const char* format, ...)
{
    assert(format);

    char formatted_text[MAX_STR_SIZE] = {};

    va_list args;
    va_start(args, format);
    int chars_written = vsnprintf(formatted_text, MAX_STR_SIZE, format, args);
    va_end(args);

    if (bufferPosition + (size_t) chars_written + 1 >= MAX_SPEECH_BUFFER_SIZE)
    {
        speakFlush();
    }

    strcat(speech_buffer + bufferPosition, formatted_text);
    bufferPosition += (size_t) chars_written;

    if (bufferPosition > MAX_SPEECH_BUFFER_SIZE / 2)
    {
        speakFlush();
    }
}*/
