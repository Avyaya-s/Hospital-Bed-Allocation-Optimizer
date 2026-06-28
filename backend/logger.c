#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include "logger.h"

LogEvent event_log[MAX_LOG_EVENTS];
int log_count = 0;
int log_head = 0; // Circular buffer head

void get_current_time_str(char *buffer, int size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}

void log_event(const char *action, const char *format, ...) {
    char buffer[256];
    va_list args;

    // Use VA_ARGS to format the message
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // Circular buffer logic or overwrite oldest?
    // Let's implement simple circular or shift. 
    // For simplicity, shift if full, or circular.
    // Let's do simple linear for now, capping at MAX. (Or circular if requested).
    // Task says "scrollable log", implying history.
    
    // Safety cap
    if (log_count < MAX_LOG_EVENTS) {
        get_current_time_str(event_log[log_count].timestamp, 30);
        strncpy(event_log[log_count].action, action, 20);
        strncpy(event_log[log_count].message, buffer, 256);
        log_count++;
    } else {
        // Shift left to make room (O(N) but N is small 100)
        for(int i=0; i<MAX_LOG_EVENTS-1; i++) {
            event_log[i] = event_log[i+1];
        }
        get_current_time_str(event_log[MAX_LOG_EVENTS-1].timestamp, 30);
        strncpy(event_log[MAX_LOG_EVENTS-1].action, action, 20);
        strncpy(event_log[MAX_LOG_EVENTS-1].message, buffer, 256);
    }
}

void export_logs_to_json(const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) return;

    fprintf(fp, "{\n  \"logs\": [\n");
    for (int i = 0; i < log_count; i++) {
        fprintf(fp, "    { \"timestamp\": \"%s\", \"action\": \"%s\", \"message\": \"%s\" }%s\n",
            event_log[i].timestamp,
            event_log[i].action,
            event_log[i].message,
            (i < log_count - 1) ? "," : ""
        );
    }
    fprintf(fp, "  ]\n}\n");
    fclose(fp);
}

int get_log_count() { return log_count; }
LogEvent* get_log_at(int index) {
    if (index >= 0 && index < log_count) return &event_log[index];
    return NULL;
}
