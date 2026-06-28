#ifndef LOGGER_H
#define LOGGER_H

typedef struct {
    char timestamp[30];
    char action[20];
    char message[256];
} LogEvent;

#define MAX_LOG_EVENTS 100

void log_event(const char *action, const char *format, ...);
void export_logs_to_json(const char *filename);
int get_log_count();
LogEvent* get_log_at(int index);

#endif
